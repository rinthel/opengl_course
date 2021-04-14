# Advanced Feature Part 2

---

## Cubemap

- Cubemap
  - 2D 텍스처 6장으로 정육면체 형태로 이어붙인 텍스처 맵
  - 주변 환경을 그리거나, 주변 환경을 반사시켜서 사실감을 높이는데 효과적

---

## Cubemap

- Cubemap
  - 원점을 기준으로 하는 3차원 방향 벡터로부터 텍스처 픽셀값을 샘플링함

<div>
<img src="/opengl_course/note/images/12_cubemap_sampling.png" width="35%" />
</div>

---

## Skybox

- 3D 장면의 뒷배경을 채워주는 큰 큐브
- 적은 정점으로 주변 환경을 그릴 수 있음

<div>
<img src="/opengl_course/note/images/12_cubemap_skybox_example.png" width="60%" />
</div>

---

## Skybox

- 예제 다운로드
  - [learnopengl.com/img/textures/skybox.zip](https://learnopengl.com/img/textures/skybox.zip)
  - 압축을 풀어서 `image/skybox` 디렉토리에 저장

<div>
<img src="/opengl_course/note/images/12_cubemap_skybox_developed.png" width="55%" />
</div>

---

## Cubemap Creation

- `src/texture.h`에 큐브 텍스처를 위한 `CubeTexture` 클래스 선언 추가

```cpp
CLASS_PTR(CubeTexture)
class CubeTexture {
public:
  static CubeTextureUPtr CreateFromImages(
    const std::vector<Image*> images);
  ~CubeTexture();

  const uint32_t Get() const { return m_texture; }
  void Bind() const;
private:
  CubeTexture() {}
  bool InitFromImages(const std::vector<Image*> images);
  uint32_t m_texture { 0 };
};
```

---

## Cubemap Creation

- `src/texture.cpp`에 `CubeTexture` 클래스의 멤버 메소드 구현 추가

```cpp

CubeTextureUPtr CubeTexture::CreateFromImages(const std::vector<Image*> images) {
  auto texture = CubeTextureUPtr(new CubeTexture());
  if (!texture->InitFromImages(images))
    return nullptr;
  return std::move(texture);
}

CubeTexture::~CubeTexture() {
  if (m_texture) {
    glDeleteTextures(1, &m_texture);
  }
}

void CubeTexture::Bind() const {
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);    
}

bool CubeTexture::InitFromImages(const std::vector<Image*> images) {
  glGenTextures(1, &m_texture);
  Bind();

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  for (uint32_t i = 0; i < (uint32_t)images.size(); i++) {
    auto image = images[i];
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount()) {
        default: break;
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
    }

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB,
      image->GetWidth(), image->GetHeight(), 0,
      format, GL_UNSIGNED_BYTE,
      image->GetData());
  }

  return true;
}
```

---

## OpenGL Remarks

- `GL_TEXTURE_2D`가 아닌 `GL_TEXTURE_CUBE_MAP` 사용
- wrapping 옵션에 `GL_TEXTURE_WRAP_R` 까지 설정
- `glTexImage2D()`를 호출할 때 `GL_TEXTURE_CUBE_MAP_POSITIVE_X` 등
  6면의 이미지를 텍스처 메모리에 복사

---

## Cubemap Loading

- `src/image.h`, `src/image.cpp` 수정
  - 이미지 수직 플리핑을 파라미터로 조정할 수 있도록 함

```cpp
class Image {
  static ImageUPtr Load(const std::string& filepath, bool flipVertical = true);
  // ...
  bool LoadWithStb(const std::string& filepath, bool flipVertical);
  // ...
};
```

```cpp [1, 3, 5-6]
ImageUPtr Image::Load(const std::string& filepath, bool flipVertical) {
  auto image = ImageUPtr(new Image());
  if (!image->LoadWithStb(filepath, flipVertical))
// ...
bool Image::LoadWithStb(const std::string& filepath, bool flipVertical) {
    stbi_set_flip_vertically_on_load(flipVertical);
```

---

## Skybox Program

- Skybox를 그리기 위한 쉐이더 작성
  - `shader/skybox.vs`

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;
out vec3 texCoord;

uniform mat4 transform;

void main() {
  texCoord = aPos;
  gl_Position = transform * vec4(aPos, 1.0);
}
```

---

## Skybox Program

- Skybox를 그리기 위한 쉐이더 작성
  - `shader/skybox.fs`

```glsl
#version 330 core
out vec4 fragColor;
in vec3 texCoord;

uniform samplerCube skybox;

void main() {
    fragColor = texture(skybox, texCoord);
}
```

---

## Skybox Draw

- `Context` 클래스에 Skybox 텍스처와 Skybox 프로그램을 위한
  멤버 추가

```cpp
  // skybox
  CubeTextureUPtr m_skyboxTexture;
  ProgramUPtr m_skyboxProgram;
```

---

## Skybox Draw

- `Context::Init()`에서 프로그램 및 텍스처 로딩

```cpp
  auto cubeRight = Image::Load("./image/skybox/right.jpg", false);
  auto cubeLeft = Image::Load("./image/skybox/left.jpg", false);
  auto cubeTop = Image::Load("./image/skybox/top.jpg", false);
  auto cubeBottom = Image::Load("./image/skybox/bottom.jpg", false);
  auto cubeFront = Image::Load("./image/skybox/front.jpg", false);
  auto cubeBack = Image::Load("./image/skybox/back.jpg", false);
  m_skyboxTexture = CubeTexture::CreateFromImages({
    cubeRight.get(),
    cubeLeft.get(),
    cubeTop.get(),
    cubeBottom.get(),
    cubeFront.get(),
    cubeBack.get(),
  });
  m_skyboxProgram = Program::Create("./shader/skybox.vs", "./shader/skybox.fs");
```

---

## Skybox Draw

- `Context::Render()`에서 기본 큐브를 이용하여 렌더링

```cpp [2, 8-15]
  auto projection = glm::perspective(glm::radians(45.0f),
    (float)m_width / (float)m_height, 0.01f, 100.0f);
  auto view = glm::lookAt(
    m_cameraPos,
    m_cameraPos + m_cameraFront,
    m_cameraUp);

  auto skyboxModelTransform =
      glm::translate(glm::mat4(1.0), m_cameraPos) *
      glm::scale(glm::mat4(1.0), glm::vec3(50.0f));
  m_skyboxProgram->Use();
  m_skyboxTexture->Bind();
  m_skyboxProgram->SetUniform("skybox", 0);
  m_skyboxProgram->SetUniform("transform", projection * view * skyboxModelTransform);
  m_box->Draw(m_skyboxProgram.get());
```

---

## Skybox

- 빌드 및 실행 결과

<div>
<img src="/opengl_course/note/images/12_cubemap_skybox_result.png" width="75%" />
</div>

---

## Cube
- creation
- skybox
- environment map

---

## Advanced Data

- `glBufferSubData`
- `glMapBuffer`, `glUnmapBuffer`
- `glCopyBufferSubData`

---

## Advanced GLSL

- built-in variables
- interface blocks
- uniform block / uniform buffer object

---

## Geometry Shader

- explanation
- exploding object
- normal vector visualization

---

## Instancing

- asteroid field

---

## Anti-aliasing

- what it is?
- multisampling
- msaa
- offscreen msaa

---

## Congratulation!
### 수고하셨습니다!
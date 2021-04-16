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
  // cubemap
  CubeTextureUPtr m_cubeTexture;
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
  m_cubeTexture = CubeTexture::CreateFromImages({
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
  m_cubeTexture->Bind();
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

## Environment Mapping

- 주변을 감싸고 있는 환경을 렌더링하고자 하는 오브젝트에 적용하는 방식
  - 주변 환경이 물체에 반사되거나
  - 주변 환경이 물체에 굴절되서 보이는 경우

---

## Environment Mapping

- 반사
  - 시선 벡터와 물체의 법선 벡터를 이용하여 반사 벡터를 계산
  - 벡터 방향으로부터 큐브맵 텍스처의 픽셀값을 가져옴

<div>
<img src="/opengl_course/note/images/12_cubemap_environment_map_reflection_explain.png" width="35%" />
</div>

---

## Environment Mapping

- `shader/env_map.vs` 추가

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 normal;
out vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    normal = mat3(transpose(inverse(model))) * aNormal;
    position = vec3(model * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(position, 1.0);
}
```

---

## Environment Mapping

- `shader/env_map.fs` 추가

```glsl
#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main() {
    vec3 I = normalize(position - cameraPos);
    vec3 R = reflect(I, normalize(normal));
    fragColor = vec4(texture(skybox, R).rgb, 1.0);
}
```

---

## Environment Mapping

- `Context` 클래스에 프로그램을 위한 멤버 추가

```cpp [4]
    // cubemap
    CubeTextureUPtr m_cubeTexture;
    ProgramUPtr m_skyboxProgram;
    ProgramUPtr m_envMapProgram;
```

---

## Environment Mapping

- `Context::Init()` 에서 프로그램 로드

```cpp [3-4]
  m_skyboxProgram = Program::Create(
    "./shader/skybox.vs", "./shader/skybox.fs");
  m_envMapProgram = Program::Create(
    "./shader/env_map.vs", "./shader/env_map.fs");
```

---

## Environment Mapping

- `Context::Render()` 에서 `m_envMapProgram`을
  이용하여 박스 그리기

```cpp
  modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.75f, -2.0f)) *
    glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
  m_envMapProgram->Use();
  m_envMapProgram->SetUniform("model", modelTransform);
  m_envMapProgram->SetUniform("view", view);
  m_envMapProgram->SetUniform("projection", projection);
  m_envMapProgram->SetUniform("cameraPos", m_cameraPos);
  m_cubeTexture->Bind();
  m_envMapProgram->SetUniform("skybox", 0);
  m_box->Draw(m_envMapProgram.get());
```

---

## Environment Mapping

- 빌드 및 실행 결과

<div>
<img src="/opengl_course/note/images/12_cubemap_environment_map_reflection_result.png" width="75%" />
</div>

---

## Environment Mapping

- 보통 단독으로 사용하지는 않고 일반 쉐이더 (phong, PBS 등)에 섞어서 사용
- cube map에서만 색상을 가져오므로 근처의 오브젝트가 비치지 않아서 비현실성이
  있음

---

## Environment Mapping

- Reflection(반사) 외에도 Refraction(굴절)로도 구현 가능
  - 굴절률 수치를 바탕으로 굴절각 계산
  - glsl에서 제공하는 `refract()` 함수를 사용하여 굴절된 벡터 계산

<div>
<img src="/opengl_course/note/images/12_cubemap_environment_map_refraction_explain.png" width="35%" />
</div>

---

## Environment Mapping

- Dynamic environment map
  - 고정된 이미지를 로딩하는 형태가 아닌, 매 프레임마다 변하는 환경 맵
  - Framebuffer를 이용하여 매 프레임 cube map의 각 면을 렌더링
  - 결국 6번의 렌더링이 사전에 이루어져야 하므로 성능에 영향을 미침

---

## Advanced Data

- VBO, IBO의 데이터를 관리하는 방법
  - 지금까지 사용했던 VBO에 데이터 관리: `glBufferData()`
  - `glBufferData()`에 메모리 포인터 인자에 `nullptr`을
    넘겨주면, 메모리 할당만 하고 복사는 일어나지 않음

---

## Advanced Data

- OpenGL에서 제공하는 버퍼 데이터 업데이트 방법
  - 일부 복사: `glBufferSubData()`
  - 직접 억세스: `glMapBuffer()` / `glUnmapBuffer()`
  - 버퍼에서 버퍼로 복사하기: `glCopyBufferSubData()`

---

## Advanced Data

- `glBufferSubData(target, offset, size, ptr)`
  - `target`에 바인딩된 버퍼에 `offset`부터 `size`
    만큼의 공간에 `ptr`부터 `size` 만큼의 데이터를 복사

```cpp
// Range: [24, 24 + sizeof(data)]
glBufferSubData(GL_ARRAY_BUFFER, 24, sizeof(data), &data);
```

---

## Advanced Data

- `glMapBuffer(target, usage)`
  - `target`에 바인딩된 버퍼에 `usage` 목적으로 접근 가능한
    데이터 포인터를 가져옴
- `glUnmapBuffer(target)`
  - 포인터를 통한 버퍼 접근 종료

---

## Advanced Data

```cpp
float data[] = {
  0.5f, 1.0f, -0.35f
  [...]
};
glBindBuffer(GL_ARRAY_BUFFER, buffer);
// get pointer
void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
// now copy data into memory
memcpy(ptr, data, sizeof(data));
// make sure to tell OpenGL we’re done with the pointer
glUnmapBuffer(GL_ARRAY_BUFFER);
```

---

## Advanced Data

- `glCopyBufferSubData(rtarget, wtarget, roffset, woffset, size)`
  - `rtarget`에 바인딩된 버퍼의 `roffset`부터 `size` 만큼의 데이터를
    `wtarget`에 바인딩된 버퍼의 `wtarget`부터 `size` 만큼의 공간에
    복사

```cpp
glBindBuffer(GL_COPY_READ_BUFFER, vbo1);
glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
  0, 0, 8 * sizeof(float));
```

---

## Advanced GLSL

- Built-in variable
  - shader 내에서 사용할 수 있는 미리 선언된 변수
  - vertex shader의 gl_Position은 built-in output variable
  - 그 외의 built-in variable을 알아보자

---

## GLSL Built-in variable

- `gl_PointSize`
  - `GL_POINTS`를 이용하여 점을 그리고자 할 경우
    vertex shader에서 설정할 수 있는 점의 크기값
  - `glEnable(GL_PROGRAM_POINT_SIZE);` 으로 활성화 한 뒤
    사용 가능

```glsl
void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  gl_PointSize = gl_Position.z;
}
```

---

## GLSL Built-in variable

- `gl_VertexID`
  - 현재 vertex shader에서 처리 중인 정점의 인덱스
- `gl_FragCoord`
  - fragment shader에서 사용할 수 있는 현재 픽셀의 화면상 위치값

---

## GLSL Built-in variable

```glsl
void main() {
  if(gl_FragCoord.x < 400)
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
  else
    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
```

<div>
<img src="/opengl_course/note/images/12_glsl_fragcoord.png" width="40%" />
</div>

---

## GLSL Built-in variable

- `gl_FrontFacing`
  - fragment shader에서 사용할 수 있음
  - 현재 픽셀이 앞면인지 혹은 뒷면인지를 알 수 있는 bool 값
  - 앞면 / 뒷면에 따라 다른 재질을 입히거나 할 때 유용

---

## GLSL Built-in variable

```glsl
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D frontTexture;
uniform sampler2D backTexture;

void main() {
  if(gl_FrontFacing)
    FragColor = texture(frontTexture, TexCoords);
  else
    FragColor = texture(backTexture, TexCoords);
}
```

---

## GLSL Built-in variable

<div>
<img src="/opengl_course/note/images/12_glsl_frontfacing.png" width="50%" />
</div>

---

## GLSL Built-in variable

- `gl_FragDepth`
  - fragment shader에서 픽셀의 깊이값을 직접 설정할 수 있게 해주는 변수
  - `gl_FragCoord`는 read-only
    - 기본: `gl_FragDepth = gl_FragCoord.z;`
  - 이 값을 사용하기 시작하면 early depth test가 비활성화 된다는 단점이 있음

---

## Advanced GLSL

- Interface block
  - vertex shader와 fragment shader 간의 link가 제대로 되려면
    둘 사이의 in/out 변수 연결이 제대로 이루어져야함
  - 이를 관리 하기 위한 `struct` 비슷한 변수 그룹
  - 구조체와 마찬가지로 `.` 연산자를 이용하여 변수 접근

```glsl
out VS_OUT {
  vec2 TexCoords;
} vs_out;
```

---

## Interface Block

```glsl [8-10, 14]
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out VS_OUT {
  vec2 TexCoords;
} vs_out;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
  vs_out.TexCoords = aTexCoords;
}
```

---

## Interface Block

- 블록 이름 `VS_OUT`은 동일해야 되지만 변수명은 달라도 상관 없음

```glsl [5-7, 11]
#version 330 core

out vec4 FragColor;

in VS_OUT {
  vec2 TexCoords;
} fs_in;
uniform sampler2D texture;

void main() {
  FragColor = texture(texture, fs_in.TexCoords);
}
```

---

## Advanced GLSL

- Uniform buffer object (UBO)
  - 여러 shader program들이 같은 uniform variable을 사용하는 경우가 많음
    - `model`, `view`, `projection` 등의 transform 등
  - 다른 program을 사용할때마다 uniform 변수를 새로 설정해주는 번거로움이 있음
  - UBO를 사용하면 모든 shader들이 같이 사용할 수 있는 global uniform
    설정 가능

---

## Uniform Buffer Object

- UBO 생성 방법
  - VBO, IBO와 마찬가지로 `glGenBuffers()` 함수로 생성
  - `GL_UNIFORM_BUFFER`를 타겟으로 바인딩
  - `glBufferData()`, `glBufferSubData()` 등으로 데이터 복사

---

## Uniform Buffer Object

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices {
  mat4 projection;
  mat4 view;
};

uniform mat4 model;

void main() {
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
```

---

## Uniform Buffer Object

- `layout (std140)`
  - 메모리 레이아웃 설정
  - 기본 레이아웃의 경우 최적화가 이루어짐
    - GPU 메모리의 접근 효율성을 위해 4/16바이트 단위로 정렬됨
  - `std140` 메모리 레이아웃의 경우 메모리 공간은 다소 많이
    사용하지만 예측가능한 레이아웃 제공

---

## Uniform Buffer Object

| **타입**     |  **레이아웃 규칙**  |
|----------|----------------|
| 스칼라값  |    4 byte      |
| vecN     |    16 byte     |
| 스칼라값 / vecN 의 배열 | 16 * (배열 개수) byte |
| matN     | 16 * N byte |
| 구조체    | 16 byte 크기로 정렬 |

---

## Uniform Buffer Object

```glsl
layout (std140) uniform ExampleBlock {
                   // base alignment // aligned offset
  float value;     // 4              // 0
  vec3 vector;     // 16             // 16 (multiple of 16: 4->16)
  mat4 matrix;     // 16             // 32 (column 0)
                   // 16             // 48 (column 1)
                   // 16             // 64 (column 2)
                   // 16             // 80 (column 3)
  float values[3]; // 16             // 96 (values[0])
                   // 16             // 112 (values[1])
                   // 16             // 128 (values[2])
  bool boolean;    // 4              // 144
  int integer;     // 4              // 148
};
```

---

## Uniform Buffer Object

- UBO 사용하기
  - 데이터 입력

```glsl
unsigned int uboExampleBlock;
glGenBuffers(1, &uboExampleBlock);
glBindBuffer(GL_UNIFORM_BUFFER, uboExampleBlock);
glBufferData(GL_UNIFORM_BUFFER, 152, NULL, GL_STATIC_DRAW); // 152 bytes
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

---

## Uniform Buffer Object

- UBO 사용하기
  - `programId`가 가진 `Lights` 유니폼 블럭이
    2번 UBO를 가져다 쓰도록 바인딩
  - `uboExampleBlock`을 2번 UBO로 바인딩

```cpp
auto lightIndex = glGetUniformBlockIndex(programId, "Lights");
glUniformBlockBinding(programId, lightIndex, 2);
glBindBufferBase(GL_UNIFORM_BUFFER, 2, uboExampleBlock);
```

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
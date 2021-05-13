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

## Advanced Data

- Additional note
  - `glTexSubImage2D()`
    - 이미 GPU 메모리가 할당된 2D texture에 새로운 CPU 이미지 데이터를
      업데이트하고 싶을때 사용하는 함수

```cpp
glBindBuffer(GL_TEXTURE_2D, texture);
glTexSubImage2D(GL_TEXTURE_2D, level,
  xoffset, yoffset, width, height,
  GL_RGBA, GL_UNSIGNED_BYTE, imageptr);
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

- Vertex shader와 Fragment shader 사이에 추가할 수 있는 shader
- 하나의 primitive를 입력받음
- 이를 다른 primitive로 출력할 수 있는 기능을 제공

---

## Geometry Shader

- point primitive를 line strip primitive로 바꾸는 예제

```glsl
#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 2) out;

void main() {
  gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
  EmitVertex();

  gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
  EmitVertex();

  EndPrimitive();
}
```

---

## Geometry Shader

- 입력 받고자 하는 primitive를 `layout () in`에 정의
- 출력할 primitive를 `layout () out`에 명시
- `EmitVertex()` 함수로 `gl_Position`에 설정된 정점을 생성함
- `EndPrimitive()`로 현재까지 생성한 정점들을 묶은 primitive 생성

---

## Geomoetry Shader

- 사용례 1: exploding object
  - 삼각형을 입력받아서 각 삼각형의 normal 방향으로 위치를 약간 움직이기

<div>
<img src="/opengl_course/note/images/12_geometry_shader_exploding_object.png" width="40%" />
</div>

---

## Geomoetry Shader

- 사용례 2: normal vector visualization
  - 삼각형의 normal 방향을 계산하여 line 형태로 primitive를 생성하여
    normal vector를 시각화

<div>
<img src="/opengl_course/note/images/12_geometry_shader_normal_vector.png" width="40%" />
</div>

---

## Instancing

- 동일한 오브젝트를 여러번 그려야 하는 경우
  - 수풀 그리기: 몇개의 vertex로 되어 있는 풀 object를 여러번 그리기
  - `glDrawArrays()`와 같은 함수를 여러번 호출하면 성능 저하가 발생

```cpp
for (unsigned int i = 0; i < amount_of_models_to_draw; i++) {
  DoSomePreparations(); // bind VAO, bind textures, set uniforms etc.
  glDrawArrays(GL_TRIANGLES, 0, amount_of_vertices);
}
```

---

## Instancing

- Instancing
  - 여러 오브젝트를 한번의 draw call로 그리도록 해주는 기능
  - CPU -> GPU 간 통신을 줄여서 성능 저하를 방지
  - `glDrawArraysInstanced()`, `glDrawElementsInstanced()`
    함수 사용
  - 파라미터로 인스턴스의 개수를 제공

---

## Instancing

- `gl_InstanceID`
  - vertex shader 내에서 제공하는 built-in variable
  - 서로 다른 instance를 구분짓는 용도로 사용

---

## Instancing

- uniform을 사용하여 instanced drawing 하는 방법

```glsl
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 fColor;
uniform vec2 offsets[100];

void main() {
  vec2 offset = offsets[gl_InstanceID];
  gl_Position = vec4(aPos + offset, 0.0, 1.0);
  fColor = aColor;
}
```

```cpp
glBindVertexArray(quadVAO);
glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
```

---

## Instancing

- 결과

<div>
<img src="/opengl_course/note/images/12_instanced_draw.png" width="60%" />
</div>

---

## Instancing

- VBO를 이용하여 instanced drawing 하는 방법

```cpp
unsigned int instanceVBO;
glGenBuffers(1, &instanceVBO);
glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100,
  &translations[0], GL_STATIC_DRAW);
glBindBuffer(GL_ARRAY_BUFFER, 0);

// vertex attribute 0, 1 setting ...
glEnableVertexAttribArray(2);
glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
glBindBuffer(GL_ARRAY_BUFFER, 0);
glVertexAttribDivisor(2, 1);
```

---

## Instancing

- `glVertexAttribDivisor(n, div)`
  - `n`번째 attribute를 `div`개의 instance가 등장할때마다 업데이트하도록 설정
  - `glVertexAttribDivisor(2, 1)`
    - 2번째 attribute (offset) 은 `gl_VertexID`가 아닌
      `gl_InstanceID`가 1 증가할 때마다 값이 달라짐

---

## Instancing

- VBO를 이용하여 instanced drawing 하는 방법
  - `aPos[gl_VertexID] + aOffset[gl_InstanceID]`

```glsl
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

out vec3 fColor;

void main() {
  gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
  fColor = aColor;
}
```

---

## Instancing

- 풀밭 렌더링
  - `image/grass.png`에 저장

<div>
<img src="/opengl_course/note/images/12_grass.png" width="40%" />
</div>

---

## Instancing

- `shader/grass.vs` 추가

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
out vec2 texCoord;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}
```

---

## Instancing

- `shader/grass.fs` 추가

```glsl
#version 330 core

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    vec4 pixel = texture(tex, texCoord);
    if (pixel.a < 0.05)
        discard;
    fragColor = pixel;
}
```

---

## Instancing

- `Context` 클래스에 멤버 변수 추가

```cpp
  TexturePtr m_grassTexture;
  ProgramUPtr m_grassProgram;
  std::vector<glm::vec3> m_grassPos;
```

---

## Instancing

- `Context::Init()` 함수에서 텍스처 / 프로그램 / 풀 위치 초기화
  - x/z 위치 및 y축 회전

```cpp
m_grassTexture = Texture::CreateFromImage(
  Image::Load("./image/grass.png").get());
m_grassProgram = Program::Create("./shader/grass.vs", "./shader/grass.fs");
m_grassPos.resize(10000);
for (size_t i = 0; i < m_grassPos.size(); i++) {
  m_grassPos[i].x = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 5.0f;
  m_grassPos[i].z = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 5.0f;
  m_grassPos[i].y = glm::radians((float)rand() / (float)RAND_MAX * 360.0f);
}
```

---

## Instancing

- `Context::Render()` 함수에서 일반적인 방식으로 그리기

```cpp
glEnable(GL_BLEND);
m_grassProgram->Use();
m_grassProgram->SetUniform("tex", 0);
m_grassTexture->Bind();
for (size_t i = 0; i < m_grassPos.size(); i++) {
  modelTransform = 
      glm::translate(glm::mat4(1.0f), glm::vec3(m_grassPos[i].x, 0.5f, m_grassPos[i].z)) *
      glm::rotate(glm::mat4(1.0f), m_grassPos[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
  transform = projection * view * modelTransform;
  m_grassProgram->SetUniform("transform", transform);
  m_plane->Draw(m_grassProgram.get());
}
```

---

## Instacing

- 빌드 및 결과
  - 매우 느린 렌더링 속도

<div>
<img src="/opengl_course/note/images/12_instancing_result.png" width="60%" />
</div>

---

## Instancing

- `shader/grass.vs` 수정

```glsl [5, 11-18]
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aOffset;
out vec2 texCoord;

uniform mat4 transform;

void main() {
    float c = cos(aOffset.y);
    float s = sin(aOffset.y);
    mat4 offsetMat = mat4(
        c, 0.0, -s, 0.0,
        0.0, 1.0, 0.0, 0.0,
        s, 0.0, c, 0.0,
        aOffset.x, 0.0, aOffset.z, 1.0);
    gl_Position = transform * offsetMat * vec4(aPos, 1.0);
    texCoord = aTexCoord;
}
```

---

## Instancing

- `Context` 클래스에 instancing을 위한 멤버 추가

```cpp
  BufferUPtr m_grassPosBuffer;
  VertexLayoutUPtr m_grassInstance;
```

---

## Instancing

- `Context::Init()`에서 instancing을 위한 VBO, VAO 초기화

```cpp [6-20]
for (size_t i = 0; i < m_grassPos.size(); i++) {
  m_grassPos[i].x = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 5.0f;
  m_grassPos[i].z = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 5.0f;
  m_grassPos[i].y = glm::radians((float)rand() / (float)RAND_MAX * 360.0f);
}
m_grassInstance = VertexLayout::Create();
m_grassInstance->Bind();
m_plane->GetVertexBuffer()->Bind();
m_grassInstance->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
m_grassInstance->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  offsetof(Vertex, normal));
m_grassInstance->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
  offsetof(Vertex, texCoord));

m_grassPosBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW,
    m_grassPos.data(), sizeof(glm::vec3), m_grassPos.size());
m_grassPosBuffer->Bind();
m_grassInstance->SetAttrib(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
glVertexAttribDivisor(3, 1);
m_plane->GetIndexBuffer()->Bind();
```

---

## Instancing

- `Context::Render()`에서 instancing을 이용한 draw call

```cpp
glEnable(GL_BLEND);
m_grassProgram->Use();
m_grassProgram->SetUniform("tex", 0);
m_grassTexture->Bind();
m_grassInstance->Bind();
modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
transform = projection * view * modelTransform;
m_grassProgram->SetUniform("transform", transform);
glDrawElementsInstanced(GL_TRIANGLES,
  m_plane->GetIndexBuffer()->GetCount(),
  GL_UNSIGNED_INT, 0,
  m_grassPosBuffer->GetCount());
```

---

## Instancing

- 빌드 및 결과
  - 동일한 결과, 매우 빠른 속도

<div>
<img src="/opengl_course/note/images/12_instancing_result.png" width="60%" />
</div>

---

## Anti-aliasing

- 앨리어싱 (aliasing) 현상
  - 렌더링된 오브젝트의 경계 부분에 도트로 된 계단이 눈에 띄는 현상

<div>
<img src="/opengl_course/note/images/12_aliasing.png" width="40%" />
</div>

---

## Anti-aliasing

- 앨리어싱 현상의 원인
  - 삼각형을 픽셀로 전환 (rasterization) 할 때 경계 부분을 채울지
    말지 두 경우만 있기 때문에 발생

<div>
<img src="/opengl_course/note/images/12_aliasing_rasterizer_before.png" height="25%" />
<img src="/opengl_course/note/images/12_aliasing_rasterizer_after.png" height="25%" />
</div>

---

## Anti-aliasing

- Anti-aliasing
  - 계단 현상을 없애는 기법을 총칭함
  - 예전의 기법: Supersampling Anti-Aliasing (SSAA)
    - 원래 해상도보다 더 큰 해상도로 렌더링
    - 렌더링된 영상을 작게 다운샘플링
    - 큰 해상도로 렌더링할때 매우 비용이 커지는 문제 발생
  - **Multisample Anti-Aliasing (MSAA)**
    - OpenGL에서 기본으로 제공하는 anti-aliasing 기법

---

## Anti-aliasing

- MSAA 아이디어
  - 한 픽셀이 삼각형 안에 들어갈지 말지를 픽셀의 중심 위치 하나가
    아닌, 여러 위치에 대해서 조사하자 (multi-sample)
  - 삼각형 안에 포함된 샘플의 개수에 따라 알파값을 조절하자

<div>
<img src="/opengl_course/note/images/12_aliasing_msaa_idea.png" height="30%" />
</div>

---

## Anti-aliasing

- MSAA 아이디어

<div>
<img src="/opengl_course/note/images/12_aliasing_rasterizer_msaa_before.png" height="25%" />
<img src="/opengl_course/note/images/12_aliasing_rasterizer_msaa_after.png" height="25%" />
</div>

---

## Anti-aliasing

- MSAA 적용하기
  - window 생성전 glfw에게 multisample 활성화 요청
```cpp
glfwWindowHint(GLFW_SAMPLES, 4);
```
  - `glEnable()` 함수로 MSAA 활성화
```cpp
glEnable(GL_MULTISAMPLE);
```

---

## Anti-aliasing

- post processing 관련 코드 주석 처리 후 실행

<div>
<img src="/opengl_course/note/images/12_aliasing_msaa_before.png" width="45%" />
<img src="/opengl_course/note/images/12_aliasing_msaa_after.png" width="45%" />
</div>

---

## Anti-aliasing

- post processing 관련 코드 주석 처리 후 실행

<div>
<img src="/opengl_course/note/images/12_aliasing_msaa_before_closeup.png" width="45%" />
<img src="/opengl_course/note/images/12_aliasing_msaa_after_closeup.png" width="45%" />
</div>

---

## Anti-aliasing

- Off-screen MSAA
  - 직접 생성한 framebuffer에 MSAA를 활성화하는 방법
  - texture target을 `GL_TEXTURE_2D_MULTISAMPLE`로 사용하기

```cpp
// creating texture for render target
glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA,
  width, height, GL_TRUE);
glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
// attaching texture to framebuffer
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
  GL_TEXTURE_2D_MULTISAMPLE, tex, 0);
```

---

## Anti-aliasing

- Off-screen MSAA
  - render buffer 설정시에도 multi sample을 위한 함수를 대신 사용

```cpp
glRenderbufferStorageMultisample(GL_RENDERBUFFER,
  4, GL_DEPTH24_STENCIL8, width, height);
```

---

## Anti-aliasing

- Off-screen MSAA
  - `GL_TEXTURE_2D`에 직접 그림을 그리는 형태가 아니므로 resolve 단계가 필요함
    - `GL_TEXTURE_2D_MULTISAMPLE` 텍스처의 픽셀을 `GL_TEXTURE_2D` 텍스처로
      옮기는 작업

---

## Anti-aliasing

- Off-screen MSAA

```cpp
unsigned int msFBO = CreateFBOWithMultiSampledAttachments();
// then create another FBO with a normal texture color attachment [...]
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
  GL_TEXTURE_2D, screenTexture, 0);
// [...]
while(!glfwWindowShouldClose(window)) {
  // [...]
  glBindFramebuffer(msFBO);
  ClearFrameBuffer();
  DrawScene();
  // now resolve multisampled buffer(s) into intermediate FBO
  glBindFramebuffer(GL_READ_FRAMEBUFFER, msFBO);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
  glBlitFramebuffer(0, 0, width, height,
    0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  // now scene is stored in 2D texture, use that for post-processing
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  ClearFramebuffer();
  glBindTexture(GL_TEXTURE_2D, screenTexture);
  DrawPostProcessingQuad();
  // [...]
}
```

---

## Congratulation!
### 수고하셨습니다!
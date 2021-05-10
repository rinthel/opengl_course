# Advanced Feature Part 1

---

## 예제 준비

- `object_example` 프로젝트를 복사하여 `advanced_feature_example` 프로젝트 생성
- 빌드 및 실행 확인

---

## 예제 준비

- scene 구성 준비를 위한 텍스처 저장
  - [marble.jpg](https://github.com/JoeyDeVries/LearnOpenGL/blob/master/resources/textures/marble.jpg?raw=true")
  - `image` 디렉토리에 저장

<div>
<img src="https://github.com/JoeyDeVries/LearnOpenGL/blob/master/resources/textures/marble.jpg?raw=true" width="40%"/>
</div>

---

## 예제 준비

- `Context`에서 model 로딩 / 드로잉 관련 코드 제거
- `model` 디렉토리 제거

---

## 예제 준비

- `Context` 클래스에 머티리얼 멤버 추가

```cpp [3-5]
  MeshUPtr m_box;

  MaterialPtr m_planeMaterial;
  MaterialPtr m_box1Material;
  MaterialPtr m_box2Material;
```

---

## 예제 준비

- `Context::Init()`에서 머티리얼 초기화

```cpp
TexturePtr darkGrayTexture = Texture::CreateFromImage(
  Image::CreateSingleColorImage(4, 4,
    glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());

TexturePtr grayTexture = Texture::CreateFromImage(
  Image::CreateSingleColorImage(4, 4,
    glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
        
m_planeMaterial = Material::Create();
m_planeMaterial->diffuse = Texture::CreateFromImage(
  Image::Load("./image/marble.jpg").get());
m_planeMaterial->specular = grayTexture;
m_planeMaterial->shininess = 128.0f;

m_box1Material = Material::Create();
m_box1Material->diffuse = Texture::CreateFromImage(
  Image::Load("./image/container.jpg").get());
m_box1Material->specular = darkGrayTexture;
m_box1Material->shininess = 16.0f;

m_box2Material = Material::Create();
m_box2Material->diffuse = Texture::CreateFromImage(
  Image::Load("./image/container2.png").get());
m_box2Material->specular = Texture::CreateFromImage(
  Image::Load("./image/container2_specular.png").get());
m_box2Material->shininess = 64.0f;
```

---

## 예제 준비

- `Context::Render()`에서 머티리얼과 박스를 이용하여 씬 구성

```cpp
auto modelTransform =
  glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) *
  glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
auto transform = projection * view * modelTransform;
m_program->SetUniform("transform", transform);
m_program->SetUniform("modelTransform", modelTransform);
m_planeMaterial->SetToProgram(m_program.get());
m_box->Draw(m_program.get());

modelTransform =
  glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
  glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
  glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
transform = projection * view * modelTransform;
m_program->SetUniform("transform", transform);
m_program->SetUniform("modelTransform", modelTransform);
m_box1Material->SetToProgram(m_program.get());
m_box->Draw(m_program.get());

modelTransform =
  glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
  glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
  glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
transform = projection * view * modelTransform;
m_program->SetUniform("transform", transform);
m_program->SetUniform("modelTransform", modelTransform);
m_box2Material->SetToProgram(m_program.get());
m_box->Draw(m_program.get());
```

---

## 예제 준비

- `Context` 클래스 내의 light 및 camera 초기 파라미터 변경

```cpp [3,5,6,16,19]
  // light parameter
  struct Light {
    glm::vec3 position { glm::vec3(1.0f, 4.0f, 4.0f) };
    glm::vec3 direction { glm::vec3(-1.0f, -1.0f, -1.0f) };
    glm::vec2 cutoff { glm::vec2(120.0f, 5.0f) };
    float distance { 128.0f };
    glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
    glm::vec3 diffuse { glm::vec3(0.8f, 0.8f, 0.8f) };
    glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
  };
  Light m_light;

  // camera parameter
  bool m_cameraControl { false };
  glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
  float m_cameraPitch { -20.0f };
  float m_cameraYaw { 0.0f };
  glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
  glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) };
  glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
```

---

## 예제 준비

- 빌드 및 실행

<div>
<img src="/opengl_course/note/images/11_scene_example.png" width="75%"/>
</div>

---

## Depth testing

- Depth buffer: 깊이값을 저장할 수 있는 버퍼
- Depth test
  - 그리려고 하는 픽셀의 깊이값과 버퍼에 저장된 깊이값을 비교
  - 비교에 성공하면 그림을 그리고, 아니면 그리지 않음

---

## Depth testing

- 깊이 테스트 사용하기

```cpp
glEnable(GL_DEPTH_TEST);
```

- 깊이 테스트 사용하지 않기

```cpp
glDisable(GL_DEPTH_TEST);
```

---

## Depth testing

- depth buffer의 업데이트 막기

```cpp
glDepthMask(GL_FALSE);
```

- depth buffer의 초기값 설정하기

```cpp
glClearDepth(1.0f);
```

---

## Depth testing

- depth test 비교 연산자 변경하기

```cpp
glDepthFunc(GL_LESS);
```

- 사용 가능한 비교 연산자
  - `GL_ALWAYS`, `GL_NEVER`
  - `GL_LESS`, `GL_LEQUAL`
  - `GL_GREATER`, `GL_GEQUAL`
  - `GL_EQUAL`, `GL_NOTEQUAL`

---

## Depth testing

- `glDisable(GL_DEPTH_TEST)` 를 예제에 추가했을 경우
  - 순차적으로 삼각형을 그려나가면서 이전에 그려진 삼각형을 덮어씌움
  - `glEnable(GL_DEPTH_TEST); glDepthFunc(GL_ALWAYS);` 로 변경해도 동일한 결과 확인

---

## Depth testing

<div>
<img src="/opengl_course/note/images/11_depth_test_off.png" width="80%"/>
</div>

---

## Depth visualization

- `shader/light.fs`의 픽셀값 설정 코드를 수정

```glsl
fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
```

---

## Depth visualization

- `Context::Render()`에서 투영행렬의 near 값 조절

```cpp [2]
auto projection = glm::perspective(glm::radians(45.0f),
  (float)m_width / (float)m_height, 0.5f, 20.0f);
```

---

## Depth visualization

- 빌드 및 결과

<div>
<img src="/opengl_course/note/images/11_depth_visualization.png" width="70%"/>
</div>

---

## Depth visualization

- 관측 결과
  - 가까운곳은 어두운색이지만, 멀어질수록 급격하게 밝아진다
  - 먼곳과 중간지점과 밝기 차이가 그다지 크지 않다

---

## Depth visualization

- 깊이 값의 왜곡
  - perspective projection을 적용하면 깊이값을 0~1 사이로 정규화하면서 w값으로 나누는 과정을 거침
  - 정규화된 z값은 1/z 꼴의 함수 형태로 분포가 나타남

<div>
<img src="/opengl_course/note/images/11_depth_value_graph.png" width="40%"/>
</div>

---

## Depth visualization

- 깊이 값의 왜곡
  - 멀리 있는 픽셀 간에 z값의 오차가 크지 않아서 문제가 발생할 수 있음

---

## Z-fighting

- `shader/light.fs` 코드 원복
- `Context::Render()`에서 투영행렬의 near / far 조절

```cpp [2]
auto projection = glm::perspective(glm::radians(45.0f),
  (float)m_width / (float)m_height, 0.01f, 1000.0f);
```

- `Context::Render()`에서 두번째 박스의 위치 조절

```cpp
modelTransform =
  glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.749f, 2.0f)) *
  glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
  glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
```

---

## Z-fighting

- 결과
  - 원거리에서 박스와 지면의 z값 차이가 얼마 나지 않아서 잘못된 렌더링 발생

<div>
<img src="/opengl_course/note/images/11_z_fighting.png" width="60%"/>
</div>

---

## Z-fighting

- 방지법
  - 면과 면을 너무 붙어있게 하지 않을 것
  - near의 값을 너무 작게 하지 말것
  - 좀더 정확한 depth buffer를 설정하여 사용할 것

---

## Stencil buffer

- color / depth 외에 특정 픽셀에만 그림을 그리도록 하는 용도의 8-bit 정수형 버퍼

<div>
<img src="/opengl_course/note/images/11_stencil_test_example.png" width="60%"/>
</div>

---

## Stencil test

- depth test 이전에 stencil test를 진행
- 사용 방법은 depth buffer와 유사함
- 매우 다양한 연산 제공 및 다양한 용도로 활용

```cpp
glEnable(GL_STENCIL_TEST); // 스텐실 테스트 활성화
glStencilMask(0xFF); // 업데이트 되는 스텐실 버퍼의 비트 설정
glStencilFunc(GL_EQUAL, 1, 0xFF); // 스텐실 테스트 함수 설정
```

---

## Stencil operation

```cpp
glStencilOp(sfail, dpfail, dppass)
```

- `sfail`: stencil test가 실패했을 경우 동작
- `dpfail`: stencil test는 통과했지만 depth test가 실패했을 경우 동작
- `dppass`: stencil / depth test 모두 통과했을 때 동작

---

## Stencil operation

- 가능한 동작
  - `GL_KEEP`: 현재의 스텐실 버퍼값 유지
  - `GL_ZERO`: 스텐실 버퍼를 0으로 변경
  - `GL_REPLACE`: `glStencilFunc`을 통해 설정된 값으로 변경
  - `GL_INVERT`: 스텐실 버퍼값을 bitwise invert 시킴. `0x0f => 0xf0`

---

## Stencil operation

- 가능한 동작
  - `GL_INCR`: 스텐실 버퍼값을 1 증가시킴. 최대값인 경우 그대로 유지
  - `GL_INCR_WRAP`: 스텐실 버퍼값을 1 증가시킴. 최대값인 경우 0으로 돌아감
  - `GL_DECR`: 스텐실 버퍼값을 1 감소시킴. 최소값인 경우 그대로 유지
  - `GL_DECR_WRAP`: 스텐실 버퍼값을 1 감소시킴. 최소값인 경우 255로 돌아감

---

## Object outlining

- 스텐실 버퍼를 이용한 오브젝트 외곽선 그리기
  - stencil buffer를 0으로 클리어
  - stencil test를 GL_ALWAYS로 설정하고 그려진 부분의 stencil value를 1로 설정
  - 오브젝트 렌더링

---

## Object outlining

- 스텐실 버퍼를 이용한 오브젝트 외곽선 그리기
  - depth test 비활성화 / stencil update 비활성화
  - 오브젝트의 크기를 약간 더 키우고 외곽선을 그릴 shader로 설정
  - 오브젝트 렌더링, stencil value가 1이 아닌 픽셀에 대해서만 그리도록 설정
  - depth test 활성화, stencil function 은 다시 `GL_KEEP`으로 변경

---

## Object outlining

- `Context::Render()`의 두번째 상자 그리는 부분의 코드 변경

```cpp
glEnable(GL_STENCIL_TEST);
glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
glStencilFunc(GL_ALWAYS, 1, 0xFF);
glStencilMask(0xFF);

modelTransform =
  glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
  glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
  glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
transform = projection * view * modelTransform;
m_program->SetUniform("transform", transform);
m_program->SetUniform("modelTransform", modelTransform);
m_box2Material->SetToProgram(m_program.get());
m_box->Draw(m_program.get());

glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
glStencilMask(0x00);
glDisable(GL_DEPTH_TEST);
m_simpleProgram->Use();
m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));
m_simpleProgram->SetUniform("transform", transform *
  glm::scale(glm::mat4(1.0f), glm::vec3(1.05f, 1.05f, 1.05f)));
m_box->Draw(m_simpleProgram.get());

glEnable(GL_DEPTH_TEST);
glDisable(GL_STENCIL_TEST);
glStencilFunc(GL_ALWAYS, 1, 0xFF);
glStencilMask(0xFF);
```

---

## Object outlining

- 빌드 및 결과

<div>
<img src="/opengl_course/note/images/11_stencil_outline.png" width="75%"/>
</div>

---

## Blending

- 그리려고 하는 픽셀과 프레임버퍼에 저장된 픽셀 간의 연산
- 대부분의 경우 반투명한 오브젝트를 그리는 경우에 사용함

<div>
<img src="/opengl_course/note/images/11_blend_example.png" width="50%"/>
</div>

---

## Blending

- 블랜딩 활성화

```cpp
glEnable(GL_BLEND);
```

- 블랜딩 함수

```cpp
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

---

## Blending

- 블랜딩 수식

<div>
<img src="/opengl_course/note/images/11_blending_equation.png" width="50%"/>
</div>

- `glBlendFunc`으로 `F` 값을 설정할 수 있음
- `glBlendEquation`으로 가운데 연산자 설정 가능

---

## Blending

<div>
<img src="/opengl_course/note/images/11_blending_example_init.png" width="30%"/>
<img src="/opengl_course/note/images/11_blending_example_eq.png" width="30%"/>
<img src="/opengl_course/note/images/11_blending_example_result.png" width="20%"/>
</div>

---

## Blending

- `glBlendFunc`에서 사용 가능한 인자
  - `GL_ZERO`, `GL_ONE`
  - `GL_SRC_COLOR`, `GL_SRC_ALPHA`
  - `GL_ONE_MINUS_SRC_COLOR`, `GL_ONE_MINUS_SRC_ALPHA`
  - `GL_DST_COLOR`, `GL_DST_ALPHA`
  - `GL_ONE_MINUS_DST_COLOR`, `GL_ONE_MINUS_DST_ALPHA`
  - `GL_CONSTANT_COLOR`, `GL_CONSTANT_ALPHA`
  - `GL_ONE_MINUS_CONSTANT_COLOR`, `GL_ONE_MINUS_CONSTANT_ALPHA`

---

## Blending

- `glBlendFuncSeparate` 함수를 이용하여 color / alpha 별로 별도의 수식 적용 가능

```cpp
glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
```

---

## Blending

- `glBlendEquation` 에서 사용 가능한 인자
  - `GL_FUNC_ADD`: `src + dst`
  - `GL_FUNC_SUBTRACT`: `src - dst`
  - `GL_FUNC_REVERSE_SUBTRACT`: `dst - src`
  - `GL_MIN`: `min(src, dst)`
  - `GL_MAX`: `max(src, dst)`

---

## Blending

- 예제에 사용될 반투명 텍스처 다운로드
  - [blending_transparent_window.png](https://learnopengl.com/img/advanced/blending_transparent_window.png)

<div>
<img src="https://learnopengl.com/img/advanced/blending_transparent_window.png" width="30%"/>
</div>

---

## Blending

- `shader/texture.fs` 수정

```glsl
#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    fragColor = texture(tex, texCoord);
}
```

---

## Blending

- `Context`에 `m_textureProgram` 멤버 추가

```cpp [4]
  bool Init();
  ProgramUPtr m_program;
  ProgramUPtr m_simpleProgram;
  ProgramUPtr m_textureProgram;
```

---

## Blending

- `Context::Init()`에서 `m_textureProgram` 초기화

```cpp
  m_textureProgram = Program::Create("./shader/texture.vs", "./shader/texture.fs");
  if (!m_textureProgram)
      return false;
```

---

## Blending

- `Mesh::CreatePlane()` 함수 추가

```cpp [9]
CLASS_PTR(Mesh);
class Mesh {
public:
  static MeshUPtr Create(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    uint32_t primitiveType);
  static MeshUPtr CreateBox();
  static MeshUPtr CreatePlane();
```

---

## Blending

- `Mesh::CreatePlane()` 함수 구현

```cpp
MeshUPtr Mesh::CreatePlane() {
  std::vector<Vertex> vertices = {
    Vertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
    Vertex { glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
    Vertex { glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
    Vertex { glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
  };

  std::vector<uint32_t> indices = {
    0,  1,  2,  2,  3,  0,
  };

  return Create(vertices, indices, GL_TRIANGLES);
}
```

---

## Blending

- `Context`에 `m_plane` 멤버 및 `m_windowTexture` 추가

```cpp [2,7]
  MeshUPtr m_box;
  MeshUPtr m_plane;

  MaterialPtr m_planeMaterial;
  MaterialPtr m_box1Material;
  MaterialPtr m_box2Material;
  TexturePtr m_windowTexture;
```

---

## Blending

- `Context::Init()`에서 `m_plane`, `m_windowTexture` 초기화

```cpp
m_plane = Mesh::CreatePlane();
```

```cpp
m_windowTexture = Texture::CreateFromImage(
  Image::Load("./image/blending_transparent_window.png").get());
```

---

## Blending

- `Context::Render()`에서 `m_plane`, `m_windowTexture`, `m_textureProgram` 으로 드로잉

```cpp
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_textureProgram->Use();
  m_windowTexture->Bind();
  m_textureProgram->SetUniform("tex", 0);

  modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 4.0f));
  transform = projection * view * modelTransform;
  m_textureProgram->SetUniform("transform", transform);
  m_plane->Draw(m_textureProgram.get());
```

---

## Blending

- 빌드 및 결과
  - 반투명한 창문 표현 확인

<div>
<img src="/opengl_course/note/images/11_one_window_blend.png" width="60%"/>
</div>

---

## Blending

- 창문을 여러 개로 늘려보자

```cpp
  modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.5f, 5.0f));
  transform = projection * view * modelTransform;
  m_textureProgram->SetUniform("transform", transform);
  m_plane->Draw(m_textureProgram.get());

  modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.5f, 6.0f));
  transform = projection * view * modelTransform;
  m_textureProgram->SetUniform("transform", transform);
  m_plane->Draw(m_textureProgram.get());
```

---

## Blending

- 빌드 및 결과
  - 정면에서 보면 정확한 결과물

<div>
<img src="/opengl_course/note/images/11_multiple_blend_good.png" width="60%"/>
</div>

---

## Blending

- 빌드 및 결과
  - 뒤에서 보면 잘못된 결과가 발생

<div>
<img src="/opengl_course/note/images/11_multiple_blend_bad.png" width="60%"/>
</div>

---

## Blending

- 잘못된 결과가 발생하는 원인
  - 카메라 앞의 유리창을 먼저 그리는 경우 이 유리창이 depth buffer 값을 갱신
  - 뒤의 유리창은 **depth test를 통과하지 못하고 그려지지 않음**

---

## Blending

- fragment discard
  - fragment shader에서 discard를 호출하면 해당 픽셀을 그리지 않을 수 있음

---

## Blending

- `shader/texture.fs`를 다음과 같이 수정

```glsl
#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    vec4 pixel = texture(tex, texCoord);
    if (pixel.a < 0.01)
        discard;
    fragColor = pixel;
}
```

---

## Blending

- 빌드 및 결과
  - 투명하게 나와야 되는 창틀 구석은 해결
  - 반투명한 유리 부분은 여전히 문제점

<div>
<img src="/opengl_course/note/images/11_multiple_blend_discard.png" width="60%"/>
</div>

---

## Blending

- 해결책
  - 그리기 전 보는 방향을 기준으로 반투명한 물체를 sort하기
  - [order-independency transparency](https://learnopengl.com/Guest-Articles/2020/OIT/Introduction)

---

## Face culling

- 삼각형을 이루는 점의 순서에 따라 앞면/뒷면을 결정
  - 일반적으로 반시계방향(CCW)을 앞면으로 취급

<div>
<img src="/opengl_course/note/images/11_winding_order.png" width="50%"/>
</div>

---

## Face culling

- Face culling
  - 앞면/뒷면을 그리지 않도록 추려내는 작업
    - 뒷면은 그리지 않는다

<div>
<img src="/opengl_course/note/images/11_face_culling.png" width="50%"/>
</div>

---

## Face culling

```cpp
glEnable(GL_CULL_FACE); // face culling 활성화
glDisable(GL_CULL_FACE); // face culling 비활성화
glFrontFace(GL_CCW); // CCW 방향의 면을 앞면으로 설정
glCullFace(GL_BACK); // 뒷면을 그리지 않기
glCullFace(GL_FRONT); // 앞면을 그리지 않기
```

---

## Face culling

- `glCullFace(GL_FRONT)`로 앞면을 그리지 않았을 경우

<div>
<img src="/opengl_course/note/images/11_face_cull_front.png" width="60%"/>
</div>

---

## Framebuffer

- 프레임버퍼
  - OpenGL 함수를 통해 그려질 색상/깊이/스텐실 버퍼의 집합체
  - GLFW를 통해 OpenGL context를 생성하면 화면에 그림을 그리기 위한
    프레임버퍼가 자동 생성됨
  - 개발자가 직접 프레임버퍼를 생성하고 활용할 수 있음
    - **렌더링된 장면을 텍스처로 활용하는 경우**
    - **포스트 프로세싱**

---

## Framebuffer

- 프레임버퍼 사용 과정
  - 프레임버퍼 객체 생성
  - 렌더버퍼 (color / depth / stencil) 생성
  - 렌더버퍼를 프레임버퍼에 연결
  - 해당 렌더버퍼의 조합이 프레임버퍼로 사용 가능한지 확인

---

## Framebuffer

- `Texture` 클래스 수정
  - `width`, `height`, `format`을 내부적으로 갖고 있도록 함
  - 이미지 로딩 외에 비어있는 텍스처 생성 함수를 추가

---

## Framebuffer

```cpp [9,18-20,29-31]
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture {
public:
  static TextureUPtr Create(int width, int height, uint32_t format);
  static TextureUPtr CreateFromImage(const Image* image);
  ~Texture();

  const uint32_t Get() const { return m_texture; }
  void Bind() const;
  void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
  void SetWrap(uint32_t sWrap, uint32_t tWrap) const;

  int GetWidth() const { return m_width; }
  int GetHeight() const { return m_height; }
  uint32_t GetFormat() const { return m_format; }

private:
  Texture() {}
  void CreateTexture();
  void SetTextureFromImage(const Image* image);
  void SetTextureFormat(int width, int height, uint32_t format);

  uint32_t m_texture { 0 };
  int m_width { 0 };
  int m_height { 0 };
  uint32_t m_format { GL_RGBA };
};

#endif // __TEXTURE_H__
```

---

## Framebuffer

- `Texture::Create()` 구현

```cpp
TextureUPtr Texture::Create(int width, int height, uint32_t format) {
  auto texture = TextureUPtr(new Texture());
  texture->CreateTexture();
  texture->SetTextureFormat(width, height, format);
  texture->SetFilter(GL_LINEAR, GL_LINEAR);
  return std::move(texture);
}
```

---

## Framebuffer

- `Texture::SetTextureFromImage()` 구현 수정

```cpp [10-17]
void Texture::SetTextureFromImage(const Image* image) {
  GLenum format = GL_RGBA;
  switch (image->GetChannelCount()) {
    default: break;
    case 1: format = GL_RED; break;
    case 2: format = GL_RG; break;
    case 3: format = GL_RGB; break;
  }

  m_width = image->GetWidth();
  m_height = image->GetHeight();
  m_format = format;
  
  glTexImage2D(GL_TEXTURE_2D, 0, m_format,
    m_width, m_height, 0,
    format, GL_UNSIGNED_BYTE,
    image->GetData());

  glGenerateMipmap(GL_TEXTURE_2D);
}
```

---

## Framebuffer

- `Texture::SetTextureFormat()` 구현

```cpp
void Texture::SetTextureFormat(int width, int height, uint32_t format) {
  m_width = width;
  m_height = height;
  m_format = format;

  glTexImage2D(GL_TEXTURE_2D, 0, m_format,
    m_width, m_height, 0,
    m_format, GL_UNSIGNED_BYTE,
    nullptr);
}
```

---

## Framebuffer

- `src/framebuffer.h` 생성후 `Framebuffer` 클래스 설계

```cpp
#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "texture.h"

CLASS_PTR(Framebuffer);
class Framebuffer {
public:
    static FramebufferUPtr Create(const TexturePtr colorAttachment);
    static void BindToDefault();
    ~Framebuffer();

    const uint32_t Get() const { return m_framebuffer; }
    void Bind() const;
    const TexturePtr GetColorAttachment() const { return m_colorAttachment; }

private:
    Framebuffer() {}
    bool InitWithColorAttachment(const TexturePtr colorAttachment);

    uint32_t m_framebuffer { 0 };
    uint32_t m_depthStencilBuffer { 0 };
    TexturePtr m_colorAttachment;
};

#endif // __FRAMEBUFFER_H__
```

---

## Framebuffer

- 제일 단순한 형태의 프레임버퍼 설계
  - 렌더링이 될 색상 버퍼에 텍스처를 설정할 수 있음
  - 나머지 깊이 / 스텐실 버퍼는 렌더버퍼를 사용

---

## Framebuffer

- `src/framebuffer.cpp` 생성후 구현

```cpp
#include "framebuffer.h"

FramebufferUPtr Framebuffer::Create(const TexturePtr colorAttachment) {
  auto framebuffer = FramebufferUPtr(new Framebuffer());
  if (!framebuffer->InitWithColorAttachment(colorAttachment))
    return nullptr;
  return std::move(framebuffer);
}

Framebuffer::~Framebuffer() {
  if (m_depthStencilBuffer) {
    glDeleteRenderbuffers(1, &m_depthStencilBuffer);
  }
  if (m_framebuffer) {
    glDeleteFramebuffers(1, &m_framebuffer);
  }
}

void Framebuffer::BindToDefault() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachment(const TexturePtr colorAttachment) {
  m_colorAttachment = colorAttachment;
  glGenFramebuffers(1, &m_framebuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  glFramebufferTexture2D(GL_FRAMEBUFFER,
    GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
    colorAttachment->Get(), 0);

  glGenRenderbuffers(1, &m_depthStencilBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
  glRenderbufferStorage(
    GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
    colorAttachment->GetWidth(), colorAttachment->GetHeight());
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glFramebufferRenderbuffer(
    GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
    GL_RENDERBUFFER, m_depthStencilBuffer);

  auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result != GL_FRAMEBUFFER_COMPLETE) {
    SPDLOG_ERROR("failed to create framebuffer: {}", result);
    return false;
  }

  BindToDefault();

  return true;
}
```

---

## Framebuffer

- `Context` 클래스 내에 `m_framebuffer` 멤버 추가

```cpp [5-6]
  glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
  glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) };
  glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };

  // framebuffer
  FramebufferUPtr m_framebuffer;
```

---

## Framebuffer

- `Context::Reshape()`에서 화면 크기와 동일한 프레임버퍼 생성

```cpp [6-7]
void Context::Reshape(int width, int height) {
  m_width = width;
  m_height = height;
  glViewport(0, 0, m_width, m_height);

  m_framebuffer = Framebuffer::Create(
    Texture::Create(width, height, GL_RGBA));
}
```

---

## Framebuffer

- `Context::Render()`에서 장면을 그리기 전 생성한 프레임버퍼를 바인딩

```cpp [3]
  // ... imgui codes

  m_framebuffer->Bind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  m_cameraFront =
    glm::rotate(glm::mat4(1.0f),
      glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
  // ...
```

---

## Framebuffer

- 장면 그리기가 끝났을 때 기본 프레임버퍼 (화면) 으로 바인딩을 변경함
- 앞에서 렌더링한 장면이 저장된 텍스처를 가지고 화면을 꽉 채우는 사각형을 그림

```cpp
  // ...
  Framebuffer::BindToDefault();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  m_textureProgram->Use();
  m_textureProgram->SetUniform("transform",
      glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));
  m_framebuffer->GetColorAttachment()->Bind();
  m_textureProgram->SetUniform("tex", 0);
  m_plane->Draw(m_textureProgram.get());    
```

---

## Framebuffer

- 빌드 및 결과
  - 육안으로 달라진점은 없음
  - 대신 화면에 직접 그리는 형태가 아닌, 텍스처에 렌더링 후 이 텍스처를
    가지고 다시 그림을 그리는 2단계의 구조를 완성하였음

---

## Post-processing

- 렌더링된 텍스처에 추가 효과를 넣어보자

---

## Post-processing

- `shader/invert.fs` 추가

```glsl
#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    vec4 pixel = texture(tex, texCoord);
    fragColor = vec4(1.0 - pixel.rgb, 1.0);
}
```

---

## Post-processing

- `Context`에 `m_postProgram` 멤버 추가

```cpp [4]
  ProgramUPtr m_program;
  ProgramUPtr m_simpleProgram;
  ProgramUPtr m_textureProgram;
  ProgramUPtr m_postProgram;
```

---

## Post-processing

- `Context::Init()`에서 `m_postProgram` 초기화

```cpp
  m_postProgram = Program::Create("./shader/texture.vs",
    "./shader/invert.fs");
  if (!m_postProgram)
    return false;
```

---

## Post-processing

- `Context::Render()`에서 프레임버퍼 텍스처로 그리는 프로그램을 `m_postProgram`으로 변경

```cpp
  Framebuffer::BindToDefault();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  m_postProgram->Use();
  m_postProgram->SetUniform("transform",
    glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));
  m_framebuffer->GetColorAttachment()->Bind();
  m_postProgram->SetUniform("tex", 0);
  m_plane->Draw(m_postProgram.get());
```

---

## Post-processing

- 빌드 및 결과: 반전된 렌더링

<div>
<img src="/opengl_course/note/images/11_post_process_invert.png" width="60%"/>
</div>

---

## Post-processing

- `shader/gamma.fs` 추가

```cpp
#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform float gamma;

void main() {
  vec4 pixel = texture(tex, texCoord);
  fragColor = vec4(pow(pixel.rgb, vec3(gamma)), 1.0);
}
```

---

## Post-processing

- `Context`에 `m_gamma` 멤버 추가

```cpp [5]
  ProgramUPtr m_program;
  ProgramUPtr m_simpleProgram;
  ProgramUPtr m_textureProgram;
  ProgramUPtr m_postProgram;
  float m_gamma {1.0f};
```

---

## Post-processing

- `Context::Init()`에서 `m_postProgram`이 사용할
  fragment shader를 `gamma.fs`로 교체

```cpp [2]
  m_postProgram = Program::Create("./shader/texture.vs",
    "./shader/gamma.fs");
  if (!m_postProgram)
      return false;
```

---

## Post-processing

- `Context::Render()`에서
  - `m_gamma`를 조정할 수 있는 UI를 추가
  - `m_gamma`를 uniform으로 세팅

```cpp [4]
  if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) {
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
  }
  ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
```

```cpp [3]
  m_postProgram->SetUniform("transform",
    glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));
  m_postProgram->SetUniform("gamma", m_gamma);
  m_framebuffer->GetColorAttachment()->Bind();
```

---

## Post-processing

- 빌드 및 결과
  - gamma correction

<div>
<img src="/opengl_course/note/images/11_post_process_gamma.png" width="60%"/>
</div>

---

## Congratulation!
### 수고하셨습니다!
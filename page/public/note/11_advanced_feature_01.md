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
<img src="/opengl_course/note/images/11_scene_example.png" width="75%">
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
<img src="/opengl_course/note/images/11_depth_test_off.png" width="80%">
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
<img src="/opengl_course/note/images/11_depth_visualization.png" width="70%">
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
<img src="/opengl_course/note/images/11_depth_value_graph.png" width="40%">
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
<img src="/opengl_course/note/images/11_z_fighting.png" width="60%">
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
<img src="/opengl_course/note/images/11_stencil_test_example.png" width="60%">
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
<img src="/opengl_course/note/images/11_stencil_outline.png" width="75%">
</div>

---

## Blending

- fragment discard
- blending
- rendering semi-transparent
- order independent alpha

---

## Face culling

- winding order: CW or CCW
- face culling

---

## Framebuffer

- creating framebuffer
- texture attachment
- render to texture
- post-processing

---

## Cubemap

- creation
- skybox
- environment map

---

## Congratulation!
### 수고하셨습니다!
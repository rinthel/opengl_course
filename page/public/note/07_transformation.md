# Transformation

---

## 개요

- 화면에서 오브젝트를 임의의 위치에 그려낼 방법
  - vertex buffer object의 vertex position 값을 매번 수정한다
    - 가능하지만, 비효율적임
  - 선형 변환 (linear transformation) 을 이용한다 
    - 가장 기초적이고 일반적인 방법
    - 기본적인 선형 대수에 대한 이해가 필요

---

## Vector

- 크기와 방향을 갖는 값
- 좌표계 상의 벡터: 여러 차원의 좌표값으로 표현
- 덧셈/뺄셈: 각 차원의 원소 별로 덧셈/뺄셈 계산
- 스칼라곱: 각 차원에 스칼라 값을 곱함

![vector add](/opengl_course/note/images/07_vector_add.png)

---

## Vector Length

- 벡터의 크기 (길이) 계산
  - 각 좌표값의 제곱의 합의 제곱근
  - (4, 2) 벡터의 길이
- 단위 벡터 (unit vector)
  - 길이가 1인 벡터

![vector length](/opengl_course/note/images/07_vector_length.png)

---

## Vector Multiplication

- 백터의 내적 (Inner product)
  - Dot product 라고도 함
  - 두 벡터의 길이의 곱 * `cos(두 벡터의 사잇각)`
  - 각 차원별 좌표값의 곱의 합
  - **곱의 결과는 스칼라 값**
  - **기하학적 의미: 투영 (projection)**

![vector dot product](/opengl_course/note/images/07_vector_dot_product.png)

---

## Vector Multiplication

- 백터의 외적 (Cross product)
  - 3차원 벡터에서만 정의
  - **곱의 결과도 3차원 벡터**
  - 기하학적 의미
    - 길이: 두 벡터의 길이의 곱 * `sin(두 벡터의 사잇각)`
    - 즉, 두 벡터가 만들어내는 평행사변형의 넓이
    - 방향: **두 벡터와 수직한 벡터**
  - **교환 법칙이 성립하지 않음**
    - anti-symmetric: A x B = -B x A

---

## Vector Multiplication

- 내적의 응용법
  - 사잇각 구하기: theta = acos(v * w / (|v|*|w|))
  - 두 벡터가 수직인 경우 내적의 값은 0

- 외적의 주요 응용법
  - 두 3차원 벡터와 수직하는 벡터 구하기
  - Orthogonalization

---

## Vector Multiplication

![vector cross product figure](/opengl_course/note/images/07_vector_cross_product_figure.png)

![vector cross product](/opengl_course/note/images/07_vector_cross_product.png)

---

## Matrix

- 행렬
- 행(row)과 열(column)로 이뤄진 숫자 묶음으로 구성된 값
- 덧셈/뺄셈: 같은 행과 열에 위치한 원소별로 연산
- 스칼라곱: 스칼라값을 모든 원소에 곱셈 연산

![matrix add](/opengl_course/note/images/07_matrix_add.png)
![matrix scalar](/opengl_course/note/images/07_matrix_scalar.png)

---

## Matrix Multiplication

- A x B
  - A의 column 개수와 B의 row 개수가 일치해야 계산 가능
  - (i, j)의 값: A의 i번째 행 벡터 (row vector) 와
    B의 j번째 열 벡터 (column vector) 간의 내적
  - **교환 법칙 성립 안됨**

![matrix multiplication](/opengl_course/note/images/07_matrix_multiplication.png)

---

## Linear Transformation

- 선형 변환
  - 선형: f(ax + by) = a f(x) + b f(y)
  - 행렬식에서 벡터의 선형 변환
    - f(x) = A x
    - 즉, 선형 변환을 나타내는 행렬과 벡터 간의 곱
  - **여러 개의 선형 변환 적용은 결국 하나의 선형 변환으로 바꿀 수 있다**
    - f(x) = A x
    - g(x) = B x
    - f(g(x)) = A (B x) = C x

---

## Homogeneous Coordinates

- 동차 좌표계
- 표현하고자 하는 차원수보다 1차원 늘어난 벡터로 좌표를 표시하는 방법
  - 마지막 원소 값을 1로 두면 **점**을 표현
  - 3D 좌표 (x, y, z)는 4차원 벡터 (x, y, z, 1)
  - 동차 좌표계에서 (wx, wy, wz, w)는 (x, y, z, 1)과 같은 위치
- 이렇게 하는 이유
  - 평행 이동을 선형 변환으로 표현할 수 있게 됨
  - 원근 투영을 선형 변환으로 표현할 수 있게 됨

---

## Identity Matrix

- 단위 행렬
- 행렬 곱셈의 항등원
  - A x I = I x A = A
- 대각선의 원소값은 1, 나머지는 0
- 출력 벡터는 입력 벡터와 동일해진다

![identity matrix](/opengl_course/note/images/07_identity_matrix.png)

---

## Scaling

- 원점을 기준으로 벡터의 크기를 확대하거나 축소
- 대각 성분에 각 차원별 배율을 지정한 행렬로 표현

![scaling](/opengl_course/note/images/07_scaling.png)

---

## Translation

- 평행이동 행렬
- 4번째 열 벡터에 평행이동 벡터를 가진 행렬로 표현

![translation](/opengl_course/note/images/07_translation.png)

---

## Rotation

- x축에 대한 회전

![rotation x](/opengl_course/note/images/07_rotation_x.png)

---

## Rotation

- y축에 대한 회전

![rotation y](/opengl_course/note/images/07_rotation_y.png)

---

## Rotation

- z축에 대한 회전

![rotation z](/opengl_course/note/images/07_rotation_z.png)

---

## Rotation

- 임의의 축 (Rx, Ry, Rz)에 대한 회전

![rotation axis](/opengl_course/note/images/07_rotation_axis.png)

---

## Orthogonal Matrix

- 다음의 조건을 만족하는 행렬
  - 각 column vector (혹은 row vector) 의 길이가 1
  - column vector 간의 내적값이 0 (수직)

- Orthogonal Matrix
  - inv(A) = transpose(A)

- Rotation matrix는 Orthogonal matrix

---

## Combine Matrices

- 주어진 벡터를 2배로 확대한 다음 (1, 2, 3)만큼 평행이동 시키는 행렬

![combine_matrices](/opengl_course/note/images/07_combine_matrices.png)

- 선형 변환은 하나의 행렬로 나타내지므로, 여러 선형 변환을 연속으로
  적용시키는 것은 행렬 곱으로 만들어진 하나의 선형 변환으로 나타낼 수 있다

---

## C++ 행렬 / 벡터 연산

- GLSL의 경우 내부적으로 행렬 및 벡터과 관련된 다양한 기능 및 내부 함수 제공
- C++에는 기본적인 수학적 연산 외에 선형대수 관련 기능은 제공하지 않음
  - 라이브러리를 이용하자

---

## C++ 행렬 / 벡터 연산

- C++ 선형대수 라이브러리
  - Eigen3: 가장 많이 사용되는 C++ 선형대수 라이브러리
    - 일반적인 N차원 선형대수 연산
  - **GLM**: OpenGL Math 라이브러리
    - 3D 그래픽스에 필요한 4차원 선형대수 연산

---

## 예제 준비

- `texture_example` 프로젝트를 복사하여 `transformation_example` 준비
- 프로젝트명 변경 및 빌드 확인

---

## glm Dependency

- `Dependency.cmake`에 다음을 추가

```cpp
# glm
ExternalProject_Add(
  dep_glm
  GIT_REPOSITORY "https://github.com/g-truc/glm"
  GIT_TAG "0.9.9.8"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  TEST_COMMAND ""
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_BINARY_DIR}/dep_glm-prefix/src/dep_glm/glm
    ${DEP_INSTALL_DIR}/include/glm
  )
set(DEP_LIST ${DEP_LIST} dep_glm)
```

---

## glm Dependency

- `src/common.h`에 glm 라이브러리 헤더 추가

```cpp [4-6]
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
```

---

## glm test

- `Context::Init()` 내에서 glm을 사용한 벡터 및 행렬 연산 테스트

```cpp
// 위치 (1, 0, 0)의 점. 동차좌표계 사용
glm::vec4 vec(1.0f,  0.0f, 0.0f, 1.0f);
// 단위행렬 기준 (1, 1, 0)만큼 평행이동하는 행렬
auto trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));
// 단위행렬 기준 z축으로 90도만큼 회전하는 행렬
auto rot = glm::rotate(glm::mat4(1.0f),
  glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
// 단위행렬 기준 모든 축에 대해 3배율 확대하는 행렬
auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));
// 확대 -> 회전 -> 평행이동 순으로 점에 선형 변환 적용
vec = trans * rot * scale * vec;
SPDLOG_INFO("transformed vec: [{}, {}, {}]", vec.x, vec.y, vec.z);
```

```console
[2021-01-20 12:29:56.358] [info] [context.cpp:95] transformed vec: [0.9999999, 4, 0]
```

---

## Vertex Transformation

- 정점에 대한 변환의 일반적인 방식
  - VBO 상의 정점은 고정
  - vertex shader에서 변환 행렬을 uniform으로 입력
  - vertex shader 내에서 행렬곱 계산

---

## Vertex Transformation

- `shader/texture.vs` 수정

```glsl [6,12]
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;

out vec4 vertexColor;
out vec2 texCoord;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    vertexColor = vec4(aColor, 1.0);
    texCoord = aTexCoord;
}
```

---

## Vertex Transformation

- `Context::Init()`에서 `transform`에 유니폼 값 전달

```cpp
// 0.5배 축소후 z축으로 90도 회전하는 행렬
auto transform = glm::rotate(
    glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)),
    glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)
    );
auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
```

---

## Vertex Transformation

- 빌드 및 실행
  - 행렬값을 바꿔보면서 어떻게 변하는지 실험해보자

<img src="/opengl_course/note/images/07_first_transform_example.png" width="45%" />

---

## Coordinate System

- 좌표계
  - 어떤 정점의 위치를 기술하기 위한 기준
  - 선형 변환은 **한 좌표계로 기술된 벡터를 다른 좌표계에 대해
    기술하는 변환**으로 해석할 수 있음

---

## Coordinate System Explained

- 기준이 되는 전역 좌표계 W
- v = [5, 4]

<div>
<img src="/opengl_course/note/images/07_coordinate_explain_01.png" width="25%" />
</div>

---

## Coordinate System Explained

- 좌표계 W와 [3, 3] 만큼 떨어져 있는 지역 좌표계 L
- L을 기준으로한 v = [2, 1]

<div>
<img src="/opengl_course/note/images/07_coordinate_explain_02.png" width="25%" />
</div>

---

## Coordinate System Explained

- [2, 1]을 [3, 3] 만큼 평행이동 시킨다 =>
- L을 기준으로 [2, 1]에 위치한 점은 W를 기준으로 [3, 3]만큼 평행이동되어 있다

<div>
<img src="/opengl_course/note/images/07_coordinate_explain_02.png" width="25%" />
</div>

---

## Coordinate System Explained

- 좌표계 W와 [5, 2]만큼 떨어져 있고 z축 방향으로 45도 회전한 지역 좌표계 L
- L을 기준으로 기술된 v의 위치: [1.414.., 1.414..]

<div>
<img src="/opengl_course/note/images/07_coordinate_explain_03.png" width="25%" />
</div>

---

## Coordinate System

- 자주 사용되는 좌표계 용어
  - World space
  - Local (Object) space
  - View (Eye) scale
  - Screen space

---

## Coordinate System

- 좌표 공간 간의 변환
  - OpenGL의 그림이 그려지는 공간:
    - [-1, 1] 범위로 normalized된 공간
    - Canonical space
  - Object들은 Local space를 기준으로 기술
  - Local space -> World space ->
    View space -> Canonical space로 변환

---

## Coordinate System

![transform process](/opengl_course/note/images/07_transform_process.png)

---

## Coordinate System

- Transform Matrix
  - Model: Local을 World로
  - View: World를 Camera로
  - Projection: Camera를 Canonical로
  - Clip space에서 [-1, 1] 범위 밖으로 벗어난 면들은 clipping

---

## Orthogonal Projection

- 직교 투영
  - 원근감 없이 평행한 선이 계속 평행하도록 투영하는 방식
  - 설계 도면 등을 그려낼때 때 유용

<div>
<img src="/opengl_course/note/images/07_orthogonal_projection.png" width="35%" />
</div>

---

## Orthogonal Projection

- 직교 투영
  - 6개 파라미터: left, right, bottom, top, near, far
  - z축에 -1: Clip space 이후에는 오른손 좌표계에서 왼손 좌표계로 변경

<div>
<img src="/opengl_course/note/images/07_orthogonal_projection_matrix.png" width="25%" style="background: white; padding: 10px"/>
</div>

---

## Orthogonal Projection

- 오른손 좌표계 / 왼손 좌표계
  - x축, y축을 화면의 오른/위 방향으로 했을때
  - 오른손 좌표계: z축이 화면에서부터 **나오는** 방향
  - 왼손 좌표계: z축이 화면으로 **들어가는** 방향

<div>
<img src="/opengl_course/note/images/07_left_and_right_hand.png" width="35%"/>
</div>

---

## Perspective Projection

- 원근 투영
  - 변환 이전에 평행한 선이 변환 후에 한점에서 만남 (소실점)
  - 멀리 있을 수록 물체가 작아져 원근감이 발생

<div>
<img src="/opengl_course/note/images/07_perspective_projection.png" width="35%"/>
</div>

---

## Perspective Projection

- 원근 투영
  - 4개의 파라미터
    - 종횡비 (aspect ratio)
    - 화각 (field of view, FoV)
    - near, far

<div>
<img src="/opengl_course/note/images/07_perspective_projection_matrix.jpg" width="50%" style="background: white; padding:10px"/>
</div>

---

## Transformation

- 모든 변환의 조합
  - Local space를 기준으로 한 좌표 V의 Clip space에서의 좌표는?
  - 합쳐서 MVP (model-view-projection) matrix라고도 함

<div>
<img src="/opengl_course/note/images/07_all_transform.png" width="60%"/>
</div>

---

## 3D Transformation

- `Context::Init()`에 코드를 수정

```cpp
// x축으로 -55도 회전
auto model = glm::rotate(glm::mat4(1.0f),
  glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
// 카메라는 원점으로부터 z축 방향으로 -3만큼 떨어짐
auto view = glm::translate(glm::mat4(1.0f),
  glm::vec3(0.0f, 0.0f, -3.0f));
// 종횡비 4:3, 세로화각 45도의 원근 투영
auto projection = glm::perspective(glm::radians(45.0f),
  (float)640 / (float)480, 0.01f, 10.0f);
auto transform = projection * view * model;
auto transformLoc = glGetUniformLocation(m_program->Get(), "transform");
```

---

## 3D Transformation

- 빌드 및 실행

![3d transform example](/opengl_course/note/images/07_3d_transform_example.png)

---

## More 3D

- 큐브 그리기

```cpp
float vertices[] = {
  -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
   0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
   0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
   0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
   0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

  -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

   0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
   0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
   0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
   0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

  -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
   0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
   0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

  -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
   0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
   0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
};

uint32_t indices[] = {
   0,  2,  1,  2,  0,  3,
   4,  5,  6,  6,  7,  4,
   8,  9, 10, 10, 11,  8,
  12, 14, 13, 14, 12, 15,
  16, 17, 18, 18, 19, 16,
  20, 22, 21, 22, 20, 23,
};
```

---

## More 3D

- 큐브 그리기

```cpp [4, 6-7, 11]
m_vertexLayout = VertexLayout::Create();
m_vertexBuffer = Buffer::CreateWithData(
  GL_ARRAY_BUFFER, GL_STATIC_DRAW,
  vertices, sizeof(float) * 120);

m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, sizeof(float) * 3);

m_indexBuffer = Buffer::CreateWithData(
    GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
    indices, sizeof(uint32_t) * 36);
```

---

## More 3D

- 큐브 그리기

```cpp [4]
void Context::Render() {
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
```

---

## More 3D

- 빌드 및 실행 결과
  - 박스의 뒷면이 앞면을 덮어서 그려짐

![no depth test](/opengl_course/note/images/07_no_depth_test_cube.png)

---

## Depth Buffer

- Z 버퍼 (Z buffer) 라고도 함
- 각 픽셀의 컬러 값을 저장하는 버퍼 외에, 해당 픽셀의 깊이값 (z축값)을 저장
- 깊이 테스트 (Depth test)
  - 어떤 픽셀의 값을 업데이트 하기 전, 현재 그리려는 픽셀의 z값과
    깊이 버퍼에 저장된 해당 위치의 z값을 비교해 봄
  - 비교 결과 현재 그리려는 픽셀이 이전에 그려진 픽셀보다 뒤에 있을 경우
    픽셀을 그리지 않음

---

## Depth Test in OpenGL

- OpenGL의 Depth Buffer 초기값은 1
- 1이 가장 뒤에 있고, 0이 가장 앞을 의미 (왼손 좌표계)
- `glEnable(GL_DEPTH_TEST)` / `glDisable(GL_DEPTH_TEST)`로 깊이 테스트를 켜고 끌 수 있음
- `glDepthFunc()`을 이용하여 깊이 테스트 통과 조건을 변경할 수 있음
- 깊이 테스트 통과 조건의 기본값은 `GL_LESS`

---

## Depth Test in OpenGL

- `Context::Render()` 코드 수정

```cpp [2-3]
void Context::Render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
```

---

## Depth Test in OpenGL

- 빌드 및 실행

![depth test](/opengl_course/note/images/07_depth_test_cube.png)

---

## Refactoring Uniform

- `Program` 클래스에 uniform 값을 설정하는 메소드 추가

```cpp
// ... in Program class declaration
void SetUniform(const std::string& name, int value) const;
void SetUniform(const std::string& name, const glm::mat4& value) const;
```

```cpp
// program.cpp
void Program::SetUniform(const std::string& name, int value) const {
  auto loc = glGetUniformLocation(m_program, name.c_str());
  glUniform1i(loc, value);
}

void Program::SetUniform(const std::string& name,
  const glm::mat4& value) const {
  auto loc = glGetUniformLocation(m_program, name.c_str());
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}
```

---

## Refactoring Uniform

- `Context::Init()`에 사용된 `glUniform` 대신 `Program::SetUniform` 사용

```cpp [2-3,8]
m_program->Use();
m_program->SetUniform("tex", 0);
m_program->SetUniform("tex2", 1);

// ...

auto transform = projection * view * model;
m_program->SetUniform("transform", transform);
```

---

## Transformation on Every Frame

- transform의 지정을 매 프레임 해보자

```cpp
void Context::Render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  auto projection = glm::perspective(glm::radians(45.0f),
      (float)640 / (float)480, 0.01f, 10.0f);
  auto view = glm::translate(glm::mat4(1.0f),
      glm::vec3(0.0f, 0.0f, -3.0f));
  auto model = glm::rotate(glm::mat4(1.0f),
      glm::radians((float)glfwGetTime() * 120.0f),
      glm::vec3(1.0f, 0.5f, 0.0f));
  auto transform = projection * view * model;
  m_program->SetUniform("transform", transform);

  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
```

---

## Multiple Cube

- 여러 개의 큐브를 그려보자

```cpp
void Context::Render() {
    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto projection = glm::perspective(glm::radians(45.0f),
        (float)640 / (float)480, 0.01f, 20.0f);
    auto view = glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -3.0f));

    for (size_t i = 0; i < cubePositions.size(); i++){
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(model,
            glm::radians((float)glfwGetTime() * 120.0f + 20.0f * (float)i),
            glm::vec3(1.0f, 0.5f, 0.0f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}
```

---

## Multiple Cube

- 빌드 및 실행
  - 빙글빙글 도는 여러 개의 큐브

<img src="/opengl_course/note/images/07_multiple_cube.png" width="55%" />

---

## Camera

- Camera/View space
  - 3D 공간을 어느 시점에서 어떤 방향으로 바라볼 것인가를 결정
  - 카메라를 조작하기 위한 파라미터로부터 view transform을 유도

![camera space](/opengl_course/note/images/07_camera_space.png)

---

## Camera

- 카메라 파라미터
  - camera position: 카메라의 위치
  - camera target: 카메라가 바라보는 중심 위치
  - camera up vector: 카메라 화면의 세로 축 방향

- 결과 행렬
  - camera의 local-to-world transform의 inverse

<div>
<img src="/opengl_course/note/images/07_camera_matrix.gif" width="30%" style="background: white; padding: 10px" />
</div>

---

## Camera

- 카메라의 3축 결정 과정

<div>
<img src="/opengl_course/note/images/07_camera_matrix_derive.gif" width="30%" style="background: white; padding: 10px" />
</div>

---

## Camera

- `Context::Render()`에서 직접 계산해보자

```cpp
auto cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
auto cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

auto cameraZ = glm::normalize(cameraPos - cameraTarget);
auto cameraX = glm::normalize(glm::cross(cameraUp, cameraZ));
auto cameraY = glm::cross(cameraZ, cameraX);

auto cameraMat = glm::mat4(
  glm::vec4(cameraX, 0.0f),
  glm::vec4(cameraY, 0.0f),
  glm::vec4(cameraZ, 0.0f),
  glm::vec4(cameraPos, 1.0f));

auto view = glm::inverse(cameraMat);
```

---

## Camera

- 앞의 연산 과정을 해주는 `glm::lookAt` 함수의 활용

```cpp
auto cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
auto cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

auto view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
```

---

## Camera

- 주변을 빙글빙글 도는 카메라

```cpp
float angle = glfwGetTime() * glm::pi<float>() * 0.5f;
auto x = sinf(angle) * 10.0f;
auto z = cosf(angle) * 10.0f;
auto cameraPos = glm::vec3(x, 0.0f, z);
auto cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
auto cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

auto view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
```

---

## Interactive Camera

- 카메라 조작을 키보드 / 마우스로 할 수 있게 해보자
  - W/A/S/D/Q/E: 전후좌우 + 상하 이동
  - 마우스 커서: 카메라 회전

---

## Interactive Camera

- `Context` 클래스에 입력 처리 함수 및 카메라 관련 파라미터 추가

```cpp [4,11-14]
class Context {
public:
  static ContextUPtr Create();
  void Render();
  void ProcessInput(GLFWwindow* window);

private:

  // ...

  // camera parameter
  glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 3.0f) };
  glm::vec3 m_cameraFront { glm::vec3(0.0f, 0.0f, -1.0f) };
  glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
};
```

---

## Interactive Camera

- `Context::ProcessInput()` 구현

```cpp
void Context::ProcessInput(GLFWwindow* window) {
  const float cameraSpeed = 0.05f;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    m_cameraPos += cameraSpeed * m_cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    m_cameraPos -= cameraSpeed * m_cameraFront;

  auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    m_cameraPos += cameraSpeed * cameraRight;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    m_cameraPos -= cameraSpeed * cameraRight;    

  auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    m_cameraPos += cameraSpeed * cameraUp;
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    m_cameraPos -= cameraSpeed * cameraUp;
}
```

---

## Interactive Camera

- `Context::Render()`에서 카메라 파라미터를 이용한 look-at 행렬 계산

```cpp [3-6]
  auto projection = glm::perspective(glm::radians(45.0f),
      (float)640 / (float)480, 0.01f, 20.0f);
  auto view = glm::lookAt(
      m_cameraPos,
      m_cameraPos + m_cameraFront,
      m_cameraUp);
```

---

## Interactive Camera

- `main()`의 메인 루프에서 `Context::ProcessEvent()` 호출

```cpp [2-5]
while (!glfwWindowShouldClose(window)) {
  glfwPollEvents();
  context->ProcessInput(window);
  context->Render();
  glfwSwapBuffers(window);
}
```

---

## Interactive Camera

- 빌드 및 실행
  - WASDQE 키로 전후/좌우/상하로 움직이는 카메라 확인

---

## Refactoring

- 화면 크기 관련 처리 리팩토링

```cpp [6,11-12]
class Context {
public:
  static ContextUPtr Create();
  void Render();
  void ProcessInput(GLFWwindow* window);
  void Reshape(int width, int height);

private:
  // ...

  int m_width {640};
  int m_height {480};

  // ...
```

---

## Refactoring

- `Context::Reshape()` 함수 구현 및 프로젝션 행렬 계산식 수정

```cpp
void Context::Reshape(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}
```

```cpp [1-2]
auto projection = glm::perspective(glm::radians(45.0f),
  (float)m_width / (float)m_height, 0.01f, 20.0f);
auto view = glm::lookAt(
  m_cameraPos,
  m_cameraPos + m_cameraFront,
  m_cameraUp);
```

---

## Refactoring

- glfw callback 내에서 context 사용
  - user pointer 기능을 이용

```cpp
auto context = Context::Create();
if (!context) {
    SPDLOG_ERROR("failed to create context");
    glfwTerminate();
    return -1;
}
glfwSetWindowUserPointer(window, context.get());

OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
glfwSetKeyCallback(window, OnKeyEvent);
```

---

## Refactoring

- glfw callback 내에서 context 사용

```cpp
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
  SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
  auto context = (Context*)glfwGetWindowUserPointer(window);
  context->Reshape(width, height);
}
```

---

## Refactoring

- 빌드 및 실행
  - 화면 크기 변경 후에도 종횡비 유지

![aspect ratio](/opengl_course/note/images/07_aspect_ratio.png)

---

## Interactive Camera

- Euler angle
  - 물체의 회전 정보를 나타내는 대표적인 방식
  - roll(z), pitch(x), yaw(y) 3개의 회전각

![euler angle](/opengl_course/note/images/07_euler_angle.png)

---

## Interactive Camera

- 카메라 회전각
  - 카메라 회전에 roll은 보통 사용하지 않음
    - 대신 up vector를 기준으로 roll을 설정
  - 따라서 yaw, pitch만 가지고 camera front 방향을 결정

---

## Interactive Camera

- `Context` 클래스에 카메라 회전 관련 파라미터 및 처리 함수 추가

```cpp [7,12-13]
class Context {
public:
  static ContextUPtr Create();
  void Render();
  void ProcessInput(GLFWwindow* window);
  void Reshape(int width, int height);
  void MouseMove(double x, double y);

  //...

  // camera parameter
  float m_cameraPitch { 0.0f };
  float m_cameraYaw { 0.0f };
  glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
  glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 3.0f) };
  glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
};
```

---

## Interactive Camera

- `Context::MouseMove()` 구현

```cpp
void Context::MouseMove(double x, double y) {
  static glm::vec2 prevPos = glm::vec2((float)x, (float)y);
  auto pos = glm::vec2((float)x, (float)y);
  auto deltaPos = pos - prevPos;

  const float cameraRotSpeed = 0.8f;
  m_cameraYaw -= deltaPos.x * cameraRotSpeed;
  m_cameraPitch -= deltaPos.y * cameraRotSpeed;

  if (m_cameraYaw < 0.0f)   m_cameraYaw += 360.0f;
  if (m_cameraYaw > 360.0f) m_cameraYaw -= 360.0f;

  if (m_cameraPitch > 89.0f)  m_cameraPitch = 89.0f;
  if (m_cameraPitch < -89.0f) m_cameraPitch = -89.0f;

  prevPos = pos;    
}
```

---

## Interactive Camera

- `Context::Render()`에서 `m_cameraFront`를 yaw / pitch에 따라 결정
  - (0, 0, -1) 방향을 x축, y축에 따라 회전

```cpp [1-6]
m_cameraFront =
  glm::rotate(glm::mat4(1.0f),
    glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
  glm::rotate(glm::mat4(1.0f),
    glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
  glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

auto projection = glm::perspective(glm::radians(45.0f),
  (float)m_width / (float)m_height, 0.01f, 20.0f);
auto view = glm::lookAt(
  m_cameraPos,
  m_cameraPos + m_cameraFront,
  m_cameraUp);
```

---

## Interactive Camera

- `src/main.cpp`에서 마우스 커서 콜백 함수 구현 및 설정

```cpp
void OnCursorPos(GLFWwindow* window, double x, double y) {
  auto context = (Context*)glfwGetWindowUserPointer(window);
  context->MouseMove(x, y);
}
```

```cpp [5]
// ... in main() function
OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
glfwSetKeyCallback(window, OnKeyEvent);
glfwSetCursorPosCallback(window, OnCursorPos);
```

---

## Interactive Camera

- 빌드 및 결과
  - 화면 안으로 커서가 들어오면 카메라 회전 시작
  - 커서가 화면 밖으로 나가면 카메라 회전 종료
  - 마우스 x축 움직임으로 좌우 회전
  - 마우스 y축 움직임으로 상하 회전

- **마우스 우버튼을 누르고 있는 경우에만** 카메라 조작을 허용해보자

---

## Interactive Camera

- `Context` 클래스에 콜백 및 제어 파라미터 추가

```cpp [4,6-7]
class Context {
// ...
  void MouseMove(double x, double y);
  void MouseButton(int button, int action, double x, double y);
// ...
  bool m_cameraControl { false };
  glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
  float m_cameraPitch { 0.0f };
// ...
};
```

---

## Interactive Camera

- `Context::MouseButton()` 구현

```cpp
void Context::MouseButton(int button, int action, double x, double y) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      // 마우스 조작 시작 시점에 현재 마우스 커서 위치 저장
      m_prevMousePos = glm::vec2((float)x, (float)y);
      m_cameraControl = true;
    }
    else if (action == GLFW_RELEASE) {
      m_cameraControl = false;
    }
  }
}
```

---

## Interactive Camera

- `Context::ProcessInput()`, `Context::MouseMove()` 구현 수정

```cpp [2-3]
void Context::ProcessInput(GLFWwindow* window) {
  if (!m_cameraControl)
    return;
  // ...
```

```cpp [2-5,9]
void Context::MouseMove(double x, double y) {
  if (!m_cameraControl)
    return;
  auto pos = glm::vec2((float)x, (float)y);
  auto deltaPos = pos - m_prevMousePos;

  // ...

  m_prevMousePos = pos;    
}

```

---

## Interactive Camera

- `src/main.cpp`에 마우스 버튼 관련 콜백 구현 및 설정

```cpp
void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {
  auto context = (Context*)glfwGetWindowUserPointer(window);
  double x, y;
  glfwGetCursorPos(window, &x, &y);
  context->MouseButton(button, action, x, y);
}
```

```cpp [6]
// ... in main() function
OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
glfwSetKeyCallback(window, OnKeyEvent);
glfwSetCursorPosCallback(window, OnCursorPos);
glfwSetMouseButtonCallback(window, OnMouseButton);
```

---

## Interactive Camera

- 빌드 및 결과
  - 마우스 우버튼을 누른 동안에만 카메라 조작 활성화

<div>
<img src="/opengl_course/note/images/07_camera_control.png" width="50%" />
</div>

---

## Congratulation!
### 수고하셨습니다!

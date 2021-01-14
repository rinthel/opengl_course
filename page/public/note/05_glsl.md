# GLSL

---

## GLSL

- OpenGL Shader Language
  - Shader: GPU 상에서 동작하는 그림을 그리기 위한 작은 프로그램
  - 정점별 / 픽셀별로 병렬 수행되어 성능을 높임
  - GLSL: OpenGL에서 shader를 작성하기 위해 제공하는 C 기반 언어
  - 그 외의 대표적인 shader language
    - HLSL: DirectX용 shader language
    - Metal: Metal용 shader language
    - cg: nVidia가 제시한 shader language

---

## GLSL

- GLSL source code의 대략적인 구조

```glsl
#version version_number

in type in_variable_name;
in type in_variable_name;

out type out_variable_name;

uniform type uniform_name;

void main() {
  // process input(s) and do some weird graphics stuff ...
  // output processed stuff to output variable
  out_variable_name = weird_stuff_we_processed;
}
```

---

## GLSL

- GLSL에서 사용 가능한 data type
  - 기본 타입: `int, float, double, uint, bool`
  - **벡터 타입**:
    - `vecX`: `float`형 벡터
    - `bvecX`: `bool`형 벡터
    - `ivecX`: `int`형 벡터
    - `uvecX`: `uint`형 벡터
    - `dvecX`: `double`형 벡터
  - `X`에는 2, 3, 4 사용 가능

---

## GLSL

- GLSL에서 사용 가능한 data type
  - **행렬 타입**:
    - `matX`: `float`형 행렬
    - `bmatX`: `bool`형 행렬
    - `imatX`: `int`형 행렬
    - `umatX`: `uint`형 행렬
    - `dmatX`: `double`형 행렬
  - `X`에는 2, 3, 4 사용 가능

---

## Vector

- 벡터 원소 접근
  - `.x`, `.y`, `.z`, `.w` 인덱스를 사용
  - swizzling 가능
    - 얻어오고 싶은 인덱스를 연속으로 쓰기
    - `.xyz` => `vec3`
  - `.rgba`, `.stpq`도 동일한 방식으로 사용 가능

```glsl
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yxzy;
```

---

## Vector

- 벡터 초기값 선언
  - 생성자 사용
  - 다른 벡터를 섞어서 사용 가능

```glsl
vec2 vect = vec2(0.5, 0.7);
vec4 result = vec4(vect, 0.0, 0.0);
vec4 otherResult = vec4(result.xyz, 1.0);
```

---

## In / Out

- Shader의 입출력 구조
  - 모든 shader는 용도에 맞는 입출력이 선언되어 있어야 함
  - `in`, `out`: shader의 입출력을 가리키는 type qualifier

---

## In / Out

- Vertex shader
  - 각 정점 별로 설정된 vertex attribute를 입력받는다
  - 몇 번째 attribute를 입력받을지에 대한 추가적인 설정을 할 수 있음
    - `layout (location = n)`
  - 반드시 정점의 출력 위치 `gl_Position`값을 계산해줘야 함

---

## In / Out

- Rasterization
  - Vertex shader의 출력값을 primitive에 맞게 보간하여
    픽셀별 값으로 변환 (Rasterization)
- Fragment shader
  - Rasterization 과정을 거쳐 픽셀별로 할당된 vertex shader의
    출력값이 입력됨
  - 각 픽셀의 실제 색상 값이 출력되어야 함

---

## In / Out

```glsl
#version 330 core
layout (location = 0) in vec3 aPos; // 0번째 attribute가 정점의 위치

out vec4 vertexColor; // fragment shader로 넘어갈 컬러값

void main() {
  gl_Position = vec4(aPos, 1.0); // vec3를 vec4 생성자에 사용
  vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // 어두운 빨간색을 출력값으로
}
```

```glsl
#version 330 core

in vec4 vertexColor; // vs로부터 입력된 변수 (같은 변수명, 같은 타입)
out vec4 FragColor; // 최종 출력 색상

void main() {
  FragColor = vertexColor;
}
```

---

## 예제 준비

- `triangle_example`의 코드를 복사하여 `glsl_example` 프로젝트 생성
- `CMakeLists.txt`의 프로젝트명 변경
- VSCode로 열어서 빌드 및 실행 확인

---

## 예제 준비

- `shader/simple.vs`, `shader/simple.fs`을 앞의 슬라이드와 같이 수정
- 빌드 및 실행

---

## 결과

![dark red rectangle](/opengl_course/note/images/05_dark_red.png)

---

## Uniform

- Uniform
  - shader에 전달 가능한 global value
    - 병렬로 수행되는 모든 shader thread들이 동일한 값을 전달받는다
  - 변수 선언 앞에 `uniform` type qualifier를 써서 선언

```glsl
#version 330 core
out vec4 FragColor;

uniform vec4 ourColor; // uniform 변수

void main() {
  FragColor = ourColor;
}
```

---

## Uniform

- `shader/simple.vs`, `shader/simple.fs`을 아래와 같이 변경

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
  gl_Position = vec4(aPos, 1.0);
}
```

```glsl
#version 330 core
uniform vec4 color;
out vec4 fragColor;

void main() {
    fragColor = color;
}
```

---

## Uniform

- uniform variable에 값을 입력하는 과정
  - `glGetUniformLocation()`을 사용하여 program object로부터 uniform handle을 얻는다
  - program이 바인딩 된 상태에서 `glUniform...()`를 사용하여 값을 입력한다

---

## Uniform

- `Context::Init()`에서 `Program` 인스턴스 생성 이후
  uniform 값 입력과정 추가

```cpp [6-8]
m_program = Program::Create({fragShader, vertShader});
if (!m_program)
  return false;
SPDLOG_INFO("program id: {}", m_program->Get());

auto loc = glGetUniformLocation(m_program->Get(), "color");
m_program->Use();
glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);

glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
```

---

## 빌드 및 실행

![yellow rectangle](/opengl_course/note/images/05_yellow.png)

---

## Additional Note

- `Context::Render()`에서 매 프레임마다 uniform 값을 다르게 입력해보자

```cpp
void Context::Render() {
  glClear(GL_COLOR_BUFFER_BIT);

  static float time = 0.0f;
  float t = sinf(time) * 0.5f + 0.5f;
  auto loc = glGetUniformLocation(m_program->Get(), "color");
  m_program->Use();
  glUniform4f(loc, t*t, 2.0f*t*(1.0f-t), (1.0f-t)*(1.0f-t), 1.0f);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  time += 0.016f;
}
```

---

## Vertex Attributes

- 하나의 vertex가 가지는 정보는 여러가지일 수 있다
  - position
  - normal
  - tangent
  - color
  - texture coordinates
  - ...
- 각각이 하나의 vertex attribute가 된다

---

## Vertex Attributes

- vertex 별로 다른 색상을 가진 사각형을 그려보자
  - attribute #0: position
  - attribute #1: color

---

## Vertex Attributes

- `Context::Init()`에서 `vertices` 정보 변경

```cpp [1-6,13]
float vertices[] = {
  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top right, red
  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right, green
  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left, blue
  -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, // top left, yellow
};

// ...

m_vertexLayout = VertexLayout::Create();
m_vertexBuffer = Buffer::CreateWithData(
  GL_ARRAY_BUFFER, GL_STATIC_DRAW,
  vertices, sizeof(float) * 24);
```

---

## Vertex Attributes

- `VertexLayout` 인스턴스를 이용하여 두번째 attribute 설정

```cpp
m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE,
  sizeof(float) * 6, 0);
m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE,
  sizeof(float) * 6, sizeof(float) * 3);
```

---

## Vertex Attributes

- 각 attribute 별 stride, offset 값

![position/color attributes](/opengl_course/note/images/05_position_color_attributes.png)

---

## Per-Vertex Color Shader

- `shader/per_vertex_color.vs` 작성

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec4 vertexColor;

void main() {
    gl_Position = vec4(aPos, 1.0);
    vertexColor = vec4(aColor, 1.0);
}
```

---

## Per-Vertex Color Shader

- `shader/per_vertex_color.fs` 작성

```glsl
#version 330 core
in vec4 vertexColor;
out vec4 fragColor;

void main() {
    fragColor = vertexColor;
}
```

---

## Per-Vertex Color Shader

- `Context::Init()`에서 읽어들일 shader 코드 변경

```cpp
ShaderPtr vertShader = Shader::CreateFromFile(
  "./shader/per_vertex_color.vs", GL_VERTEX_SHADER);
ShaderPtr fragShader = Shader::CreateFromFile(
  "./shader/per_vertex_color.fs", GL_FRAGMENT_SHADER);
```

---

## 빌드 및 결과

![per-vertex color](/opengl_course/note/images/05_per_vertex_color.png)

---

## 정리

- GLSL
  - C와 비슷한 문법 체계의 shader용 language
  - 기본적인 수치값 타입 외에 벡터, 행렬 타입이 있음
  - `in`, `out`을 사용하여 각 shader의 입력 출력을 지정

---

## 정리

- GLSL
  - vertex shader의 경우 `layout`을 사용하여 attribute index 지정
  - vertex shader의 경우 gl_Position을, fragment shader의 경우 픽셀의
    최종 색상값을 출력해야 함
  - vertex shader의 out 변수들은 Rasterization 과정을 거쳐 픽셀단위로
    보간되어 fragment shader의 in 변수들로 입력

---

## 정리

- GLSL
  - uniform variable을 사용하면 shader에서 사용할 수 있는 global 값을
    설정할 수 있음
  - `glGetShaderLocation()`으로 uniform handle을 얻을 수 있음
  - program이 바인딩된 상태에서 `glUniform...()`으로 uniform variable
    값 설정 가능
  - 필요에 따라 정점 별로 여러 개의 attribute를 설정하여 사용 가능

---

## Congratulation!
### 수고하셨습니다!

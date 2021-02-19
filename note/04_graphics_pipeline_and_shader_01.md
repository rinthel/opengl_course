# Graphics Pipeline and Shader

## Part 1

---

## 첫 예제 준비 (1)

- 새로운 프로젝트 디렉토리 `first_opengl_example` 생성

```console
> mkdir first_opengl_example
> cd first_opengl_example
```

---

## 첫 예제 준비 (2)

- `cmake_project_example` 프로젝트에서 다음을 복사

```console
> cp ../cmake_project_example/CMakeLists.txt .
> cp ../cmake_project_example/Dependency.cmake .
> cp ../cmake_project_example/.gitignore .
> mkdir src
> cp ../cmake_project_example/src/main.cpp ./src
```

---

## 첫 예제 준비 (3)

- git repo 초기화

```console
> git init
```

- VSCode로 프로젝트 열기

```console
> code .
```

---

## 첫 예제 준비 (4)

- `CMakeLists.txt`의 프로젝트 이름 변경

```cmake [1,4]
set(PROJECT_NAME first_opengl_example)
set(CMAKE_CXX_STANDARD 17)

set(WINDOW_NAME "First OpenGL Example")
set(WINDOW_WIDTH 640)
set(WINDOW_HEIGHT 480)
```

- 빌드 도구 선택 후 빌드
- 실행 후 빈 윈도우 화면 확인

---

## GLAD dependency 추가하기

- [GLAD](https://github.com/Dav1dde/glad)
  - GL/GLES Loader-Generator
    - OpenGL은 spec과 구현체 (driver, dll) 가 따로 존재
    - OpenGL 함수를 사용하기 전에 해당 함수들의 구현체가 어디 있는지 로딩하는 과정이 필요

---

## GLAD dependency 추가하기

- `Dependency.cmake`에 다음을 추가

```cmake
# glad
ExternalProject_Add(
    dep_glad
    GIT_REPOSITORY "https://github.com/Dav1dde/glad"
    GIT_TAG "v0.1.34"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLAD_INSTALL=ON
    TEST_COMMAND ""
    )
set(DEP_LIST ${DEP_LIST} dep_glad)
set(DEP_LIBS ${DEP_LIBS} glad)
```

---

## OpenGL 초기화 (1)

- GLFW 윈도우를 생성하기 전 만들기 희망하는 OpenGL 버전 추가

```cpp
// ... glfwInit() 호출 후

glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

// ... glfwCreateWindow() 호출 전
```

---

## OpenGL 초기화 (2)

- 윈도우 생성 후 함께 생성된 OpenGL Context를 사용

```cpp [9]
// glfw 윈도우 생성, 실패하면 에러 출력후 종료
SPDLOG_INFO("Create glfw window");
auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
  nullptr, nullptr);
if (!window) {
    SPDLOG_ERROR("failed to create glfw window");
    glfwTerminate();
    return -1;
}
glfwMakeContextCurrent(window);
```

---

## OpenGL 초기화 (3)

- GLAD 라이브러리 사용을 위해 `#include` 구문 사용
- 반드시 `#include <GLFW/GLFW.h>` 이전에 추가할 것

```cpp [2]
#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
```

---

## OpenGL 초기화 (4)

- GLAD 라이브러리를 통해 OpenGL 함수 로딩
- OpenGL Context 생성 이후 실행
- 생성된 OpenGL context 버전 출력

```cpp
// glad를 활용한 OpenGL 함수 로딩
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    SPDLOG_ERROR("failed to initialize glad");
    glfwTerminate();
    return -1;
}
auto glVersion = glGetString(GL_VERSION);
SPDLOG_INFO("OpenGL context version: {}", glVersion);
```

---

## OpenGL 초기화 (5)

- 빌드/실행 후 드라이버 로딩/버전 확인

```console
[2021-01-11 12:24:53.460] [info] [main.cpp:31] OpenGL context version: 3.3.0 NVIDIA 456.71
```

---

## GLFW Callbacks (1)

- GLFW로 생성된 윈도우에 특정 이벤트가 발생했을 떄 실행되는 콜백함수 지정
  - 윈도우의 크기가 변경되었을 때
  - 윈도우에 마우스 입력이 들어왔을 때
  - 윈도우에 키보드 입력이 들어왔을 때
  - 기타 등등...
- `glfwXXXXCallback()` 의 함수 프로토타입을 가지고 있음

---

## GLFW Callbacks (2)

- 윈도우의 프레임버퍼 크기가 변경되었을 때 호출하기 위한 콜백 정의
- `glViewport()`: OpenGL이 그림을 그릴 영역 지정

```cpp
void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    glViewport(0, 0, width, height);
}
```

---

## GLFW Callbacks (3)

- 키보드 입력 콜백 정의
- `Escape` 키 누르면 프로그램 종료

```cpp
void OnKeyEvent(GLFWwindow* window,
    int key, int scancode, int action, int mods) {
    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
```

---

## GLFW Callbacks (4)

- 정의한 콜백을 윈도우에 등록
- 윈도우 생성 직후에는 프레임버퍼 변경 이벤트가 발생하지 않으므로 첫 호출은 수동으로 함

```cpp
// ... context 지정 부분

OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
glfwSetKeyCallback(window, OnKeyEvent);

// ... loop 시작 부분
```

---

## 렌더링 코드 (1)

- 렌더링 함수 정의

```cpp
void Render() {
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
```

---

## 렌더링 코드 (2)

- 루프 내에서 렌더링 함수 호출
- 프레임버퍼 스왑 코드 호출

```cpp [3-4]
SPDLOG_INFO("Start main loop");
while (!glfwWindowShouldClose(window)) {
    Render();
    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

---

## Framebuffer swap

- 화면에 그림을 그리는 과정
  - 프레임버퍼 2개를 준비 (front / back)
  - back buffer에 그림 그리기
  - front와 back을 바꿔치기
  - 위의 과정을 반복
- 그림이 그려지는 과정이 노출되지 않도록 해줌

---

### 결과

![first opengl example](/opengl_course/note/images/04_first_opengl_example.jpg)

---

## 결과

- 빌드/실행 결과: 푸른 화면
- 윈도우 크기를 조절했을때 콜백 함수가 호출되어 로그가 출력되는지 확인
- 키보드를 눌렀을때 로그가 출력되는지 확인
- `Escape` 키를 눌렀을 떄 윈도우가 닫히고 프로그램이 종료되는지 확인

```console
[2021-01-11 12:35:37.179] [info] [main.cpp:5] framebuffer size changed: (635 x 472)
[2021-01-11 12:35:37.188] [info] [main.cpp:5] framebuffer size changed: (632 x 470)
[2021-01-11 13:52:32.150] [info] [main.cpp:18] key: 342, scancode: 56, action: Pressed, mods: --A
[2021-01-11 13:52:32.250] [info] [main.cpp:18] key: 342, scancode: 56, action: Released, mods: ---
[2021-01-11 13:52:33.434] [info] [main.cpp:18] key: 256, scancode: 1, action: Pressed, mods: ---
```

---

## OpenGL Remarks (1)

- `glViewport()`: OpenGL이 그림을 그릴 화면의 위치 및 크기 설정
- `glClearColor()`: 컬러 프레임버퍼 화면을 클리어할 색상 지정
- `glClear()`: 프레임버퍼 클리어

---

## OpenGL Remarks (2)

- State-setting function
  - State가 OpenGL context에 저장됨
  - `glViewport()`, `glClearColor()`
- State-using function
  - OpenGL context에 저장된 State를 이용
  - `glClear()`

---

## Graphics Pipeline

- 그림이 그려지는 과정

![graphics pipeline](/opengl_course/note/images/04_graphics_pipeline.jpg)

---

## Graphics Pipeline

- **Application**: 그리고 싶은 정점의 위치 / 색상 등을 입력
- **Geometry**: 정점 단위의 처리. 정점의 위치 결정
- **Rasterization**: 정점 단위로 구성된 삼각형을 픽셀 단위로 변경
- **Pixel**: 픽셀 단위의 처리. 픽셀 색상의 결정

---

## Graphics Pipeline

- **Application**: 애플리케이션 프로그램 영역. OpenGL 함수 호출
- **Geometry, Rasterization, Pixel**: GPU 영역

---

## Programmable Shader

- **Shader**: 각 파이프라인 단계마다 GPU 상에서 실행되는 작은 프로그램
- GLSL(GL Shading Language)라는 C기반 프로그래밍 언어로 작성

![programmable shader](/opengl_course/note/images/04_programmable_shader.jpg)

---

## OpenGL Shader

- OpenGL은 그림을 그려내기 위해 **두 개의 shader가 반드시 필요**
  - **Vertex shader**
  - **Fragment shader**
- Shader 코드는 OpenGL 코드 내에서 빌드 / 로딩됨
- 미리 빌드한 뒤 로딩하는 방법: SPIR-V 이용
  - 이번 수업에서는 다루지 않을 예정

---

## 두번째 예제 준비

- `first_opengl_example` 예제를 복사하여 `point_example` 예제 생성
- cmake 프로젝트 명 변경
- VSCode로 프로젝트 열기

---

## Shader code loading

- 임의의 shader 파일을 읽는다
- shader 오브젝트를 만들고 shader 읽어들인 shader 코드를 세팅한다
- shader를 컴파일한다
- shader 컴파일 결과가 실패했다면 에러를 레포팅한다

---

## Text file loading

- `src/common.h` 파일 작성
  - 프로젝트가 공통적으로 이용할 기능들을 정의할 파일

```cpp
#ifndef __COMMON_H__
#define __COMMON_H__

#include <memory>
#include <string>
#include <optional>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <spdlog/spdlog.h>

std::optional<std::string> LoadTextFile(const std::string& filename);

#endif // __COMMON_H__
```

---

## Text file loading

- `optional<>`
  - C++17부터 사용 가능한 표준 라이브러리
  - 어떤 값이 있거나 없는 경우를 포인터 없이 표현 가능

---

## Text file loading

- `src/common.cpp` 파일 작성

```cpp
#include "common.h"
#include <fstream>
#include <sstream>

std::optional<std::string> LoadTextFile(const std::string& filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        SPDLOG_ERROR("failed to open file: {}", filename);
        return {};
    }
    std::stringstream text;
    text << fin.rdbuf();
    return text.str();
}
```

---

## Shader class design

- `Shader` 클래스 설계
  - OpenGL shader object를 가지고 있다
  - 인스턴스가 생성될 때 로딩할 파일명을 입력받자
  - 입력된 파일명으로부터 인스턴스 생성이 실패하면 메모리 할당 해제
  - **C++11 smart pointer 활용**

---

## Smart pointer

- Smart pointer
  - C++11 부터 사용 가능한 **좀 더 안전한** 포인터
  - 메모리 할당을 받을 때 **소유권에 대한 정보가 있다**
  - 명시적인 `delete` 구문이 필요없다
  - std::unique_ptr<>: 해당 메모리 블록을 단독으로 소유
  - std::shared_ptr<>: 해당 메모리 블록의 소유권을 공유
  - std::weak_ptr<>: 해당 메모리 소유권은 없지만 접근은 가능

---

## Smart pointer

- 어떻게 더 안전한가?
  - 일반 포인터를 이용하는 경우: 반드시 메모리 해제를 해줘야 함

```cpp
{
  int* a = new int;
  int* b = a;

  // ...
  // a 메모리 블록 만큼의 memory leak 발생
}
```

---

## Smart pointer

- 어떻게 더 안전한가?
  - 일반 포인터를 이용하는 경우: 메모리를 두번 해제하면 문제 발생

```cpp
{
  int* a = new int;
  int* b = a;

  // ...

  delete a;
  delete b; // 두 번 메모리를 해제하여 문제 발생
}
```

---

## Smart pointer

- 어떻게 더 안전한가?
  - unique_ptr<>를 사용하는 경우: 소유권을 가진 인스턴스가
    스코프 밖으로 벗어났을때 메모리 자동 해제

```cpp
{
  std::unique_ptr<int> a = std::make_unique();
  int* b = a.get();

  // ...
}
```

---

## Smart pointer

- 어떻게 더 안전한가?
  - unique_ptr<>를 사용하는 경우: 소유권을 가진 인스턴스가
    일반적인 방법으로 다른 쪽에 소유권을 이전하려는 경우 에러 발생

```cpp
{
  std::unique_ptr<int> a = std::make_unique();
  std::unique_ptr<int> b = a; // error 발생

  // ...
}
```

---

## Smart pointer

- 어떻게 더 안전한가?
  - unique_ptr<>를 사용하는 경우: `std::move()` 함수를 사용해서
    명시적으로 소유권 이전 가능. 대신 이전에 소유권을 가진 인스턴스는
    `nullptr`를 갖게됨

```cpp
{
  std::unique_ptr<int> a = std::make_unique();
  std::unique_ptr<int> b = std::move(a);

  // a는 nullptr가 되어 더이상 사용 불가능
}
```

---

## `Shader` 클래스 설계

- `src/common.h`에 다음 매크로를 추가
  - `std::unique_ptr` 대신 `클래스이름UPtr` 사용

```cpp
#define CLASS_PTR(klassName) \
class klassName; \
using klassName ## UPtr = std::unique_ptr<klassName>; \
using klassName ## Ptr = std::shared_ptr<klassName>; \
using klassName ## WPtr = std::weak_ptr<klassName>;
```

---

## `Shader` 클래스 설계

- `src/shader.h` 생성

```cpp
#ifndef __SHADER_H__
#define __SHADER_H__

#include "common.h"

// ... 본문은 여기에

#endif // __SHADER_H__
```

---

## `Shader` 클래스 설계

- `Shader` 클래스 정의를 `src/shader.h`에 추가 

```cpp
CLASS_PTR(Shader);
class Shader {
public:
  static ShaderUPtr CreateFromFile(const std::string& filename,
    GLenum shaderType);

  ~Shader();
  uint32_t Get() const { return m_shader; }    
private:
  Shader() {}
  bool LoadFile(const std::string& filename, GLenum shaderType);
  uint32_t m_shader { 0 };
};
```

---

## `Shader` 클래스 설계

- 이렇게 설계된 이유
  - 생성자가 `private`인 이유: `CreateFromFile()` 함수 외에
    다른 방식의 `Shader` 인스턴스 생성을 막기 위해서
  - `Get()`은 있는데 `Set()`는 없는 이유:
    shader 오브젝트의 생성 관리는 `Shader` 내부에서만 관리
  - `LoadFile()`이 `bool`을 리턴하는 이유: 생성에 실패할 경우
    `false`를 리턴하기 위해서

---

## `Shader` 클래스 구현

- `src/shader.cpp` 생성

```cpp
#include "shader.h"
```

---

## `Shader` 클래스 구현

- `CreateFromFile()` 구현

```cpp
ShaderUPtr Shader::CreateFromFile(const std::string& filename,
  GLenum shaderType) {
  auto shader = std::unique_ptr<Shader>(new Shader());
  if (!shader->LoadFile(filename, shaderType))
    return nullptr;
  return std::move(shader);
}
```

---

## `Shader` 클래스 구현

- `LoadFile()` 구현
  - 파일 로딩 실패시 `false` 리턴
  - 성공시 로딩된 텍스트 포인터 및 길이 가져오기

```cpp
bool Shader::LoadFile(const std::string& filename, GLenum shaderType) {
  auto result = LoadTextFile(filename);
  if (!result.has_value())
    return false;

  auto& code = result.value();
  const char* codePtr = code.c_str();
  int32_t codeLength = (int32_t)code.length();
```

---

## `Shader` 클래스 구현

- `LoadFile()` 구현
  - `glCreateShader()`를 이용한 shader 오브젝트 생성
  - `glShaderSource()`로 소스코드 입력
  - `glCompileShader()`로 shader 컴파일

```cpp
// create and compile shader
m_shader = glCreateShader(shaderType);
glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
glCompileShader(m_shader);
```

---

## `Shader` 클래스 구현

- `LoadFile()` 구현
  - `glGetShaderiv()`로 컴파일 상태 조회
  - 만약에 성공이 아니라면 `glGetShaderInfoLog()`로 에러 로그 가져오기

```cpp
  // check compile error
  int success = 0;
  glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
    SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
    SPDLOG_ERROR("reason: {}", infoLog);
    return false;
  }
  return true;
}
```
---

## `Shader` 클래스 구현

- 소멸자 `~Shader` 구현
  - `m_shader`는 처음에 `0`으로 초기화
  - `m_shader`에 `0`이 아닌 다른 값이 들어가 있다면 `glDeleteShader()`를
    호출하여 shader object 제거

```cpp
Shader::~Shader() {
  if (m_shader) {
    glDeleteShader(m_shader);
  }
}
```

---

## `Shader` 클래스 테스트

- `src/main.cpp`에서 `shader.h` 포함

```cpp
#include "common.h"
#include "shader.h"
```

---

## `Shader` 클래스 테스트

- `Shader` 클래스 생성 및 shader 코드 컴파일 테스트
- `src/main.cpp`에서 GLAD 초기화 이후 아래 코드를 추가하여 테스트

```cpp
auto vertexShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
auto fragmentShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
SPDLOG_INFO("vertex shader id: {}", vertexShader->Get());
SPDLOG_INFO("fragment shader id: {}", fragmentShader->Get());
```

---

## VSCode GLSL Extension

- Extension 탭에서 `shader`로 검색
- Shader languages support for VS Code 설치
  - glsl 코드 syntax highlight

---

## Vertex shader code

- 가장 단순한 vertex shader 작성
- `shader/simple.vs`

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
  gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}
```

---

## Fragment shader code

- 가장 단순한 fragment shader 작성
- `shader/simple.fs`

```glsl
#version 330 core
out vec4 fragColor;

void main() {
  fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
```

---

## 빌드 세팅

- `CMakeLists.txt`에 방금 작성한 파일들을 추가

```cmake
add_executable(${PROJECT_NAME}
  src/main.cpp
  src/common.cpp src/common.h
  src/shader.cpp src/shader.h
  )
```

---

## 실행 결과

- 로딩 실패 메세지 출력 후 실행 멈춤

```console
[2021-01-11 19:59:26.883] [error] [common.cpp:8] failed to open file: ./shader/simple.vs
[2021-01-11 19:59:26.883] [error] [common.cpp:8] failed to open file: ./shader/simple.fs
```

---

## 실행 결과

- `.vscode/settings.json`를 열어서 다음과 같이 디버깅 환경변수를 추가

```json [3-5]
{
  "C_Cpp.intelliSenseEngineFallback": "Enabled",
  "cmake.debugConfig": {
    "cwd": "${workspaceFolder}"
  }
}
```

---

## 실행 결과

- 로딩 및 컴파일이 성공하면 shader object id (정수)가 출력는 것을 확인

```console
[2021-01-11 20:01:18.493] [info] [main.cpp:71] vertex shader id: 1
[2021-01-11 20:01:18.493] [info] [main.cpp:72] fragment shader id: 2
```

---

## OpenGL Remarks (3)

- `glCreateShader()`: OpenGL shader object 생성
- `glShaderSource()`: shader에 소스 코드 설정
- `glCompileShader()`: shader 컴파일

---

## OpenGL Remarks (4)

- `glGetShaderiv()`: shader에 대한 정수형 정보를 얻어옴
- `glGetShaderInfoLog()`: shader에 대한 로그를 얻어옴. 컴파일 에러 얻어내는 용도로 사용
- `glDeleteShader()`: shader object 제거

---

## Program class design

- `Program` 클래스 설계
  - vertex shader, fragment shader를 연결한 pipeline program
  - 이 program을 이용해서 최종적으로 그림을 그린다
  - 두 개의 shader를 입력 받아서 program을 링크시킨다
  - 싱크에 성공하면 OpenGL program object를 생성
  - 실패하면 메모리 할당 해제

---

## `Program` 클래스 설계

- `src/program.h` 생성

```cpp
#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "common.h"
#include "shader.h"

// ... 본문은 여기에

#endif // __PROGRAM_H__
```

---

## `Program` 클래스 설계

- `Program` 클래스 정의를 `src/program.h`에 추가 

```cpp
CLASS_PTR(Program)
class Program {
public:
    static ProgramUPtr Create(
        const std::vector<ShaderPtr>& shaders);

    ~Program();
    uint32_t Get() const { return m_program; }    
private:
    Program() {}
    bool Link(
        const std::vector<ShaderPtr>& shaders);
    uint32_t m_program { 0 };
};
```

---

## `Program` 클래스 설계

- 이렇게 설계된 이유
  - vertex, fragment shader 외에 여러 개의 `Shader`를 링크할 수도 있게 함
  - `Shader` 인스턴스 인자는 필요하지만 소유할 필요는 없음
  - `Shader` 인스턴스는 다른 `Program` 인스턴스를 만드는 데 재사용할 수도 있음
  - 따라서 shared pointer를 사용: `ShaderPtr`

---

## `Program` 클래스 구현

- `src/program.cpp` 생성

```cpp
#include "program.h"
```

---

## `Program` 클래스 구현

- `Create()` 구현

```cpp
ProgramUPtr Program::Create(
  const std::vector<ShaderPtr>& shaders) {
  auto program = ProgramUPtr(new Program());
  if (!program->Link(shaders))
    return nullptr;
  return std::move(program);
}
```

---

## `Program` 클래스 구현

- `Link()` 구현
  - `glCreateProgram()`으로 새로운 OpenGL program object 생성
  - `glAttachShader()`로 program에 shader를 붙이기
  - `glLinkProgram()`으로 program 링크

```cpp
bool Program::Link(
  const std::vector<ShaderPtr>& shaders) {
  m_program = glCreateProgram();
  for (auto& shader: shaders)
    glAttachShader(m_program, shader->Get());
  glLinkProgram(m_program);
```

---

## `Program` 클래스 구현

- `Link()` 구현
  - `glGetProgramiv()`로 프로그램 링크 상태 확인
  - 링크에 실패했다면 `glGetProgramInfoLog()`로 에러 로그 가져오기

```cpp
  int success = 0;
  glGetProgramiv(m_program, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetProgramInfoLog(m_program, 1024, nullptr, infoLog);
    SPDLOG_ERROR("failed to link program: {}", infoLog);
    return false;
  }
  return true;
}
```

---

## `Program` 클래스 구현

- 소멸자 구현
  - `glDeleteProgram()`으로 OpenGL program object 제거

```cpp
Program::~Program() {
  if (m_program) {
    glDeleteProgram(m_program);
  }
}
```

---

## `Program` 클래스 테스트

- `src/main.cpp`에 `program.h` 포함

```cpp [3]
#include "common.h"
#include "shader.h"
#include "program.h"
```

---

## `Program` 클래스 테스트

- `src/main.cpp`에서 `Shader` 인스턴스 생성 후 `Program` 인스턴스 생성
  - `Shader` 인스턴스가 `unique_ptr`에서 `shared_ptr`로 변환되었음을 유의

```cpp
ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

auto program = Program::Create({fragShader, vertShader});
SPDLOG_INFO("program id: {}", program->Get());
```

---

## 빌드 세팅

- `CMakeLists.txt`에 `program.*` 파일들 추가

```cmake
ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

auto program = Program::Create({fragShader, vertShader});
SPDLOG_INFO("program id: {}", program->Get());
```

---

## 실행 결과

- 빌드 및 실행 결과
  - 링크가 성공하여 program object id가 로그에 출력되는 것을 확인

```console
[2021-01-12 11:33:58.368] [info] [main.cpp:72] vertex shader id: 1
[2021-01-12 11:33:58.368] [info] [main.cpp:73] fragment shader id: 2
[2021-01-12 11:33:58.383] [info] [main.cpp:76] program id: 3
```

---

## OpenGL Remarks (5)

- `glCreateProgram()`: OpenGL program object 생성
- `glAttachShader()`: program에 shader를 붙이기
- `glLinkProgram()`: program 링크

---

## OpenGL Remarks (6)

- `glGetProgramiv()`: program에 대한 정수형 정보를 얻어옴
- `glGetProgramInfoLog()`: program에 대한 로그를 얻어옴. 링크 에러 얻어내는 용도로 사용
- `glDeleteProgram()`: program object 제거

---

## 리팩토링

- 프로그램 라이프사이클을 고려하여 코드 리팩토링
  - GLFW / OpenGL Context / GLAD 초기화
  - **그림을 그리기 위한 OpenGL objects 생성 (shader / program)**
  - **렌더링**
  - **OpenGL objects 제거**
  - GLFW 종료 / 프로그램 종료
- OpenGL object 들을 관리하고 렌더링하는 코드를 분리하자

---

## `Context` 클래스 디자인

- `src/context.h` 생성

```cpp
#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"

// ... context 클래스 선언

#endif // __CONTEXT_H__
```

---

## `Context` 클래스 디자인

- `src/context.h`에 `Context` 클래스 선언

```cpp
CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();
    void Render();    
private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
};
```

---

## `Context` 클래스 구현

- `src/context.cpp` 생성

```cpp
#include "context.h"
```

---

## `Context` 클래스 구현

- `Create()` 구현

```cpp
ContextUPtr Context::Create() {
  auto context = ContextUPtr(new Context());
  if (!context->Init())
    return nullptr;
  return std::move(context);
}
```

---

## `Context` 클래스 구현

- `Init()`에 `src/main.cpp`에서 테스트하던 코드를 가지고 오기

```cpp
bool Context::Init() {
  ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
  ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
  if (!vertShader || !fragShader)
    return false;
  SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
  SPDLOG_INFO("fragment shader id: {}", fragShader->Get());
```

---

## `Context` 클래스 구현

- `Shader`혹은 `Program` 생성 실패시 `false`를 리턴하여 메모리 할당을 자동 해제
- 한번만 실행해도 되는 `glClearColor()`를 이쪽으로 이동시킴

```cpp
  m_program = Program::Create({fragShader, vertShader});
  if (!m_program)
    return false;
  SPDLOG_INFO("program id: {}", m_program->Get());

  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
  return true;
}
```

---

## `Context` 클래스 구현

- `Render()`가 `glClear()`를 호출하도록 구현

```cpp
void Context::Render() {
  glClear(GL_COLOR_BUFFER_BIT);
}
```

---

## `Context` 클래스 사용

- `CMakeLists.txt`에 `src/context.*`를 추가
- `src/main.cpp`에서 `src/context.h` 포함시키기
- GLAD 초기화까지 완료된 이후 다음과 같이 우리의 context 생성

```cpp
auto context = Context::Create();
if (!context) {
  SPDLOG_ERROR("failed to create context");
  glfwTerminate();
  return -1;
}
```

---

## `Context` 클래스 사용

- 메인 루프에서 이전의 `Render()` 대신 `Context`의 `Render()` 호출
- 메인 루푸 종료 후 `std::unique_ptr`의 `reset()`을 호출하여 메모리 정리
- 빌드 및 실행후 파란 화면 / 로그 확인

```cpp
SPDLOG_INFO("Start main loop");
while (!glfwWindowShouldClose(window)) {
  context->Render();
  glfwSwapBuffers(window);
  glfwPollEvents();
}
context.reset();
```

---

## Shader program 테스트

- 테스트를 위해 `Context::Init()`에서 vertex array object를 생성
  - vertex array object는 Part 2에서 다룰 예정

```cpp [3-5]
  glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

  uint32_t vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  return true;
}
```

---

## Shader program 테스트

- 테스트를 위해 `Context::Render()`에서 draw call
  - 그리기 관련 코드는 Part 2에서 다룰 예정

```cpp [4-5]
void Context::Render() {
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(m_program->Get());
  glDrawArrays(GL_POINTS, 0, 1);
}
```

---

## 실행 결과

- 화면 가운데에 빨간 점 확인

![point draw](/opengl_course/note/images/04_point_draw.png)

---

## Additional note

- `shader/simple.vs`에서 정점의 위치를 바꿔보자

```glsl
gl_Position = vec4(0.5, 0.5, 0.0, 1.0);
```

- `shader/simple.fs`에서 픽셀의 색상을 바꿔보자

```glsl
fragColor = vec4(1.0, 1.0, 1.0, 1.0);
```

---

## Congratulation!
### 수고하셨습니다!
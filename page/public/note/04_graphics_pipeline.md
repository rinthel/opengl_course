# Graphics Pipeline

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

```cmake
set(PROJECT_NAME first_opengl_example)
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

```cpp
// glfw 윈도우 생성, 실패하면 에러 출력후 종료
SPDLOG_INFO("Create glfw window");
auto window = glfwCreateWindow(640, 480, "Hello, OpenGL!", nullptr, nullptr);
if (!window) {
    SPDLOG_ERROR("failed to create glfw window");
    glfwTerminate();
    return -1;
}
glfwMakeContextCurrent(window); // new line
```

---

## OpenGL 초기화 (3)

- GLAD 라이브러리 사용을 위해 `#include` 구문 사용
- 반드시 `#include <GLFW/GLFW.h>` 이전에 추가할 것

```cpp
#include <spdlog/spdlog.h>
#include <glad/glad.h> // new line
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

OnFramebufferSizeChange(window, 640, 480);
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

```cpp
SPDLOG_INFO("Start main loop");
while (!glfwWindowShouldClose(window)) {
    Render(); // new line
    glfwSwapBuffers(window); // new line
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

- `first_opengl_example` 예제를 복사하여 `triangle_example` 예제 생성
- cmake 프로젝트 명 변경
- VSCode로 프로젝트 열기

---

## Vertex shader code

- 가장 단순한 vertex shader 작성
- `shader/vs_simple.glsl`

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
- `shader/fs_simple.glsl`

```glsl
#version 330 core
out vec4 fragColor;

void main() {
  fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
```

---

## Shader code loading

---

## Vertex input

- 정점 데이터 준비

```cpp
float vertices[] = {
  -0.5f, -0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  0.0f, 0.5f, 0.0f
  };
```

---


## Congratulation!
### 수고하셨습니다!
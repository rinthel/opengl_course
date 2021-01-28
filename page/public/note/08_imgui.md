# ImGui

---

## GUI in OpenGL

- OpenGL 화면에 그림을 그리기 위해서는 Shader, Program, VBO,
  VAO, EBO, Uniform, Texture 등이 필요
- Button, Slider, Textfield 등 기본적인 GUI를 구현하는 것에도
  매우 큰 노력이 필요함

---

## GUI in OpenGL

- 두 가지 접근 방식
  - GUI 프레임워크를 사용하여 GUI 화면을 구성하고 그 중에서
    OpenGL surface를 만들어 원하는 장면을 그린다
    - Win32, MFC, QT, Cocoa, Android ...
  - OpenGL 화면 내부에 GUI 콤포넌트를 만들고 이벤트를
    처리할 수 있는 라이브러리를 사용한다

---

## ImGui

- [github.com/ocornut/imgui](https://github.com/ocornut/imgui)
- Omar Cornut가 만든 오픈 소스 라이브러리
- Immediate-Mode GUI
- Graphics API를 이용하여 GUI 콤포넌트를 직접 그리는 기능 제공

---

## ImGUi

![imgui example](https://raw.githubusercontent.com/wiki/ocornut/imgui/web/v149/gallery_TheDragonsTrap-01-thumb.jpg)

---

## ImGUi

![imgui example](https://raw.githubusercontent.com/wiki/ocornut/imgui/web/v176/tracy_profiler.png)

---

## Feature of ImGui

- **Immediate Mode** GUI
- Rendering Backend 분리
  - OpenGL 뿐만 아니라 다양한 Graphics API와 함께 사용 가능
- Graphics programming에 필요한 GUI component
  - Vector editor
  - Color picker
- Nearly zero-dependency
  - 빌드가 간단

---

## Immediate Mode

- 매 렌더링 루프마다 UI를 그린다

```cpp
// ... in render loop
ImGui::Text("Hello, world %d", 123);
if (ImGui::Button("Save"))
    MySaveFunction();
ImGui::InputText("string", buf, IM_ARRAYSIZE(buf));
ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
```

<div>
<img src="https://raw.githubusercontent.com/wiki/ocornut/imgui/web/v175/capture_readme_styles_0001.png" width=40%/>
<img src="https://raw.githubusercontent.com/wiki/ocornut/imgui/web/v175/capture_readme_styles_0002.png" width=40%/>
</div>

---

## 예제 준비

- `transformation_example` 프로젝트를 복사하여 `imgui_example` 준비
- 프로젝트 명 변경 및 빌드 확인

---

## ImGui Install

- [github.com/ocornut/imgui](https://github.com/ocornut/imgui)에 접속
- v1.80 태그 선택 후 코드 다운로드
- 프로젝트 루트에 `imgui` 디렉토리 생성
- 라이브러리 루트의 소스 코드 및 라이센스 파일을 `imgui`에 복사
- `backend` 디렉토리에 있는 `imgui_impl_glfw.*` 파일을 `imgui`에 복사
- `backend` 디렉토리에 있는 `imgui_impl_opengl3.*` 파일을 `imgui`에 복사

---

## ImGui Install

- `Dependency.cmake`에 다음을 추가하고 빌드가 잘 되는지 확인

```cmake
add_library(imgui
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/imgui_impl_glfw.cpp
    imgui/imgui_impl_opengl3.cpp
    )
target_include_directories(imgui PRIVATE ${DEP_INCLUDE_DIR})
add_dependencies(imgui ${DEP_LIST})
set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
set(DEP_LIST ${DEP_LIST} imgui)
set(DEP_LIBS ${DEP_LIBS} imgui)
```

---

## ImGui Initialization

- `src/main.cpp`의 상단에 헤더파일 추가

```cpp [3-4]
#include "common.h"
#include "context.h"
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
```

---

## ImGui Initialization

- OpenGL context 초기화 후 ImGui 초기화

```cpp [10-15]
// glad를 활용한 OpenGL 함수 로딩
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    SPDLOG_ERROR("failed to initialize glad");
    glfwTerminate();
    return -1;
}
auto glVersion = glGetString(GL_VERSION);
SPDLOG_INFO("OpenGL context version: {}", glVersion);

auto imguiContext = ImGui::CreateContext();
ImGui::SetCurrentContext(imguiContext);
ImGui_ImplGlfw_InitForOpenGL(window, false);
ImGui_ImplOpenGL3_Init();
ImGui_ImplOpenGL3_CreateFontsTexture();
ImGui_ImplOpenGL3_CreateDeviceObjects();
```

---

## ImGui Initialization

- 메인 루프 변경

```cpp [3-4,9-10]
while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    context->ProcessInput(window);
    context->Render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}
```

---

## ImGui Initialization

- 종료 전 메모리 반환 코드 추가

```cpp [3-6]
context.reset();

ImGui_ImplOpenGL3_DestroyFontsTexture();
ImGui_ImplOpenGL3_DestroyDeviceObjects();
ImGui_ImplGlfw_Shutdown();
ImGui::DestroyContext(imguiContext);

glfwTerminate();
```

---

## First ImGui Window

- `Context::Render()`에서 테스트 코드 작성

```cpp [3]
#include "context.h"
#include "image.h"
#include <imgui.h>
```

```cpp [2-4]
void Context::Render() {
  if (ImGui::Begin("my first ImGui window")) {
    ImGui::Text("This is first text...");
  }
  ImGui::End();
  // ...
```

---

## First ImGui Window

- 빌드 및 실행

![first imgui window](/opengl_course/note/images/08_first_imgui_window.png)

---

## First ImGui Window

- 빌드 및 실행
  - ImGui가 그리는 UI 화면이 OpenGL 화면에 오버래핑
  - 마우스로 창의 크기를 조절하거나 창을 이동시킬 수 있음
    - 메인 루프에서 실행되는 `ImGui_ImplGlfw_NewFrame()`가
      `GLFWwindow`로부터 화면 크기 및 마우스 상태 등을 업데이트
    - 따라서 별도의 callback 연결 없이 마우스 인터렉션이 가능

---

## UI/Parameter Binding

- 예제의 몇몇 파라미터와 액션을 UI에 연결해보자
  - 카메라 파라미터
  - 카메라 초기화 버튼
  - 클리어 색상

---

## UI/Parameter Binding

- `Context` 클래스에 클리어 색상 멤버 변수 추가

```cpp [4-5]
int m_width {640};
int m_height {480};

// clear color
glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 0.0f) };

// camera parameter
bool m_cameraControl { false };
```

---

## UI/Parameter Binding

- `Context::Render()` 에서 UI 코드 작성

```cpp
if (ImGui::Begin("ui window")) {
  if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor))) {
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
  }
  ImGui::Separator();
  ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
  ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
  ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
  ImGui::Separator();
  if (ImGui::Button("reset camera")) {
    m_cameraYaw = 0.0f;
    m_cameraPitch = 0.0f;
    m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
  }
}
ImGui::End();
```

---

## UI/Parameter Binding

- 빌드 및 실행

![ui parameter binding](/opengl_course/note/images/08_ui_parameter_binding.png)

---

## UI/Parameter Binding

- ImGui UI 코드의 특성
  - 함수 하나가 UI component 하나에 대응
  - boolean 타입의 값을 리턴
  - 리턴 값이 `true`인 경우 해당 UI가 조작되었음을 의미
    - UI 조작 이벤트에 대한 액션 로직을 `if`으로 작성할 수 있음

---

## ImGui Callback Setting

- 현재 ImGui는 별도의 입력 콜백을 설정하지 않아서
  키보드 입력을 지원하지 않는 상태
- 숫자를 직접 입력하거나, 마우스 스크롤,
  ctrl+c / ctrl+v 동작이 가능하도록 해보자

---

## ImGui Callback Setting

- `src/main.cpp`에 새 콜백 함수 추가

```cpp
void OnCharEvent(GLFWwindow* window, unsigned int ch) {
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnScroll(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}
```

---

## ImGui Callback Setting

- 기존 콜백 함수에서 ImGui 콜백 호출

```cpp [3]
void OnKeyEvent(GLFWwindow* window,
  int key, int scancode, int action, int mods) {
  ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
  // ...
```

```cpp [2]
void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {
  ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);
  // ...
```

---

## ImGui Callback Setting

- `main()` 에서 콜백 함수 설정

```cpp [5,8]
// ... in main() function
OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
glfwSetKeyCallback(window, OnKeyEvent);
glfwSetCharCallback(window, OnCharEvent);
glfwSetCursorPosCallback(window, OnCursorPos);
glfwSetMouseButtonCallback(window, OnMouseButton);
glfwSetScrollCallback(window, OnScroll);
```

---

## ImGui Callback Setting

- 빌드 및 실행
  - ImGui UI의 숫자를 더블 클릭해서 키보드로 숫자 입력 가능
  - 입력된 숫자를 선택하여 copy & paste 할 수 있음
  - tab 키를 사용하여 이전/이후 UI 콤포넌트로 커서를 움직일 수 있음

---

## More On ImGui

- 별도의 문서가 없음
- 제공되는 거의 모든 UI 콤포넌트에 대한 정보 및 사용법은
  `imgui.h` 파일을 참고

---

## Congratulation!
### 수고하셨습니다!

# Development Environment

Computer Graphics with OpenGL

---

## 목표

- Cross-platform 개발환경 구축하기
- CMake dependency 관리

---

## VSCode

- 마이크로소프트가 만든 다기능 텍스트 에디터
- 가볍다 / 크로스플랫폼 / 수많은 익스텐션
- [https://code.visualstudio.com](https://code.visualstudio.com)
- Windows
  - 콘솔창에서 code 명령어로 vscode 열기 기능 체크 확인
- macOS, Linux
  - 설치 및 실행 후 <span class="keystroke">ctrl-shift-P</span> 를 눌러서 커멘드 팔렛 열기
  - `shell` 로 검색하여 동일한 기능 활성화 가능

---

## Visual Studio 2019 Community Edition

- 마이크로소프트가 만든 개발 환경
- C++ 컴파일러 및 링커 등이 여기에 포함되어 있음
- [https://visualstudio.microsoft.com/ko/vs/community/](https://visualstudio.microsoft.com/ko/vs/community/)

---

## Build tools for Visual Studio 2019

- Windows용 Visual C++ 컴파일러 (cl) 및 링커, Windows SDK
- [https://visualstudio.microsoft.com/ko/visual-cpp-build-tools/](https://visualstudio.microsoft.com/ko/visual-cpp-build-tools/)

---

## XCode

- macOS용 C++ 컴파일러 (clang)
```console
> xcode-select --install
```

---

## GCC

- Linux용 C++ 컴파일러
```console
> sudo apt-get update
> sudo apt-get install gcc-9
```

---

## CMake

- [https://cmake.org](https://cmake.org/)
- Windows: 인스톨러 이용
- macOS: brew 이용
```console
> brew install cmake
```
- Linux (ubuntu 기준)
```console
> sudo apt-get install cmake
```

---

## 프로젝트 시작하기

- 새 프로젝트를 위한 디렉토리를 생성한 뒤 VSCode 실행

```console
> mkdir cmake_project_example
> cd cmake_project_example
> code .
```

---

## VSCode 화면

![](/opengl_course/note/images/03_vscode.png)

---

## VSCode 주요 기능 훑어보기

- 화면 구성
  - Sidebar (화면 좌측)
    - 파일 탐색, 검색, 소스 컨트롤, 디버그, 익스텐션
  - Editor (화면 우측)
    - 문서 에디팅 화면
  - 화면 하단
    - 터미널, 에러, 출력 화면 등

---

## VSCode 주요 기능 훑어보기

- 주로 쓰는 기능
  - 파일 이동: ctrl+P
  - 커멘트 팔렛: ctrl+shift+P
  - 같은 단어를 선택한 커서 추가: ctrl+D
  - cmake:
    - 빌드: F7
    - 디버그 실행: ctrl+F5

---

## VSCode Extension 설치

- 익스텐션 탭을 열고 다음을 검색 후 설치
  - C/C++
  - C++ Intellisense
  - CMake
  - CMake Tools

---

## CMake 프로젝트 시작하기

- `src/main.cpp` 파일 작성

```cpp
#include <iostream>

int main(int argc, const char** argv) {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}
```

---

## CMake 프로젝트 시작하기

- `CMakeLists.txt` 파일 작성

```cmake
cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME cmake_project_example)
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} src/main.cpp)
```

---

## CMake의 작동 방식

- Meta-project description
  - 각 플랫폼 / 선호하는 빌드 방식에 맞는 프로젝트 파일을 생성
  - Windows의 경우 visual studio project
  - Linux의 경우 Makefile
  - macOS의 경우 XCode / Makefile
  - Android의 경우 Ninja build
  - 기타 등등...

---

## CMake 커맨드

- configure

```console
> cmake -Bbuild . -DCMAKE_BUILD_TYPE=[Debug|Release]
```

- build

```console
> cmake --build build --config Debug
```

- 귀찮다

---

## VSCode + CMake 커맨드

- 익스텐션 기능을 통해 CMake 커맨드를 실행시켜줄 수 있음
- 여러 가지 방식을 지원
  - 커맨드 팔렛에서 `cmake`로 검색하여 원하는 기능 실행
  - 화면 최하단 상태 바의 메뉴로 원하는 기능 실행
  - 화면 좌측에 추가로 생긴 cmake 탭에서 원하는 기능 실행

---

## VSCode + CMake 프로젝트 설정

- 커멘드 팔렛을 열고 (ctrl+shift+P) cmake 검색
- 'cmake: configure' 명령 선택
- 이 프로젝트를 빌드할 도구셋 선택
  - Windows+Intel: `Visual Studio 2019 Release - x86_64`
  - Windows+AMD: `Visual Studio 2019 Release - amd64`
  - build 디렉토리 자동 생성
- Intellisense 구성 허용
  - `.vscode` 디렉토리 생성

---

## VSCode + CMake 프로젝트 설정

- 빌드: F7
- 디버그 실행: ctrl+F5
- Debug Console 화면에 `Hello, World!` 출력 확인

```text
Hello World!
```

---

## VCS

- Version Control System / Source Control
- 소스 코드의 생성 / 삭제 / 수정 내역을 기록하여 소프트웨어의 버전을 관리해주는 시스템
- svn, perforce, mercury, git...
- 다양한 장점
  - 코드의 버전 관리
  - 코드의 백업
  - 다른 사람들과 코드를 공유
  - 여러 컴퓨터에서 코드를 사용할 수 있음

---

## git

- Linux를 만든 Linus B. Torvalds 가 여러 Linux 개발자들과 코드 관리를 하기 위해 개발
- 가장 대표적인 VCS이면서, 가장 많이 사용되고 있음
- 다양한 호스팅 업체들 서비스중
  - [github](https://github.com/)
  - [gitlab](https://about.gitlab.com/)
  - [bitbucket](https://bitbucket.org/) 등 
- 코드 관리, 협업에서 가장 핵심적인 역할을 하는 도구

---

## git 용어 정리

- repository: 저장소, 하나의 프로젝트
- clone: 저장소 전체를 내려받기
- commit: 코드 변경 내역 저장 단위
  - commit hash: 각각의 commit 마다 매겨지는 고유 hash id 값
- push: 변경 내역 업로드
- pull: 변경 내역 다운로드

---

## git 시작하기

- 새로운 git 프로젝트 초기화
- 프로젝트 루트에서 다음을 실행

```console
> git init
```

- 실행하면 `.git` 디렉토리가 생성되고 git 실행에 필요한 데이터가 여기 저장된다

---

## git status

- 현재의 git 프로젝트 상태를 확인하는 명령

```console
$ git status
On branch master

No commits yet  

Untracked files:
  (use "git add <file>..." to include in what will be committed)

        CMakeLists.txt
        build/
        src/

nothing added to commit but untracked files present (use "git add" to track)
```

- VSCode의 Source Control 탭에도 변경사항에 대한 정보를 확인할 수 있음

---

## gitignore

- 관리 및 백업이 필요치 않은 파일들을 제외시키는 작업
- `.gitignore` 파일에 관리 대상에서 제외할 리스트를 작성

```
build/
```

- Sidebar의 build 디렉토리 이하의 모든 파일이 회색이 된 것을 확인

---

## git commit

- 파일 변경 사항을 모두 staging하고 변경 내역을 저장

```console
> git add .
> git commit -m first commit
```

- `git log`를 사용하여 현재까지 적용된 commit들을 확인할 수 있음

---

## git client

- git을 보다 편하게 사용할 수 있게 도와주는 GUI 클라이언트
- SourceTree, GitHub Desktop, TortoiseGit 등등...

---

## Fork git client

- [https://git-fork.com/](https://git-fork.com/)
- macOS / Windows 크로스 플랫폼
- 깔끔한 Native UI
- 개인적으로 사용해본 git client 중 가장 빠르고 말썽이 없음

---

## Fork git client

![](/opengl_course/note/images/03_fork.png)

---

## github

- 가장 대표적인 git 호스팅 서비스
  - git을 이용한 프로젝트 호스팅 기능
  - issue / pull request 등 협업에 필요한 기능
  - fork 기능: github에 올려진 프로젝트를 내 계정으로 복사해서 수정 가능
  - public / private repository: 소스 코드를 공개하거나 나만 볼 수 있게 할 수 있음
  - 수많은 오픈 소스 프로젝트들이 github을 중심으로 작업 / 배포되는 중
- [github.com](https://github.com) 에서 계정부터 만들자

---

## github에서 repository 만들기

- 우상단의 + 버튼을 클릭하고 `New repository` 선택
- repository 이름 설정: `cmake_project_example`
- Public / Private 선택
- Create project 클릭하여 생성

---

## github에 프로젝트 올리기

```console
> git remote add origin \
  https://github.com/${계정명}/cmake_project_example.git
> git branch -M main
> git push -u origin main
```

- 완료되면 github repository 페이지에서 새로고침하여 코드가 업로드된걸 확인해보자

---

## 일반적인 git 작업의 흐름

- 코드 다운로드
  - 코드 클론 (일괄 다운로드)
  - 브랜치 풀 (pull)
- 브랜치 생성
- 브랜치 체크아웃
- 코드 작성
- 코드 커밋
- 브랜치 푸시
- 브랜치 머지

---

## CMake를 이용한 외부 라이브러리 사용하기

- `CMakeLists.txt`에 다음을 추가 (1)

```cmake
# ExternalProject 관련 명령어 셋 추가
include(ExternalProject)

# Dependency 관련 변수 설정
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)
```

---

## CMake를 이용한 외부 라이브러리 사용하기

- `CMakeLists.txt`에 다음을 추가 (2)

```cmake
# spdlog: fast logger library
ExternalProject_Add(
    dep-spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.x"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
    TEST_COMMAND ""
)
# Dependency 리스트 및 라이브러리 파일 리스트 추가
set(DEP_LIST ${DEP_LIST} dep-spdlog)
set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)
```

---

## CMake를 이용한 외부 라이브러리 사용하기

- `CMakeLists.txt`에 다음을 추가 (3)

```cmake
# 우리 프로젝트에 include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

# Dependency들이 먼저 build 될 수 있게 관계 설정
add_dependencies(${PROJECT_NAME} ${DEP_LIST})
```

---

### spdlog로 로그 출력해보기

- `src/main.cpp`를 다음과 같이 수정

```cpp
#include <spdlog/spdlog.h>

int main(int argc, const char** argv) {
    SPDLOG_INFO("Hello, world!");
    return 0;
}
```

- 빌드 후 결과 확인

```text
[2021-01-02 19:45:11.051] [info] [main.cpp:4] Hello, world!
```

---

## CMake 파일 나눠서 관리하기

- `Dependency.cmake` 파일을 생성
- 앞의 cmake 추가 (1) (2) 번 내용을 옮겨넣기

```cmake
# ExternalProject 관련 명령어 셋 추가
include(ExternalProject)

# Dependency 관련 변수 설정
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)

# spdlog: fast logger library
ExternalProject_Add(
    dep-spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.x"
    GIT_SHALLOW 1
    UPDATE_COMMAND "" TEST_COMMAND "" PATCH_COMMAND ""
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
    )
# Dependency 리스트 및 라이브러리 파일 리스트 추가
set(DEP_LIST ${DEP_LIST} dep-spdlog)
set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)
```

---

## CMake 파일 나눠서 관리하기

- `CMakeLists.txt` 파일에서 `Dependency.cmake` 불러오기

```cmake
cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME cmake_project_example)
set(CMAKE_CXX_STANDARD 17)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} src/main.cpp)

include(Dependency.cmake)

# 우리 프로젝트에 include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

# Dependency들이 먼저 build 될 수 있게 관계 설정
add_dependencies(${PROJECT_NAME} ${DEP_LIST})
```

---

## CMake 파일 나눠서 관리하기

- 빌드 후 재실행해서 프로젝트 세팅이 올바른지 확인

---

## GLFW dependency 추가하기

- OpenGL은 3D 그래픽을 위한 API일 뿐
- 화면에 그림을 그리기 위해서는 다음과 같은 작업이 추가적으로 필요함
  - 윈도우 생성하기 (당연히 OS에 따라 윈도우 생성 방식이 다름)
  - 윈도우에 OpenGL을 위한 surface 생성하고 연결하기
  - 키보드 / 마우스 입력 연결하기

---

## GLFW dependency 추가하기

- [GLFW](https://www.glfw.org/)
  - Open-source, cross-platform library for
    - creating windows, contexts, and surfaces,
    - receiving input and events

---

## GLFW dependency 추가하기


- `Dependency.cmake`에 다음을 추가

```cmake
# glfw
ExternalProject_Add(
    dep_glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.3.2"
    GIT_SHALLOW 1
    UPDATE_COMMAND "" PATCH_COMMAND "" TEST_COMMAND ""
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
        -DGLFW_BUILD_EXAMPLES=OFF
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF
    )
set(DEP_LIST ${DEP_LIST} dep_glfw)
set(DEP_LIBS ${DEP_LIBS} glfw3)
```

---

## GLFW로 윈도우를 생성하기

- `CMakeLists.txt`에 환경 변수 추가

```cmake [6-8]
cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME cmake_project_example)
set(CMAKE_CXX_STANDARD 17)

set(WINDOW_NAME "Hello, OpenGL!")
set(WINDOW_WIDTH 640)
set(WINDOW_HEIGHT 480)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} src/main.cpp)
```

---

## GLFW로 윈도우를 생성하기

- 환경 변수를 predefined macro로 프로젝트에 추가

```cmake [5-9]
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

target_compile_definitions(${PROJECT_NAME} PUBLIC
  WINDOW_NAME="${WINDOW_NAME}"
  WINDOW_WIDTH=${WINDOW_WIDTH}
  WINDOW_HEIGHT=${WINDOW_HEIGHT}
  )
```


---

## GLFW로 윈도우를 생성하기

- `src/main.cpp`를 다음과 같이 수정 (1)

```cpp
#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

int main(int argc, const char** argv) {
    // 시작을 알리는 로그
    SPDLOG_INFO("Start program");
```

---

## GLFW로 윈도우를 생성하기

- `src/main.cpp`를 다음과 같이 수정 (2)

```cpp
    // glfw 라이브러리 초기화, 실패하면 에러 출력후 종료
    SPDLOG_INFO("Initialize glfw");
    if (!glfwInit()) {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
        return -1;
    }
```

---

## GLFW로 윈도우를 생성하기

- `src/main.cpp`를 다음과 같이 수정 (3)

```cpp
    // glfw 윈도우 생성, 실패하면 에러 출력후 종료
    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
      nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        return -1;
    }
```

---

## GLFW로 윈도우를 생성하기

- `src/main.cpp`를 다음과 같이 수정 (4)

```cpp
    // glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
```

---

## GLFW로 윈도우를 생성하기

- 빌드 후 결과 확인
  - 빈 화면의 윈도우가 생성
  - 윈도우 타이틀이 `Hello, OpenGL!`로 설정
  - 윈도우의 close 버튼을 누르면 프로그램 종료
  - Debug console에 `SPDLOG_INFO()`로 출력한 로그 내역이 나오는 것 확인

```text
[2021-01-02 20:21:37.402] [info] [main.cpp:6] Start program
[2021-01-02 20:21:37.404] [info] [main.cpp:9] Initialize glfw
[2021-01-02 20:21:37.544] [info] [main.cpp:18] Create glfw window
[2021-01-02 20:21:37.824] [info] [main.cpp:27] Start main loop
```

---

## CMake debug tip

- `message()` 를 활용하여 중간의 변수값을 출력해볼 수 있다
- `set(CMAKE_VERBOSE_MAKEFILE ON)` 을 활용하면 세부적인 빌드 커맨드를 볼 수 있다

---

## Congratulation!
### 수고하셨습니다!
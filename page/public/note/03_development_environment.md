# 개발 환경

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
  - <span class="keystroke">ctrl-shift-P</span> 를 눌러서 커멘드 팔렛 열기
  - `shell` 로 검색하여 동일한 기능 설치

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

```console
> mkdir cmake_project_example
> cd cmake_project_example
> code .
```

---

## VSCode 주요 기능 훑어보기

- 화면 구성
  - Sidebar (화면 좌측)
    - 파일 탐색, 검색, SCM, 디버그, 익스텐션
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

- 익스텐션 설치하기
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
cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME cmake_project_example)
set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} src/main.cpp)
```

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

- 여러 가지 방식을 지원
  - 커맨드 팔렛에서 `cmake`로 검색하여 원하는 기능 실행
  - 화면 최하단 상태 바의 메뉴로 원하는 기능 실행
  - 화면 좌측에 추가로 생긴 cmake 탭에서 원하는 기능 실행

---

## VCS

- 소스 코드의 생성 / 삭제 / 수정 내역을 기록하여 소프트웨어의 버전을 관리해주는 시스템
- svn, perforce, mercury, git...

---

## git

- 현재 가장 대표적인 VCS
- Linux를 만든 Linus B. Torvalds 가 여러 Linux 개발자들과 코드 관리를 하기 위해 개발
- 다양한 호스팅 업체들 서비스중
  - [github](https://github.com/)
  - [gitlab](https://about.gitlab.com/)
  - [bitbucket](https://bitbucket.org/) 등 
- 코드 관리, 협업에서 가장 핵심적인 역할을 하는 도구

---

## 일반적인 git 작업의 흐름

- 코드 다운로드
  - 코드 클론 (일괄 다운로드)
  - 브랜치 풀 (pull)
- 브랜치 생성
- 브랜치 체크아웃
- 코드 작성
- 코드 커밋 (commit = 저장)
- 브랜치 푸시 (push = 업로드)
- 브랜치 머지 (merge = 코드 합치기)

---

## git 시작하기

- 프로젝트 루트에서 다음을 실행

```console
> git init
```

---

## gitignore

- `.gitignore` 파일을 작성

```
build/
```

- Sidebar의 build 디렉토리 이하의 모든 파일이 회색이 된 것을 확인

---

## git commit

- 파일 변경 사항을 모두 staging하고 변경 내역 저장

```console
> git add .
> git commit -m first commit
```


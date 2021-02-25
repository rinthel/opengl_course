# OpenGL Course by Rinthel

## Introduction

- 강원대학교 2021년 1학기 컴퓨터 그래픽스 수업을 위한 저장소입니다
- 수업을 위한 슬라이드 노트 및 예제 코드를 저장하고 있습니다

## How To Use

### Slide pages

- `node`, `yarn`이 설치되어 있어야 합니다
- 윈도우의 경우 `cmd`, 맥이나 리눅스의 경우 `bash`를 통해 터미널을 엽니다
- 다음을 입력하여 dependency 설치 및 빌드를 수행합니다

```bash
$ cd page
$ yarn
$ yarn start
```

- 성공적으로 실행이 되면 chrome 등의 브라우저에서 슬라이드 노트를 조회할 수 있습니다

### Example projects

- `cmake`, c++ 컴파일러가 필요합니다
  - 윈도우의 경우 Build tools for Visual Studio, Visual Studio 2019 community edition
  - 맥의 경우 XCode
  - 리눅스의 경우 GCC
- 빌드하길 원하는 디렉토리로 이동한 뒤 `cmake`를 통해 빌드합니다
```bash
$ cd projects/03_development_environment/cmake_project_example
$ cmake . -Bbuild
$ cmake --build build
```
- 빌드가 완료되면 `build` 디렉토리에 생성된 실행파일을 실행할 수 있습니다
```bash
$ ./build/cmake_project_example
```

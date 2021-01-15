# Texture

---

## Texture

- 개요
  - vertex에 color를 지정하는 방법만으로 그럴싸한 3D 물체를 표현하기는 힘들다
    - 너무 많은 수의 vertex가 필요할 것
  - Texture
    - 아이디어: 면에 이미지로된 스티커를 입히자
    - 저렴한 방식으로 고품질의 렌더링 결과를 생성할 수 있음

---

## Texture

- 텍스처로 사용할 이미지
  - https://learnopengl.com/img/textures/wall.jpg

<img src="https://learnopengl.com/img/textures/wall.jpg" style="width:30%"/>

---

## Texture

- 이미지를 텍스처로 사용하여 삼각형 채우기

![texture example](/opengl_course/note/images/06_texture_example.png)

---

## Texture Coordinates

- 텍스처 좌표
  - 각 정점 별로 이미지의 어느 지점을 입힐 것인지 별도의 좌표를 지정
  - [0, 1] 사이로 정규화된 (normalized) 좌표계
  - 이미지의 좌하단 지점이 원점임을 주의

<img src="/opengl_course/note/images/06_texture_coordinates.png" style="width:30%"/>

---

## Texture Coordinates

- 텍스처가 삼각형에 입혀지는 과정
  - 텍스처 좌표가 정점의 위치와 함께 vertex attributes 형태로 vertex shader에 입력됨
  - Rasterization 과정을 거쳐서 각 픽셀별 텍스처 좌표값이 계산됨
  - fragment shader에서 텍스처 좌표를 바탕으로 텍스처 이미지의 색상 값을 가지고 옴

---

## Texture Wrapping

- [0, 1] 범위를 벗어난 텍스처 좌표값을 처리하는 옵션

![texture wrapping](/opengl_course/note/images/06_texture_wrapping.png)

---

## Texture Filtering

- Filtering option
  - 텍스처로 사용할 이미지의 크기가 화면보다 크거나 작을 경우 처리하는 옵션
  - `GL_NEAREST`: 텍스처 좌표값에 가장 가까운 픽셀값을 사용
  - `GL_LINEAR`: 텍스처 좌표값 주변 4개의 픽셀값을 보간하여 사용

---

## Texture Filtering

- `GL_NEAREST`

![texture filter: nearest](/opengl_course/note/images/06_texture_filter_nearest.png)

---

## Texture Filtering

- `GL_LINEAR`

![texture filter: linear](/opengl_course/note/images/06_texture_filter_linear.png)

---

## Texture Filtering

- `GL_NEAREST` v.s. `GL_LINEAR`

![texture filter comparison](/opengl_course/note/images/06_texture_filter_comparison.png)

---

## Texture in OpenGL

- OpenGL에서의 텍스처 사용 과정
  - OpenGL texture object 생성 및 바인딩
  - wrapping, filtering option 설정
  - 이미지 데이터를 GPU 메모리로 복사
  - shader 프로그램이 바인딩 되었을때 사용하고자 하는 texture를 uniform 형태로 프로그램에 전달

---

## 예제 준비

- `glsl_example` 프로젝트를 복사하여 `texture_example` 준비
- `PROJECT_NAME` 변경 후 빌드 확인

---

## 이미지 로딩

- [stb](https://github.com/nothings/stb)
  - Sean Barrett이라는 인디 게임 제작자가 만든 라이브러리
  - Single-file public domain library
    - header file 하나에 라이브러리가 제공하고자 하는 모든 기능이 구현되어 있음
    - 빌드가 매우 간편함
  - `stb_image`
    - jpg, png, tga, bmp, psd, gif, hdr, pic 포맷을 지원하는 이미지 로딩 라이브러리

---

## 이미지 로딩

- `Dependency.cmake`에 다음을 추가

```cmake
# stb
ExternalProject_Add(
    dep_stb
    GIT_REPOSITORY "https://github.com/nothings/stb"
    GIT_TAG "master"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_BINARY_DIR}/dep_stb-prefix/src/dep_stb/stb_image.h
        ${DEP_INSTALL_DIR}/include/stb/stb_image.h
    )
set(DEP_LIST ${DEP_LIST} dep_stb)
```

---

## Congratulation!
### 수고하셨습니다!

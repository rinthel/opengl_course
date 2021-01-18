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

## 이미지 로딩

- `src/image.h`

```cpp
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "common.h"

CLASS_PTR(Image)
class Image {
public:
    static ImageUPtr Load(const std::string& filepath);
    ~Image();

    const uint8_t* GetData() const { return m_data; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }

private:
    Image() {};
    bool LoadWithStb(const std::string& filepath);
    int m_width { 0 };
    int m_height { 0 };
    int m_channelCount { 0 };
    uint8_t* m_data { nullptr };
};

#endif // __IMAGE_H__
```

---

## 이미지 로딩

- `src/image.cpp`

```cpp
#include "image.h"
#include <stb/stb_image.h>

ImageUPtr Image::Load(const std::string& filepath) {
    auto image = ImageUPtr(new Image());
    if (!image->LoadWithStb(filepath))
        return nullptr;
    return std::move(image);
}

Image::~Image() {
    if (m_data) {
        stbi_image_free(m_data);
    }
}

bool Image::LoadWithStb(const std::string& filepath) {
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data) {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    return true;
}
```

---

## 이미지 로딩

- 텍스처 이미지 다운로드
  - [https://learnopengl.com/img/textures/container.jpg](https://learnopengl.com/img/textures/container.jpg)
  - `image/container.jpg`에 저장

<img src="https://learnopengl.com/img/textures/container.jpg" style="width:30%"/>

---

## 이미지 로딩

- `src/context.cpp`에서 이미지 로딩

```cpp [2]
#include "context.h"
#include "image.h"
```

```cpp
// ... end of Context::Init()
auto image = Image::Load("./image/container.jpg");
if (!image) 
  return false;
SPDLOG_INFO("image: {}x{}, {} channels",
  image->GetWidth(), image->GetHeight(), image->GetChannelCount());
```

---

## 이미지 로딩

- 빌드 후 실행
  - 이미지 로딩 후 이미지 사이즈 및 채널 개수가 출력되는 것 확인

```console
[2021-01-18 12:45:15.664] [info] [context.cpp:70] image: 512x512, 3 channels
```

---

## 텍스처 생성

- `Context` 클래스 내에 texture id 저장을 위한 변수 선언

```cpp [3]
    BufferUPtr m_vertexBuffer;
    BufferUPtr m_indexBuffer;
    uint32_t m_texture;
};
```

---

## 텍스처 생성

- `Context::Init()`에서 텍스처 생성 및 설정
  - 이미지 로딩 후 코드 적용

```cpp
glGenTextures(1, &m_texture);
glBindTexture(GL_TEXTURE_2D, m_texture);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
    image->GetWidth(), image->GetHeight(), 0,
    GL_RGB, GL_UNSIGNED_BYTE, image->GetData());
```

---

## 텍스처 적용

- 텍스처 적용을 위해 추가적으로 할 일
  - vertex attribute에 텍스처 좌표를 추가하기
  - 텍스처를 읽어들여 픽셀값을 결정하는 shader 작성하기

---

## 텍스처 적용

- vertex 정보 추가
  - `[x, y, z, r, g, b, s, t]`

```cpp
float vertices[] = {
  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
  -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
};
```

---

## 텍스처 적용

- vertex buffer 및 vertex layout 수정

```cpp
m_vertexLayout = VertexLayout::Create();
m_vertexBuffer = Buffer::CreateWithData(
  GL_ARRAY_BUFFER, GL_STATIC_DRAW,
  vertices, sizeof(float) * 32);

m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE,
  sizeof(float) * 8, 0);
m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE,
  sizeof(float) * 8, sizeof(float) * 3);
m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE,
  sizeof(float) * 8, sizeof(float) * 6);
```

---

## 텍스처 적용

- `shader/texture.vs` 생성 및 작성

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 vertexColor;
out vec2 texCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    vertexColor = vec4(aColor, 1.0);
    texCoord = aTexCoord;
}
```

---

## 텍스처 적용

- `shader/texture.fs` 생성 및 작성

```glsl
#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
    fragColor = texture(tex, texCoord);
}
```

---

## 텍스처 적용

- `Context::Init()`에서 새로 작성한 shader 컴파일

```cpp
ShaderPtr vertShader = Shader::CreateFromFile(
  "./shader/texture.vs", GL_VERTEX_SHADER);
ShaderPtr fragShader = Shader::CreateFromFile(
  "./shader/texture.fs", GL_FRAGMENT_SHADER);
```

---

## 텍스처 적용

- 빌드 및 실행

![first texture result](/opengl_course/note/images/06_first_texture_result.png)

---

## OpenGL Remarks

- OpenGL API 설명
  - `glGenTextures()`: OpenGL texture object 생성
  - `glBindTexture()`: 사용하고자 하는 텍스처 바인딩
  - `glTexParameteri()`: 텍스처 필터 / 래핑 방식 등 파라미터 설정

---

## OpenGL Remarks

- `glTexImage2D(target, level, internalFormat, width, height, border, format, type, data)`
  - 바인딩된 텍스처의 크기 / 픽셀 포맷을 설정하고 GPU에 이미지 데이터를 복사
  - `target`: 대상이 될 바인딩 텍스처
  - `level`: 설정할 텍스처 레벨. 0레벨이 base. 추후 설명
  - `internalFormat`: **텍스처**의 픽셀 포맷

---

## OpenGL Remarks

- `glTexImage2D(target, level, internalFormat, width, height, border, format, type, data)`
  - `width`: 텍스처 / 이미지의 가로 크기
  - `height`: 텍스처 / 이미지의 세로 크기
  - `border`: 텍스처 외곽의 border 크기
  - `format`: 입력하는 **이미지**의 픽셀 포맷
  - `type`: 입력하는 **이미지**의 채널별 데이터 타입
  - `data`: 이미지 데이터가 기록된 메모리 주소

---

## Additional Notes

- `glTexImage2D()`의 internalFormat 파라미터를 다른걸로 바꿔보자
  - `GL_RED`, `GL_RG`, `GL_RGBA8`

- 텍스처의 크기
  - 가로/세로의 크기가 2의 지수 형태일 때 GPU가 가장 효율적으로 처리할 수 있다
  - NPOT (Non-Power-Of-Two) texture
    - 2의 지수 크기가 아닌 텍스처는 GPU의 스펙에 따라 지원을 안하는 경우도 있다

---

## Texture Refactoring

- `src/texture.h` 생성 후 `Texture` 클래스 선언

```cpp
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "image.h"

CLASS_PTR(Texture)
class Texture {
public:
    static TextureUPtr CreateFromImage(const Image* image);
    ~Texture();

    const uint32_t Get() const { return m_texture; }
    void Bind() const;
    void SetFilter(uint32_t minFilter, uint32_t magFilter) const;
    void SetWrap(uint32_t sWrap, uint32_t tWrap) const;
    
private:
    Texture() {}
    void CreateTexture();
    void SetTextureFromImage(const Image* image);

    uint32_t m_texture { 0 };
};

#endif // __TEXTURE_H__
```

---

## Texture Refactoring

- 왜 `ImagePtr`이나 `ImageUPtr`이 아닌 `Image*`를 인자로 쓰는가?
  - `ImageUPtr`: 이미지 인스턴스 소유권이 함수 안으로 넘어오게 됨
  - `ImagePtr`: 이미지 인스턴스 소유권을 공유함
  - `Image*`: 소유권과 상관없이 인스턴스에 접근

---

## Texture Refactoring

- `src/texture.cpp` 생성 후 `Texture` 클래스 구현

```cpp
#include "texture.h"

TextureUPtr Texture::CreateFromImage(const Image* image) {
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFromImage(image);
    return std::move(texture);
}

Texture::~Texture() {
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
    }
}

void Texture::Bind() const {
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::SetFilter(uint32_t minFilter, uint32_t magFilter) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetWrap(uint32_t sWrap, uint32_t tWrap) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}
    
void Texture::CreateTexture() {
    glGenTextures(1, &m_texture);
    // bind and set default filter and wrap option
    Bind();
    SetFilter(GL_LINEAR, GL_LINEAR);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFromImage(const Image* image) {
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount()) {
        default: break;
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        image->GetWidth(), image->GetHeight(), 0,
        format, GL_UNSIGNED_BYTE,
        image->GetData());
}
```

---

## Texture Refactoring

- `Context` 클래스 내 멤버 변수를 `TextureUPtr`로 교체

```cpp [3]
  BufferUPtr m_vertexBuffer;
  BufferUPtr m_indexBuffer;
  TextureUPtr m_texture;
};
```

---

## Texture Refactoring

- `Context::Init()`의 텍스처 초기화 코드 변경

```cpp [7]
auto image = Image::Load("./image/container.jpg");
if (!image) 
  return false;
SPDLOG_INFO("image: {}x{}, {} channels",
  image->GetWidth(), image->GetHeight(), image->GetChannelCount());

m_texture = Texture::CreateFromImage(image.get());
```

---

## Mipmap

---

## Multiple Texture

---

## Congratulation!
### 수고하셨습니다!

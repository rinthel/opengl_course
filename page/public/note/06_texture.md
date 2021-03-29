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

## More on Filtering

- 체커 이미지를 만들어보자

```cpp [5, 13, 18]
CLASS_PTR(Image)
class Image {
public:
    static ImageUPtr Load(const std::string& filepath);
    static ImageUPtr Create(int width, int height, int channelCount = 4);
    ~Image();

    const uint8_t* GetData() const { return m_data; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
    int GetChannelCount() const { return m_channelCount; }

    void SetCheckImage(int gridX, int gridY);
    
private:
    Image() {};
    bool LoadWithStb(const std::string& filepath);
    bool Allocate(int width, int height, int channelCount);
    int m_width { 0 };
    int m_height { 0 };
    int m_channelCount { 0 };
    uint8_t* m_data { nullptr };
};
```

---

## More on Filtering

- 이미지를 위한 메모리 할당 함수 구현

```cpp
ImageUPtr Image::Create(int width, int height, int channelCount) {
    auto image = ImageUPtr(new Image());
    if (!image->Allocate(width, height, channelCount))
        return nullptr;
    return std::move(image);
}

bool Image::Allocate(int width, int height, int channelCount) {
    m_width = width;
    m_height = height;
    m_channelCount = channelCount;
    m_data = (uint8_t*)malloc(m_width * m_height * m_channelCount);
    return m_data ? true : false;
}
```

---

## More on Filtering

- 체커 이미지 설정 함수
  - `gridX`, `gridY` 크기의 흑백 타일로 구성된 체커 보드 이미지
  - 알파 체널은 항상 255로 설정

```cpp
void Image::SetCheckImage(int gridX, int gridY) {
    for (int j = 0; j < m_height; j++) {
        for (int i = 0; i < m_width; i++) {
            int pos = (j * m_width + i) * m_channelCount;
            bool even = ((i / gridX) + (j / gridY)) % 2 == 0;
            uint8_t value = even ? 255 : 0;
            for (int k = 0; k < m_channelCount; k++)
                m_data[pos + k] = value;
            if (m_channelCount > 3)
                m_data[3] = 255;
        }
    }
}
```

---

## More on Filtering

- 체커 이미지를 `Context::Init()`에서 텍스처 설정에 사용

```cpp [1-2]
auto image = Image::Create(512, 512);
image->SetCheckImage(16, 16);

m_texture = Texture::CreateFromImage(image.get());
```

---

## More on Filtering

- 빌드 및 실행

![checker normal](/opengl_course/note/images/06_checker_normal.png)

---

## More on Filtering

- 창의 크기를 축소하면 예기치 않은 무늬가 생김

![checker normal](/opengl_course/note/images/06_checker_small.png)

---

## More on Filtering

- 이런 현상이 발생하는 이유
  - 화면에 그리는 픽셀보다 텍스처 픽셀의 영역이 커지면 linear filter로도 충분
  - 화면에 그리는 픽셀이 여러 텍스처 픽셀을 포함하게되면 문제가 발생

---

## Mipmap

- Mipmap
  - 화면 픽셀이 여러 텍스처 픽셀을 포함하게 될 경우를 위해서
    작은 사이즈의 이미지를 미리 준비하는 기법

![mipmap](/opengl_course/note/images/06_mipmap.png)

---

## Mipmap

- Mipmap
  - 가장 큰 이미지를 기본 레벨 0으로 함
  - 가로세로 크기를 절반씩 줄인 이미지를 미리 계산하여 레벨을 1씩 증가시키며 저장
    - 512x512 이미지 => level 0~9까지 생성
  - 원본 이미지 저장을 위해 필요한 메모리보다 1/3 만큼을 더 소요

---

## Mipmap in OpenGL

- `Texture` 클래스의 구현 변경

```cpp [5]
void Texture::CreateTexture() {
    glGenTextures(1, &m_texture);
    // bind and set default filter and wrap option
    Bind();
    SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}
```

---

## Mipmap in OpenGL

- `Texture` 클래스의 구현 변경

```cpp [15]
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

    glGenerateMipmap(GL_TEXTURE_2D);
}
```

---

## Mipmap in OpenGL

- 빌드 및 실행
  - 창의 크기를 줄였을 때 발생되는 무늬가 없어지는 것을 확인

![mipmap](/opengl_course/note/images/06_checker_mipmap.png)

---

## Additional Notes

- Mipmap 필터 옵션의 의미
  - `GL_NEAREST_MIPMAP_NEAREST`: 적합한 레벨의 텍스처를 선택한 뒤
    nearest neighbor 픽셀을 선택한다
  - `GL_LINEAR_MIPMAP_LINEAR`: 적합한 두 레벨의 텍스처에서
    linear filtering된 값을 다시 linear interpolation한다
    (trilinear interpolation)

---

## Multiple Textures

- 하나의 shader program에서 여러 장의 텍스처 이용하기
  - [learnopengl.com/.../awesomeface.png](https://learnopengl.com/img/textures/awesomeface.png)
  - `image/awesomeface.png`에 저장

<img src="https://learnopengl.com/img/textures/awesomeface.png" width="30%"/>

---

## Multiple Textures

- `Context` 클래스에 텍스처 멤버를 추가하고 텍스처 이미지 생성

```cpp [3]
    // ... Context class declaration
    TextureUPtr m_texture;
    TextureUPtr m_texture2;
};
```

```cpp [4-5]
// ... Context::Init()
m_texture = Texture::CreateFromImage(image.get());

auto image2 = Image::Load("./image/awesomeface.png");
m_texture2 = Texture::CreateFromImage(image2.get());
```

---

## Multiple Textures

- 빌드 및 실행
  - 제일 마지막에 초기화한 텍스처가 그려짐

![mipmap](/opengl_course/note/images/06_awesome_face.png)

---

## Multiple Textures

- Fragment shader 코드 수정
  - 두 개의 sampler2D를 사용
  - 두 sampler2D로부터 얻어온 텍스처 컬러를 4 : 1 비율로 블랜딩

```glsl
#version 330 core
in vec4 vertexColor;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform sampler2D tex2;

void main() {
    fragColor = texture(tex, texCoord) * 0.8 +
        texture(tex2, texCoord) * 0.2;
}
```

---

## Multiple Textures

- 텍스처를 shader program에 올바르게 제공하는 방법
  - `glActiveTexture(textureSlot)` 함수로 현재 다루고자 하는 텍스처 슬롯을 선택
  - `glBindTexture(textureType, textureId)` 함수로 현재
    설정중인 텍스처 슬롯에 우리의 텍스처 오브젝트를 바인딩
  - `glGetUniformLocation()` 함수로 shader 내의 sampler2D uniform 핸들을 얻어옴
  - `glUniform1i()` 함수로 sampler2D uniform에 **텍스처 슬롯 인덱스**를 입력

---

## Multiple Textures

- `Context::Init()`에 다음을 추가

```cpp
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, m_texture->Get());
glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

m_program->Use();
glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);
glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);
```

---

## Multiple Textures

- 빌드 및 실행

![multiple texture](/opengl_course/note/images/06_multiple_texture.png)

---

## Additional Notes

- 이미지 상하 반전의 이유
  - 보통의 이미지는 좌상단을 원점으로 함
  - OpenGL은 좌하단을 원점으로 함
  - 이미지 로딩시 상하를 반전시켜서 문제를 해결할 수 있음

```cpp [2]
bool Image::LoadWithStb(const std::string& filepath) {
    stbi_set_flip_vertically_on_load(true);
    m_data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_channelCount, 0);
    if (!m_data) {
        SPDLOG_ERROR("failed to load image: {}", filepath);
        return false;
    }
    return true;
}
```

---

## Congratulation!
### 수고하셨습니다!

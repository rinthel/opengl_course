# Graphics Pipeline and Shader

## Part 2

---

## 세 번째 예제 준비

- `point_example`의 코드를 복사하여 `triangle_example` 프로젝트 생성
- `CMakeLists.txt`의 프로젝트명 변경
- VSCode로 열어서 빌드 및 실행 확인

---

## 정점 입력

- 정점을 3개 입력하여 삼각형을 그려보자
  - 정점 데이터 준비
  - Vertex buffer object (VBO) 준비
  - Vertex buffer object에 정점 데이터 입력
    - CPU memory 상에 있는 정점 데이터를 GPU로 옮기는 작업
  - Vertex array object (VAO) 준비
    - 우리의 정점 데이터의 구조를 알려주는 descriptor object
  - Program, VBO, VAO를 사용하여 그림 그리기

---

## 정점 데이터

- 정점 데이터를 담은 array를 `Context::Init()`에 선언

```cpp [2-6]
bool Context::Init() {
  float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f,
  };
```

---

## Vertex Buffer Object

- Vertex buffer object (VBO)
  - 정점 데이터를 담은 버퍼 오브젝트
  - 정점에 대한 다양한 데이터를 GPU가 접근 가능한 메모리에 저장해둔다
    - position, normal, tangent, color, texture coordinate...

---

## Vertex Buffer Object

- VBO를 담아둘 `uint32_t`형 멤버 변수를 `Context` 클래스에 선언

```cpp [6]
private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;

    uint32_t m_vertexBuffer;
};

#endif // __CONTEXT_H__
```

---

## Vertex Buffer Object

- `Context::Init()`에서 VBO 생성 및 정점 데이터를 GPU로 복사

```cpp [8-10]
bool Context::Init() {
  float vertices[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f,
  };

  glGenBuffers(1, &m_vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, vertices,
    GL_STATIC_DRAW);
```

---

## Vertex Buffer Object

- 함수 설명
  - `glGenBuffers()`: 새로운 buffer object를 만든다
  - `glBindBuffer()`: 지금부터 사용할 buffer object를 지정한다
    - `GL_ARRAY_BUFFER`: 사용할 buffer object는 vertex data를 저장할 용도임을 알려줌

---

## Vertex Buffer Object

- 함수 설명
  - `glBufferData()`: 지정된 buffer에 데이터를 복사한다
    - 데이터의 총 크기, 데이터 포인터, 용도를 지정
    - 용도는 "`STATIC | DYNAMIC | STREAM`", "`DRAW | COPY | READ`"의 조합

---

## Vertex Buffer Object

- flag 설명
  - `GL_STATIC_DRAW`: 딱 한번만 세팅되고 앞으로 계속 쓸 예정
  - `GL_DYNAMIC_DRAW`: 앞으로 데이터가 자주 바뀔 예정
  - `GL_STREAM_DRAW`: 딱 한번만 세팅되고 몇번 쓰다 버려질 예정
  - 용도에 맞는 flag를 지정해야 효율이 올라감

---

## Vertex Buffer Object

- 우리의 VBO에 복사한 데이터의 구조

![vertex buffer structure](/opengl_course/note/images/04_vertex_buffer_structure.png)

---

## Vertex Buffer Object

- 우리의 VBO에 복사한 데이터의 구조
  - 정점이 총 3개
  - 각 정점의 위치가 기록
  - 위치에 대해서 x/y/z 좌표값을 가짐
  - 각 좌표값마다 float = 4byte의 크기를 가짐
  - 첫번째 정점과 두번째 정점 간의 간격이 12byte 만큼 차이남
- VBO가 가진 정점에 대한 구조(layout)를 알려줄 방법이 있어야 함

---

## Vertex Array Object

- Vertex Array Object (VAO)
  - 정점 데이터의 구조를 알려주는 오브젝트
  - 각 정점은 몇 byte로 구성되었는가?
  - 두 정점은 몇 byte만큼 떨어져 있는가?
  - 정점의 0번째 데이터는 어떤 사이즈의 데이터가 몇개 있는 형태인가?

---

## Vertex Array Object

- Vertex Array Object (VAO)

![vertex array object](/opengl_course/note/images/04_vertex_array_object.png)

---

## Vertex Array Object

- `Context` 클래스에 VAO를 담아둘 변수 선언

```cpp [6]
private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;

    uint32_t m_vertexArrayObject;
    uint32_t m_vertexBuffer;
};
```

---

## Vertex Array Object

- `Context::Init()`에서 VAO 생성 및 설정

```cpp [1-2, 8-9]
glGenVertexArrays(1, &m_vertexArrayObject);
glBindVertexArray(m_vertexArrayObject);

glGenBuffers(1, &m_vertexBuffer);
glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9, vertices, GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
```

---

## Vertex Array Object

- 함수 설명
  - `glGenVertexArrays()`: VAO 생성
  - `glBindVertexArray()`: 지금부터 사용할 VAO 설정
  - `glEnableVertexAttribArray(n)`: 정점 attribute 중 `n`번째를 사용하도록 설정

---

## Vertex Array Object

- 함수 설명
  - `glVertexAttribPointer(n, size, type, normailzed, stride, offset)`
    - `n`: 정점의 `n`번째 attribute
    - `size`: 해당 attribute는 몇개의 값으로 구성되어 있는가?
    - `type`: 해당 attribute의 데이터 타입
    - `normalized`: 0~1사이의 값인가
    - `stride`: 두 정점간의 간격 (byte 단위)
    - `offset`: 첫 정점의 헤당 attribute까지의 간격 (byte 단위)

---

## Vertex Array Object

- 순서 주의
  - VAO binding
  - **VBO binding**
  - vertex attribute setting
  - vertex attribute을 설정하기 전에 VBO가 바인딩 되어있을 것

---

## Vertex Shader

- `shader/simple.vs`를 수정

```glsl [5]
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
```

---

## Vertex Shader

- `location = 0`이 곧 VAO attribute 0번을 의미

```glsl [2]
#version 330 core
layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
```

---

## Draw Array

- `Context::Render()`를 다음과 같이 변경

```cpp [250]
void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_program->Get());
    glDrawArrays(GL_TRIANGLES, 0, 3);
}
```

---

## Draw Array

- 함수 설명
  - `glDrawArray(primitive, offset, count)`
    - 현재 설정된 program, VBO, VAO로 그림을 그린다
    - `primitive`: 그리고자 하는 primitive 타입
    - `offset`: 그리고자 하는 첫 정점의 index
    - `count`: 그리려는 정점의 총 개수

---

## 빌드 및 실행

- 드디어 첫 삼각형을 그렸습니다!

![first triangle](/opengl_course/note/images/04_first_triangle.png)

---

## Additional Note

- 여러가지를 바꿔서 테스트해보자
  - `vertices` 내의 값 바꿔보기
  - `GL_TRIANGLES` 대신 `GL_LINE_STRIP` 사용해보기
  - `shader/simple.fs`의 `fragColor` 색상 바꿔보기

---

## 사각형 그리기

- 삼각형을 두 개 그려서 사각형을 만들어보자

```cpp [2-11,14-15]
bool Context::Init() {
  float vertices[] = {
    // first triangle
    0.5f, 0.5f, 0.0f, // top right
    0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, 0.5f, 0.0f, // top left
    // second triangle
    0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f // top left
  };

  // ...
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, vertices,
    GL_STATIC_DRAW);
```

---

## 사각형 그리기

- 삼각형을 두 개 그려서 사각형을 만들어보자

```cpp [5]
void Context::Render() {
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(m_program->Get());
  glDrawArrays(GL_TRIANGLES, 0, 6);
}
```

---

## 실행 결과

- 사각형이 그려지는 것을 확인

![first quad](/opengl_course/note/images/04_first_quad.png)

---

## 사각형 그리기

- 문제점
  - 사각형을 그리기 위해서는 정점이 4개가 필요
  - `glDrawArrays()`를 이용하여 그림을 그리려면 정점이 6개가 필요
  - **정점 2개가 중복된다**

---

## Element Buffer Object

- Element Buffer Object (EBO)
  - 정점의 인덱스를 저장할 수 있는 버퍼
  - 인덱스 버퍼라고도 부름
  - 정점 정보와 별개로 정점의 인덱스로만 구성된 삼각형 정보를 전달 가능
  - *indexed drawing*

---

## Element Buffer Object

- 정점은 4개만 선언하고 인덱스 배열을 추가

```cpp
float vertices[] = {
  0.5f, 0.5f, 0.0f, // top right
  0.5f, -0.5f, 0.0f, // bottom right
  -0.5f, -0.5f, 0.0f, // bottom left
  -0.5f, 0.5f, 0.0f, // top left
};
uint32_t indices[] = { // note that we start from 0!
  0, 1, 3, // first triangle
  1, 2, 3, // second triangle
};
```

---

## Element Buffer Object

- `Context` 클래스에 인덱스 버퍼로 사용할 멤버 추가

```cpp [3]
uint32_t m_vertexArrayObject;
uint32_t m_vertexBuffer;
uint32_t m_indexBuffer;
```

---

## Element Buffer Object

- `Context::Init()`에서 VBO/EBO 생성 및 초기화

```cpp [3,8-11]
glGenBuffers(1, &m_vertexBuffer);
glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertices, GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

glGenBuffers(1, &m_indexBuffer);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * 6,
  indices, GL_STATIC_DRAW);
```

---

## Element Buffer Object

- `Context::Render()`에서 `glDrawElements()`를 이용
- 동일한 사각형이 그려지는 것을 확인

```cpp [5]
void Context::Render() {
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(m_program->Get());
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
```

---

## OpenGL Remarks

- `glDrawElements(primitive, count, type, pointer/offset)`
  - 현재 바인딩된 VAO, VBO, EBO를 바탕으로 그리기
  - `primitive`: 그려낼 기본 primitive 타입
  - `count`: 그리고자 하는 EBO 내 index의 개수
  - `type`: index의 데이터형
  - `pointer/offset`: 그리고자 하는 EBO의 첫 데이터로부터의 오프셋

---

## Pritimive Types

![primitives](/opengl_course/note/images/04_primitives.png)

---

## Refactoring

- `Buffer` 클래스
  - VBO 혹은 EBO를 가질 수 있음
  - 생성시 정점 데이터 혹은 인덱스 데이터를 제공하면,
    해당 데이터를 GPU 메모리에 저장한 버퍼 오브젝트 생성
  - 메모리 해제시 버퍼 오브젝트 제거

---

## `Buffer` 클래스 디자인

- `src/buffer.h` 추가 및 클래스 디자인

```cpp
#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "common.h"

CLASS_PTR(Buffer)
class Buffer {
public:
    static BufferUPtr CreateWithData(
        uint32_t bufferType, uint32_t usage,
        const void* data, size_t dataSize);
    ~Buffer();
    uint32_t Get() const { return m_buffer; }
    void Bind() const;

private:
    Buffer() {}
    bool Init(
        uint32_t bufferType, uint32_t usage,
        const void* data, size_t dataSize);
    uint32_t m_buffer { 0 };
    uint32_t m_bufferType { 0 };
    uint32_t m_usage { 0 };
};

#endif // __BUFFER_H__
```

---

## `Buffer` 클래스 구현

- `src/buffer.cpp` 추가 및 구현

```cpp
#include "buffer.h"

BufferUPtr Buffer::CreateWithData(
    uint32_t bufferType, uint32_t usage,
    const void* data, size_t dataSize) {
    auto buffer = BufferUPtr(new Buffer());
    if (!buffer->Init(bufferType, usage, data, dataSize))
        return nullptr;
    return std::move(buffer);
}

Buffer::~Buffer() {
    if (m_buffer) {
        glDeleteBuffers(1, &m_buffer);
    }
}

void Buffer::Bind() const {
    glBindBuffer(m_bufferType, m_buffer);
}

bool Buffer::Init(
    uint32_t bufferType, uint32_t usage,
    const void* data, size_t dataSize) {
    m_bufferType = bufferType;
    m_usage = usage;
    glGenBuffers(1, &m_buffer);
    Bind();
    glBufferData(m_bufferType, dataSize, data, usage);
    return true;
}
```

---

## `Buffer` 클래스 테스트

- `Context` 클래스 멤버로 `Buffer` 클래스 사용

```cpp [2-3]
uint32_t m_vertexArrayObject;
BufferUPtr m_vertexBuffer;
BufferUPtr m_indexBuffer;
```

---

## `Buffer` 클래스 테스트

- `Context::Init()`에서 `Buffer` 클래스 생성

```cpp [1-3,8-10]
m_vertexBuffer = Buffer::CreateWithData(
  GL_ARRAY_BUFFER, GL_STATIC_DRAW,
  vertices, sizeof(float) * 12);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

m_indexBuffer = Buffer::CreateWithData(
  GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
  indices, sizeof(uint32_t) * 6);
```

---

## `Program` 클래스 메소드

- `glUseProgram()`을 호출해주는 `Program::Use()`

```cpp [4]
// src/program.h
~Program();
uint32_t Get() const { return m_program; }
void Use() const;
```

```cpp [2-4]
// src/program.cpp
void Program::Use() const {
    glUseProgram(m_program);
}
```

---

## `Program` 클래스 메소드

- `Context::Render()`에서 `Program::Use()` 사용

```cpp [4]
void Context::Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    m_program->Use();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
```

---

## 빌드 및 실행

- `CMakeLists.txt`에 새로운 파일을 리스트에 추가
- 빌드 및 실행해서 사각형이 제대로 나오는지 확인

---

## `VertexLayout` 클래스

- `VertexLayout`
  - VAO 생성 및 메모리 해제시 VAO 제거
  - vertex attribute 설정 기능 제공

---

## `VertexLayout` 클래스

- `src/vertex_layout.h`

```cpp
#ifndef __VERTEX_LAYOUT_H__
#define __VERTEX_LAYOUT_H__

#include "common.h"

CLASS_PTR(VertexLayout)
class VertexLayout {
public:
    static VertexLayoutUPtr Create();
    ~VertexLayout();

    uint32_t Get() const { return m_vertexArrayObject; }
    void Bind() const;
    void SetAttrib(
        uint32_t attribIndex, int count,
        uint32_t type, bool normalized,
        size_t stride, uint64_t offset) const;
    void DisableAttrib(int attribIndex) const;

private:
    VertexLayout() {}
    void Init();
    uint32_t m_vertexArrayObject { 0 };
};

#endif // __VERTEX_LAYOUT_H__
```

---

## `VertexLayout` 클래스

- `src/vertex_layout.cpp`

```cpp
#include "vertex_layout.h"

VertexLayoutUPtr VertexLayout::Create() {
    auto vertexLayout = VertexLayoutUPtr(new VertexLayout());
    vertexLayout->Init();
    return std::move(vertexLayout);
}

VertexLayout::~VertexLayout() {
    if (m_vertexArrayObject) {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
    }
}

void VertexLayout::Bind() const {
    glBindVertexArray(m_vertexArrayObject);
}

void VertexLayout::SetAttrib(
    uint32_t attribIndex, int count,
    uint32_t type, bool normalized,
    size_t stride, uint64_t offset) const {
    glEnableVertexAttribArray(attribIndex);
    glVertexAttribPointer(attribIndex, count,
        type, normalized, stride, (const void*)offset);
}

void VertexLayout::Init() {
    glGenVertexArrays(1, &m_vertexArrayObject);
    Bind();
}
```

---

## `VertexLayout` 클래스

- `Context` 클래스에서 `VertexLayout` 인스턴스 멤버를 활용

```cpp [1]
VertexLayoutUPtr m_vertexLayout;
BufferUPtr m_vertexBuffer;
BufferUPtr m_indexBuffer;
```

```cpp [1, 6]
m_vertexLayout = VertexLayout::Create();
m_vertexBuffer = Buffer::CreateWithData(
    GL_ARRAY_BUFFER, GL_STATIC_DRAW,
    vertices, sizeof(float) * 12);

m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

m_indexBuffer = Buffer::CreateWithData(
    GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
    indices, sizeof(uint32_t) * 6);
```

---

## 빌드 및 실행

- `CMakeLists.txt`에 새로운 파일들을 리스트에 추가
- 빌드 및 실행하여 사각형이 잘 그려지는지 확인

---

## 정리

- OpenGL을 이용하여 삼각형을 그리는 긴 여정
  - shader object 생성 / 소스 컴파일
  - program object 생성 / shader link
  - VAO: VBO의 구조에 대한 description, 바인딩된 VBO, EBO 기억
  - VBO: 정점 데이터를 GPU 메모리 상에 위치시킨 object
  - EBO: 인덱스 데이터를 GPU 메모리 상에 위치시킨 object

---

## 정리

- OpenGL을 이용하여 삼각형을 그리는 긴 여정
  - 대부분의 OpenGL object는 `glBindXX()`라는 함수를 이용하여
    지금부터 사용할 object를 선택한 뒤 이용함
    - `glBindBuffer()`
    - `glBindVertexArray()`
    - `glBindTexture()`
    - `glBindFramebuffer()`
    - ...

---

## 정리

- C++ 코드 리팩토링
  - Smart pointer의 기능을 이용한 리소스 자동 해제
  - static method / private constructor를 이용한 인스턴스 생성 방식 제한
  - private member 설정을 통한 수정 권한 제한

---

## Congratulation!
### 수고하셨습니다!

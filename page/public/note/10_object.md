# Object Loading
### with Assimp library

---

## Objective

- 박스 외의 오브젝트를 렌더링 해보자
  - 실제 게임 등에 사용되는 오브젝트를 렌더링하기 위해서는 어떤 작업을 해야될까?
    - 3D Modeling tool를 이용한 3D 모델 디자인
    - 3D 모델 파일 포맷으로 저장
    - 저장된 파일을 읽어들이는 object loader 구현

---

## 3D Modeling Tool

- 3D 모델을 생성하고 편집할 수 있는 도구
  - 다양한 종류의 작업
    - Modeling / Sculpting / UV unwrapping / Rigging / Animation / ...
  - 대표적인 도구들
    - 3D Studio Max
    - Maya
    - Blender 3D

---

## 3D Model files

- 다양한 툴들을 통해서 만들어지는 다양한 3D 모델 파일들
  - FBX, Collada(dae), GLTF, OBJ, PLY, ...
  - 개별 파서를 만드는 일은 쉽지 않다

---

## Assimp

- Open **Ass**et **Imp**ort Library
- 매우 다양한 3D 모델 파일을 지원
- Cross-platform
- C / C++ interface

---

## 예제 준비

- `lighting_example` 프로젝트를 복사하여 `object_example` 프로젝트 생성
- 빌드 및 실행 확인

---

## Refactoring

- 박스가 아닌 메쉬 모델을 그리기 전에 메쉬 모델에 대한 클래스를
  디자인하고 코드를 정리하자

---

## Refactoring

- `Buffer` 클래스 수정
  - `dataSize` 대신 `stride`, `count`를 입력
  - `dataSize = stride * count`

```cpp [5, 8, 9, 16, 20, 21]
class Buffer {
public:
  static BufferUPtr CreateWithData(
    uint32_t bufferType, uint32_t usage,
    const void* data, size_t stride, size_t count);
  ~Buffer();
  uint32_t Get() const { return m_buffer; }
  size_t GetStride() const { return m_stride; }
  size_t GetCount() const { return m_count; }
  void Bind() const;

private:
    Buffer() {}
    bool Init(
      uint32_t bufferType, uint32_t usage,
      const void* data, size_t stride, size_t count);
    uint32_t m_buffer { 0 };
    uint32_t m_bufferType { 0 };
    uint32_t m_usage { 0 };
    size_t m_stride { 0 };
    size_t m_count { 0 };
};
```

---

## Refactoring

- `src/mesh.h` 파일을 추가하고 `Mesh` 클래스 디자인

```cpp
#ifndef __MESH_H__
#define __MESH_H__

#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

CLASS_PTR(Mesh);
class Mesh {
public:
  static MeshUPtr Create(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    uint32_t primitiveType);
  static MeshUPtr MakeBox();

  const VertexLayout* GetVertexLayout() const {
    return m_vertexLayout.get();
  }
  BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
  BufferPtr GetIndexBuffer() const { return m_indexBuffer; }

  void Draw() const;

private:
  Mesh() {}
  void Init(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    uint32_t primitiveType);

  uint32_t m_primitiveType { GL_TRIANGLES };
  VertexLayoutUPtr m_vertexLayout;
  BufferPtr m_vertexBuffer;
  BufferPtr m_indexBuffer;
};

#endif // __MESH_H__
```

---

## Refactoring

- `VertexLayout`은 unique pointer를, `Buffer`는 shared pointer를 사용하는 이유
  - VBO, IBO는 다른 VAO와 연결하여 재사용할 수 있다
  - 반면 VAO는 해당 메쉬를 그리는데만 사용하게 될 것이다

---

## Refactoring

- `src/mesh.cpp`를 추가
- `Mesh::Create()` 함수 구현

```cpp
MeshUPtr Mesh::Create(
  const std::vector<Vertex>& vertices,
  const std::vector<uint32_t>& indices,
  uint32_t primitiveType) {
  auto mesh = MeshUPtr(new Mesh());
  mesh->Init(vertices, indices, primitiveType);
  return std::move(mesh);
}
```

---

## Refactoring

- `Mesh::Init()` 함수 구현

```cpp
void Mesh::Init(
  const std::vector<Vertex>& vertices,
  const std::vector<uint32_t>& indices,
  uint32_t primitiveType) {
  m_vertexLayout = VertexLayout::Create();
  m_vertexBuffer = Buffer::CreateWithData(
    GL_ARRAY_BUFFER, GL_STATIC_DRAW,
    vertices.data(), sizeof(Vertex), vertices.size());
  m_indexBuffer = Buffer::CreateWithData(
    GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
    indices.data(), sizeof(uint32_t), indices.size());
  m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false,
    sizeof(Vertex), 0);
  m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false,
    sizeof(Vertex), offsetof(Vertex, normal));
  m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false,
    sizeof(Vertex), offsetof(Vertex, texCoord));
}
```

---

## Refactoring

- `Mesh::Draw()` 함수 구현
  - `m_indexBuffer`가 가지고 있는 원소 개수를 이용

```cpp
void Mesh::Draw() const {
  m_vertexLayout->Bind();
  glDrawElements(m_primitiveType, m_indexBuffer->GetCount(),
    GL_UNSIGNED_INT, 0);
}
```

---

## Refactoring

- `Mesh::Create()`를 이용한 `Mesh::CreateBox()` 구현

```cpp
MeshUPtr Mesh::CreateBox() {
  std::vector<Vertex> vertices = {
    Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
    Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
    Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
    Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },

    Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
    Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
    Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
    Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },

    Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
    Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
    Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
    Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

    Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
    Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
    Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
    Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

    Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
    Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
    Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
    Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

    Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
    Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
    Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
    Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
  };

  std::vector<uint32_t> indices = {
     0,  1,  2,  2,  3,  0,
     4,  5,  6,  6,  7,  4,
     8,  9, 10, 10, 11,  8,
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20,
  };

  return Create(vertices, indices, GL_TRIANGLES);
}
```

---

## Refactoring

- `Context` 클래스 내에 `Buffer`, `VertexLayout` 대신 `Mesh` 멤버 변수 추가

```cpp [4]
  ProgramUPtr m_program;
  ProgramUPtr m_simpleProgram;

  MeshUPtr m_box;

  int m_width {640};
  int m_height {480};
```

---

## Refactoring

- `Context::Init()`의 박스 생성 관련 코드 대신 `Mesh::CreateBox()` 활용
- `Context::Render()`의 `glDrawElements()` 직접 호출 대신 `Mesh::Draw()` 활용

```cpp [2]
bool Context::Init() {
  m_box = Mesh::CreateBox();

  m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
```

```cpp [2]
m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
m_box->Draw();
```

---

## Refactoring

- 빌드 및 결과 확인
  - 오류 없이 박스가 그대로 렌더링 되는 것을 확인

---

## 3D 파일 다운로드

- [learnopengl.com/data/models/backpack.zip](https://learnopengl.com/data/models/backpack.zip) 다운로드
- `model` 디렉토리에 압축 해제
  - `obj`: 3D 모델 파일
  - `mtl`: 재질이 정의된 파일

<div>
<img src="https://learnopengl.com/img/model_loading/model_diffuse.png" style="width: 50%"/>
</div>

---

## Scene tree

---

## Congratulation!
### 수고하셨습니다!

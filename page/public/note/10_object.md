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
     0,  2,  1,  2,  0,  3,
     4,  5,  6,  6,  7,  4,
     8,  9, 10, 10, 11,  8,
    12, 14, 13, 14, 12, 15,
    16, 17, 18, 18, 19, 16,
    20, 22, 21, 22, 20, 23,
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

## Dependency

- `Dependency.cmake` 파일에 assimp 라이브러리 추가

```cmake
# assimp
ExternalProject_Add(
  dep_assimp
  GIT_REPOSITORY "https://github.com/assimp/assimp"
  GIT_TAG "v5.0.1"
  GIT_SHALLOW 1
  UPDATE_COMMAND ""
  PATCH_COMMAND ""
  CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
      -DBUILD_SHARED_LIBS=OFF
      -DASSIMP_BUILD_ASSIMP_TOOLS=OFF
      -DASSIMP_BUILD_TESTS=OFF
      -DASSIMP_INJECT_DEBUG_POSTFIX=OFF
      -DASSIMP_BUILD_ZLIB=ON
  TEST_COMMAND ""
  )
set(DEP_LIST ${DEP_LIST} dep_assimp)
set(DEP_LIBS ${DEP_LIBS}
  assimp-vc142-mt$<$<CONFIG:Debug>:d>
  zlibstatic$<$<CONFIG:Debug>:d>
  IrrXML$<$<CONFIG:Debug>:d>
  )
```

---

## Scene tree

- 3D 장면(scene)을 트리 형식으로 관리하는 방식
- 대부분의 3D Modeling Tool 들이 이러한 방식으로 장면을 관리
  - 부모/자식 관계
  - 자식의 transform 정보 (position / rotation / scale)은
    부모의 local coordinates를 기준으로 기술됨

---

## Scene tree

- Assimp 라이브러리의 클래스 구조

![](/opengl_course/note/images/10_assimp_structure.png)

---

## Object Loading

- `src/model.h` 파일 생성
  - 먼저 메쉬 데이터만 로딩하고 그려보자

```cpp
#ifndef __MODEL_H__
#define __MODEL_H__

#include "common.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CLASS_PTR(Model);
class Model {
public:
    static ModelUPtr Load(const std::string& filename);

    int GetMeshCount() const { return (int)m_meshes.size(); }
    MeshPtr GetMesh(int index) const { return m_meshes[index]; }
    void Draw() const;

private:
    Model() {}
    bool LoadByAssimp(const std::string& filename);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    void ProcessNode(aiNode* node, const aiScene* scene);
        
    std::vector<MeshPtr> m_meshes;
};

#endif // __MODEL_H__
```

---

## Object Loading

- `src/model.cpp` 파일 생성 후 `Model::Load()` 구현

```cpp
ModelUPtr Model::Load(const std::string& filename) {
  auto model = ModelUPtr(new Model());
  if (!model->LoadByAssimp(filename))
    return nullptr;
  return std::move(model);
}
```

---

## Object Loading

- `Model::LoadByAssimp()` 구현
  - `Assimp::Importer` 클래스의 `ReadFile` 함수 이용
  - `scene->mRootNode`부터 재귀적으로 처리

```cpp
bool Model::LoadByAssimp(const std::string& filename) {
  Assimp::Importer importer;
  auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    SPDLOG_ERROR("failed to load model: {}", filename);
    return false;
  }

  ProcessNode(scene->mRootNode, scene);
  return true;
}
```

---

## Object Loading

- `Model::ProcessNode()` 구현
  - `aiNode` 구조체가 가진 `aiMesh` 에 대해서 처리

```cpp
void Model::ProcessNode(aiNode* node, const aiScene* scene) {
  for (uint32_t i = 0; i < node->mNumMeshes; i++) {
    auto meshIndex = node->mMeshes[i];
    auto mesh = scene->mMeshes[meshIndex];
    ProcessMesh(mesh, scene);
  }

  for (uint32_t i = 0; i < node->mNumChildren; i++) {
    ProcessNode(node->mChildren[i], scene);
  }
}
```

---

## Object Loading

- `Model::ProcessMesh()` 구현

```cpp
void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
  SPDLOG_INFO("process mesh: {}, #vert: {}, #face: {}",
    mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces);

  std::vector<Vertex> vertices;
  vertices.resize(mesh->mNumVertices);
  for (uint32_t i = 0; i < mesh->mNumVertices; i++) {
    auto& v = vertices[i];
    v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
    v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
  }

  std::vector<uint32_t> indices;
  indices.resize(mesh->mNumFaces * 3);
  for (uint32_t i = 0; i < mesh->mNumFaces; i++) {
    indices[3*i  ] = mesh->mFaces[i].mIndices[0];
    indices[3*i+1] = mesh->mFaces[i].mIndices[1];
    indices[3*i+2] = mesh->mFaces[i].mIndices[2];
  }

  auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES);
  m_meshes.push_back(std::move(glMesh));
}

```

---

## Object Loading

- `Model::Draw()` 구현

```cpp
void Model::Draw() const {
  for (auto& mesh: m_meshes) {
    mesh->Draw();
  }
}
```

---

## Object Loading

- `Context` 클래스에 `Model` 인스턴스 멤버 추가

```cpp [2]
MeshUPtr m_box;
ModelUPtr m_model;
```

---

## Object Loading

- `Context::Init()`에서 obj 파일 로딩

```cpp
m_model = Model::Load("./model/backpack.obj");
if (!m_model)
    return false;
```

---

## Object Loading

- 적절한 텍스처를 로딩하기 단일 색상 텍스처를 이용하기 위해 `Image` 클래스에
  새로운 생성 함수 `Image::CreateSingleColorImage()` 추가

```cpp [5-6]
class Image {
public:
  static ImageUPtr Load(const std::string& filepath);
  static ImageUPtr Create(int width, int height, int channelCount = 4);
  static ImageUPtr CreateSingleColorImage(
    int width, int height, const glm::vec4& color);
  ~Image();
```

---

## Object Loading

- `Image::CreateSingleColorImage()` 구현

```cpp
ImageUPtr Image::CreateSingleColorImage(
  int width, int height, const glm::vec4& color) {
  glm::vec4 clamped = glm::clamp(color * 255.0f, 0.0f, 255.0f);
  uint8_t rgba[4] = {
    (uint8_t)clamped.r, 
    (uint8_t)clamped.g, 
    (uint8_t)clamped.b, 
    (uint8_t)clamped.a, 
  };
  auto image = Create(width, height, 4);
  for (int i = 0; i < width * height; i++) {
    memcpy(image->m_data + 4 * i, rgba, 4);
  }
  return std::move(image);
}
```

---

## Object Loading

- `image/container*` 파일 로딩 대신 흰색 diffuse 텍스처, 회색 specular 텍스처 생성

```cpp
m_material.diffuse = Texture::CreateFromImage(
  Image::CreateSingleColorImage(4, 4,
    glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)).get());

m_material.specular = Texture::CreateFromImage(
  Image::CreateSingleColorImage(4, 4,
    glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());
```

---

## Object Loading

- `Context::Render()`의 회전하는 박스 렌더링 코드 제거
- `Model` 렌더링 코드 추가

```cpp
m_program->Use();
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("light.position", m_light.position);
m_program->SetUniform("light.direction", m_light.direction);
m_program->SetUniform("light.cutoff", glm::vec2(
  cosf(glm::radians(m_light.cutoff[0])),
  cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
m_program->SetUniform("light.ambient", m_light.ambient);
m_program->SetUniform("light.diffuse", m_light.diffuse);
m_program->SetUniform("light.specular", m_light.specular);

m_program->SetUniform("material.diffuse", 0);
m_program->SetUniform("material.specular", 1);
m_program->SetUniform("material.shininess", m_material.shininess);
glActiveTexture(GL_TEXTURE0);
m_material.diffuse->Bind();
glActiveTexture(GL_TEXTURE1);
m_material.specular->Bind();

auto modelTransform = glm::mat4(1.0f);
auto transform = projection * view * modelTransform;
m_program->SetUniform("transform", transform);
m_program->SetUniform("modelTransform", modelTransform);
m_model->Draw();
```

---

## Object Loading

- 빌드 및 실행 결과

<div>
<img src="/opengl_course/note/images/10_assimp_model_loading.png" style="width: 75%"/>
</div>

---

## Material Loading

- 모델의 모든 재질 데이터 생성
- 각 메쉬별로 재질 할당

---

## Material Loading

- `Context` 클래스 내에 선언되어 있던 재질 구조체를 `src/mesh.h`로 이동
  - `Context` 클래스 내의 `Material` 관련 코드들 수정

```cpp
CLASS_PTR(Material);
class Material {
public:
    static MaterialUPtr Create() {
        return MaterialUPtr(new Material());
    }
    TexturePtr diffuse;
    TexturePtr specular;
    float shininess { 32.0f };

private:
    Material() {}
};
```

---

## Material Loading

- `Mesh` 클래스 내에 `Material` 하나를 저장할 멤버 변수 선언

```cpp [3-4, 8]
public:
  // ...
  void SetMaterial(MaterialPtr material) { m_material = material; }
  MaterialPtr GetMaterial() const { return m_material; }

private:
  // ...
  MaterialPtr m_material;
```

- `Model` 클래스 내에 `Material` 들을 저장할 배열 멤버 변수 선언

```cpp [2]
  std::vector<MeshPtr> m_meshes;
  std::vector<MaterialPtr> m_materials;
```

---

## Material Loading

- `Model::LoadByAssimp()` 함수 내에 텍스처를 읽어들여서 `Material`
  배열을 생성하는 코드를 `ProcessNode()` 호출 전에 추가

```cpp
auto dirname = filename.substr(0, filename.find_last_of("/"));
auto LoadTexture = [&](aiMaterial* material, aiTextureType type) -> TexturePtr {
  if (material->GetTextureCount(type) <= 0)
    return nullptr;
  aiString filepath;
  material->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);
  auto image = Image::Load(fmt::format("{}/{}", dirname, filepath.C_Str()));
  if (!image)
    return nullptr;
  return Texture::CreateFromImage(image.get());
};

for (uint32_t i = 0; i < scene->mNumMaterials; i++) {
  auto material = scene->mMaterials[i];
  auto glMaterial = Material::Create();
  glMaterial->diffuse = LoadTexture(material, aiTextureType_DIFFUSE);
  glMaterial->specular = LoadTexture(material, aiTextureType_SPECULAR);
  m_materials.push_back(std::move(glMaterial));
}

ProcessNode(scene->mRootNode, scene);
```

---

## Material Loading

- 람다 클로저 (C++11 이상)
  - `std::function<>` 타입에 대입 가능한 함수
  - 캡처 부분에 `&`를 넣으면 해당 클로저 상위 스코프의 모든 값에 접근 가능

```cpp
[capture, ...] (param_type parameters, ...) -> return_type {
  // ... closure body statements
}
```

---

## Material Loading

- `Model::ProcessNode()` 함수에서 `aiMesh::mMaterialIndex`를 보고
  `Material`을 설정

```cpp [1-3]
auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES);
if (mesh->mMaterialIndex >= 0)
  glMesh->SetMaterial(m_materials[mesh->mMaterialIndex]);

m_meshes.push_back(std::move(glMesh));
```

---

## Material Loading

- `Material` 클래스 내에 `SetToProgram()` 함수 선언

```cpp [10]
CLASS_PTR(Material);
class Material {
public:
    static MaterialUPtr Create() {
        return MaterialUPtr(new Material());
    }
    TexturePtr diffuse;
    TexturePtr specular;
    float shininess { 32.0f };

    void SetToProgram(const Program* program) const;

private:
    Material() {}
};
```

---

## Material Loading

- `src/mesh.cpp` 내에 `Material::SetToProgram()` 함수 구현
  - `Program`을 인자로 받아서 텍스처를 설정

```cpp
void Material::SetToProgram(const Program* program) const {
  int textureCount = 0;
  if (diffuse) {
    glActiveTexture(GL_TEXTURE0 + textureCount);
    program->SetUniform("material.diffuse", textureCount);
    diffuse->Bind();
    textureCount++;
  }
  if (specular) {
    glActiveTexture(GL_TEXTURE0 + textureCount);
    program->SetUniform("material.specular", textureCount);
    specular->Bind();
    textureCount++;
  }
  glActiveTexture(GL_TEXTURE0);
  program->SetUniform("material.shininess", shininess);
}
```

---

## Material Loading

- `Mesh::Draw()` 구현 변경

```cpp
void Mesh::Draw(const Program* program) const {
  m_vertexLayout->Bind();
  if (m_material) {
    m_material->SetToProgram(program);
  }
  glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}
```

---

## Material Loading

- `Model::Draw()` 구현 변경

```cpp [1, 3]
void Model::Draw(const Program* program) const {
  for (auto& mesh: m_meshes) {
    mesh->Draw(program);
  }
}
```

---

## Material Loading

- `Context::Render()` 구현 변경

```cpp [17]
m_program->Use();
m_program->SetUniform("viewPos", m_cameraPos);
m_program->SetUniform("light.position", m_light.position);
m_program->SetUniform("light.direction", m_light.direction);
m_program->SetUniform("light.cutoff", glm::vec2(
    cosf(glm::radians(m_light.cutoff[0])),
    cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
m_program->SetUniform("light.ambient", m_light.ambient);
m_program->SetUniform("light.diffuse", m_light.diffuse);
m_program->SetUniform("light.specular", m_light.specular);

auto modelTransform = glm::mat4(1.0f);
auto transform = projection * view * modelTransform;
m_program->SetUniform("transform", transform);
m_program->SetUniform("modelTransform", modelTransform);
m_model->Draw(m_program.get());
```

---

## Material Loading

- 빌드 및 실행 결과

<div>
<img src="/opengl_course/note/images/10_assimp_model_loading_texture.png" style="width: 75%"/>
</div>

---

## TO-DO

- 아직 로딩하지 못한 텍스처
  - `normal.png`: normal mapping
  - `roughness.jpg`: physics-based shader
  - `ao.jpg`: ambient occlusion map
  - 고급 쉐이딩 기법에서 다룸

---

## Congratulation!
### 수고하셨습니다!

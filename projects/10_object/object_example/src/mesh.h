#ifndef __MESH_H__
#define __MESH_H__

#include "common.h"
#include "texture.h"
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
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<TexturePtr> textures;

    VertexLayoutPtr vertexLayout;
    BufferUPtr vertexBuffer;
    BufferUPtr indexBuffer;
};

CLASS_PTR(Model);
class Model {
public:
    std::vector<MeshPtr> meshes;
};

#endif // __MESH_H__
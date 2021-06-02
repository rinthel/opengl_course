#include "mesh.h"

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

MeshUPtr Mesh::CreateBox() {
    std::vector<Vertex> vertices = {
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },

        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) },
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

MeshUPtr Mesh::CreatePlane() {
    std::vector<Vertex> vertices = {
        Vertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
    };

    std::vector<uint32_t> indices = {
         0,  1,  2,  2,  3,  0,
    };

    return Create(vertices, indices, GL_TRIANGLES);
}

MeshUPtr Mesh::CreateSphere(uint32_t latiSegmentCount, uint32_t longiSegmentCount) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t circleVertCount = longiSegmentCount + 1;
    vertices.resize((latiSegmentCount + 1) * circleVertCount);
    for (uint32_t i = 0; i <= latiSegmentCount; i++) {
        float v = (float)i / (float)latiSegmentCount;
        float phi = (v - 0.5f) * glm::pi<float>();
        auto cosPhi = cosf(phi);
        auto sinPhi = sinf(phi);
        for (uint32_t j = 0; j <= longiSegmentCount; j++) {
            float u = (float)j / (float)longiSegmentCount;
            float theta = u * glm::pi<float>() * 2.0f;
            auto cosTheta = cosf(theta);
            auto sinTheta = sinf(theta);
            auto point = glm::vec3(cosPhi * cosTheta, sinPhi, -cosPhi * sinTheta);
            
            vertices[i * circleVertCount + j] = Vertex {
                point * 0.5f, point, glm::vec2(u, v), glm::vec3(0.0f)
            };
        }
    }

    indices.resize(latiSegmentCount * longiSegmentCount * 6);
    for (uint32_t i = 0; i < latiSegmentCount; i++) {
        for (uint32_t j = 0; j < longiSegmentCount; j++) {
            uint32_t vertexOffset = i * circleVertCount + j;
            uint32_t indexOffset = (i * longiSegmentCount + j) * 6;
            indices[indexOffset    ] = vertexOffset;
            indices[indexOffset + 1] = vertexOffset + 1;
            indices[indexOffset + 2] = vertexOffset + 1 + circleVertCount;
            indices[indexOffset + 3] = vertexOffset;
            indices[indexOffset + 4] = vertexOffset + 1 + circleVertCount;
            indices[indexOffset + 5] = vertexOffset + circleVertCount;
        }
    }

    return Create(vertices, indices, GL_TRIANGLES);
}

MeshUPtr Mesh::Create(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    uint32_t primitiveType) {
    auto mesh = MeshUPtr(new Mesh());
    mesh->Init(vertices, indices, primitiveType);
    return std::move(mesh);
}

void Mesh::Init(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    uint32_t primitiveType) {
    if (primitiveType == GL_TRIANGLES) {
        ComputeTangents(const_cast<std::vector<Vertex>&>(vertices), indices);
    }
    m_vertexLayout = VertexLayout::Create();
    m_vertexBuffer = Buffer::CreateWithData(
        GL_ARRAY_BUFFER, GL_STATIC_DRAW,
        vertices.data(), sizeof(Vertex), vertices.size());
    m_indexBuffer = Buffer::CreateWithData(
        GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW,
        indices.data(), sizeof(uint32_t), indices.size());
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, normal));
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, texCoord));
    m_vertexLayout->SetAttrib(3, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, tangent));
}

void Mesh::Draw(const Program* program) const {
    m_vertexLayout->Bind();
    if (m_material) {
        m_material->SetToProgram(program);
    }
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);
}

void Mesh::ComputeTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {

    auto compute = [](const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3,
        const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3) -> glm::vec3 {

        auto edge1 = pos2 - pos1;
        auto edge2 = pos3 - pos1;
        auto deltaUV1 = uv2 - uv1;
        auto deltaUV2 = uv3 - uv1;
        float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        if (det != 0.0f) {
            auto invDet = 1.0f / det;
            return deltaUV2.y * edge1 - deltaUV1.y * edge2;
        }
        else {
            return glm::vec3(1.0f, 0.0f, 0.0f);
        }
    };

    // initialize
    std::vector<glm::vec3> tangents;
    std::vector<float> weights;
    tangents.resize(vertices.size());
    weights.resize(vertices.size());
    memset(tangents.data(), 0, tangents.size() * sizeof(glm::vec3));
    memset(weights.data(), 0, weights.size() * sizeof(float));

    // accumulate triangle tangents to each vertex
    for (size_t i = 0; i < indices.size(); i += 3) {
        auto v1 = indices[i  ];
        auto v2 = indices[i+1];
        auto v3 = indices[i+2];

        tangents[v1] += compute(
            vertices[v1].position, vertices[v2].position, vertices[v3].position,
            vertices[v1].texCoord, vertices[v2].texCoord, vertices[v3].texCoord);
        weights[v1] += 1.0f;

        tangents[v2] = compute(
            vertices[v2].position, vertices[v3].position, vertices[v1].position,
            vertices[v2].texCoord, vertices[v3].texCoord, vertices[v1].texCoord);
        weights[v2] += 1.0f;

        tangents[v3] = compute(
            vertices[v3].position, vertices[v1].position, vertices[v2].position,
            vertices[v3].texCoord, vertices[v1].texCoord, vertices[v2].texCoord);
        weights[v3] += 1.0f;
    }

    // normalize
    for (size_t i = 0; i < vertices.size(); i++) {
        vertices[i].tangent = glm::normalize(tangents[i]);
    }
}
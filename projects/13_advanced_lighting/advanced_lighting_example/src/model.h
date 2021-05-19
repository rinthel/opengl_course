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
    void Draw(const Program* program) const;

private:
    Model() {}
    bool LoadByAssimp(const std::string& filename);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    void ProcessNode(aiNode* node, const aiScene* scene);

    std::vector<MeshPtr> m_meshes;
    std::vector<MaterialPtr> m_materials;
};

#endif // __MODEL_H__
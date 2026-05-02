//
// Created by User on 26. 5. 2..
//

#ifndef __MODEL_H__
#define __MODEL_H__

#include "common.h"
#include "mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

CLASS_PTR(Model);


class Model
{
public:
    static ModelUPtr Load(const std::string& filename);

    [[nodiscard]] int GetMeshCount() const { return static_cast<int>(m_meshes.size()); }
    [[nodiscard]] MeshPtr GetMesh(int index) const { return m_meshes[index]; }
    void Draw() const;

private:
    Model()
    {
    }

    bool LoadByAssimp(const std::string& filename);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    void ProcessNode(aiNode* node, const aiScene* scene);

    std::vector<MeshPtr> m_meshes;
};


#endif //__MODEL_H__

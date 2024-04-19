#include "model.h"

ModelUPtr Model::Load(const std::string &filename){
    auto model = ModelUPtr(new Model());
    if (!model->LoadByAssimp(filename))
        return nullptr;
    return std::move(model);
}

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

void Model::ProcessNode(aiNode *node, const aiScene *scene){
    for (uint32_t i = 0; i < node->mNumMeshes; i++){
        auto meshIndex = node->mMeshes[i];
        auto mesh = scene->mMeshes[meshIndex];
        ProcessMesh(mesh, scene);
    }

    for (uint32_t i = 0; i < node->mNumChildren; i++){
        ProcessNode(node->mChildren[i], scene);
    }
}

void Model::ProcessMesh(aiMesh *mesh, const aiScene *scene){
    spdlog::info("process mesh: {}, #vert:{}, #face{}",
    mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces);

    std::vector<Vertex> vertices;
    vertices.resize(mesh->mNumVertices);
    for (uint32_t i=0; i < mesh->mNumVertices; i++){
        auto &v = vertices[i];
        v.position = glm::vec3(mesh->mVertices[i].x, 
            mesh->mVertices[i].y, mesh->mVertices[i].z);
        v.normal = glm::vec3(mesh->mNormals[i].x,
             mesh->mNormals[i].y, mesh->mNormals[i].z);
        v.texcoord = glm::vec2(mesh->mTextureCoords[0][i].x, 
            mesh->mTextureCoords[0][i].y);
    }

    std::vector<uint32_t> indides;
    indides.resize(mesh->mNumFaces * 3);
    for (uint32_t i = 0; i < mesh->mNumFaces; i++){
        indides[3*i+0] = mesh->mFaces[i].mIndices[0];
        indides[3*i+1] = mesh->mFaces[i].mIndices[1];
        indides[3*i+2] = mesh->mFaces[i].mIndices[2];
    }

    auto glMesh = Mesh::Create(vertices, indides, GL_TRIANGLES);
    m_meshes.push_back(std::move(glMesh));
}

void Model::Draw() const {
    for (auto &mesh : m_meshes) {
        mesh->Draw();
    }
}
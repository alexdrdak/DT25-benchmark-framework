#define _USE_MATH_DEFINES
#include <cmath>
#include <sys/stat.h>
#include "d_Object.h"
#include "../engine_objs/d_Heatmap.h"

/*
*   _dVertCutoutCount: -1 defaults to the provided _path. Otherwise, a file of requested _dVertCutoutCount is tried to fetch.
*/
template<bool dT_instanced>
d_Object<dT_instanced>::d_Object(const std::string& _path, GLuint _spID, int _dVertCutoutCount, bool _dIsFrustumCulled, const std::vector<float>& _data) : d_ObjectBase(_path, _spID, _dVertCutoutCount, _dIsFrustumCulled, _data)
{    
    std::filesystem::path d_tempPath(_path);   

    if(_dVertCutoutCount != -1)
    {
        d_tempPath.replace_filename(d_tempPath.stem().string() + "-C" + std::to_string(_dVertCutoutCount) + d_tempPath.extension().string());
        if (!std::filesystem::exists(d_tempPath) && _dVertCutoutCount != 4) {
            d_Logger::err("'" + d_tempPath.string() + "' not found.Check if the file exists or use - C to chose a different cutout configuration.");
            exit(6);
        }
        else if (!std::filesystem::exists(d_tempPath)) {        
            d_tempPath = _path;
        }
    }

    d_offsetVals = _data;
    d_loadModel(d_tempPath.string());

    assert(_data.size() < d_Defaults::D_SSBO_SIZE, "SSBO too large...");
    glUseProgram(_spID);
    d_amount = _data.size() / 3;

    glGenBuffers(1, &d_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, d_ssbo);
    auto d_data = std::make_unique<d_ssbo_struct>();

    int i = 0;
    for (; i < _data.size() / 3; i++) {
        d_data->d_OffsetPos[i] = glm::vec4(_data[3 * i], _data[3 * i + 1], _data[3 * i + 2], 0.0f);
    }

    for (; i < d_Defaults::D_SSBO_SIZE; i++) {
        d_data->d_OffsetPos[i] = glm::vec4(0.0f);
    }

    glNamedBufferData(d_ssbo, d_Defaults::D_SSBO_SIZE * sizeof(glm::vec4) + sizeof(int), d_data.get(), GL_DYNAMIC_DRAW);

    d_aabb d_temp_bv;
    for (auto& mesh : d_meshes) {
        for (auto& vertex : mesh->d_vertices) {
            d_temp_bv.minAABB.x = std::min(d_temp_bv.minAABB.x, vertex.d_pos.x);
            d_temp_bv.minAABB.y = std::min(d_temp_bv.minAABB.y, vertex.d_pos.y);
            d_temp_bv.minAABB.z = std::min(d_temp_bv.minAABB.z, vertex.d_pos.z);

            d_temp_bv.maxAABB.x = std::max(d_temp_bv.maxAABB.x, vertex.d_pos.x);
            d_temp_bv.maxAABB.y = std::max(d_temp_bv.maxAABB.y, vertex.d_pos.y);
            d_temp_bv.maxAABB.z = std::max(d_temp_bv.maxAABB.z, vertex.d_pos.z);
        }
    }

    d_sbv.d_center = (d_temp_bv.maxAABB + d_temp_bv.minAABB) * 0.5f;
    d_sbv.d_rad = glm::length(d_temp_bv.maxAABB - d_temp_bv.minAABB) * 0.5f;

    if (_dIsFrustumCulled) {
        d_getCommandThroughFrustumCulling = [&](std::array<glm::vec4, 6> _frustum, unsigned int _ssboId, std::vector<d_IndirectCommand>& _commands) {
            if (d_IsOnFrustum(_frustum, _ssboId))
                _commands.push_back({ GLuint(d_elemCount), GLuint(1), GLuint(0), 0, GLuint(_ssboId) });
        };

        d_drawThroughCulling = [&](std::array<glm::vec4, 6> _frustum, unsigned int _ssboId, GLuint _spID, int& _rendered) {
            if (d_IsOnFrustum(_frustum, _ssboId))
            {
                for (auto& mesh : d_meshes)
                    mesh->d_RenderDrawElements(_spID);

                _rendered++;
            }
        };
    }
    else
    {
        d_getCommandThroughFrustumCulling = [&](std::array<glm::vec4, 6> _frustum, unsigned int _ssboId, std::vector<d_IndirectCommand>& _commands) {
            _commands.push_back({ GLuint(d_elemCount), GLuint(1), GLuint(0), 0, GLuint(_ssboId) });
        };

        d_drawThroughCulling = [&](std::array<glm::vec4, 6> _frustum, unsigned int _ssboId, GLuint _spID, int& _rendered) {
            for (auto& mesh : d_meshes) {
                mesh->d_RenderDrawElements(_spID);
            }
            _rendered++;
        };
    }
    
}

template <bool dT_instanced>
d_Object<dT_instanced>::~d_Object() {
    for (auto& mesh : d_meshes)
        mesh->d_Clear();

    glDeleteBuffers(1, &d_ssbo);

}

template<bool dT_instanced>
void d_Object<dT_instanced>::d_loadModel(const std::string& _path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        d_Logger::err("[ASSIMP:]" + std::string(importer.GetErrorString()));
        return;
    }
    d_directory = _path.substr(0, _path.find_last_of('/'));

    d_processNode(scene->mRootNode, scene);
}
template <bool dT_instanced>
std::vector<d_Texture> d_Object<dT_instanced>::d_loadModelTextures(aiMaterial *mat, aiTextureType type) {
    std::vector<d_Texture> textures;
    for(size_t i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        aiGetMaterialTexture(mat, type, i, &str);

        bool skip = false;
        for(size_t j = 0; j < d_texLoaded.size(); j++) {
            if(std::strcmp(d_texLoaded[j].getPath().string().c_str(), str.C_Str()) == 0) {
                textures.push_back(d_texLoaded[j]);
                skip = true;
                break;
            }
        }

        if(!skip) {
            switch (type) {
                case aiTextureType_DIFFUSE: {
                    d_Texture d_newTexture(d_directory + "/" + str.C_Str(), d_TextureType::D_TEXTURE_DIFFUSE);
                    textures.emplace_back(d_newTexture);
                    d_texLoaded.push_back(d_newTexture);
                    break;
                }
                case aiTextureType_SPECULAR: {
                    d_Texture d_newTexture(d_directory + "/" + str.C_Str(), d_TextureType::D_TEXTURE_SPECULAR);
                    textures.emplace_back(d_newTexture);
                    d_texLoaded.push_back(d_newTexture);
                    break;
                }
                default:
                    d_Logger::debug("loadMaterialTextures: texture type not supported.");
                    break;
            }
        }
    }

    return textures;
}
template <bool dT_instanced>
void d_Object<dT_instanced>::d_processNode(aiNode* _node, const aiScene* _scene) {
    for(size_t i = 0; i < _node->mNumMeshes; i++) {
        aiMesh *mesh = _scene->mMeshes[_node->mMeshes[i]];
        d_meshes.push_back(d_processMesh(mesh, _scene));
    }

    for(size_t i = 0; i < _node->mNumChildren; i++) {
        d_processNode(_node->mChildren[i], _scene);
    }
}

template <bool dT_instanced>
std::unique_ptr<d_Mesh> d_Object<dT_instanced>::d_processMesh(aiMesh* _mesh, const aiScene* _scene) {
    std::vector<d_Vertex> d_vertices;
    std::vector<unsigned int> d_indices;
    std::vector<d_Texture> d_textures;

    // == VERTICES ====
    for (unsigned int i = 0; i < _mesh->mNumVertices; i++)
    {
        d_Vertex vertex;
        vertex.d_pos = { _mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z };
        vertex.d_norm = { _mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z };

        if (_mesh->mTextureCoords[0]) { 
            vertex.d_texCoords = { _mesh->mTextureCoords[0][i].x, _mesh->mTextureCoords[0][i].y };
        }
        else {
            vertex.d_texCoords = glm::vec2(0.0f, 0.0f);
        }

        d_vertices.push_back(vertex);
    }

        // == FACES ======
        for (unsigned int i = 0; i < _mesh->mNumFaces; i++)
        {
            aiFace face = _mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                d_indices.push_back(face.mIndices[j]);
        }
        


    // == MATERIAL ===
    if (_mesh->mMaterialIndex >= 0) {
        aiMaterial* material = _scene->mMaterials[_mesh->mMaterialIndex];

        auto diffuseMaps = d_loadModelTextures(material, aiTextureType_DIFFUSE);
        d_textures.insert(d_textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        auto specularMaps = d_loadModelTextures(material, aiTextureType_SPECULAR);
        d_textures.insert(d_textures.end(), specularMaps.begin(), specularMaps.end());
    }

    d_elemCount = d_indices.size(); // protoze vsechno renderovani delame skrze SSBO, tohle si muzem dolovit.
    return std::make_unique<d_Mesh>(d_vertices, d_indices, d_textures);
}

template <bool dT_instanced>
void d_Object<dT_instanced>::d_RenderDraw(GLuint _spID, std::unique_ptr<d_Camera>& _camera, unsigned int& _dRenderedObjects, std::optional<glm::mat4> _transform) {
    glUseProgram(_spID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, d_ssbo);

    glActiveTexture(GL_TEXTURE0);
    glBindImageTexture(0, d_Heatmap::d_heatTexID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    d_ShaderProgram::d_SetUniformMatrix4v(_spID, "view", _camera->d_getLookAt());
    d_ShaderProgram::d_SetUniformMatrix4v(_spID, "projection", _camera->d_getProjectionMat());
    d_ShaderProgram::d_SetUniformMatrix4v(_spID, "transform", d_transform);

    if constexpr (dT_instanced)
    {
        for (auto& mesh : d_meshes) {
            mesh->d_RenderDrawElementsInstanced(_spID, d_amount);
        }

        _dRenderedObjects = d_amount;
    }
    else {
        int rendered = 0;
        auto d_frustum = _camera->d_getFrustum();
        for (int i = 0; i < d_amount; i++)
        {
            d_ShaderProgram::d_SetUniform1i(_spID, "d_ssbo_index", i);
            d_drawThroughCulling(d_frustum, i, _spID, rendered);
        }
        _dRenderedObjects = rendered;        
    }   
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
template <bool dT_instanced>
void d_Object<dT_instanced>::d_RenderDrawIndirect(GLuint _spID, std::vector<d_IndirectCommand> _commands, std::unique_ptr<d_Camera>& _camera, std::optional<glm::mat4> _transform)
{
    glUseProgram(_spID);    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, d_ssbo);

    glActiveTexture(GL_TEXTURE0);
    glBindImageTexture(0, d_Heatmap::d_heatTexID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    d_ShaderProgram::d_SetUniformMatrix4v(_spID, "view", _camera->d_getLookAt());
    d_ShaderProgram::d_SetUniformMatrix4v(_spID, "projection", _camera->d_getProjectionMat());
    d_ShaderProgram::d_SetUniformMatrix4v(_spID, "transform", d_transform);
    
    for (auto& mesh : d_meshes) {
        mesh->d_RenderMultiDrawElementIndirect(_spID, _commands);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template<bool dT_instanced>
void d_Object<dT_instanced>::d_RenderDrawArrays(GLuint _spID, std::unique_ptr<d_Camera>& _camera, std::optional<glm::mat4> _transform)
{
    glUseProgram(_spID);    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, d_ssbo);

    glActiveTexture(GL_TEXTURE0);
    glBindImageTexture(0, d_Heatmap::d_heatTexID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI);

    d_ShaderProgram::d_SetUniformMatrix4v(_spID, "view", _camera->d_getLookAt());
    d_ShaderProgram::d_SetUniformMatrix4v(_spID, "projection", _camera->d_getProjectionMat());
    d_ShaderProgram::d_SetUniformMatrix4v(_spID, "transform", d_transform);

    if constexpr (dT_instanced)
    {
        for (auto& mesh : d_meshes) {
            mesh->d_RenderDrawArraysInstanced(_spID, d_amount);
        }
    }
    else {
        for (int i = 0; i < d_amount; i++)
        {
            d_ShaderProgram::d_SetUniform1i(_spID, "d_ssbo_index", i);
            for (auto& mesh : d_meshes) {
                mesh->d_RenderDrawArrays(_spID);
            }
        }
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

template<bool dT_instanced>
void d_Object<dT_instanced>::d_UpdateTransformLocal(glm::mat4 _trans)
{
    //for (auto& m : d_meshes)
    //{
    //    m->d_OffsetTransform(_trans);
    //}

    d_transform = _trans * d_transform;
}

template<bool dT_instanced>
void d_Object<dT_instanced>::d_SetLocalPos(glm::vec3 _offset)
{
    d_posLocal = _offset;
    d_RecomputeTransform();
}
template<bool dT_instanced>
void d_Object<dT_instanced>::d_SetRotateLocal(glm::vec3 _rot)
{
    d_rotLocal = _rot;
    d_RecomputeTransform();
}
template<bool dT_instanced>
void d_Object<dT_instanced>::d_SetScaleLocal(glm::vec3 _scale)
{
    d_scaleLocal = _scale;
    d_RecomputeTransform();
}

template<bool dT_instanced>
void d_Object<dT_instanced>::d_RecomputeTransform()
{
    const glm::mat4 d_rotMat =
        glm::rotate(glm::mat4(1.0f), glm::radians(d_rotLocal.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(d_rotLocal.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(d_rotLocal.z), glm::vec3(0.0f, 0.0f, 1.0f));

    // translation * rotation * scale (also know as TRS matrix)
    d_transform = glm::translate(glm::mat4(1.0f), d_posLocal) * d_rotMat * glm::scale(glm::mat4(1.0f), d_scaleLocal);
}
template<bool dT_instanced>
void d_Object<dT_instanced>::d_OffsetTransform(glm::mat4 _offset)
{
    d_transform = _offset * d_transform;
}

template<bool dT_instanced>
bool d_Object<dT_instanced>::d_IsOnFrustum(const std::array<glm::vec4, 6> _frustum, unsigned int _ssboId)
{
    float maxScale = glm::max(glm::length(glm::vec3(d_transform[0])), glm::max(glm::length(glm::vec3(d_transform[1])), glm::length(glm::vec3(d_transform[2]))));

    glm::vec3 d_center = glm::vec3(d_transform * glm::vec4(d_offsetVals[3 * _ssboId], d_offsetVals[3 * _ssboId + 1], d_offsetVals[3 * _ssboId + 2], 1.0f));
    for (const glm::vec4& plane : _frustum)
        if ((glm::dot(glm::vec3(plane), d_center) + plane.w) < -(d_sbv.d_rad * maxScale))
            return false;

    return true;
}

template<bool dT_instanced>
void d_Object<dT_instanced>::d_ConstructCommands(std::unique_ptr<d_Camera>& _camera, std::vector<d_IndirectCommand>& _commands)
{
    auto frustum = _camera->d_getFrustum();
    for (int i = 0; i < d_offsetVals.size() / 3; i++)
    {
        d_getCommandThroughFrustumCulling(frustum, i, _commands);
    }

    return;
}


template class d_Object<true>;
template class d_Object<false>;
#ifndef DT25_D_OBJECT_TEMPLATED_H
#define DT25_D_OBJECT_TEMPLATED_H


#include <memory>
#include <optional>
#include "d_Mesh.h"
#include "../OGL_objs/d_ShaderProgram.h"
#include "d_NURBS.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "boost/filesystem.hpp"
#include <filesystem>
#include "d_ObjectBase.h"
#include "d_Camera.h"
#include <array>

template <bool dT_instanced>
class d_Object : public d_ObjectBase {
protected:
    glm::vec3 d_posLocal = { 0.0f, 0.0f, 0.0f };
    glm::vec3 d_rotLocal = { 0.0f, 0.0f, 0.0f };
    glm::vec3 d_scaleLocal = { 1.0f, 1.0f, 1.0f };
    glm::mat4 d_transform = glm::mat4(1.0f);
    void d_Object::d_RecomputeTransform();

public:    
    d_Object(const std::string& _path, GLuint _spID, int _dVertCutoutCount, bool _dIsFrustumCulled, const std::vector<float>& _data);
    ~d_Object();

    void d_loadModel(const std::string& _path);
    std::vector<d_Texture> d_loadModelTextures(aiMaterial* _mat, aiTextureType _type);    
    void d_processNode(aiNode* _node, const aiScene* _scene);
    std::unique_ptr<d_Mesh> d_processMesh(aiMesh* _mesh, const aiScene* _scene);
        
    void d_RenderDraw(GLuint _spID, std::unique_ptr<d_Camera>& _camera, unsigned int& _dRenderedObjects, std::optional<glm::mat4> _transform = std::nullopt);
    void d_RenderDrawIndirect(GLuint _spID, std::vector<d_IndirectCommand> _commands, std::unique_ptr<d_Camera>& _camera, std::optional<glm::mat4> _transform = std::nullopt);
    void d_RenderDrawArrays(GLuint _spID, std::unique_ptr<d_Camera>& _camera, std::optional<glm::mat4> _transform = std::nullopt); // for *special* guys
   
    void d_UpdateTransformLocal(glm::mat4 _trans);

    void d_SetLocalPos(glm::vec3 _offset);
    void d_SetRotateLocal(glm::vec3 _rot);
    void d_SetScaleLocal(glm::vec3 _scale);
    void d_OffsetTransform(glm::mat4 _offset);

    inline glm::vec3 d_GetLocalPos() { return d_posLocal; };
    inline glm::vec3 d_GetLocalRot() { return d_rotLocal; };
    inline glm::vec3 d_GetLocalScale() { return d_scaleLocal; };
    inline glm::mat4 d_GetTrans() { return d_transform; };

    bool d_IsOnFrustum(std::array<glm::vec4, 6> _frustum, unsigned int _ssboId);
    void d_ConstructCommands(std::unique_ptr<d_Camera>& _camera, std::vector<d_IndirectCommand>& _commands);
    std::function<void(std::array<glm::vec4, 6>, unsigned int, std::vector<d_IndirectCommand>&)> d_getCommandThroughFrustumCulling;
    std::function<void(std::array<glm::vec4, 6>, unsigned int, GLuint, int&)> d_drawThroughCulling;    
};

#endif

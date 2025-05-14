#ifndef DT25_d_ObjectBase_H
#define DT25_d_ObjectBase_H


#include <memory>
#include "d_Mesh.h"
#include "../OGL_objs/d_ShaderProgram.h"
#include "d_NURBS.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "boost/filesystem.hpp"
#include <filesystem>
#include "d_Camera.h"
#include <optional>

struct d_ssbo_struct {
    glm::vec4 d_OffsetPos[d_Defaults::D_SSBO_SIZE];
};

struct d_aabb {
    glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
    glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
};

struct d_sphere_bv {
    glm::vec3 d_center = glm::vec3(0.0f);
    float d_rad = 0.0f;
};

class d_ObjectBase {
protected:
public:
    d_ObjectBase(const std::string& _path, GLuint _shaderProgramID, int _dVertCutoutCount, bool _dIsFrustumCulled, const std::vector<float>& _data);
    ~d_ObjectBase();

    std::vector<std::unique_ptr<d_Mesh>> d_meshes;
    std::string d_directory;
    std::vector<d_Texture> d_texLoaded;
    unsigned int d_amount = 0;
    glm::mat4 d_transform = glm::mat4(1.0f);
    GLuint d_ssbo = 0;
    std::vector<float> d_offsetVals;
    d_aabb d_bv;
    d_sphere_bv d_sbv;
    GLuint d_elemCount = 6;

    virtual void d_loadModel(const std::string& _path) = 0;
    virtual std::vector<d_Texture> d_loadModelTextures(aiMaterial* _mat, aiTextureType _type) = 0;
    virtual void d_processNode(aiNode* _node, const aiScene* _scene) = 0;
    virtual std::unique_ptr<d_Mesh> d_processMesh(aiMesh* _mesh, const aiScene* _scene) = 0;
            
    virtual void d_RenderDraw(GLuint _spID, std::unique_ptr<d_Camera>& _camera, unsigned int& _dRenderedObjects, std::optional<glm::mat4> _transform = std::nullopt) = 0;
    virtual void d_RenderDrawIndirect(GLuint _spID, std::vector<d_IndirectCommand> _commands, std::unique_ptr<d_Camera>& _camera, std::optional<glm::mat4> _transform = std::nullopt) = 0;
    
    virtual void d_UpdateTransformLocal(glm::mat4 _trans) = 0;

    virtual bool d_IsOnFrustum(std::array<glm::vec4, 6> _frustum, unsigned int _ssboId) = 0;
    virtual void d_ConstructCommands(std::unique_ptr<d_Camera>& _camera, std::vector<d_IndirectCommand>& _commands) = 0;
    std::function<void(std::array<glm::vec4, 6>, unsigned int)> d_getCommandThroughFrustumCulling;

};

#endif

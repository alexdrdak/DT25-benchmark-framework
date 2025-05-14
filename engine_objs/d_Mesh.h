#ifndef DT25_D_MESH_H
#define DT25_D_MESH_H


#include "../OGL_objs/d_ShaderProgram.h"
#include "../OGL_objs/VertexBufferLayout.h"
#include "d_Texture.h"
#include "../d_glob.h"

struct d_Vertex {
    glm::vec3 d_pos;
    glm::vec3 d_norm;
    glm::vec2 d_texCoords;
};

typedef  struct {
    GLuint  count;
    GLuint instanceCount;
    GLuint  firstIndex;
    GLuint  baseVertex;
    GLuint  baseInstance;
} d_IndirectCommand;

class d_Mesh {
private:
    void d_InitTextures(GLuint _shaderID);

public:
    GLuint d_VBO;
    GLuint d_VAO;
    GLuint d_EBO;
    GLuint d_ICB;

    std::vector<d_Vertex>       d_vertices;
    std::vector<unsigned int>   d_indices;
    std::vector<d_Texture>      d_textures;

    d_Mesh(std::vector<d_Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<d_Texture>& textures);
    ~d_Mesh();

    void d_RenderDrawElements(GLuint _shaderID);    
    void d_RenderDrawElementsInstanced(GLuint _shaderID, unsigned int instanceNumber);    
    void d_RenderDrawElementIndirect(GLuint _shaderID, d_IndirectCommand const _commandQueue);
    void d_RenderMultiDrawElementIndirect(GLuint _shaderID, std::vector<d_IndirectCommand> const _commandQueue);    

    void d_RenderDrawArrays(GLuint _shaderID);
    void d_RenderDrawArraysInstanced(GLuint _shaderID, unsigned int _instanceNumber);

    void d_Clear();
};


#endif

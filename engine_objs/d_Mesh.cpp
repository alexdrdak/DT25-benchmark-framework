#include "d_Mesh.h"

using d_IndexType = GLuint;
constexpr GLenum d_IndexTypeEnum = GL_UNSIGNED_INT;

d_Mesh::d_Mesh(std::vector<d_Vertex>& _vertices, std::vector<d_IndexType>& _indices, std::vector<d_Texture>& _textures): d_vertices(_vertices), d_indices(_indices), d_textures(_textures) {

    glGenVertexArrays(1, &d_VAO);
    glBindVertexArray(d_VAO);

    glGenBuffers(1, &d_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, d_VBO);
    glBufferData(GL_ARRAY_BUFFER, d_vertices.size() * sizeof(d_Vertex), d_vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &d_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, d_indices.size() * sizeof(d_IndexType), d_indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &d_ICB);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, d_ICB);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, d_Defaults::D_SSBO_SIZE * sizeof(d_IndirectCommand), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

    VertexBufferLayout d_vbl;
    d_vbl.d_Push<float>(3, "pos");          // pos
    d_vbl.d_Push<float>(3, "norm");         // normal
    d_vbl.d_Push<float>(2, "texcoord");     // texcoord

    unsigned int n = 0;
    GLint d_Offset = 0;
    GLsizei d_Stride = d_vbl.d_GetStride();
    for (auto& element : d_vbl.d_GetElements()) {
        glVertexAttribPointer(n, element.count, element.type, element.normalized, d_Stride, (const void*)d_Offset);
        glEnableVertexAttribArray(n++);

        d_Offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }

    glBindVertexArray(0);
}

d_Mesh::~d_Mesh()
{
    glDeleteVertexArrays(1, &d_VAO);
    glDeleteBuffers(1, &d_VBO);
    glDeleteBuffers(1, &d_EBO);
    glDeleteBuffers(1, &d_ICB);
}

void d_Mesh::d_RenderDrawElements(GLuint _shaderID) 
{
    d_InitTextures(_shaderID);

    glBindVertexArray(d_VAO);
    glDrawElements(GL_TRIANGLES, d_indices.size(), d_IndexTypeEnum, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}
void d_Mesh::d_RenderDrawElementsInstanced(GLuint _shaderID, unsigned int _instanceNumber) {
    d_InitTextures(_shaderID);

    glBindVertexArray(d_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, d_indices.size(), d_IndexTypeEnum, nullptr, _instanceNumber);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
};
void d_Mesh::d_RenderMultiDrawElementIndirect(GLuint _shaderID, std::vector<d_IndirectCommand> const _commandQueue)
{
    d_InitTextures(_shaderID);

    glBindVertexArray(d_VAO);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, d_ICB);
    glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, _commandQueue.size() * sizeof(d_IndirectCommand), _commandQueue.data());

    GLint bound;
    glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, &bound);
    assert(bound == d_ICB);
    assert(glIsBuffer(d_ICB));

    glMultiDrawElementsIndirect(GL_TRIANGLES, d_IndexTypeEnum, nullptr, _commandQueue.size(), 0);

    glBindVertexArray(0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
}
void d_Mesh::d_RenderDrawElementIndirect(GLuint _shaderID, d_IndirectCommand const _command)
{
    d_InitTextures(_shaderID);

    glBindVertexArray(d_VAO);    
    glDrawElementsIndirect(GL_TRIANGLES, d_IndexTypeEnum, &_command);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void d_Mesh::d_RenderDrawArrays(GLuint _shaderID) {
    d_InitTextures(_shaderID);

    glBindVertexArray(d_VAO);
    glDrawArrays(GL_TRIANGLES, 0, d_vertices.size());
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}
void d_Mesh::d_RenderDrawArraysInstanced(GLuint _shaderID, unsigned int _instanceNumber)
{
    d_Logger::err("not happening");
}

void d_Mesh::d_Clear()
{
    for (auto& tex : d_textures) {
        glDeleteTextures(1, &tex.d_texID);
    }
}

void d_Mesh::d_InitTextures(GLuint _shaderID) 
{
    glUseProgram(_shaderID);

    unsigned int d_diffNr = 1; // diffuse number
    unsigned int d_specNr = 1; // specular number

    unsigned int i = 1;
    for (auto& tex : d_textures) {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string d_locName;

        if (tex.d_getType() == d_TextureType::D_TEXTURE_DIFFUSE) {
            d_locName = ("texture_diffuse") + std::to_string(d_diffNr++);
        }
        else if (tex.d_getType() == d_TextureType::D_TEXTURE_SPECULAR) {
            d_locName = ("texture_specular") + std::to_string(d_specNr++);
        }

        glBindTexture(GL_TEXTURE_2D, tex.d_texID);
        d_ShaderProgram::d_SetUniform1i(_shaderID, d_locName, i++);
    }

}



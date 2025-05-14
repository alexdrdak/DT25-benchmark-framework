#ifndef DT25_D_SHADERPROGRAM_H
#define DT25_D_SHADERPROGRAM_H

#include <vector>
#include "../d_glob.h"

class d_ShaderProgram {
private:    

    static GLuint d_CompileShaderFromFilepath(const std::string& _fpath, GLuint _type, const std::string& _defines = "");
    static GLuint d_CompileShaderFromSource(std::string& _content, GLuint _type, const std::string& _defines = "");
public:
    d_ShaderProgram() = delete;
    ~d_ShaderProgram() = delete;
    
    static void d_FindAndReplaceDefines(std::string& _content, std::string const _defines = "");

    static GLuint d_CreateShaderProgram(const std::string& _vsFPath, const std::string& _fsFPath, const std::string& _defines = "");
    static GLuint d_CreateShaderProgramFromSource(std::string& _vsContent, std::string& _fsContent, const std::string& _defines = "");
    static GLuint d_GetLocation(const GLuint _dID, const std::string& locationName);
    static GLuint d_CompileShaderProgramFromShaders(GLuint _vsId, GLuint _fsId);

    static void d_SetUniform4f(GLuint _shaderID, const std::string& locationName, float v0, float v1, float v2, float v3);
    static void d_SetUniform3f(GLuint _shaderID, const std::string& locationName, float v0, float v1, float v2);
    static void d_SetUniform3f(GLuint _shaderID, const std::string& locationName, glm::vec3 vector3f);
    static void d_SetUniform1f(GLuint _shaderID, const std::string& locationName, float i0);
    static void d_SetUniform1i(GLuint _shaderID, const std::string& locationName, int i0);
    
    static void d_SetUniformMatrix4v(GLuint _shaderID, const std::string &locationName, glm::mat4 transMat);


    static void d_FreeShader(GLuint _shID);
};


#endif

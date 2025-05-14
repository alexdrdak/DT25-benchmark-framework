#include "d_ShaderProgram.h"
#include "../d_glob.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <regex>


GLuint d_ShaderProgram::d_CompileShaderProgramFromShaders(GLuint _vsId, GLuint _fsId)
{

    GLuint d_programID = glCreateProgram();
    glAttachShader(d_programID, _vsId);
    glAttachShader(d_programID, _fsId);
    glLinkProgram(d_programID);
    int success;
    glGetProgramiv(d_programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(d_programID, 512, NULL, infoLog);
        d_Logger::err("[SHADER:] PROGRAM COMPILATION_FAILED. info: '" + std::string(infoLog) + "'");
        return 0;
    }
    glValidateProgram(d_programID);
 
    glDeleteShader(_vsId);
    glDeleteShader(_fsId);

    return d_programID;
}

GLuint d_ShaderProgram::d_CompileShaderFromFilepath(const std::string& _fpath, GLuint _type, const std::string& _defines)
{
    std::ifstream file(_fpath);
    std::stringstream buffer;
    buffer << file.rdbuf();

    auto content = buffer.str();
    if (content.empty()) {
        d_Logger::err("Empty shader code provided.");
        file.close();
        return 0;
    }

    file.close();

    return d_CompileShaderFromSource(content, _type, _defines);
}

GLuint d_ShaderProgram::d_CompileShaderFromSource(std::string& _content, GLuint _type, const std::string& _defines)
{
    d_FindAndReplaceDefines(_content, _defines);

    GLuint d_glID = glCreateShader(_type);
    const char* temp_src = _content.c_str();
    glShaderSource(d_glID, 1, &temp_src, nullptr);
    glCompileShader(d_glID);

    int compileSuccess;
    glGetShaderiv(d_glID, GL_COMPILE_STATUS, &compileSuccess);
    if (!compileSuccess) {
        char infoLog[512];
        glGetShaderInfoLog(d_glID, 512, NULL, infoLog);
        d_Logger::err("[SHADER:] PROGRAM COMPILATION_FAILED. info: '" + std::string(infoLog) + "'");
        return 0;
    }

    return d_glID;
}

GLuint d_ShaderProgram::d_CreateShaderProgram(const std::string& _vsFPath, const std::string& _fsFPath, const std::string& _defines)
{
    if (std::ifstream(_vsFPath).bad()) 
    {
        d_Logger::err("Invalid d_CreateShaderProgram::_vsFpath: " + _vsFPath);
    }
    else if (std::ifstream(_fsFPath).bad()) {
        d_Logger::err("Invalid d_CreateShaderProgram::_fsFpath: " + _fsFPath);
    }

    auto d_vsId = d_CompileShaderFromFilepath(_vsFPath, GL_VERTEX_SHADER, _defines);
    auto d_fsId = d_CompileShaderFromFilepath(_fsFPath, GL_FRAGMENT_SHADER, _defines);
    if(d_vsId == 0 || d_fsId == 0) 
    {
        d_Logger::err("[SHADER:] COMPILATION_FAILED");
        return 0;
    }

    return d_CompileShaderProgramFromShaders(d_vsId, d_fsId);
}
GLuint d_ShaderProgram::d_CreateShaderProgramFromSource(std::string& _vsContent, std::string& _fsContent, const std::string& _defines)
{
    auto d_vsId = d_CompileShaderFromSource(_vsContent, GL_VERTEX_SHADER, _defines);
    auto d_fsId = d_CompileShaderFromSource(_fsContent, GL_FRAGMENT_SHADER, _defines);
    if (d_vsId == 0 || d_fsId == 0)
    {
        d_Logger::err("[SHADER:] COMPILATION_FAILED");
        return 0;
    }
    
    return d_CompileShaderProgramFromShaders(d_vsId, d_fsId);
}

void d_ShaderProgram::d_FindAndReplaceDefines(std::string& _content, std::string const _defines)
{    
    std::regex pattern("\\%\\{D_DEFINES\\}\\%");

    _content = std::regex_replace(_content, pattern, _defines);
}

void d_ShaderProgram::d_SetUniform4f(GLuint _shaderID, const std::string &locationName, float v0, float v1, float v2, float v3) 
{
    glUniform4f(d_GetLocation(_shaderID, locationName), v0, v1, v2, v3);
}

void d_ShaderProgram::d_SetUniform3f(GLuint _shaderID, const std::string &locationName, float v0, float v1, float v2) 
{
    glUniform3f(d_GetLocation(_shaderID, locationName), v0, v1, v2);
}

void d_ShaderProgram::d_SetUniform3f(GLuint _shaderID, const std::string &locationName, glm::vec3 vector3f) 
{
    glUniform3f(d_GetLocation(_shaderID, locationName), vector3f[0], vector3f[1], vector3f[2]);
}

void d_ShaderProgram::d_SetUniform1f(GLuint _shaderID, const std::string &locationName, float v0) 
{
    glUniform1f(d_GetLocation(_shaderID, locationName), v0);
}

void d_ShaderProgram::d_SetUniform1i(GLuint _shaderID, const std::string &locationName, int i0) 
{
    glUniform1i(d_GetLocation(_shaderID, locationName), i0);
}   

void d_ShaderProgram::d_SetUniformMatrix4v(GLuint _shaderID, const std::string &locationName, glm::mat4 transMat) 
{
    glUniformMatrix4fv(d_GetLocation(_shaderID, locationName), 1, GL_FALSE, glm::value_ptr(transMat));
}

GLuint d_ShaderProgram::d_GetLocation(const GLuint _dID, const std::string& locationName)
{
    GLuint location = glGetUniformLocation(_dID, locationName.c_str());
    if(location < 0) {
        d_Logger::debug("Location '" + locationName + "' not found.");       
    }

    return location;
}

void d_ShaderProgram::d_FreeShader(GLuint _shID)
{
    glDeleteProgram(_shID);
}

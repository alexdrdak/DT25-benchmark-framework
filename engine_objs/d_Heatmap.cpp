#include "d_Heatmap.h"

GLuint d_Heatmap::d_ShaderID = 0;
GLuint d_Heatmap::d_quadVAO = 0;
GLuint d_Heatmap::d_heatTexID = 0;
GLuint d_Heatmap::d_tempVBO = 0;

void d_Heatmap::d_Init()
{
    if (d_ShaderID != 0)
        d_Logger::warn("d_Heatmap::d_Init() issued, but the heatmap was already initiated. The heatmap will now be reinitiated");

    d_ShaderID = d_ShaderProgram::d_CreateShaderProgram("shaders/d_heatmap.vert", "shaders/d_heatmap.frag");

    static const float quadVertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &d_quadVAO);
    glGenBuffers(1, &d_tempVBO);
    glBindVertexArray(d_quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, d_tempVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenTextures(1, &d_heatTexID);
    glBindTexture(GL_TEXTURE_2D, d_heatTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, 800, 600, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void d_Heatmap::d_renderHeatmap()
{
	glDisable(GL_DEPTH_TEST);

	glUseProgram(d_ShaderID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, d_heatTexID);
    d_ShaderProgram::d_SetUniform1i(d_ShaderID, "heatmapTex", 0);

	glBindVertexArray(d_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
	glEnable(GL_DEPTH_TEST);
    glClearTexImage(d_heatTexID, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
}

void d_Heatmap::d_Clear()
{
    glDeleteBuffers(1, &d_tempVBO);
    glDeleteVertexArrays(1, &d_quadVAO);
    d_ShaderProgram::d_FreeShader(d_ShaderID);
    glDeleteTextures(1, &d_heatTexID);
}


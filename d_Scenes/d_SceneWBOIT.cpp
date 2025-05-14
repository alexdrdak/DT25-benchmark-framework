#include "d_SceneWBOIT.h"

d_SceneWBOIT::d_SceneWBOIT(std::string& _configPath, int _dRepeats, int _dVertCutoutCount, bool _dWireframed, bool _dHeatmapWrite) : d_SceneBase(_configPath, _dRepeats, _dVertCutoutCount, _dWireframed, _dHeatmapWrite) {
	
	d_shaderID = d_ShaderProgram::d_CreateShaderProgram("shaders/cmoudik.vert", "shaders/d_WBOIT.frag", d_SceneDefines);

	std::string vsContents = "#version 460 core\n"
		"layout(location = 0) in vec2 aPos;\n"
		"layout(location = 1) in vec2 aTexCoord;\n"

		"out vec2 TexCoord;\n"

		"void main()\n"
		"{\n"
			"TexCoord = aTexCoord;\n"
			"gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
		"}\n";

	std::string fsContents = "#version 460 core\n"
		"in vec2 TexCoord;\n"
		"out vec4 FragColor;\n"

		"uniform sampler2D accumColor;\n"
		"uniform sampler2D revealage;\n"

		"void main()\n"
		"{\n"			
			"vec4 accum = texelFetch(accumColor, ivec2(gl_FragCoord.xy), 0);\n"			
			"float reveal = texelFetch(revealage, ivec2(gl_FragCoord.xy), 0).r;\n"
			
			"FragColor = vec4(accum.rgb / max(accum.a, 1e-5), reveal);\n"
		"}";

	d_flushShaderID = d_ShaderProgram::d_CreateShaderProgramFromSource(vsContents, fsContents, d_SceneDefines);

	float planeVertices[] = {
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	glGenVertexArrays(1, &d_flushVAO);
	glGenBuffers(1, &d_tempVBO);
	glBindVertexArray(d_flushVAO);
	glBindBuffer(GL_ARRAY_BUFFER, d_tempVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));	
	glBindVertexArray(0);

	glGenFramebuffers(1, &d_accumFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, d_accumFBO);

	glGenTextures(2, d_texs);
	glBindTexture(GL_TEXTURE_2D, d_texs[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d_texs[0], 0);
	glBindTexture(GL_TEXTURE_2D, d_texs[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 800, 600, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, d_texs[1], 0);

	glUseProgram(d_flushShaderID);	
	d_ShaderProgram::d_SetUniform1i(d_flushShaderID, "accumColor", 0);	
	d_ShaderProgram::d_SetUniform1i(d_flushShaderID, "revealage", 1);
	glUseProgram(0);

	GLenum d_drawBuffs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, d_drawBuffs);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		d_Logger::info("WBOIT init OK.");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

d_SceneWBOIT::~d_SceneWBOIT()
{
	d_ShaderProgram::d_FreeShader(d_shaderID);
	d_ShaderProgram::d_FreeShader(d_flushShaderID);
	glDeleteBuffers(1, &d_tempVBO);
	glDeleteVertexArrays(1, &d_flushVAO);
	glDeleteTextures(2, d_texs);
	glDeleteFramebuffers(1, &d_accumFBO);
}

void d_SceneWBOIT::d_Execute(GLFWwindow* _window)
{

	auto currTime = glfwGetTime();
	d_deltaTime = currTime - d_lastTime;
	d_lastTime = currTime;

	d_camera->d_UpdatePathPosition(float(d_deltaTime));

	unsigned int d_renderedObjects = 0;
	for (auto& object : d_SceneObjectsOpaque) {
		object->d_RenderDraw(d_shaderOpaqueID, d_camera, d_renderedObjects);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, d_accumFBO);
	
	static const float clear01[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	glClearTexImage(d_texs[0], 0, GL_RGBA, GL_FLOAT, clear01);	
	static const float clear02[1] = { 1.0f };
	glClearTexImage(d_texs[1], 0, GL_RED, GL_FLOAT, clear02);	

	glEnable(GL_BLEND);

	glEnablei(GL_BLEND, 0);
	glEnablei(GL_BLEND, 1);
	glBlendFunci(0, GL_ONE, GL_ONE);					// GL_COLOR_ATTACHMENT0
	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);	// GL_COLOR_ATTACHMENT1
	glUseProgram(d_shaderID);	
	
	glDepthMask(GL_FALSE);
	d_SceneBase::d_StartQuery(GL_SAMPLES_PASSED);
	for (const auto& object : d_SceneObjectsTransparencies) {
		d_drawCallOverride(*object, d_shaderID);
	}
	glDepthMask(GL_TRUE);

	d_SceneBase::d_EndQuery(GL_SAMPLES_PASSED);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

	glUseProgram(d_flushShaderID);
	glUniform1i(glGetUniformLocation(d_flushShaderID, "accumColor"), 0);
	glUniform1i(glGetUniformLocation(d_flushShaderID, "revealage"), 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, d_texs[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, d_texs[1]);

	glBindVertexArray(d_flushVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);	
	glBindVertexArray(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

}
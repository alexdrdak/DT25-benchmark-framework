///*
//* Sources:
//*	- https://github.com/autumn-canfield/dual-depth-peeling/blob/master/main.c
//*	- https://developer.download.nvidia.com/SDK/10/opengl/src/dual_depth_peeling/doc/DualDepthPeeling.pdf
//*/
//
//#define D_PEELS_NUM 3
//
//#include "d_SceneDPOIT.h"
//
//static GLenum d_drawBuffs[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
//static const float d_clearDepth[2] = { -1.0f, -1.0f };
//static const float d_clearColorFront[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
//static const float d_clearColorBack[4] = { 0.0, 0.0, 0.0, 0.0f };
//
//d_SceneDPOIT::d_SceneDPOIT(std::string& _configPath, int _dVertCutoutCount, bool _dWireframed, bool _dHeatmapWrite) : d_SceneBase(_configPath, _dVertCutoutCount, _dWireframed, _dHeatmapWrite) 
//{	
//	d_shaderID = d_ShaderProgram::d_CreateShaderProgram("shaders/cmoudik.vert", "shaders/cmoudik.frag", d_SceneDefines);
//}
//
//void d_SceneDPOIT::d_Execute(GLFWwindow* window)
//{
//	auto currTime = glfwGetTime();
//	d_deltaTime = currTime - d_lastTime;
//	d_lastTime = currTime;
//
//	d_camera->d_UpdatePathPosition(float(d_deltaTime));
//
//	glDisable(GL_DEPTH_TEST);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	for (auto& object : d_SceneObjectsOpaque) {
//		object->d_RenderDraw(d_shaderOpaqueID, d_camera);
//	}
//	
//
//	d_SceneBase::d_StartQuery(GL_SAMPLES_PASSED);
//	for (auto& object : d_SceneObjectsTransparencies)
//	{
//		std::vector<d_IndirectCommand> d_commands;
//
//		d_SceneBase::d_ConstructCommands(d_commands);
//
//		object->d_RenderDrawIndirect(d_shaderID, d_commands, d_camera);
// 	}
//	d_SceneBase::d_EndQuery(GL_SAMPLES_PASSED);
//	glBindVertexArray(0);
//	glEnable(GL_DEPTH_TEST);
//
//}
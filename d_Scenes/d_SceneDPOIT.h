//#include "../d_glob.h"
//#include "d_SceneBase.h"
//#include <array>
//
//class d_SceneDPOIT : public d_SceneBase {
//
//private:
//	GLuint d_flushVAO;
//
//	GLuint d_shaderID;
//
//	//ping-pong these mfs...
//	GLuint d_peelFBO[2];
//	GLuint d_peelDepth[2];		// RT0: RG32F
//	GLuint d_peelColorFront;	// RT1: RGBA8
//	GLuint d_peelColorBack;		// RT2: RGBA8
//
//public:
//	d_SceneDPOIT(std::string& _configPath, int _dVertCutoutCount = 4, bool _dWireframed = false, bool _dHeatmapWrite = false);
//
//	void d_Execute(GLFWwindow* window) override;
//};
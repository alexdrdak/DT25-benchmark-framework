#include "../d_glob.h"
#include "d_SceneBase.h"

class d_SceneWBOIT : public d_SceneBase {

private:
	GLuint d_accumFBO, d_shaderID, d_flushShaderID, d_flushVAO;
	GLuint d_texs[2];


	GLuint d_tempVBO;
public:
	d_SceneWBOIT(std::string& _configPath, int _dRepeats = 10, int _dVertCutoutCount = 4, bool _dWireframed = false, bool _dHeatmapWrite = false);
	~d_SceneWBOIT();

	void d_Execute(GLFWwindow* _window) override;
};
#include "d_SceneBase.h"
#include <vector>
#include <string>

class d_SceneOverride : public d_SceneBase {
private:
	float t = 0.0;

public:
	d_SceneOverride(std::string& config_path, int _dRepeats = 10, int _dVertCutoutCount = 4, bool _dWireframed = false, bool _dHeatmapWrite = false);

	void d_Execute(GLFWwindow* window) override;
	void d_Reset() override;
};

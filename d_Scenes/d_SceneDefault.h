#include "d_SceneBase.h"

class d_SceneDefault : public d_SceneBase {
public:
	d_SceneDefault(std::string& config_path, int _dRepeats = 10, int _dVertCutoutCount = 4, bool _dWireframed = false, bool _dHeatmapWrite = false);

	void d_Execute(GLFWwindow* window) override;
};
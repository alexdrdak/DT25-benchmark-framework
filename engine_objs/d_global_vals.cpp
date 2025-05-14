#include "d_global_vals.h"

namespace d_Defaults
{

	const float d_cameraNearPlane = 0.1f;
	const float d_cameraFarPlane = 1000.0f;
	const float d_cameraFOV = 60.0f;
	const float d_cameraTime = 1.0f;
	const int d_cameraPathType = 1;

	const int d_ParticleGenSeed = 42;
	const unsigned int d_particleGenType = 1;

	const bool d_debugMessages = true;
	const int d_repeats = 10;
	const int d_majorVersion = 4;
	const int d_minorVersion = 6;

	const std::string d_windowName = "DP25|AXDRDAK@FAV-ZCU";
	const unsigned int d_windowHeight = 600;
	const unsigned int d_windowWidth = 800;
	const unsigned int d_windowPixels = d_windowHeight * d_windowWidth;
	const unsigned int d_viewportWidth = 600;
	const unsigned int d_viewportHeight = 800;
	const unsigned int d_viewportPixels = d_viewportHeight * d_viewportWidth;
	const std::string d_clearColor = "0.1,0.1,0.1,1.0";

	const bool d_instanced = false;
	const bool d_frustumCulled = false;
	const bool d_heatmapWrite = false;

}
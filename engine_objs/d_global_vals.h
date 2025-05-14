#pragma once

#include <string>

namespace d_Defaults
{
	constexpr const unsigned int D_SSBO_SIZE = 4096*4096;

	extern const float d_cameraNearPlane;
	extern const float d_cameraFarPlane;
	extern const float d_cameraFOV;
	extern const float d_cameraTime;
	extern const int d_cameraPathType;

	extern const int d_ParticleGenSeed;
	extern const unsigned int d_particleGenType;

	extern const bool d_debugMessages;
	extern const int d_repeats;
	extern const int d_majorVersion;
	extern const int d_minorVersion;

	extern const std::string d_windowName;
	extern const unsigned int d_windowHeight;
	extern const unsigned int d_windowWidth;
	extern const unsigned int d_windowPixels;
	extern const unsigned int d_viewportWidth;
	extern const unsigned int d_viewportHeight;
	extern const unsigned int d_viewportPixels;
	extern const std::string d_clearColor;

	extern const bool d_instanced;
	extern const bool d_frustumCulled;
	extern const bool d_heatmapWrite;


}
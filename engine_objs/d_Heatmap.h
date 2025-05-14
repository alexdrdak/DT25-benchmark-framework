#include "../d_glob.h"
#include "../OGL_objs/d_ShaderProgram.h"

class d_Heatmap {
public:
	static GLuint d_ShaderID;
	static GLuint d_quadVAO;
	static GLuint d_heatTexID;
	static GLuint d_tempVBO;

	d_Heatmap() = delete;
	~d_Heatmap() = delete;

	static void d_Init();
	static void d_renderHeatmap();
	static void d_Clear();
};
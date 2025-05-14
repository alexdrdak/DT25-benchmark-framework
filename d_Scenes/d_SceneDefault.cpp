#include "d_SceneDefault.h"
#include "../engine_objs/d_Heatmap.h"

d_SceneDefault::d_SceneDefault(std::string& config_path, int _dRepeats, int _dVertCutoutCount, bool _dWireframed, bool _dHeatmapWrite) : d_SceneBase(config_path, _dRepeats, _dVertCutoutCount, _dWireframed, _dHeatmapWrite)
{
}

void d_SceneDefault::d_Execute(GLFWwindow* window)
{
    auto currTime = glfwGetTime();
    d_deltaTime = currTime - d_lastTime;
    d_lastTime = currTime;

    d_camera->d_UpdatePathPosition(float(d_deltaTime));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& object : d_SceneObjectsOpaque) {
        d_drawCallOverride(*object, d_shaderOpaqueID);
    }
    
    glDepthMask(GL_FALSE);
    d_SceneBase::d_StartQuery(GL_SAMPLES_PASSED);    

    for (const auto& object : d_SceneObjectsTransparencies) {        
        d_drawCallOverride(*object, d_shaderTransparencyID);
    }

    glDepthMask(GL_TRUE);
    d_SceneBase::d_EndQuery(GL_SAMPLES_PASSED);
}
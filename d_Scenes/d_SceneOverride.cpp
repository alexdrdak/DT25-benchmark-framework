#include "d_SceneOverride.h"

d_SceneOverride::d_SceneOverride(std::string& config_path, int _dRepeats, int _dVertCutoutCount, bool _dWireframed, bool _dHeatmapWrite) : d_SceneBase(config_path, _dRepeats, _dVertCutoutCount, _dWireframed, _dHeatmapWrite)
{   }

void d_SceneOverride::d_Execute(GLFWwindow* window)
{
    auto currTime = glfwGetTime();
    d_deltaTime = currTime - d_lastTime;
    d_lastTime = currTime;

    d_camera->d_UpdatePathPosition(float(d_deltaTime));
    glDepthMask(GL_FALSE);
    
    unsigned int d_renderedObjects = 0;
    for (auto& object : d_SceneObjectsOpaque) {
        object->d_RenderDraw(d_shaderOpaqueID, d_camera, d_renderedObjects);
    }

    d_SceneBase::d_StartQuery(GL_SAMPLES_PASSED);

    for (const auto& object : d_SceneObjectsTransparencies) {
        d_drawCallOverride(*object, d_shaderTransparencyID);
    }

    glDepthMask(GL_TRUE);
    d_SceneBase::d_EndQuery(GL_SAMPLES_PASSED);
}

void d_SceneOverride::d_Reset() {
    d_SceneBase::d_Reset();
}
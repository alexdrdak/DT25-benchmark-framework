#ifndef DT25_D_SCENE_H
#define DT25_D_SCENE_H

#include <optional>
#include <vector>
#include "../d_glob.h"
#include "../engine_objs/d_ObjectBase.h"
#include "../engine_objs/d_Object.h"
#include "../engine_objs/d_Camera.h"
#include "../engine_objs/d_ParticleGenerator.h"
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>

class d_SceneBase {
protected:
    std::string d_Name;
    std::string d_Description;

    unsigned int d_CurrProgress = 0;
    double d_lastTime = 0.0f;
    double d_deltaTime = 0.0f;

    float d_TargetTime;

    std::optional<d_ParticleGenerator> d_ParGen;

private:
    std::vector<std::string> d_GetVectorFromConfig(const boost::property_tree::ptree& _configTree, const std::string& _optionName);

public:
    std::vector<unsigned int> d_sampleCount;
    GLuint d_transparentQueries[2];
    int d_currentQueryIndex = 0;
    GLuint d_lastTransparentSamples = 0;
    int d_sampleIt = 0;
    int d_cutoutVerts = 4;
    unsigned int d_renderedObjects = 0;

    std::vector<std::unique_ptr<d_ObjectBase>> d_SceneObjectsOpaque;
    std::vector<std::unique_ptr<d_ObjectBase>> d_SceneObjectsTransparencies;
    GLuint d_shaderOpaqueID, d_shaderTransparencyID;
    std::unique_ptr<d_Camera> d_camera;
    std::vector <float> d_ParticleData;

    bool d_Wireframe, d_HeatmapWrite;
    std::string d_SceneDefines = "";

    d_SceneBase(std::string& _config_path, int _dRepeats = 10, int _dVertCutoutCount = 4, bool _dWireframed = false, bool _dHeatmapWrite = false);
    ~d_SceneBase();

    bool d_IsFinished() const;
    virtual void d_Reset();

    virtual void d_Execute(GLFWwindow* window);

    inline float d_GetTargetTime() const { return d_TargetTime; }

    void d_StartQuery(GLenum _type);
    void d_EndQuery(GLenum _type);

    void d_fetchModelPathsAmounts(const boost::property_tree::ptree& _dConfigTree, std::vector<std::string>& _dModelPaths, std::vector<std::string>& _dModelAmounts, const int _dTransparentOpaque = 0);

    inline std::string d_getName() { return d_Name; }
    inline std::string d_getDescription() { return d_Description; }

    std::function<void(d_ObjectBase&, GLuint)> d_drawCallOverride;

    inline std::vector<unsigned int>& d_getSamples() { return d_sampleCount; }
    inline unsigned int d_getRenderedObjects() { return d_renderedObjects; }
};

#endif

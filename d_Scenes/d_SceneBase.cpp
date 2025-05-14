#include "d_SceneBase.h"

std::vector<std::string> d_SceneBase::d_GetVectorFromConfig(const boost::property_tree::ptree& _configTree, const std::string& _optionName)
{
    std::string wholeString = _configTree.get<std::string>(_optionName, "");
    if (wholeString.empty())
    {
        d_Logger::err("'" + _optionName + "' undefined. Returning empty array...");
        return {};
    }
    wholeString.pop_back();
    wholeString.erase(wholeString.begin());
    std::vector<std::string> result;
    boost::split(result, wholeString, boost::is_any_of(","));

    return result;
}

d_SceneBase::d_SceneBase(std::string& _config_path, int _dRepeats, int _dVertCutoutCount, bool _dWireframed, bool _dHeatmapWrite): d_cutoutVerts(_dVertCutoutCount) {
    d_Logger::info("====== LOADING '" + _config_path + "' ======");

    if (!std::filesystem::exists(_config_path))
    {
        d_Logger::err("'" + _config_path + "' config not found! Aborting...");
        exit(7);
    }

    boost::property_tree::ptree configTree;
    boost::property_tree::ini_parser::read_ini(_config_path, configTree);


    // general info load
    d_Name = configTree.get<std::string>("GENERAL.name", "");
    d_Description = configTree.get<std::string>("GENERAL.description", "");
    if (d_Name.empty() || d_Description.empty())
    {
        d_Logger::err("GENERAL.name || GENERAL.descriptor missing or provided in a wrong format. Exiting.");
        exit(6);
    }

    // particle generator
    int d_PartGenType = configTree.get<int>("PARTICLEGEN.type", d_Defaults::d_particleGenType);
    switch (d_PartGenType) {
    case 1:
    {
        std::vector<float> d_PartGenXArr;
        std::vector<float> d_PartGenYArr;
        std::vector<float> d_PartGenZArr;

        if (!configTree.get_child_optional("PARTICLEGEN.x"))
        {
            d_Logger::err("PARTICLEGEN.x could not be found. Scene construction failed, check the README.md for correct scene configuration.");
            exit(6);
        }

        std::vector<std::string> d_PartGenX = d_GetVectorFromConfig(configTree, "PARTICLEGEN.x");
        std::for_each(d_PartGenX.begin(), d_PartGenX.end(), [&](auto a) { d_PartGenXArr.push_back(std::stof(a)); });

        if (!configTree.get_child_optional("PARTICLEGEN.y"))
        {
            d_Logger::err("PARTICLEGEN.y could not be found. Default [0, 1] option will be used.");
            d_PartGenYArr.push_back(0);
            d_PartGenYArr.push_back(1);
        }
        else
        {
            std::vector<std::string> d_PartGenY = d_GetVectorFromConfig(configTree, "PARTICLEGEN.y");
            std::for_each(d_PartGenY.begin(), d_PartGenY.end(), [&](auto a) { d_PartGenYArr.push_back(std::stof(a)); });
        }
        if (!configTree.get_child_optional("PARTICLEGEN.z"))
        {
            d_Logger::err("PARTICLEGEN.z could not be found. Scene construction failed, check the README.md for correct scene configuration.");
            exit(6);
        }

        std::vector<std::string> d_PartGenZ = d_GetVectorFromConfig(configTree, "PARTICLEGEN.z");
        std::for_each(d_PartGenZ.begin(), d_PartGenZ.end(), [&](auto a) { d_PartGenZArr.push_back(std::stof(a)); });

        auto optional_seed = configTree.get_child_optional("PARTICLEGEN.seed");
        if (!optional_seed)
        {
            d_Logger::info("PARTICLEGEN.seed not set. Setting default 42 seed.");
        }
        d_ParGen = d_ParticleGenerator(d_PartGenType,
            glm::vec3(d_PartGenXArr[0], d_PartGenXArr[1], 0),
            glm::vec3(d_PartGenYArr[0], d_PartGenYArr[1], 0),
            glm::vec3(d_PartGenZArr[0], d_PartGenZArr[1], 0),
            configTree.get<int>("PARTICLEGEN.seed",
                d_Defaults::d_ParticleGenSeed)
        );

        break;
    }

    case 2: {
        std::string startString = configTree.get<std::string>("PARTICLEGEN.start", "");
        std::string endString = configTree.get<std::string>("PARTICLEGEN.end", "");
        if (startString.empty() || endString.empty())
        {
            d_Logger::err("PARTICLEGEN.start|end not defined. Exiting...");
            exit(6);
        }

        if (startString[0] == '[')
        {

            startString.pop_back();
            startString.erase(startString.begin());
            endString.pop_back();
            endString.erase(endString.begin());
        }
        std::vector<std::string> startStringArr;
        std::vector<std::string> endStringArr;
        boost::split(startStringArr, startString, boost::is_any_of(","));
        boost::split(endStringArr, endString, boost::is_any_of(","));
        glm::vec3 d_Start(std::stof(startStringArr[0]), std::stof(startStringArr[1]), std::stof(startStringArr[2]));
        glm::vec3 d_End(std::stof(endStringArr[0]), std::stof(endStringArr[1]), std::stof(endStringArr[2]));
        //            int number = configTree.get<int>("PARTICLEGEN.number");

        d_ParGen = d_ParticleGenerator(d_PartGenType, d_Start, d_End);
        break;
    }

    case 3:
    {
        std::string centerString = configTree.get<std::string>("PARTICLEGEN.ltop", "");
        std::string centerStringFallback = configTree.get<std::string>("PARTICLEGEN.center", "");
        std::string axis1String = configTree.get<std::string>("PARTICLEGEN.axis1", "");
        std::string axis2String = configTree.get<std::string>("PARTICLEGEN.axis2", "");
        if ((centerString.empty() && centerStringFallback.empty()) || axis1String.empty() || axis2String.empty())
        {
            d_Logger::err("PARTICLEGEN.ltop|| .axis1 || .axis2 required for PARTICLEGEN.mode=3 not defined. Exiting...");
            exit(6);
        }

        if (centerString[0] == '[')
        {

            centerString.pop_back();
            centerString.erase(centerString.begin());
        }
        if (centerStringFallback[0] == '[')
        {

            centerStringFallback.pop_back();
            centerStringFallback.erase(centerString.begin());
        }
        if (axis1String[0] == '[')
        {
            axis1String.pop_back();
            axis1String.erase(axis1String.begin());
        }
        if (axis2String[0] == '[')
        {
            axis2String.pop_back();
            axis2String.erase(axis2String.begin());
        }

        std::vector<std::string> centerStringArray;
        std::vector<std::string> centerStringFallbackArray;
        std::vector<std::string> axis1StringArray;
        std::vector<std::string> axis2StringArray;
        boost::split(centerStringArray, centerString, boost::is_any_of(","));
        boost::split(centerStringFallbackArray, centerStringFallback, boost::is_any_of(","));
        boost::split(axis1StringArray, axis1String, boost::is_any_of(","));
        boost::split(axis2StringArray, axis2String, boost::is_any_of(","));
        if (centerStringArray.size() != 3 || axis1StringArray.size() != 3 || axis2StringArray.size() != 3)
        {
            d_Logger::err("Incorrect configuration of grid distribution! Invalid corners defined. Exiting...");
            exit(6);
        }

        glm::vec3 center;
        if(!centerStringArray.empty())
            center = { std::stof(centerStringArray[0]), std::stof(centerStringArray[1]), std::stof(centerStringArray[2]) };
        else
            center = { std::stof(centerStringFallbackArray[0]), std::stof(centerStringFallbackArray[1]), std::stof(centerStringFallbackArray[2]) };
        glm::vec3 axis1 = { std::stof(axis1StringArray[0]), std::stof(axis1StringArray[1]), std::stof(axis1StringArray[2]) };
        glm::vec3 axis2 = { std::stof(axis2StringArray[0]), std::stof(axis2StringArray[1]), std::stof(axis2StringArray[2]) };

        if (center == axis1 || axis1 == axis2 || center == axis2)
        {
            d_Logger::err("Incorrect configuration of grid distribution! Points can not coincide. Exiting...");
            exit(6);
        }

        d_ParGen = d_ParticleGenerator(d_PartGenType, center, axis1, axis2);
        break;
    }

    case 4:
    case 5:
    {
        std::string centerString= configTree.get<std::string>("PARTICLEGEN.center", "");
        float radius = configTree.get<float>("PARTICLEGEN.radius", 1.0f);
        
        if (centerString.empty() ||  radius == 0)
        {
            d_Logger::err("PARTICLEGEN.center || .radius required for PARTICLEGEN.mode=4 not defined. Exiting...");
            exit(6);
        }

        if (centerString[0] == '[')
        {
            centerString.pop_back();
            centerString.erase(centerString.begin());
        }

        std::vector<std::string> centerStringArray;
        boost::split(centerStringArray, centerString, boost::is_any_of(","));
        if (centerStringArray.size() != 3)
        {
            d_Logger::err("Incorrect configuration of grid distribution! Invalid corners defined. Exiting...");
            exit(6);
        }

        d_ParGen = d_ParticleGenerator(d_PartGenType, glm::vec3(std::stof(centerStringArray[0]), std::stof(centerStringArray[1]), std::stof(centerStringArray[2])), radius);
        break;
    }

    }

    // models load
    std::vector<std::string> d_TmodelPaths = {};
    std::vector<std::string> d_TmodelAmounts = {};
    d_fetchModelPathsAmounts(configTree, d_TmodelPaths, d_TmodelAmounts, 1);

    std::vector<std::string> d_OmodelPaths = {};
    std::vector<std::string> d_OmodelAmounts = {};
    d_fetchModelPathsAmounts(configTree, d_OmodelPaths, d_OmodelAmounts, 0);


    d_Wireframe = configTree.get<bool>("MISC.wireframe", false) || _dWireframed;
    if (d_Wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    bool d_IsIndirect = configTree.get<bool>("MISC.indirect", false);
    if (d_IsIndirect)
    {
        d_drawCallOverride = [this](d_ObjectBase& object, GLuint d_shaderID) {
            std::vector<d_IndirectCommand> d_commands;
            object.d_ConstructCommands(d_camera, d_commands);
            d_renderedObjects = d_commands.size();
            object.d_RenderDrawIndirect(d_shaderID, d_commands, d_camera);
        };
    }
    else
    {
        d_drawCallOverride = [this](d_ObjectBase& object, GLuint d_shaderID) {
            object.d_RenderDraw(d_shaderID, d_camera, d_renderedObjects);
        };
    }

    d_Logger::info("Compiling shaders...");
    bool d_IsInstanced = configTree.get<bool>("MISC.instanced", d_Defaults::d_instanced);
    bool d_IsFrustumCulled = configTree.get<bool>("MISC.frustumCulled", d_Defaults::d_frustumCulled);
    int dMethod = configTree.get<int>("GENERAL.method", -1);
    if (d_IsInstanced && d_IsIndirect)
    {
        d_Logger::err("Conflicting config (instanced/indirect). Exiting...");
        exit(6);
    }
    d_SceneDefines = (d_IsInstanced) ? "#define D_INSTANCED\n" : "";
    d_SceneDefines += (d_IsIndirect) ? "#define D_INDIRECT\n" : "";
    d_SceneDefines += (d_Wireframe) ? "#define D_WIREFRAMED\n" : "";
    d_HeatmapWrite = configTree.get<bool>("MISC.heatmap", d_Defaults::d_heatmapWrite) || _dHeatmapWrite;
    d_SceneDefines += (d_HeatmapWrite) ? "#define D_HEATMAPWRITE\n" : "";

    d_Logger::info("INSTANCED [" + std::to_string(d_IsInstanced) + "] INDIRECT [" + std::to_string(d_IsIndirect) + "] WIREFRAMED: [" + std::to_string(d_Wireframe) + "] HEATMAP [" + std::to_string(d_HeatmapWrite) + "]");

    d_shaderOpaqueID = d_ShaderProgram::d_CreateShaderProgram("shaders/default.vert", "shaders/default.frag");
    d_shaderTransparencyID = d_ShaderProgram::d_CreateShaderProgram("shaders/cmoudik.vert", "shaders/cmoudik.frag", d_SceneDefines); 
    if (d_shaderOpaqueID == 0 || d_shaderTransparencyID == 0)
    {
        d_Logger::err("Error compiling shaders.");
        exit(5);
    }
    d_Logger::info("Compiling shaders OK.");

    int it = 0;
    for(auto amounts : d_TmodelAmounts)
    {
        int amountsInt = std::stoi(amounts);
        if (amountsInt == 0)
            continue;

        d_ParticleData = d_ParGen->d_GenerateData(amountsInt);
        if (d_IsInstanced)
            d_SceneObjectsTransparencies.emplace(
                d_SceneObjectsTransparencies.begin(), 
                std::make_unique<d_Object<true>>(d_TmodelPaths[it++], d_shaderTransparencyID, _dVertCutoutCount, d_IsFrustumCulled, std::move(d_ParticleData))
            );
        else
            d_SceneObjectsTransparencies.emplace(
                d_SceneObjectsTransparencies.begin(), 
                std::make_unique<d_Object<false>>(d_TmodelPaths[it++], d_shaderTransparencyID, _dVertCutoutCount, d_IsFrustumCulled, std::move(d_ParticleData))
            );
    }

    it = 0;
    for (auto amounts : d_OmodelAmounts)
    {       
        int amountsInt = std::stoi(amounts);
        if (amountsInt == 0)
            continue;

        d_SceneObjectsOpaque.emplace(
            d_SceneObjectsOpaque.begin(), 
            std::make_unique<d_Object<false>>(d_OmodelPaths[it++], d_shaderTransparencyID, _dVertCutoutCount, d_IsFrustumCulled, std::vector<float>{0, 0, 0}));
    }

    d_TargetTime = configTree.get<float>("CAMERA.time", d_Defaults::d_cameraTime);

    int cameraPathType = configTree.get<int>("CAMERA.type", d_Defaults::d_cameraPathType);

    switch (cameraPathType) {
        case 1: {
            auto startKnotString = configTree.get<std::string>("CAMERA.start_knot", "");
            auto endKnotString = configTree.get<std::string>("CAMERA.end_knot", "");
            if (startKnotString.empty() || endKnotString.empty())
            {
                d_Logger::err("Missing start_knot || end_knot in the config file. Exiting...");
                exit(6);
            }

            std::vector<std::string> startKnot;
            boost::split(startKnot, startKnotString, boost::is_any_of(","));
            std::vector<std::string> endKnot;
            boost::split(endKnot, endKnotString, boost::is_any_of(","));

            glm::vec3 start_vec = glm::vec3(std::stof(startKnot[0]), std::stof(startKnot[1]), std::stof(startKnot[2]));
            glm::vec3 end_vec = glm::vec3(std::stof(endKnot[0]), std::stof(endKnot[1]), std::stof(endKnot[2]));


            d_camera = std::make_unique<d_Camera>(
                    start_vec,
                    end_vec,
                    glm::vec3(1.0f, 0.0f, 0.0f),
                    std::vector<glm::vec3>{
                            start_vec,
                            end_vec,
                    },
                    nullptr,
                    configTree.get<float>("CAMERA.fov", d_Defaults::d_cameraFOV),
                    configTree.get<float>("RENDER.NEARPLANE", d_Defaults::d_cameraNearPlane),
                    configTree.get<float>("RENDER.FARPLANE", d_Defaults::d_cameraFarPlane),
                    d_TargetTime
            );

            if (!d_camera)
            {
                d_Logger::err("Camera was not initialized. Exiting...");
                exit(8);
            }

            break;
        }
        case 2: {
            auto startKnotString = configTree.get<std::string>("CAMERA.start_knot", "");
            auto endKnotString = configTree.get<std::string>("CAMERA.end_knot", "");
            if (startKnotString.empty() || endKnotString.empty())
            {
                d_Logger::err("Missing start_knot || end_knot in the config file. Exiting...");
                exit(6);
            }

            std::vector<std::string> startKnot;
            boost::split(startKnot, startKnotString, boost::is_any_of(","));
            std::vector<std::string> endKnot;
            boost::split(endKnot, endKnotString, boost::is_any_of(","));

            glm::vec3 start_vec = glm::vec3(std::stof(startKnot[0]), std::stof(startKnot[1]), std::stof(startKnot[2]));
            glm::vec3 end_vec = glm::vec3(std::stof(endKnot[0]), std::stof(endKnot[1]), std::stof(endKnot[2]));

            std::vector<glm::vec3> points;
            points.push_back(start_vec);

            auto midKnots = configTree.get<std::string>("CAMERA.mid_knots", "");
            if (midKnots.empty())
            {
                d_Logger::err("CAMERA.mid_knots is required for set camera option!");
                exit(6);
            }
            if (midKnots[0] != '[')
            {
                d_Logger::err("CAMERA.mid_knots has incorrect format.");
                exit(6);
            }

            std::vector<std::string> midKnotsStrings;
            boost::split(midKnotsStrings, midKnots, boost::is_any_of("]"));

            if(!midKnots.empty()) {
                midKnotsStrings[0].insert(midKnotsStrings[0].begin(), ',');
                for (auto string: midKnotsStrings) {
                    if (string.empty())
                        continue;
                    string.erase(string.begin());
                    string.erase(string.begin());

                    std::vector<std::string> parsedString;
                    boost::split(parsedString, string, boost::is_any_of(","));
                    points.emplace_back(std::stof(parsedString[0]), std::stof(parsedString[1]),
                                        std::stof(parsedString[2]));
                }
            }

            points.push_back(end_vec);

            d_NURBS camCurve(points, 2);

            d_camera = std::make_unique<d_Camera>(
                    start_vec,
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(1.0f, 0.0f, 0.0f),
                    points,
                    std::make_unique<d_NURBS>(camCurve),
                    configTree.get<float>("CAMERA.fov", d_Defaults::d_cameraFOV),
                    configTree.get<float>("RENDER.NEARPLANE", d_Defaults::d_cameraNearPlane),
                    configTree.get<float>("RENDER.FARPLANE", d_Defaults::d_cameraFarPlane),
                    d_TargetTime
            );

            if (!d_camera)
            {
                d_Logger::err("Camera was not initialized. Exiting...");
                exit(8);
            }

            break;
        }
        default:
            break;
    }

    d_Logger::info("Scene '" + d_Name + "' successfully loaded.");
    d_lastTime = glfwGetTime();

    glGenQueries(2, d_transparentQueries);
    
    d_sampleCount.resize(5000 * d_TargetTime);
}

d_SceneBase::~d_SceneBase()
{
    d_ShaderProgram::d_FreeShader(d_shaderOpaqueID);
    d_ShaderProgram::d_FreeShader(d_shaderTransparencyID);
}

bool d_SceneBase::d_IsFinished() const {
    return d_camera->d_GetProgress() >= 1.0f;
}

void d_SceneBase::d_Reset() {
    d_camera->d_Reset();
    for (auto& obj : d_SceneObjectsTransparencies) {
        obj->d_transform = glm::mat4(1.0f);
    }

    d_currentQueryIndex = 0;
    d_lastTransparentSamples = 0;
}

void d_SceneBase::d_Execute(GLFWwindow* window) {
    d_Logger::warn("Class d_SceneBase is not supposed to be used for render.");
}

void d_SceneBase::d_StartQuery(GLenum _type)
{
    glBeginQuery(_type, d_transparentQueries[d_currentQueryIndex]);
}

void d_SceneBase::d_EndQuery(GLenum _type)
{
    glEndQuery(_type);

    GLuint d_prevval;
    glGetQueryObjectuiv(d_transparentQueries[d_currentQueryIndex], GL_QUERY_RESULT, &d_prevval);
    if(d_sampleIt + 1 < d_sampleCount.size())
        d_sampleCount[d_sampleIt++] = d_prevval;
}

// _dTransparentOpaque = 0 - opaque
// _dTransparentOpaque = 1 - transparent
void d_SceneBase::d_fetchModelPathsAmounts(const boost::property_tree::ptree& _dConfigTree, std::vector<std::string>& _dModelPaths, std::vector<std::string>& _dModelAmounts, const int _dTransparentOpaque)
{
    std::string mode = (_dTransparentOpaque == 0) ? "opaque" : "transparency";
    std::string d_PathsPath = "OBJECT." + mode + "ModelPaths";
    std::string d_AmountsPath = "OBJECT." + mode + "ModelAmounts";

    auto d_modelPathsString = _dConfigTree.get<std::string>(d_PathsPath, "");
    auto d_modelAmountsString = _dConfigTree.get<std::string>(d_AmountsPath, "");
    d_Logger::info("INFO: "+ d_PathsPath +" from .ini: '" + d_modelPathsString + "'.");
    d_Logger::info("INFO: "+ d_AmountsPath +" from .ini: '" + d_modelAmountsString + "'.");

    boost::split(_dModelPaths, d_modelPathsString, boost::is_any_of(","));
    if (!d_modelAmountsString.empty())
    {
        boost::split(_dModelAmounts, d_modelAmountsString, boost::is_any_of(","));
        if (_dModelAmounts.size() != _dModelPaths.size()) {
            d_Logger::err("unequal size of ModelPaths and ModelAmounts (" + d_modelPathsString + ", " + d_modelAmountsString + ")");
            exit(6);
        }
    }
    else if (!_dModelPaths.empty()) {
        d_Logger::warn("OBJECT.modelAmounts undefined. Falling back to default [1,...,1].");
        std::fill_n(_dModelAmounts.begin(), _dModelPaths.size(), "1");
    }
    else {
        d_Logger::warn("Querried " + mode + " objects, but no models were provided...");
        return;
    }

}


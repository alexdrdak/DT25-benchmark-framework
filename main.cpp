#include <vector>
#include <thread>
#include <algorithm>
#include <regex>
#include "d_glob.h"
#include "d_Scenes/d_SceneBase.h"
#include "engine_objs/d_Stats.h"
#include "engine_objs/d_ImgExport.h"

#include "engine_objs/stb_image.cpp"
#include <filesystem>
#include "d_Scenes/d_SceneOverride.h"
#include "d_Scenes/d_SceneDefault.h"
#include "d_Scenes/d_SceneWBOIT.h"
//#include "d_Scenes/d_SceneDPOIT.h"
#include "engine_objs/d_Heatmap.h"

#include <winrt/base.h>
#include <windows.h>
#include "profileapi.h"

static inline int64_t GetTicks()
{
    LARGE_INTEGER ticks;
    if (!QueryPerformanceCounter(&ticks))
    {
        winrt::throw_last_error();
    }
    return ticks.QuadPart;
}

// Callback function for printing debug statements
void APIENTRY GLDebugMessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length,
    const GLchar* msg, const void* data)
{
    std::string _source;
    std::string _type;
    std::string _severity;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

    case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

    case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

    case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

    default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

    case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

    case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

    case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

    case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

    default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

    case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

    case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

    default:
        _severity = "UNKNOWN";
        break;
    }

    // ignore notification severity (you can add your own ignores)
    // + Adds __debugbreak if _DEBUG is defined (automatic in visual studio)
    // note: __debugbreak is specific for MSVC, won't work with gcc/clang
    // -> in that case remove it and manually set breakpoints
    if (_severity != "NOTIFICATION") {
        std::cout << "OpenGL error [" << std::to_string(id) << "]: " << _type << " of " << _severity << " severity, raised from " << _source << ": " << msg << std::endl;
#ifdef _DEBUG
        //__debugbreak();
#endif
    }
}

void setupScene(std::string& config_path);

bool d_setupGLFW();
std::vector<std::string> d_GetScenePaths(boost::property_tree::ptree& ptree);
int main(int argc, char** argv);
bool checkForRequiredConfigDisrepancies(std::vector<std::string>& vector1, std::vector<std::string>& vector2, bool _enforceOrder = false);
std::unique_ptr<d_SceneBase> d_getScene(std::vector < std::string> _sceneConfigPaths, unsigned int _dIndex, int _repeats, unsigned int _dCutoutSize, bool _dWireframeOverride, bool _dHeatmapOverride);

GLFWwindow* d_Window;

int main(int argc, char** argv) {
    if(!d_setupGLFW()) {
        d_Logger::err("GLFW could not be initialized.");
    }

    d_Logger::info("GLFW initialized.");

    int skippedScenes = 0;
    int activeSwitch = -1; // -C : 0
    bool enforcedOrder = false;
    bool d_wireframeOverride = false;
    bool d_heatmapWriteOverride = false;
    std::vector<std::vector<std::string>> options;
    options.resize(3);

    options[0] = {};
    for(int i = 1; i < argc; i++) {
        if(std::string(argv[i]) == "-S") {
            activeSwitch = 0;
            continue;
        }
        if(std::string(argv[i]) == "-SF") {
            enforcedOrder = true;
            continue;
        }

        if(std::string(argv[i]) == "-R") {
            activeSwitch = 1;
            continue;
        }

        if (std::string(argv[i]) == "-C") {
            activeSwitch = 2;
            continue;
        }

        if (std::string(argv[i]) == "-W") {
            d_wireframeOverride = true;
            continue;
        }

        if (std::string(argv[i]) == "-H") {
            d_heatmapWriteOverride = true;
            continue;
        }

        if(std::string(argv[i]).at(0) == '-') {
            d_Logger::err("Unknown parameter option '" + std::string(argv[i])+"'. Now aborting...");
            exit(1);
        } else if (activeSwitch == -1) {
            d_Logger::err("Unknown switches received: '" + std::string(argv[i]) + "'. Make sure the order of the switches is correct, otherwise consult the README.md.");
            exit(2);
        }

        switch(activeSwitch) {
            case 0:
                options[activeSwitch].emplace_back("scene_configs/"+std::string(argv[i])+".ini");
                break;
            case 1:
                options[activeSwitch].emplace_back(argv[i]);
                break;
            case 2:
                options[activeSwitch].emplace_back(argv[i]);
                break;
            default:
                break;
        }
    }

    boost::property_tree::ptree configTree;
    boost::property_tree::ini_parser::read_ini("mainconfig.ini", configTree);

    if (configTree.get<int>("DEBUG.MESSAGES", d_Defaults::d_debugMessages))
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(GLDebugMessageCallback, 0);
    }

    std::vector<std::string> scenes_configs_paths = d_GetScenePaths(configTree);

    //if (!options[0].empty() && !checkForRequiredConfigDisrepancies(options[0], scenes_configs_paths, enforcedOrder)) {
    //    exit(3);
    //}
    //if (options[2].size() > 0 && options[2].size() != options[0].size())
    //{
    //    d_Logger::err("The defined cutout # vertices does not fit with defined configs. Make sure these two align properly and exactly.");
    //    exit(3);
    //}
    
    // cmd override, ma prednost.
    if (!options[0].empty()) {
        std::string d_tempAllConfigs = "";
        std::for_each(options[0].begin(), options[0].end()-1, [&d_tempAllConfigs](const std::string el) { d_tempAllConfigs += el + ",";});
        d_tempAllConfigs += options[0].back();
        d_Logger::info("`*.ini` configs from command-line found: '" + d_tempAllConfigs + "'.");

        // sanity-check
        //std::for_each(options[0].begin(), options[0].end(), [](std::string& el) {
        //    std::filesystem::path d_file(el);
        //    if (d_file.has_extension()) {
        //        d_Logger::err("Problem with the filename provided: '"+d_file.string() + "'. Files are to be provided via names in the `scene-configs` folder only. Exiting...");
        //        exit(5);
        //    }
        //});

        scenes_configs_paths.clear();
        scenes_configs_paths = options[0];
    }
    else
    {
        std::string d_tempAllConfigs = "";
        std::for_each(scenes_configs_paths.begin(), scenes_configs_paths.end()-1, [&d_tempAllConfigs](const std::string& el) { d_tempAllConfigs += el + ",";});
        d_tempAllConfigs += scenes_configs_paths.back();
        d_Logger::info("`*.ini` configs from `mainconfig.ini` found: '" + d_tempAllConfigs + "'.");
    }
    if(scenes_configs_paths.empty()) {
        d_Logger::err("No configs requested! Please, define required configs to execute.");
        exit(4);
    }

    int temp_repeats = ((options.size() > 1 && !options[1].empty())) ? std::stoi(options[1][0]) : configTree.get<int>("EXECUTION.REPEATS", d_Defaults::d_repeats);
    temp_repeats = 1; // we no longer do repeated tests.
    size_t currSceneIndex = 0;
    
    auto currScene = d_getScene(scenes_configs_paths, currSceneIndex, temp_repeats, (!options[2].empty()) ? std::stoi(options[2][currSceneIndex]) : -1, d_wireframeOverride, d_heatmapWriteOverride);
    while(true)
    {
        if (!currScene)
        {
            d_Logger::warn("Skipping scene...");
            skippedScenes++;
            if (++currSceneIndex >= scenes_configs_paths.size())
            {
                d_Logger::info("No more scenes to address. Nothing to do anymore. There were " + std::to_string(skippedScenes) + " skipped scenarios. Exiting...");
                exit(0);
            }
            currScene = d_getScene(scenes_configs_paths, currSceneIndex, temp_repeats, (!options[2].empty()) ? std::stoi(options[2][currSceneIndex]) : -1, d_wireframeOverride, d_heatmapWriteOverride);
        }
        else {
            break;
        }
    }

    d_Logger::info("Scenes are set to repeat " + std::to_string(temp_repeats)+" times.");

    std::function<std::string()> d_getCurrentFilename = [&]() { return std::filesystem::path(scenes_configs_paths[currSceneIndex]).stem().string(); };
    
    std::vector<std::string> d_outFilepaths;
    //std::string currIniNumber = inis_nums.front();
    d_Stats d_statter("out/csv/"+currScene->d_getName() + "-" + d_getCurrentFilename() + ".csv", (int)(currScene->d_GetTargetTime() * 5000), temp_repeats);
    d_outFilepaths.push_back("out/csv/" + currScene->d_getName() + "-" + d_getCurrentFilename() + ".csv");
    //inis_nums.pop();


    bool one_time = false;
    d_Logger::info("Starting scene...");
    d_Heatmap::d_Init();

    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    int d_serie = 0;
    int tt = temp_repeats;
    int d_frames = 0;
    int64_t deltaTime = 0.0f;	// time between current frame and last frame
    int64_t lastFrame = GetTicks();

    while (!glfwWindowShouldClose(d_Window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(!currScene->d_IsFinished()) {            
            currScene->d_Execute(d_Window);

            int64_t currentFrame = GetTicks();

            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            deltaTime *= 1000000;
            deltaTime /= Frequency.QuadPart; // now, in microseconds.

            d_statter.d_writeStat({d_serie, deltaTime, currScene->d_getRenderedObjects()});

            glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT); // Ensure visibility
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);  // Ensure visibility if updated by a shader

            if (currScene->d_HeatmapWrite)
            {
                d_Heatmap::d_renderHeatmap();
            }

            glfwSwapBuffers(d_Window);
            glfwPollEvents();
            d_frames++;

//            if(!one_time) {
//                d_ImgExport::d_Capture("out/captures/depth-test-enabled.png");
//                one_time = true;
//            }
        } else {
            tt--;         

            currScene->d_sampleIt = 0;

            if(tt > 0) {
                d_Logger::info("Restaring scene [" + std::to_string(temp_repeats-tt) + "/" + std::to_string(temp_repeats)+"]...");
                currScene->d_Reset();
                d_serie++;
                d_frames = 0;
            } else {
                d_serie = 0;
                tt = temp_repeats;
                d_frames = 0;

                if(++currSceneIndex >= scenes_configs_paths.size())
                    break;

                d_Logger::info("Scene finished. Switching to next scene...");
                d_statter.d_setSamples(currScene->d_getSamples());

                currScene = d_getScene(scenes_configs_paths, currSceneIndex, temp_repeats, (!options[2].empty()) ? std::stoi(options[2][currSceneIndex]) : -1, d_wireframeOverride, d_heatmapWriteOverride);
                while (true)
                {
                    if (!currScene)
                    {
                        d_Logger::warn("Skipping scene...");
                        skippedScenes++;
                        if (++currSceneIndex >= scenes_configs_paths.size())
                        {
                            d_Logger::info("No more scenes to address. Nothing more to do. There were "+std::to_string(skippedScenes)+" skipped scenarios. Exiting...");                            
                            exit(0);
                        }
                        currScene = d_getScene(scenes_configs_paths, currSceneIndex, temp_repeats, (!options[2].empty()) ? std::stoi(options[2][currSceneIndex]) : -1, d_wireframeOverride, d_heatmapWriteOverride);
                    }
                    else {
                        break;
                    }
                }

                d_statter.d_restart("out/csv/"+ currScene->d_getName() + "-" + d_getCurrentFilename() +".csv", (int)(currScene->d_GetTargetTime() * 5000), temp_repeats);
                d_outFilepaths.push_back("out/csv/" + currScene->d_getName() + "-" + d_getCurrentFilename() + ".csv");
            }
        }
    }

    d_statter.d_setSamples(currScene->d_getSamples());
    d_Logger::info("All scenes played successfully. There were " + std::to_string(skippedScenes) + " skipped scenarios. Now quiting.");

    std::string d_outFilepathsString = "";
    std::for_each(d_outFilepaths.begin(), d_outFilepaths.end(), 
        [&d_outFilepathsString](auto& it) { 
            auto neco = std::filesystem::path(it).replace_filename(std::filesystem::path(it).stem().string() + "-mean.csv");
            d_outFilepathsString += "call python d_graphs.py -f "+it+" -c processTime samples\n"; 
            d_outFilepathsString += "call python d_graphs.py -f "+ neco.string() + " -c processTime samples\n";
        });
    std::cout << d_outFilepathsString << std::endl;
    glfwDestroyWindow(d_Window);
    glfwTerminate();
    return 0;
}

bool checkForRequiredConfigDisrepancies(std::vector<std::string>& _req1, std::vector<std::string>& _req2, bool _enforceOrder) {
    if(_req1.empty() || _req2.empty())
        return true;

    if(!_enforceOrder) {
        std::sort(_req1.begin(), _req1.end());
        std::sort(_req2.begin(), _req2.end());

        for(int i = 0; i < _req1.size(); i++) {
            if (_req1[i] != _req2[i]) {
                d_Logger::err("Discrepancy in required configs detected. Please, unite required configs in mainconfig.ini and command-line options.");
                return false;
            }
        }

        return true;
    } else {
        for(const auto& it1 : _req1) {
            if (std::find(_req2.begin(), _req2.end(), it1) == _req2.end()) {
                d_Logger::err("Discrepancy in required configs detected. Please, unite required configs in mainconfig.ini and command-line options.");
                return false;
            }
        }

        return true;
    }
}

bool d_setupGLFW() {    
    boost::property_tree::ptree configTree;
    boost::property_tree::ini_parser::read_ini("mainconfig.ini", configTree);

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, configTree.get<int>("GLFW.MAJOR_VERSION", d_Defaults::d_majorVersion));
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, configTree.get<int>("GLFW.MINOR_VERSION", d_Defaults::d_minorVersion));
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    d_Window = glfwCreateWindow(
        configTree.get<int>("WINDOW.WIDTH", d_Defaults::d_windowWidth),
        configTree.get<int>("WINDOW.HEIGHT", d_Defaults::d_windowHeight),
        configTree.get<std::string>("WINDOW.NAME", d_Defaults::d_windowName).c_str(), 
        NULL, 
        NULL);
    if (d_Window == NULL)
    {
        d_Logger::err("Failed to create GLFW window");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(d_Window);
    if(glewInit() != GLEW_OK) {
        d_Logger::err("Failed to init a GLEW.");
        return false;
    };

    glViewport(
        0, 
        0, 
        configTree.get<int>("VIEWPORT.WIDTH", d_Defaults::d_viewportWidth), 
        configTree.get<int>("VIEWPORT.HEIGHT", d_Defaults::d_viewportHeight)
    );

    glEnable(GL_DEPTH_TEST); // taky v shaderu a pozor na sortovani!
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::string clearColorString = configTree.get<std::string>("VIEWPORT.CLEARCOLOR", d_Defaults::d_clearColor);
    std::vector<std::string> temp;
    boost::split(temp, clearColorString, boost::is_any_of(","));
    std::vector<float> d_colors;
    std::for_each(temp.begin(), temp.end(), [&](auto a) { boost::trim(a); d_colors.push_back(std::stof(a)); });

    glClearColor(d_colors[0], d_colors[1], d_colors[2], d_colors[3]);

    glfwSwapInterval(0);
    
    return true;
}
std::vector<std::string> d_GetScenePaths(boost::property_tree::ptree& ptree)
{
    auto scenesString = ptree.get<std::string>("EXECUTION.SCENES", "");
    if (!scenesString.empty())
    {
        std::vector<std::string> temp;
        boost::split(temp, scenesString, boost::is_any_of(","));
        std::vector<std::string> scenes;
        std::for_each(temp.begin(), temp.end(), [&](auto a) { boost::trim(a); scenes.push_back("scene_configs/"+a+".ini"); });
        return scenes;
    } else
    {
        d_Logger::warn("mainconfig.ini: EXECUTION.SCENES missing. You can safely ignore this message if scenes are defined via command-line. ");
        return {};
    }
}
std::unique_ptr<d_SceneBase> d_getScene(std::vector < std::string> _sceneConfigPaths, unsigned int _dIndex, int _repeats, unsigned int _dCutoutSize, bool _dWireframeOverride, bool _dHeatmapOverride)
{
    if (!std::filesystem::exists(_sceneConfigPaths[_dIndex]))
    {
        d_Logger::err("Config filepath '" + _sceneConfigPaths[_dIndex] + "' does not exist.");
        return std::unique_ptr<d_SceneBase>();
    }

    boost::property_tree::ptree d_TempConfigTree;
    boost::property_tree::ini_parser::read_ini(_sceneConfigPaths[_dIndex], d_TempConfigTree);
    auto d_methodId = d_TempConfigTree.get<int>("GENERAL.method", -1);

    std::unordered_map<int, std::function<std::unique_ptr<d_SceneBase>()>> d_SceneFactory = {
        {0, [&] { return std::make_unique<d_SceneDefault>(
                            _sceneConfigPaths[_dIndex],
                            _repeats,
                            _dCutoutSize,
                            _dWireframeOverride,
                            _dHeatmapOverride); }},
        {1, [&] { return std::make_unique<d_SceneWBOIT>(
                            _sceneConfigPaths[_dIndex],
                            _repeats,
                            _dCutoutSize,
                            _dWireframeOverride,
                            _dHeatmapOverride); }}
        //{2, [&] { return std::make_unique<d_SceneDPOIT>(
        //                    _sceneConfigPaths[_dIndex],
        //                    _dCutoutSize,
        //                    _dWireframeOverride,
        //                    _dHeatmapOverride); }}
    };

    if (d_SceneFactory.find(d_methodId) == d_SceneFactory.end())
    {
        d_Logger::err("Invalid `GENERAL.method` value provided ('" + _sceneConfigPaths[_dIndex] + "'). Consult README.md for options.");
        return std::unique_ptr<d_SceneBase>();
        //exit(6);
    }
     
    return d_SceneFactory.at(d_methodId)();
}
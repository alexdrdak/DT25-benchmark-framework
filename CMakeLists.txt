cmake_minimum_required(VERSION 3.22)
project(DT25_axdrdak)
        
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_TOOLCHAIN_FILE "C:/Program Files (x86)/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Freetype REQUIRED)
find_package(assimp CONFIG REQUIRED)

add_executable(DT25_axdrdak main.cpp
        d_glob.h
        engine_objs/d_Mesh.cpp
        "d_Scenes/d_SceneBase.cpp"
        "engine_objs/d_Object.cpp"
        engine_objs/d_Texture.cpp
        OGL_objs/d_ShaderProgram.cpp
        OGL_objs/VertexBufferLayout.cpp
        engine_objs/d_Camera.cpp
        engine_objs/d_ParticleGenerator.cpp
        engine_objs/d_NURBS.cpp
        engine_objs/d_Stats.cpp
        engine_objs/d_ImgExport.cpp
         "d_Scenes/d_SceneOverride.cpp" 
         "d_Scenes/d_SceneDefault.cpp" 
         "d_Scenes/d_SceneWBOIT.cpp" 
         "d_Scenes/d_SceneDPOIT.cpp" 
         "engine_objs/d_Logger.cpp" 
         "engine_objs/d_Heatmap.h" 
         "engine_objs/d_Heatmap.cpp" 
         "engine_objs/d_ObjectBase.cpp"
         "engine_objs/d_global_vals.cpp")

target_include_directories(DT25_axdrdak PRIVATE ${FREETYPE_INCLUDE_DIRS} vendor ${Assimp_SOURCE_DIR}/include "${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb_image" ${Assimp_SOURCE_DIR}/code)
target_link_directories(DT25_axdrdak PRIVATE ${Assimp_BINARY_DIR} ${Assimp_BINARY_DIR}/lib)
target_link_libraries(DT25_axdrdak PRIVATE glfw OpenGL::GL GLEW::GLEW ${FREETYPE_LIBRARIES} assimp::assimp)

foreach(dir IN ITEMS resources scene_configs shaders)
    add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/${dir}
                ${CMAKE_CURRENT_BINARY_DIR}/${dir})
endforeach()

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/mainconfig.ini
                ${CMAKE_CURRENT_BINARY_DIR}/mainconfig.ini)
# PARTICLE BENCHMARKING FRAMEWORK

## Building
Setup your CMake tool and execute the `CMakeLists.txt`. To install all dependencies needed, run the `d_win_install.bat`.

## Configurations files
Configuration files describe scenes which act as independent tests. Config files define how each test scene should be carried out. There are two types of config files:
- main config: `mainconfig.ini`
- scene configs: `scene_configs/*.ini`


#### General remarks
1. When defining arrays, space characters after commas are prohibited, same as enclosing them in quotation marks. Take example from the config files provided.
2. When describing any scene coordinates, please be aware of the right-hand-oriented coordinate system as by OpenGL convention.

(_Please note that the order of the configuration executions is sorted alphabetically based on the config filename. If 
you wish to force a certain order, append a -SF switch._)
If duplicates are found, they are truncated to 1.

# Main Configuration File
The general configuration (such as window size, etc.) could 
be defined in `mainconfig.ini`. Shipped `mainconfig.ini` showcases all possible and required options. The `mainconfig.ini` 
also defines names of scene configuration files which will be executed at runtime - these are looked-up in the `scene-configs/`
directory and are defined by their name of the file (the extension `.ini` of the file is appended automatically). Following are the defaults of exposed variables:

| name | type | required | default | note |
|----|---|---|---|---|
| GLFW.MAJOR_VERSION | int | | 4 | - |
| GLFW.MINOR_VERSION | int | | 6 | - |
| WINDOW.WIDTH | int | | 800 | - |
| WINDOW.HEIGHT | int | | 600 | - |
| WINDOW.NAME | string | | `DP25|AXDRDAK@FAV-ZCU` | - |
| VIEWPORT.WIDTH | int | | 800 | - |
| VIEWPORT.HEIGHT | int | | 600 | - |
| VIEWPORT.CLEARCOLOR | array<int, 4> | | `0.0,0.0,0.0,1.0` | watch-out: no spaces after commas |
| EXECUTION.SCENES | array<int> | | - | Possible to leave out. In such case, scenes have to be defined using command-line option -S / -SF. If both `mainconfig.ini` and parameter `*.ini` are issued, the command line parameters are preferred. |

All mentioned defaults are set in the `engine_objs/d_global_vals.h` and are possible to be redefined.

# Individual Scene Configuration Files
Defined by `*.ini` files. The out-of-the-box `*.ini` files are in a following format:
```
(UG|U|S|G)(\d*)K-(\d+)\.csv
```
Where the first member informs about the particle distribution (**U**niform**G**rid, **U**niform, **S**pherical, **G**aussian), second parameter informs about the amount of particles (in thousands) and third member defines the identificator of a test.

## Scene
Scene is defined by its `.ini` config file placed `scene_configs/`. Each scene consists of scene-objects and 1 camera. 

Scene has its objects defined by `OBJECT.transparencyModelPaths` and `OBJECT.opaqueModelPaths`, and if instancing is desired, `MISC.instacing` has to be set. Check chapter for ParticleGenerator for more information on instancing of objects. this option corresponds with the `*ModelAmounts` option.

| name                  | type                | required | default     | note                        |
|-----------------------|---------------------|----------|-------------|-----------------------------|
| GENERAL.name          | string              |          | ''          | internal use                |
| GENERAL.description   | string              |          | ''          | internal use                |
| GENERAL.method		| int              |          | 0          | defines the approach for the scene.               |
| GENERAL.culling		| int              |          | 0          | available only for `GENERAL.method=3`. For all other configs, this setting is to be ignored. |
| OBJECT.transparencyModelPaths     | array\<string>      | ✔        |             | -                           | 
| OBJECT.transparencyModelAmounts | array\<int>         |          | [1,1,1...n] | check **ParticleGenerator**. Amounts correspond with respective `transparencyModelPaths` items. If defined, size of this field must equal the `transparencyModelPaths`. |
| OBJECT.opaqueModelPaths     | array\<string>      | ✔        |             | -                           | 
| OBJECT.opaqueModelAmounts | array\<int>         |          | [1,1,1...n] |Amounts correspond with respective `opaqueModelPaths` items. If defined, size of this field must equal the `opaqueModelPaths`. |

| id | name | description |
|---|---|---|
| 0 | Naive | Default, naive approach of 1 draw call per object. For each draw call, an uniform with index for accessing SSBO is delivered. |
| 1 | WBOIT | Rendering loop implements Weighted-Blended OIT. |


## ParticleGenerator
In scene config file defined under the `PARTICLEGEN` header. 

If an object is set to have more than 1 instance, **ParticleGenerator** has to be configured. As mentioned, the instancing of objects is enabled by `MISC.instanced`. **ParticleGenerator** comes with 5 different distribution modes. Following is the table of options for different **ParticleGenerator** modes.

#### ParticleGenerator: Uniform distribution (mode `1`)
Particles are generated uniformly (C++ built-in `<std::uniform_real_distribution>` uniform) in area user-defined.  

| name   | type                | required | default | note |
|--------|---------------------|---------|---------|------|
| x      | interval\<int, int> | ✔       | -       | -    | 
| y      | interval\<int, int> |         | [0, 1]  | -    |
| z      | interval\<int, int> | ✔       | -       | -    |
| seed   | integer             |         | 42      | -    |

#### ParticleGenerator: Linear distribution (mode `2`)
Particles generated along defined segment. The density of the set depends on the required number of particles.

| name   | type                     | required | default | note |
|--------|--------------------------|----------|---------|------|
| start  | interval\<int, int, int> | ✔        | -       | -    | 
| end    | interval\<int, int, int> | ✔        | -       | -    |

#todo: prepsat inty na floaty.

#### ParticleGenerator: Uniform Grid Distribution (mode `3`)
Particles are generated in an even grid, which is defined by its top-left corner and two axes.


| name   | type                     | required | default | note |
|--------|--------------------------|----------|---------|------|
| ltop  | vec3\<int, int, int> | ✔        | -       | -    | 
| axis1 | vec3\<int, int, int> | ✔        | -       | -    |
| axis2 | vec3\<int, int, int> | ✔        | -       | -    |


#### ParticleGenerator: Gaussian Distribution (mode `4`)
Particles are generated in a Normal distribution, centered at the `center` point and tapering out based on the `variance` parameter

| name   | type                     | required | default | note |
|--------|--------------------------|----------|---------|------|
| center| vec3\<int, int, int>		| ✔        | -       | -    | 
| radius | float	|         | 1.0f       | eventhough the of this parameter is `radius`, it is meant as a `variance` - kept due technical reasons. Will raise an error if defined as 0.0f.   |

#### ParticleGenerator: Sphere Distribution (mode `5`)
Particles are generated in a Normal distribution, centered at the `center` point and tapering out based on the `variance` parameter

| name   | type                     | required | default | note |
|--------|--------------------------|----------|---------|------|
| center| vec3\<int, int, int>		| ✔        | -       | -    | 
| radius | float	|         | 1.0f       | Will raise an error if defined as 0.0f.   |


## Camera
In scene config file defined under the `CAMERA` header. 

Scene camera is defined in each scene configuration file and has currently two types. 

Camera configuration has a few common options, described as follows:

| name      | type   | required | default | note                                    |
|-----------|--------|----------|---------|-----------------------------------------|
| time      | int    | ✔        | -       | desired time camera reaches destination | 
| fov       | double |          | 45.0    | -                                       |
| nearplane | float  |          | 0.01    | -                                       |
| farplane  |        |          | 1000.0  | -                                       |

#### Camera: Linear (mode `1`)
Camera in mode 1 travels linearily from point `start_knot` to the point `end_knot` in given time frame (set by `CAMERA.time`). Camera will be pointing towards the final `end_knot`. Please note that definining `start_knot` and `end_knot` equal is an invalid configuration.

| name       | type                     | required | default | note |
|------------|--------------------------|--------|--------|------|
| start_knot | interval\<int, int, int> | ✔      | [0, 1] | -    | 
| end_knot   | interval\<int, int, int> | ✔      | [0, 1] | -    |

#### Camera: NURBS (mode `2`)
NURBS curve is defined as a `start_knot` and `end_knot`, with possibility to add desired `mid_point`. In current version, the weights of NURBS curve are computed automatically. Camera on this path will be looking at the origin of the coordinate system.

| name       | type                     | required | default | note |
|------------|--------------------------|----------|---------|------|
| start_knot | interval\<int, int, int> | ✔        | [0, 1]  | -    | 
| mid_knot   | interval\<int, int, int> |          | -       | -    | 
| end_knot   | interval\<int, int, int> | ✔        | [0, 1]  | -    |


## Application switches

| Switch | Description                                                                                                                                               |
| -- |-----------------------------------------------------------------------------------------------------------------------------------------------------------|
| -S | Specify which config files should be executed. (_Please note that exeuction order is alphabetically sorted. -SF switch enforces the user defined order._) |
| -SF | Forces the order of the tests as the user defined them. Applies to both command-line option definition nor `mainconfig.ini` definition.                   ||
| -C | Enables arbitrary # of cutout vertices. At the same time, defines # of these vertices. Cutouts are to be stored in the same directory as the original `.obj` file. The # of cutout vertices in a `*.obj` file is expected to be denoted via suffix as `-C%D`, where `%D`is the number itself. Default is 4 (e.g. `cmoudik-C4.obj`), and if no file with suitable suffix is found, the one without any suffix is taken (e.g. `cmoudik.obj`). This option overrides a setting defined in `*.ini` file.
| -W | Enforces wireframe mode |
| -H | Enforces heatmap write mode |
## Return Codes List

| Return Code | Description                                                                                                                                                                                   |
|-------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 0           | All is well.                                                                                                                                                                                  |
| 1           | Unknown parameter option.                                                                                                                                                                     |
| 2 | Bad order of the switches, invalid option detected.                                                                                                                                           |                                                                                                                                              |
| 4 | No required config files were provided. Application needs at least one config file to execute. Define it either using command-line switch or check-out how `mainconfig.ini` file defines these. |
| 5 | Error compiling shaders.                                                                                                               |
| 6 | Invalid or missing config parameter value. |
| 7 | Required config filepath does not exist. |
| 8 | Invalid configuration of vital objects (mostly camera). | 

## Output
Benchmark results are written to the `out/csv` folder. If this folder does not exist, create it—otherwise, no output will be saved. There are two types of output files: 
1. "Log files" which document the course of the scene test.
2. "Mean files" which provides a moving averate values of the whole execution.

Logs are provided in `*.csv` format and hold present data about milliseconds per frame, samples per frame and objects that were passed through the pipeline.

---
Alexander Drdak, axdrdak@students.zcu.cz, 2024/2025

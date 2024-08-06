README
========================

# Personal OpenGL Graphics Engine

An OpenGL graphics engine largely based on Joey de Vries' [Learn OpenGL tutorial](https://learnopengl.com/) and Jasper Flick's [Catlike Coding tutorials](https://catlikecoding.com/). 

![Demo](demo.gif)

## Features
- Logging utility using the [spdlog library](https://github.com/gabime/spdlog)
- Material/shader pipeline
    - Shader object has custom pre-processor to handle defines and includes
    - Easy to manage state of materials, textures, and shader
    - Includes default materials
- Scene management
    - Scene objects are easy to add using defined meshes (cube, sphere, etc) and materials
    - Object hierarchy structure allows transforms to be relative to one-another
- Framebuffers
    - Used for capturing shadow maps, enviornmental cubemaps, etc
- PBR render pipeline
    - Calculates and stores diffuse integral, prefiltered enviornments, and BRDF into samplers
    - Supports multiple directional/point light with shadows
    - Enviornmental lighting
- Post-procesing
    - Gamma Correction
- Editor using [ImGui library](https://github.com/ocornut/imgui)
    - Edit materials and object transform during runtime

## Limitations
- Has only been tested on my own system running on Windows with the MingW64 compiler built using cmake
# Flashlight Engine
Flashlight engine is a game engine made with C.

It will be a 3D engine, with as little dependencies as possible, made from scratch and using Vulkan for rendering.

### Supported platforms
Only Windows and Linux are supported for now. Native (non-GLFW) Mac support will probably be added in the future. Android and iOS support
will possibly be added to.

## Compiling & Running

### Dependencies:
- The [Vulkan SDK](https://vulkan.lunarg.com/)  
- [XMake](https://xmake.io/) (build system)

### Building:
Flashlight Engine uses XMake as the build system. It allows easier dependency and toolchain management, and way more.

### Running:
The testbed executable should be in the `bin/plat_arch_mode` folder after you built the project.  

## Roadmap:
- Fully 3D engine with various lighting models and material types available (basic 3D lighting, PBR, etc.)  
- Cross-platform support (Windows and Linux for now, Mac in the future)
- Multiple rendering backends
    - Primarly Vulkan
    - Eventually OpenGL and DirectX, possibly Metal
- Suite of editor tools
- Asset/Game code hot-reloading
- Physics
- 2D/3D sound
- Full-fledged UI system
- Event system
- Standard keyboard/mouse input along with eventually gamepad (controller) input
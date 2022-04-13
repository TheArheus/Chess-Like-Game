@echo off
if not defined DevEnvDir (
    call vcvarsall x64
)
REM call vcvarsall x64

set LIB_VCPKG="F:\Env\vcpkg\installed\x64-windows\lib"
set INC_VCPKG="F:\Env\vcpkg\installed\x64-windows\include"

set INC_VULKAN="C:\VulkanSDK\1.2.189.2\Include"
set LIB_VULKAN="C:\VulkanSDK\1.2.189.2\Lib"

set CommonCompileFlags=-MT -nologo -fp:fast -EHa -Od -WX- -W4 -Oi -GR- -Gm- -GS -DCHESS_DEBUG=1 -wd4100 -wd4201 -wd4505 -FC -Z7 /I%INC_VCPKG%
set CommonLinkFlags=-opt:ref -incremental:no /NODEFAULTLIB:msvcrt /SUBSYSTEM:CONSOLE /LIBPATH:%LIB_VCPKG%  

call glslangValidator --target-env vulkan1.2 ..\shaders\mesh.vert.glsl -V -o ..\shaders\mesh.vert.spv
call glslangValidator --target-env vulkan1.2 ..\shaders\mesh.frag.glsl -V -o ..\shaders\mesh.frag.spv

if not exist ..\build mkdir ..\build
pushd ..\build

REM cl %CommonCompileFlags% -O2 SDL2.lib ..\code\display.cpp -LD /link -opt:ref -incremental:no /LIBPATH:%LIB_VCPKG%
cl %CommonCompileFlags% /I%INC_VULKAN% ..\code\vulkan_renderer.cpp kernel32.lib SDL2main.lib SDL2.lib vulkan-1.lib VkLayer_utils.lib -LD /link %CommonLinkFlags% /LIBPATH:%LIB_VULKAN%
cl %CommonCompileFlags% -I %INC_VULKAN% ..\code\main.cpp SDL2main.lib SDL2.lib vulkan-1.lib vulkan_renderer.obj display.obj /link %CommonLinkFlags% /LIBPATH:%LIB_VULKAN%
popd

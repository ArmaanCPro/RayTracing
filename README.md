# RayTracing
This is a software ray tracer.

The dependencies are glm, imgui, and sdl3, and a xoroshiro random library.
All you need to have installed is __CMake__, __vcpkg__ (with VCPKG_ROOT defined in your environment),
and, of course, a c++ compiler (g++, clang++, cl.exe) and build system (Ninja is a good choice for all platforms).

Although this project uses SDL GPU for rendering, the actual ray tracing is done on the CPU and rendered to a texture, essentially a render target.
If you encounter any bugs, please open an issue. In particular, if it does not build/run on your machine, let me know!

Running in release mode is _highly_ recommended! `cmake --preset release` will do the trick.

# BUILDING

This project uses CMake with certain presets if you so desire.

On Linux debug builds, you should prefer the `asan` preset, as it enables ASan and UBSan (GCC and Clang both support them).

On Windows, you can select your preferred generator/compiler, i.e. `-G "Visual Studio 18 2026 -DCMAKE_CXX_COMPILER=cl.exe"`.
_(To be well honest, on Windows you should use Ninja + cl.exe, both of which will be autodetected by any preset)_

To build, run:
- `cmake --preset <preset, e.g. release>`
- `cmake --build ./build/<preset>`

To run, you can run the executable file located in the build directory.

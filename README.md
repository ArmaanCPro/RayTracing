# RayTracing
This is a software ray tracer.
The dependencies are glm, imgui, and sdl3.

Although this project uses SDL GPU for rendering, the actual ray tracing is done on the CPU and rendered to the texture.
If you encounter any bugs, please open an issue. In particular, if it does not build/run on your machine, let me know!

Running in release mode is _highly_ recommended! `cmake --preset release` will do the trick.

# BUILDING

This project uses CMake with certain presets if you so desire.
On Linux, you should prefer the `asan` preset, as it enables ASan and UBSan (GCC and Clang both support them).
On Windows, you can select your preferred generator/compiler, i.e. `-G "Visual Studio 18 2026 -DCMAKE_CXX_COMPILER=cl.exe"`.

To build, run:
- `cmake --preset <preset, e.g. debug>`
- `cmake --build ./build/<preset> --target RT`

To run, you can run the executable located in the build director.

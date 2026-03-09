# MinecraftPE

This is leaked source code of Minecraft PE 0.6.1 with my own impovements :sunglasses:

First of all I made it build with CMake (w/o VS2012). Also I fixed some compile errors. And finally I ported it to GLFW to make it run on several platforms and remove binary dependencies.

# TODO
- [x] Add platform GLFW
- [x] Compile for Linux
- [ ] Compile for android aarch64
- [ ] Rewrite platform logic
- [x] Fix sound
- [ ] Do a server connection gui

# Build
## CMake
```
mkdir build && cd build
cmake .. -B .
make -j4
```

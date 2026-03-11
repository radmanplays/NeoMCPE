# MinecraftPE
> [!Important]
> We have a discord server, where you can report bugs or send feedback https://discord.gg/c58YesBxve

Source code for **Minecraft Pocket Edition 0.6.1 alpha** with various fixes and improvements.

This project aims to preserve and improve this early version of Minecraft PE.

# TODO / Roadmap
- [x] Add platform GLFW
- [x] Compile for Linux
- [x] Compile for android aarch64
    - [x] Touch control improvements
    - [ ] Screen fixes
- [ ] Rewrite platform logic
- [x] Fix sound
- [ ] Do a server connection GUI
- [ ] Controller support
- [ ] Minecraft server hosting
- [x] Screen fixess
- [x] Fix fog
- [x] Add sprinting
- [x] Chat (semi working) and commands
- [x] Implementing options
- [x] Better F3

# Build

## CMake
```
mkdir build && cd build
cmake .. -B .
make -j4
```
or
```
mkdir build && cd build
cmake --build . --config Release -j 10
```

## Visual Studio

1. Open the repository folder in **Visual Studio**.  
2. Visual Studio will automatically detect the `CMakeLists.txt` file and generate the project configuration.  
3. Set **MinecraftPE.exe** as the **target**.  
4. Press **Run** (or F5) to build and launch the game.

## Android
Download [r14b Android NDK](http://dl.google.com/android/repository/android-ndk-r14b-windows-x86_64.zip) and run `build.ps1`:
```
# Full build (NDK + Java + APK + install)
.\build.ps1

# Skip NDK recompile (Java/assets changed only)
.\build.ps1 -NoJava

# Skip Java recompile (C++ changed only)  
.\build.ps1 -NoCpp

# Only repackage + install (no recompile at all)
.\build.ps1 -NoBuild
```
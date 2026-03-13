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

1. Download **Android NDK r14b**:  
   http://dl.google.com/android/repository/android-ndk-r14b-windows-x86_64.zip

2. Extract it to the root of your `C:` drive so the path becomes:

   ```
   C:\android-ndk-r14b
   ```

3. Run the build script:

```powershell
# Full build (NDK + Java + APK + install)
.\build.ps1

# Skip C++ compilation (Java/assets changed only)
.\build.ps1 -NoCpp

# Skip Java compilation (C++ changed only)
.\build.ps1 -NoJava

# Only repackage + install (no compilation)
.\build.ps1 -NoBuild
```
cmake_minimum_required(VERSION 3.21)
project(MinecraftPE)

include(cmake/CPM.cmake)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_POLICY_VERSION_MINIMUM 3.10)
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-Wno-c++11-narrowing -Wno-narrowing -Wno-invalid-source-encoding -Wno-reserved-user-defined-literal")
endif()

find_package(Threads REQUIRED)

find_package(OpenSSL)
if (OpenSSL_FOUND)
    message(STATUS "found openssl ${OPENSSL_VERSION}")
endif()

CPMAddPackage("gh:madler/zlib@1.3.2")
CPMAddPackage(
    NAME "libpng"
    GIT_REPOSITORY "https://github.com/pnggroup/libpng.git"
    GIT_TAG "v1.6.55"
    OPTIONS
        "ZLIB_ROOT ${zlib_SOURCE_DIR}"
        "ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR}"
        "PNG_TOOLS OFF"
        "PNG_TESTS OFF"
        "BUILD_SHARED_LIBS ON"
)

CPMAddPackage(
    NAME "openal"
    GIT_REPOSITORY "https://github.com/kcat/openal-soft.git"
    GIT_TAG "1.25.1"
    OPTIONS
        "ALSOFT_EXAMPLES OFF"
        "ALSOFT_TESTS OFF"
        "ALSOFT_UTILS OFF"
        "BUILD_SHARED_LIBS ON"
)

CPMAddPackage(
    NAME "glfw"
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.4"
    OPTIONS
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_DOCS OFF"
        "BUILD_SHARED_LIBS ON"
)

# TODO: Clear this paths with *
file(GLOB SERVER_SOURCES
    "project/lib_projects/raknet/jni/RaknetSources/*.cpp"
    "src/NinecraftApp.cpp"
    "src/Performance.cpp"
    "src/SharedConstants.cpp"

    "src/client/IConfigListener.cpp"
    "src/client/Minecraft.cpp"
    "src/client/OptionStrings.cpp"
    "src/client/Option.cpp"
    "src/client/Options.cpp"
    "src/client/OptionsFile.cpp"
    "src/client/ServerProfiler.cpp"

    "src/client/gamemode/CreativeMode.cpp"
    "src/client/gamemode/GameMode.cpp"
    "src/client/gamemode/SurvivalMode.cpp"

    "src/client/player/LocalPlayer.cpp"
    "src/client/player/RemotePlayer.cpp"
    "src/client/player/input/KeyboardInput.cpp"

    "src/locale/I18n.cpp"

    "src/main.cpp"
    "src/main_dedicated.cpp"

    "src/nbt/Tag.cpp"

    "src/network/ClientSideNetworkHandler.cpp"
    "src/network/NetEventCallback.cpp"
    "src/network/Packet.cpp"
    "src/network/RakNetInstance.cpp"
    "src/network/ServerSideNetworkHandler.cpp"
    "src/network/command/CommandServer.cpp"

    "src/platform/CThread.cpp"
    "src/platform/HttpClient.cpp"
    "src/platform/PngLoader.cpp"
    "src/platform/time.cpp"

    "src/platform/input/Controller.cpp"
    "src/platform/input/Keyboard.cpp"
    "src/platform/input/Mouse.cpp"
    "src/platform/input/Multitouch.cpp"

    "src/server/ArgumentsSettings.cpp"
    "src/server/ServerLevel.cpp"
    "src/server/ServerPlayer.cpp"

    "src/util/DataIO.cpp"
    "src/util/Mth.cpp"
    "src/util/PerfTimer.cpp"
    "src/util/StringUtils.cpp"

    "src/world/Direction.cpp"

    "src/world/entity/*.cpp"

    "src/world/entity/ai/control/MoveControl.cpp"

    "src/world/entity/animal/*.cpp"

    "src/world/entity/item/*.cpp"

    "src/world/entity/monster/*.cpp"

    "src/world/entity/player/Inventory.cpp"
    "src/world/entity/player/Player.cpp"

    "src/world/entity/projectile/Arrow.cpp"
    "src/world/entity/projectile/Throwable.cpp"

    "src/world/food/SimpleFoodData.cpp"

    "src/world/inventory/*.cpp"
    "src/world/item/*.cpp"
    "src/world/item/crafting/*.cpp"
    "src/world/level/*.cpp"

    "src/world/level/biome/Biome.cpp"
    "src/world/level/biome/BiomeSource.cpp"

    "src/world/level/chunk/LevelChunk.cpp"

    "src/world/level/dimension/Dimension.cpp"

    "src/world/level/levelgen/*.cpp"
    "src/world/level/levelgen/feature/Feature.cpp"
    "src/world/level/levelgen/synth/*.cpp"

    "src/world/level/material/Material.cpp"

    "src/world/level/pathfinder/Path.cpp"

    "src/world/level/storage/*.cpp"
    "src/world/level/tile/*.cpp"
    "src/world/level/tile/entity/*.cpp"

    "src/world/phys/HitResult.cpp"
) 

file(GLOB CLIENT_SOURCES 
    "src/client/*.cpp"

    "src/client/gamemode/*.cpp"
    "src/client/gui/*.cpp"
    "src/client/gui/components/*.cpp"
    "src/client/gui/screens/*.cpp"
    "src/client/gui/screens/crafting/*.cpp"
    "src/client/gui/screens/touch/*.cpp"

    "src/client/model/*.cpp"
    "src/client/model/geom/*.cpp"

    "src/client/particle/*.cpp"

    "src/client/player/*.cpp"
    "src/client/player/input/*.cpp"
    "src/client/player/input/touchscreen/*.cpp"

    "src/client/renderer/*.cpp"
    "src/client/renderer/culling/*.cpp"
    "src/client/renderer/entity/*.cpp"
    "src/client/renderer/ptexture/*.cpp"
    "src/client/renderer/tileentity/*.cpp"

    "src/client/sound/*.cpp"
    
    "src/locale/*.cpp"
    
    "src/nbt/*.cpp"

    "src/network/*.cpp"
    "src/network/command/*.cpp"

    "src/platform/*.cpp"
    "src/platform/audio/SoundSystemAL.cpp"
    "src/platform/input/*.cpp"

    "src/raknet/**.cpp"
    
    "src/server/**.cpp"
    
    "src/util/**.cpp"

    "src/world/*.cpp"
    "src/world/phys/*.cpp"
    "src/world/entity/*.cpp"
    "src/world/entity/ai/control/*.cpp"
    "src/world/entity/animal/*.cpp"
    "src/world/entity/item/*.cpp"
    "src/world/entity/monster/*.cpp"
    "src/world/entity/player/*.cpp"
    "src/world/entity/projectile/*.cpp"

    "src/world/food/*.cpp"
    "src/world/inventory/*.cpp"

    "src/world/item/*.cpp"
    "src/world/item/crafting/*.cpp"

    "src/world/level/*.cpp"
    "src/world/level/biome/*.cpp"
    "src/world/level/chunk/*.cpp"
    "src/world/level/dimension/*.cpp"
    "src/world/level/levelgen/*.cpp"
    "src/world/level/levelgen/feature/*.cpp"
    "src/world/level/levelgen/synth/*.cpp"
    "src/world/level/material/*.cpp"
    "src/world/level/pathfinder/*.cpp"
    "src/world/level/storage/*.cpp"
    "src/world/level/tile/*.cpp"
    "src/world/level/tile/entity/*.cpp"

    "src/SharedConstants.cpp"
    "src/main.cpp"
    "src/NinecraftApp.cpp"
)

if(NOT DEFINED PLATFORM)
    set(PLATFORM "PLATFORM_GLFW")
endif()


if(PLATFORM STREQUAL "PLATFORM_WIN32")
    list(APPEND CLIENT_SOURCES "src/AppPlatform_win32.cpp")
endif()

if(PLATFORM STREQUAL "PLATFORM_GLFW")
    list(APPEND CLIENT_SOURCES "src/AppPlatform_glfw.cpp")
endif()

# Server
add_executable("${PROJECT_NAME}-server" ${SERVER_SOURCES})

target_compile_definitions("${PROJECT_NAME}-server" PUBLIC "STANDALONE_SERVER" "SERVER_PROFILER")

target_include_directories("${PROJECT_NAME}-server" PUBLIC 
   "${CMAKE_SOURCE_DIR}/src/"
   "project/lib_projects/raknet/jni/RaknetSources"
)

target_link_libraries("${PROJECT_NAME}-server" ${CMAKE_THREAD_LIBS_INIT} png_shared)


add_executable(${PROJECT_NAME} 
    ${CLIENT_SOURCES}
    "glad/src/glad.c"
)

if(WIN32)
    set(EXTRA_LIBS "ws2_32")
    target_compile_definitions(${PROJECT_NAME} PUBLIC "_CRT_SECURE_NO_WARNINGS")
endif()

if(PLATFORM STREQUAL "PLATFORM_WIN32" OR PLATFORM STREQUAL "PLATFORM_GLFW")
    target_compile_definitions(${PROJECT_NAME} PUBLIC "PLATFORM_DESKTOP")
endif()



target_include_directories(${PROJECT_NAME} PUBLIC 
    "${CMAKE_SOURCE_DIR}/glad/include/"
    "${CMAKE_SOURCE_DIR}/src"
    "${openal_SOURCE_DIR}/include"
    "${glfw_SOURCE_DIR}/include"
    "lib/include"
)

# Client
target_compile_definitions(${PROJECT_NAME} PUBLIC "OPENGL_ES" "NO_EGL" ${PLATFORM})
target_link_libraries(${PROJECT_NAME} zlib png_shared alsoft.common OpenAL::OpenAL glfw ${EXTRA_LIBS})

if (OpenSSL_FOUND)
    target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(${PROJECT_NAME} PUBLIC HTTPCLIENT_USE_OPENSSL)
endif()

if (NOT UNIX)
add_custom_command(
    TARGET ${PROJECT_NAME} 
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}> 
        COMMAND_EXPAND_LISTS
)
endif()

add_custom_command(
    TARGET ${PROJECT_NAME} 
    POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/data" $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)
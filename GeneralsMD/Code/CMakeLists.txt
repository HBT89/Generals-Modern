cmake_minimum_required(VERSION 3.20)
project(GeneralsZH LANGUAGES C CXX)

# Force Win32 (x86) - the game uses inline assembly and 32-bit APIs
if(NOT CMAKE_GENERATOR_PLATFORM)
    set(CMAKE_GENERATOR_PLATFORM Win32)
endif()

# =============================================================================
# Path variables
# =============================================================================
set(LIBS_VEGAS ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/WWVegas)
set(LIBS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Include)
set(LIBS_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source)
set(DX8_SDK_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/DX90SDK/Include)
set(ENGINE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/Include)
set(ENGINE_PRECOMPILED ${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/Include/Precompiled)
set(DEVICE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/GameEngineDevice/Source)
set(DEVICE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/GameEngineDevice/Include)

# =============================================================================
# Global compile options for MSVC
# =============================================================================
if(MSVC)
    add_compile_options(
        /W2                    # Warning level 2
        /Zc:forScope-          # MSVC6 for-loop scoping (deprecated but needed)
        /FS                    # Force synchronous PDB writes
        /permissive            # Permissive mode for legacy code
        /D_CRT_SECURE_NO_WARNINGS
        /D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
        /DWIN32
        /D_WINDOWS
        /DNOMINMAX
        /DDIRECTINPUT_VERSION=0x0800
    )
    # Disable specific warnings that flood the build
    add_compile_options(
        /wd4244 /wd4267 /wd4996 /wd4005 /wd4273
        /wd4018 /wd4305 /wd4309 /wd4800 /wd4838
        /wd4102 /wd4473 /wd5043 /wd4146 /wd4804
    )
    # Linker flags for inline assembly compatibility
    add_link_options(/SAFESEH:NO)
endif()

# =============================================================================
# Common include directories
# =============================================================================
set(BGFX_INCLUDE ${LIBS_SOURCE}/bgfx/include)
set(BX_INCLUDE ${LIBS_SOURCE}/bx/include)
set(BIMG_INCLUDE ${LIBS_SOURCE}/bimg/include)

set(COMMON_INCLUDES
    ${DX8_SDK_INCLUDE}
    ${LIBS_VEGAS}
    ${LIBS_VEGAS}/WWLib
    ${LIBS_VEGAS}/WWMath
    ${LIBS_VEGAS}/WWDebug
    ${LIBS_VEGAS}/WWSaveLoad
    ${LIBS_VEGAS}/WW3D2
    ${LIBS_VEGAS}/WWAudio
    ${LIBS_VEGAS}/WWDownload
    ${LIBS_VEGAS}/Wwutil
    ${LIBS_VEGAS}/wwshade
    ${LIBS_SOURCE}
    ${LIBS_SOURCE}/Compression
    ${LIBS_SOURCE}/GameSpy
    ${LIBS_INCLUDE}
    ${LIBS_INCLUDE}/MSS
    ${LIBS_INCLUDE}/Bink
    ${LIBS_INCLUDE}/Granny
    ${LIBS_INCLUDE}/Lib
    ${LIBS_INCLUDE}/rts
    ${BGFX_INCLUDE}
    ${BX_INCLUDE}
    ${BIMG_INCLUDE}
)

# =============================================================================
# Library: wwdebug
# =============================================================================
file(GLOB WWDEBUG_SRC ${LIBS_VEGAS}/WWDebug/*.cpp)
add_library(wwdebug STATIC ${WWDEBUG_SRC})
target_include_directories(wwdebug PUBLIC ${COMMON_INCLUDES})

# =============================================================================
# Library: wwlib
# =============================================================================
file(GLOB WWLIB_SRC ${LIBS_VEGAS}/WWLib/*.cpp ${LIBS_VEGAS}/WWLib/*.c)
# Exclude legacy DirectDraw files - not used with BGFX renderer
list(FILTER WWLIB_SRC EXCLUDE REGEX "ddraw\\.cpp$|dsurface\\.cpp$")
add_library(wwlib STATIC ${WWLIB_SRC})
target_include_directories(wwlib PUBLIC ${COMMON_INCLUDES})
target_link_libraries(wwlib PUBLIC wwdebug)

# =============================================================================
# Library: wwmath
# =============================================================================
file(GLOB WWMATH_SRC ${LIBS_VEGAS}/WWMath/*.cpp)
add_library(wwmath STATIC ${WWMATH_SRC})
target_include_directories(wwmath PUBLIC ${COMMON_INCLUDES})
target_link_libraries(wwmath PUBLIC wwlib)

# =============================================================================
# Library: wwsaveload
# =============================================================================
file(GLOB WWSAVELOAD_SRC ${LIBS_VEGAS}/WWSaveLoad/*.cpp)
add_library(wwsaveload STATIC ${WWSAVELOAD_SRC})
target_include_directories(wwsaveload PUBLIC ${COMMON_INCLUDES})
target_link_libraries(wwsaveload PUBLIC wwlib)

# =============================================================================
# Library: wwutil
# =============================================================================
file(GLOB WWUTIL_SRC ${LIBS_VEGAS}/Wwutil/*.cpp)
add_library(wwutil STATIC ${WWUTIL_SRC})
target_include_directories(wwutil PUBLIC ${COMMON_INCLUDES})
target_link_libraries(wwutil PUBLIC wwlib)

# =============================================================================
# Library: wwdownload
# =============================================================================
file(GLOB WWDOWNLOAD_SRC ${LIBS_VEGAS}/WWDownload/*.cpp)
add_library(wwdownload STATIC ${WWDOWNLOAD_SRC})
target_include_directories(wwdownload PUBLIC ${COMMON_INCLUDES})
target_link_libraries(wwdownload PUBLIC wwlib)

# =============================================================================
# Library: compression
# =============================================================================
file(GLOB_RECURSE COMPRESSION_SRC
    ${LIBS_SOURCE}/Compression/*.cpp
    ${LIBS_SOURCE}/Compression/*.c
)
add_library(compression STATIC ${COMPRESSION_SRC})
target_include_directories(compression PUBLIC
    ${LIBS_SOURCE}/Compression
    ${LIBS_SOURCE}/Compression/EAC
    ${LIBS_SOURCE}/Compression/LZHCompress
    ${COMMON_INCLUDES}
)
target_compile_definitions(compression PRIVATE Z_PREFIX)

# =============================================================================
# Library: wwaudio
# =============================================================================
file(GLOB WWAUDIO_SRC ${LIBS_VEGAS}/WWAudio/*.cpp)
add_library(wwaudio STATIC ${WWAUDIO_SRC})
target_include_directories(wwaudio PUBLIC ${COMMON_INCLUDES})
target_link_libraries(wwaudio PUBLIC wwlib wwdebug)

# =============================================================================
# Library: ww3d2
# =============================================================================
file(GLOB WW3D2_SRC ${LIBS_VEGAS}/WW3D2/*.cpp ${LIBS_VEGAS}/WW3D2/*.c)
# Exclude old DX8 files replaced by BGFX equivalents
list(REMOVE_ITEM WW3D2_SRC
    ${LIBS_VEGAS}/WW3D2/dx8wrapper.cpp
    ${LIBS_VEGAS}/WW3D2/dx8caps.cpp
    ${LIBS_VEGAS}/WW3D2/dx8webbrowser.cpp
    ${LIBS_VEGAS}/WW3D2/missingtexture.cpp
)
add_library(ww3d2 STATIC ${WW3D2_SRC})
target_include_directories(ww3d2 PUBLIC ${COMMON_INCLUDES})
target_link_libraries(ww3d2 PUBLIC wwlib wwmath wwaudio wwdebug bgfx bx bimg)

# =============================================================================
# Library: wwshade
# =============================================================================
file(GLOB WWSHADE_SRC ${LIBS_VEGAS}/wwshade/*.cpp)
add_library(wwshade STATIC ${WWSHADE_SRC})
target_include_directories(wwshade PUBLIC ${COMMON_INCLUDES})
target_link_libraries(wwshade PUBLIC ww3d2 wwlib)

# =============================================================================
# Library: bx (BX base library - must be built before bgfx)
# =============================================================================
add_library(bx STATIC ${LIBS_SOURCE}/bx/src/amalgamated.cpp)
target_include_directories(bx PUBLIC
    ${BX_INCLUDE}
    ${LIBS_SOURCE}/bx/3rdparty
)
# BX needs its own internal include path
target_include_directories(bx PRIVATE ${LIBS_SOURCE}/bx/src)
target_compile_definitions(bx PUBLIC __STDC_FORMAT_MACROS)
target_compile_definitions(bx PUBLIC BX_CONFIG_DEBUG=$<IF:$<CONFIG:Debug>,1,0>)
set_target_properties(bx PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
if(MSVC)
    target_include_directories(bx PUBLIC ${LIBS_SOURCE}/bx/include/compat/msvc)
    target_compile_options(bx PUBLIC /Zc:__cplusplus /Zc:preprocessor)
endif()

# =============================================================================
# Library: bimg (BIMG image library)
# =============================================================================
file(GLOB BIMG_SRC ${LIBS_SOURCE}/bimg/src/*.cpp)
add_library(bimg STATIC ${BIMG_SRC})
target_include_directories(bimg PUBLIC ${BIMG_INCLUDE})
target_include_directories(bimg PRIVATE
    ${LIBS_SOURCE}/bimg/src
    ${LIBS_SOURCE}/bimg/3rdparty
    ${LIBS_SOURCE}/bimg/3rdparty/astc-encoder
    ${LIBS_SOURCE}/bimg/3rdparty/astc-encoder/include
    ${LIBS_SOURCE}/bimg/3rdparty/tinyexr/deps/miniz
    ${LIBS_SOURCE}/bimg/3rdparty/iqa/include
)
target_link_libraries(bimg PUBLIC bx)
set_target_properties(bimg PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# =============================================================================
# Library: bgfx (BGFX rendering library - single-file amalgamated build)
# =============================================================================
add_library(bgfx STATIC ${LIBS_SOURCE}/bgfx/src/amalgamated.cpp)
target_include_directories(bgfx PUBLIC ${BGFX_INCLUDE})
target_include_directories(bgfx PRIVATE
    ${LIBS_SOURCE}/bgfx/src
    ${LIBS_SOURCE}/bgfx/3rdparty
    ${LIBS_SOURCE}/bgfx/3rdparty/khronos
    ${LIBS_SOURCE}/bgfx/3rdparty/dxsdk/include
)
target_link_libraries(bgfx PUBLIC bx bimg)
set_target_properties(bgfx PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
# Enable D3D11 renderer on Windows
target_compile_definitions(bgfx PRIVATE
    BGFX_CONFIG_RENDERER_DIRECT3D11=1
    BGFX_CONFIG_RENDERER_DIRECT3D12=0
    BGFX_CONFIG_RENDERER_VULKAN=0
    BGFX_CONFIG_RENDERER_OPENGL=0
    BGFX_CONFIG_DEBUG=$<IF:$<CONFIG:Debug>,1,0>
)

# =============================================================================
# Zlib (external)
# =============================================================================
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/Zlib)
    file(GLOB ZLIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/Zlib/*.c)
    add_library(zlib STATIC ${ZLIB_SRC})
    target_include_directories(zlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Source/Zlib)
    target_compile_definitions(zlib PRIVATE Z_PREFIX)
elseif(EXISTS "c:/TheLab/zlib-extract/zlib-1.3.1")
    file(GLOB ZLIB_SRC "c:/TheLab/zlib-extract/zlib-1.3.1/*.c")
    add_library(zlib STATIC ${ZLIB_SRC})
    target_include_directories(zlib PUBLIC "c:/TheLab/zlib-extract/zlib-1.3.1")
    target_compile_definitions(zlib PRIVATE Z_PREFIX)
endif()

# =============================================================================
# Stub libraries for proprietary dependencies
# =============================================================================

# GameSpy stubs
foreach(GSLIB gamespyhttp gamespypatching gamespypeer gamespypresence gamespystats)
    add_library(${GSLIB} STATIC ${LIBS_SOURCE}/GameSpy/gamespy_stub.cpp)
    target_include_directories(${GSLIB} PUBLIC ${COMMON_INCLUDES})
endforeach()

# Benchmark stub
if(EXISTS ${LIBS_SOURCE}/Benchmark/benchmark_stub.c)
    add_library(benchmark STATIC ${LIBS_SOURCE}/Benchmark/benchmark_stub.c)
    target_include_directories(benchmark PUBLIC ${COMMON_INCLUDES})
endif()

# EA Browser Dispatch stub
if(EXISTS ${LIBS_SOURCE}/EABrowserDispatch/eabrowser_stub.c)
    add_library(eabrowserdispatch STATIC ${LIBS_SOURCE}/EABrowserDispatch/eabrowser_stub.c)
    target_include_directories(eabrowserdispatch PUBLIC ${COMMON_INCLUDES})
endif()

# =============================================================================
# GameEngine library
# =============================================================================
file(GLOB_RECURSE GAMEENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/Source/*.cpp)
# Exclude files with missing dependencies
list(REMOVE_ITEM GAMEENGINE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/Source/Common/Audio/GameSpeech.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/Source/GameNetwork/WOLBrowser/WebBrowser.cpp
)
add_library(GameEngine STATIC ${GAMEENGINE_SRC})
target_include_directories(GameEngine PUBLIC
    ${ENGINE_PRECOMPILED}
    ${ENGINE_INCLUDE}
    ${COMMON_INCLUDES}
    ${LIBS_SOURCE}/GameSpy
    ${DX8_SDK_INCLUDE}
)
target_link_libraries(GameEngine PUBLIC
    ww3d2 wwshade wwlib wwmath wwaudio wwdebug wwsaveload wwutil wwdownload compression
)
# GameEngine uses forced include of precompiled header
target_compile_options(GameEngine PRIVATE /FI"PreRTS.h")

# =============================================================================
# GameEngineDevice library
# =============================================================================
file(GLOB_RECURSE GAMEENGINEDEVICE_SRC ${DEVICE_SOURCE}/*.cpp)
add_library(GameEngineDevice STATIC ${GAMEENGINEDEVICE_SRC})
target_include_directories(GameEngineDevice PUBLIC
    ${DEVICE_INCLUDE}
    ${ENGINE_PRECOMPILED}
    ${ENGINE_INCLUDE}
    ${COMMON_INCLUDES}
    ${DX8_SDK_INCLUDE}
)
target_link_libraries(GameEngineDevice PUBLIC GameEngine ww3d2 wwshade)
target_compile_options(GameEngineDevice PRIVATE /FI"PreRTS.h")

# =============================================================================
# GeneralsZH Executable
# =============================================================================
add_executable(GeneralsZH WIN32
    ${CMAKE_CURRENT_SOURCE_DIR}/Main/WinMain.cpp
)
target_include_directories(GeneralsZH PRIVATE
    ${ENGINE_PRECOMPILED}
    ${ENGINE_INCLUDE}
    ${DEVICE_INCLUDE}
    ${COMMON_INCLUDES}
    ${DX8_SDK_INCLUDE}
)
target_compile_options(GeneralsZH PRIVATE /FI"PreRTS.h")
target_link_libraries(GeneralsZH PRIVATE
    GameEngineDevice
    GameEngine
    ww3d2
    wwshade
    wwlib
    wwmath
    wwaudio
    wwdebug
    wwsaveload
    wwutil
    wwdownload
    compression
    bgfx
    bx
    bimg
    gamespyhttp
    gamespypatching
    gamespypeer
    gamespypresence
    gamespystats
)

# Conditionally link zlib
if(TARGET zlib)
    target_link_libraries(GeneralsZH PRIVATE zlib)
endif()

# Windows system libraries
target_link_libraries(GeneralsZH PRIVATE
    winmm
    wsock32
    ws2_32
    dinput8
    dxguid
    dsound
    vfw32
    wininet
    imagehlp
    snmpapi
    comctl32
    ole32
    oleaut32
    uuid
    shell32
    mapi32
    advapi32
    user32
    gdi32
    comdlg32
    d3d11
    dxgi
)

# =============================================================================
# Tests
# =============================================================================
enable_testing()

# Test: stub compilation
add_executable(test_stub_compilation ${CMAKE_CURRENT_SOURCE_DIR}/Tests/test_stub_compilation.cpp)
target_include_directories(test_stub_compilation PRIVATE ${COMMON_INCLUDES})
add_test(NAME StubCompilation COMMAND test_stub_compilation)

# Test: math library
add_executable(test_math ${CMAKE_CURRENT_SOURCE_DIR}/Tests/test_math.cpp)
target_include_directories(test_math PRIVATE ${COMMON_INCLUDES})
target_link_libraries(test_math wwmath wwlib wwdebug)
add_test(NAME MathLibrary COMMAND test_math)

message(STATUS "")
message(STATUS "=== Generals: Zero Hour Build Configuration ===")
message(STATUS "  Target: GeneralsZH.exe (Win32)")
message(STATUS "  Renderer: BGFX (D3D11)")
message(STATUS "  Tests: ON")
message(STATUS "================================================")
message(STATUS "")

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
        # /D_INTERNAL  # Disabled — causes static initializer crash (investigate later)
        # Enable the REAL stack-trace / exception-dump implementation.
        #
        # GameEngine/Source/Common/System/StackDump.cpp wraps its entire body in
        #   #if defined(_DEBUG) || defined(_INTERNAL) || defined(IG_DEBUG_STACKTRACE)
        # so in a Release build with _INTERNAL disabled (above) all of
        # FillStackAddresses / StackDumpFromAddresses / GetFunctionDetails /
        # DumpExceptionInfo vanished, and Main/linker_stubs.cpp supplied no-op
        # replacements. The result was that every crash produced no stack trace at
        # all, which made three separate faults this week diagnosable only by log
        # archaeology.
        #
        # IG_DEBUG_STACKTRACE is the narrow switch for exactly this block, so it
        # avoids whatever static-initializer problem _INTERNAL causes. Dbghelp is
        # pulled in by a #pragma comment(linker) inside StackDump.cpp itself, and
        # imagehlp is already linked below.
        /DIG_DEBUG_STACKTRACE
    )
    # Disable specific warnings that flood the build
    add_compile_options(
        /wd4244 /wd4267 /wd4996 /wd4005 /wd4273
        /wd4018 /wd4305 /wd4309 /wd4800 /wd4838
        /wd4102 /wd4473 /wd5043 /wd4146 /wd4804
    )
    # Enable SEH exception handling so catch(...) catches access violations
    add_compile_options(/EHa)
    # Linker flags for inline assembly compatibility
    add_link_options(/SAFESEH:NO)

    # Debug symbols in Release.
    #
    # Release was building with no PDB at all, so nothing could symbolise a
    # crash: Windows Error Reporting could only say "fault at RVA 0x29c20 in
    # GeneralsZH.exe" and neither cdb nor a map lookup could turn that into a
    # function. Combined with the gutted DumpExceptionInfo, every access
    # violation this week was diagnosed by guessing.
    #
    # /Zi emits debug info, /DEBUG writes the PDB, and /OPT:REF,/OPT:ICF restore
    # the dead-code and identical-COMDAT folding that /DEBUG otherwise disables
    # -- so this costs disk, not speed. /MAP additionally gives a plain-text
    # RVA-to-symbol table, which is what lets a WER fault offset be resolved
    # without a debugger attached.
    add_compile_options(/Zi)
    add_link_options(/DEBUG /MAP /OPT:REF /OPT:ICF)
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
)
add_library(ww3d2 STATIC ${WW3D2_SRC})
target_include_directories(ww3d2 PUBLIC ${COMMON_INCLUDES})
target_link_libraries(ww3d2 PUBLIC wwlib wwmath wwaudio wwdebug bgfx bx bimg d3dcompiler)
set_target_properties(ww3d2 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

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
file(GLOB ASTCENC_SRC ${LIBS_SOURCE}/bimg/3rdparty/astc-encoder/source/*.cpp)
add_library(bimg STATIC ${BIMG_SRC} ${ASTCENC_SRC})
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

# Profile library
file(GLOB PROFILE_SRC ${LIBS_SOURCE}/profile/*.cpp)
add_library(profile STATIC ${PROFILE_SRC})
target_include_directories(profile PUBLIC ${COMMON_INCLUDES})

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
    ${CMAKE_CURRENT_SOURCE_DIR}/Main
)
target_link_libraries(GameEngineDevice PUBLIC GameEngine ww3d2 wwshade)
target_compile_options(GameEngineDevice PRIVATE /FI"PreRTS.h")

# =============================================================================
# GeneralsZH Executable
# =============================================================================
add_executable(GeneralsZH WIN32
    ${CMAKE_CURRENT_SOURCE_DIR}/Main/WinMain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Main/linker_stubs.cpp
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
    profile
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
    imm32
    psapi
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

# Test: DX8 stub OUT-param safety (links bgfx for BGFXTexture8 destructor)
add_executable(test_dx8_stubs ${CMAKE_CURRENT_SOURCE_DIR}/Tests/test_chunk_01_dx8_stubs.cpp)
target_include_directories(test_dx8_stubs PRIVATE ${COMMON_INCLUDES})
target_link_libraries(test_dx8_stubs PRIVATE bgfx bx bimg psapi)
add_test(NAME DX8Stubs COMMAND test_dx8_stubs)

# Test: BGFX CPU buffer mechanics (links bgfx for BGFXTexture8 destructor)
add_executable(test_bgfx_buffers ${CMAKE_CURRENT_SOURCE_DIR}/Tests/test_chunk_02_bgfx_buffers.cpp)
target_include_directories(test_bgfx_buffers PRIVATE ${COMMON_INCLUDES})
target_link_libraries(test_bgfx_buffers PRIVATE bgfx bx bimg psapi)
add_test(NAME BGFXBuffers COMMAND test_bgfx_buffers)

# Test: BGFX headless init/frame/shutdown via Noop renderer
add_executable(test_bgfx_headless ${CMAKE_CURRENT_SOURCE_DIR}/Tests/test_chunk_03_bgfx_headless.cpp)
target_include_directories(test_bgfx_headless PRIVATE ${COMMON_INCLUDES})
target_link_libraries(test_bgfx_headless bgfx bx bimg d3d11 dxgi psapi)
add_test(NAME BGFXHeadless COMMAND test_bgfx_headless)

message(STATUS "")
message(STATUS "=== Generals: Zero Hour Build Configuration ===")
message(STATUS "  Target: GeneralsZH.exe (Win32)")
message(STATUS "  Renderer: BGFX (D3D11)")
message(STATUS "  Tests: ON")
message(STATUS "================================================")
message(STATUS "")

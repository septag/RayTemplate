cmake_minimum_required(VERSION 3.25)

set(APPNAME "" CACHE STRING "App name")
if (NOT APPNAME)
    message(FATAL_ERROR "APPNAME is not defined. -DAPPNAME=[Name]")
endif()
project(${APPNAME})

option(USE_LIVEPP "Use LivePP for windows debug builds. Requires LIVEPP_PATH" OFF)
if (USE_LIVEPP)
    set(LIVEPP_PATH "" CACHE PATH "Path to the root directory of LivePP")
    if (NOT EXISTS ${LIVEPP_PATH})
        message(FATAL_ERROR "LivePP path does not exist: ${LIVEPP_PATH}")
    endif()
endif()

if (CMAKE_GENERATOR MATCHES "Visual Studio" OR CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    option(MSVC_MULTITHREADED_COMPILE "Multi-threaded compilation in MSVC." ON)
endif()

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

# Remove harmful flags and set basic global flags
if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:DebugDLL>)

    string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    if (MSVC_MULTITHREADED_COMPILE AND NOT CMAKE_MAKE_PROGRAM MATCHES "ninja.exe")
        add_compile_options(/MP)    # multiprocessor compilation
    endif()
else()
    string(REPLACE "-fexceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "-frtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

    if (CMAKE_C_COMPILER_ID MATCHES "GNU")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-format-truncation")
    endif()
endif()

add_subdirectory(code/raylib)

set(SOURCE_FILES 
    code/Main.cpp 
    code/MainCommon.cpp 
    code/Main.h
    code/Core.cpp
    code/Core.h 
    code/clay/clay.h)

add_executable(${APPNAME} ${SOURCE_FILES})
target_link_libraries(${APPNAME} PRIVATE raylib)
target_compile_definitions(${APPNAME} PRIVATE APPNAME=${APPNAME})
target_precompile_headers(${APPNAME} PRIVATE code/Core.h)

if (USE_LIVEPP)
    target_include_directories(${APPNAME} PRIVATE ${LIVEPP_PATH}/API/x64)
    target_compile_definitions(${APPNAME} PRIVATE LIVEPP_PATH=${LIVEPP_PATH})
    target_compile_definitions(${APPNAME} PRIVATE USE_LIVEPP=1)
else()
    target_compile_definitions(${APPNAME} PRIVATE USE_LIVEPP=0)
endif()

set_property(TARGET ${APPNAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# LivePP
# CL: Enable Minimal Rebuild (/Gm-), Function level linking (/Gy), Optimize Global Data (/Gw)
# Link: NoReferences (/OPT:NOREF), Disable COMDAT Folding (/OPT:NOICF), Full Debug info (Already added), CreateHotpackableImage (/FUNCTIONPADMIN)
target_compile_options(${APPNAME} PUBLIC /Gm- /Gy /Gw)
target_link_options(${APPNAME} PUBLIC /OPT:NOREF /OPT:NOICF /FUNCTIONPADMIN) 

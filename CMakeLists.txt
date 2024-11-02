# ----------
# SETUP
# ----------
cmake_minimum_required(VERSION 3.28)

project(json_eval LANGUAGES CXX)

# Require GCC 13 on Linux
if(UNIX)
    set(CMAKE_C_COMPILER gcc-13)
    set(CMAKE_CXX_COMPILER g++-13)
    
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR
       CMAKE_CXX_COMPILER_VERSION VERSION_LESS 13.0)
        message(FATAL_ERROR "GCC 13 or higher required on Linux")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ----------
# EXECUTABLE
# ----------
include_directories(include)
add_executable(json_eval src/main.cpp src/json.cpp src/jsonEvaluator.cpp src/jsonParser.cpp)
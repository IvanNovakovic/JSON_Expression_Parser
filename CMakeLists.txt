# ----------
# SETUP
# ----------
cmake_minimum_required(VERSION 3.28)

project(json_eval LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ----------
# EXECUTABLE
# ----------
include_directories(include)
add_executable(json_eval src/main.cpp src/json.cpp src/jsonEvaluator.cpp src/jsonParser.cpp)
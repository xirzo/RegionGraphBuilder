cmake_minimum_required(VERSION 3.30.5)

project(
  project_name
  VERSION 1.0
  DESCRIPTION "Project description"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(lib)

add_executable(${PROJECT_NAME} ./bin/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE test)

option(BUILD_TESTS "Build tests" OFF)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

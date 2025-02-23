add_library(test test.cpp test.h)

target_include_directories(test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

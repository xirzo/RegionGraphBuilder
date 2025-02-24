add_executable(${PROJECT_NAME} main.cpp)

find_package(OGDF REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE graph_builder fetch parser json_writer json_reader OGDF COIN)

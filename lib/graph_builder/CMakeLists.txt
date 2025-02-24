add_library(graph_builder ./src/graph_builder.cpp)

add_library(graph_builder_headers INTERFACE)
target_include_directories(graph_builder_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(graph_builder
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(graph_builder
    PRIVATE
        fetch
        parser
        json_writer
        json_reader
        OGDF
        COIN
        nlohmann_json::nlohmann_json
        metrics
    PUBLIC
        graph_builder_headers
)

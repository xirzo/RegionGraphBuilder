add_library(parser ./src/code_parser.cpp ./src/neighbour_parser.cpp)

add_library(parser_headers INTERFACE)
target_include_directories(parser_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(parser
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(fetch
    PRIVATE
        nlohmann_json::nlohmann_json
    PUBLIC
        parser_headers
)

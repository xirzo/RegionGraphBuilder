add_library(json_writer ./src/json_writer.cpp)

add_library(json_writer_headers INTERFACE)
target_include_directories(json_writer_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(json_writer
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(json_writer
    PRIVATE
        nlohmann_json::nlohmann_json
    PUBLIC
        json_writer_headers
)

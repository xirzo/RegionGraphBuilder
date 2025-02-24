add_library(metrics ./src/metrics.cpp)

add_library(metrics_headers INTERFACE)
target_include_directories(metrics_headers INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_include_directories(metrics
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(metrics
    PRIVATE
        OGDF
        COIN
    PUBLIC
        metrics_headers
)

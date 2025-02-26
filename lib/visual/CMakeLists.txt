add_library(visual ./src/visual.cpp)

add_library(visual_headers INTERFACE)
target_include_directories(
  visual_headers
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

target_include_directories(
  visual
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(
  visual
  PRIVATE country OGDF COIN nlohmann_json::nlohmann_json
  PUBLIC visual_headers)

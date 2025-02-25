add_library(fetch ./src/fetch.cpp)

add_library(fetch_headers INTERFACE)
target_include_directories(
  fetch_headers INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                          $<INSTALL_INTERFACE:include>)

target_include_directories(
  fetch
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(
  fetch
  PRIVATE cpr::cpr nlohmann_json::nlohmann_json
  PUBLIC fetch_headers)

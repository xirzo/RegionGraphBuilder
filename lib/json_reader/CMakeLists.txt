add_library(json_reader ./src/json_reader.cpp)

add_library(json_reader_headers INTERFACE)
target_include_directories(
  json_reader_headers
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

target_include_directories(
  json_reader
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(
  json_reader
  PRIVATE nlohmann_json::nlohmann_json
  PUBLIC json_reader_headers)

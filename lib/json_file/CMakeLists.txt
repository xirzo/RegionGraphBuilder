add_library(json_file ./src/json_file.cpp)

add_library(json_file_headers INTERFACE)
target_include_directories(
  json_file_headers
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>)

target_include_directories(
  json_file
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(json_file PUBLIC json_file_headers country
                                       nlohmann_json::nlohmann_json)

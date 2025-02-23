add_library(fetch fetch.cpp fetch.h)

target_include_directories(fetch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(fetch PRIVATE cpr::cpr nlohmann_json::nlohmann_json)

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "graph_builder.h"

int main(void) {
    const std::string region_to_search_in = "europe";

    const char* geo_data_api_key = std::getenv("geo_data_api_key");

    if (geo_data_api_key == nullptr || strlen(geo_data_api_key) == 0) {
        std::cerr << "error: geo data api key is not set" << std::endl;
        return EXIT_FAILURE;
    }

    graph_builder builder(geo_data_api_key);

    std::string error_details;

    if (auto err = builder.build(region_to_search_in, error_details);
        err != graph_builder::error{}) {
        std::cerr << error_details << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

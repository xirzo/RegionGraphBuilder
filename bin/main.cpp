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

    graph_builder graph_builder(geo_data_api_key);

    graph_builder.build(region_to_search_in);

    return EXIT_SUCCESS;
}

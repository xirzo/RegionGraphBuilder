#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>

#include "graph_builder.h"

int main(int argc, char** argv) {
    std::string region_to_search_in = "europe";

    if (argc == 2) {
        region_to_search_in = argv[1];
    }

    const char* geo_data_source_api_key = std::getenv("geo_data_source_api_key");

    if (geo_data_source_api_key == nullptr || strlen(geo_data_source_api_key) == 0) {
        std::cerr << "Error: geodatasource api key is not set" << std::endl;
        return EXIT_FAILURE;
    }

    graph_builder builder(geo_data_source_api_key, region_to_search_in);

    return builder.build();
}

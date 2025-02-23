#include <cstdlib>
#include <cstring>
#include <iostream>

#include "fetch.h"

int main(void) {
    const char* geo_data_source_api_key = std::getenv("geo_data_source_api_key");

    if (geo_data_source_api_key == nullptr || strlen(geo_data_source_api_key) == 0) {
        std::cerr << "Error: geodatasource api key is not set" << std::endl;
        return EXIT_FAILURE;
    }

    fetch::fetch_neighboring_countries(geo_data_source_api_key, "ru");

    return EXIT_SUCCESS;
}

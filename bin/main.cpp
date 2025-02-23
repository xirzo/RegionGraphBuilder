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

    const auto codes_result = fetch::fetch_region_codes();

    if (!codes_result.has_value()) {
        std::cerr << codes_result.error() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << codes_result.value() << std::endl;

    const auto neighboring_result =
        fetch::fetch_neighboring_countries(geo_data_source_api_key, "ru");

    if (!neighboring_result.has_value()) {
        std::cerr << neighboring_result.error() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << neighboring_result.value() << std::endl;

    return EXIT_SUCCESS;
}

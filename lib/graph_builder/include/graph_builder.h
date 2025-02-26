#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <expected>
#include <string>

class graph_builder {
public:
    enum class error {
        region_codes_failed,
        countries_failed,
        countries_write_failed,
        read_region_file_error
    };

    graph_builder(const std::string geo_data_api_key)
        : geo_data_api_key_(std::move(geo_data_api_key)) {}

    error build(const std::string& region, std::string& error_details);

private:
    const std::string geo_data_api_key_;
};
#endif  // !GRAPH_BUILDER_H

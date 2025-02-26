#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <expected>
#include <string>

class graph_builder {
public:
    enum class error { region_codes_failed, countries_failed };

    graph_builder(const std::string geo_data_api_key)
        : geo_data_api_key_(std::move(geo_data_api_key)) {}
    
    error build(std::string_view region, std::string& error_details);

private:
    const std::string geo_data_api_key_;

    static constexpr std::string_view error_to_string(error err);
};
#endif  // !GRAPH_BUILDER_H

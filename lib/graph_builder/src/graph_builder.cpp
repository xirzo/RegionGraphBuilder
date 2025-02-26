#include "graph_builder.h"

#include <filesystem>
#include <format>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "country.h"
#include "fetch.h"
#include "json_file.h"
#include "visual.h"

class graph_builder::impl {
public:
    explicit impl(std::string api_key) : geo_data_api_key_(std::move(api_key)) {}

    error build(const std::string& region, std::string& error_details);

private:
    std::expected<std::unordered_map<std::string, country>, error> fetch_countries(
        std::string_view region, std::string& error_details) const;

    std::expected<std::unordered_map<std::string, country>, error>

    fetch_and_cache_countries(const std::string& region_filename,
                              const std::string& region,
                              std::string& error_details) const;

    const std::string geo_data_api_key_;
};

std::expected<std::unordered_map<std::string, country>, graph_builder::error>
graph_builder::impl::fetch_countries(std::string_view region,
                                     std::string& error_details) const {
    auto region_codes_result = fetch::fetch_region_codes(std::string(region));

    if (!region_codes_result) {
        error_details =
            std::format("region codes fetch error: {} (region: {})",
                        static_cast<int>(region_codes_result.error()), region);
        return std::unexpected(error::region_codes_failed);
    }

    auto countries_result =
        fetch::fetch_countries(geo_data_api_key_, region_codes_result.value());

    if (!countries_result) {
        error_details = std::format("countries fetch error: {} (region: {})",
                                    static_cast<int>(countries_result.error()), region);
        return std::unexpected(error::countries_failed);
    }

    return countries_result.value();
}

std::expected<std::unordered_map<std::string, country>, graph_builder::error>
graph_builder::impl::fetch_and_cache_countries(const std::string& region_filename,
                                               const std::string& region,
                                               std::string& error_details) const {
    auto countries_result = fetch_countries(region, error_details);

    if (!countries_result) {
        return std::unexpected(countries_result.error());
    }

    nlohmann::json j_umap(countries_result.value());
    auto write_result = json_file::write(region_filename, j_umap);

    if (!write_result) {
        error_details = std::format("countries write to file error: {}",
                                    static_cast<int>(write_result.error()));
        return std::unexpected(error::countries_write_failed);
    }

    return countries_result;
}

graph_builder::error graph_builder::impl::build(const std::string& region,
                                                std::string& error_details) {
    const std::string region_filename = region + ".json";
    std::unordered_map<std::string, country> countries;

    if (!std::filesystem::exists(region_filename)) {
        auto fetch_and_cache_result =
            fetch_and_cache_countries(region_filename, region, error_details);

        if (!fetch_and_cache_result) {
            return fetch_and_cache_result.error();
        }

        countries = fetch_and_cache_result.value();
    } else {
        auto region_result = json_file::read(region_filename);

        if (!region_result) {
            return error::read_region_file_error;
        }

        countries = region_result.value();
    }

    export_graph(countries, region + "graph.svg");

    return {};
}

graph_builder::graph_builder(std::string geo_data_api_key)
    : pimpl_(std::make_unique<impl>(std::move(geo_data_api_key))) {}

graph_builder::~graph_builder() = default;

graph_builder::graph_builder(graph_builder&&) noexcept = default;
graph_builder& graph_builder::operator=(graph_builder&&) noexcept = default;

graph_builder::error graph_builder::build(const std::string& region,
                                          std::string& error_details) {
    return pimpl_->build(region, error_details);
}

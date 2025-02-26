#include "graph_builder.h"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "country.h"
#include "fetch.h"
#include "json_file.h"
#include "visual.h"

inline graph_builder::error make_error(graph_builder::error::code code,
                                       std::string message, std::string operation,
                                       std::string details = "") {
    return graph_builder::error{.error_code = code,
                                .message = std::move(message),
                                .operation = std::move(operation),
                                .details = std::move(details)};
}

class graph_builder::impl {
public:
    explicit impl(std::string api_key) : geo_data_api_key_(std::move(api_key)) {}

    std::expected<void, error> build(const std::string& region);

private:
    std::expected<std::unordered_map<std::string, country>, error> fetch_countries(
        std::string_view region) const;

    std::expected<std::unordered_map<std::string, country>, error>
    fetch_and_cache_countries(const std::string& region_filename,
                              const std::string& region) const;

    const std::string geo_data_api_key_;
};

std::expected<std::unordered_map<std::string, country>, graph_builder::error>
graph_builder::impl::fetch_countries(std::string_view region) const {
    auto region_codes_result = fetch::fetch_region_codes(std::string(region));

    if (!region_codes_result) {
        const auto& fetch_err = region_codes_result.error();
        return std::unexpected(make_error(
            error::code::region_codes_failed,
            "Failed to fetch region codes for region '" + std::string(region) + "'",
            "fetch_region_codes",
            "Status code: " + std::to_string(fetch_err.status_code) + "\n" +
                "Context: " + fetch_err.context + "\n" + "Error: " + fetch_err.message +
                "\n" + "Raw error: " + fetch_err.raw_error));
    }

    auto countries_result =
        fetch::fetch_countries(geo_data_api_key_, region_codes_result.value());

    if (!countries_result) {
        const auto& fetch_err = countries_result.error();
        return std::unexpected(make_error(
            error::code::countries_failed,
            "Failed to fetch country data for region '" + std::string(region) + "'",
            "fetch_countries",
            "Status code: " + std::to_string(fetch_err.status_code) + "\n" +
                "Context: " + fetch_err.context + "\n" + "Error: " + fetch_err.message +
                "\n" + "Raw error: " + fetch_err.raw_error));
    }

    return countries_result.value();
}

std::expected<std::unordered_map<std::string, country>, graph_builder::error>
graph_builder::impl::fetch_and_cache_countries(const std::string& region_filename,
                                               const std::string& region) const {
    auto countries_result = fetch_countries(region);

    if (!countries_result) {
        return std::unexpected(countries_result.error());
    }

    nlohmann::json j_umap(countries_result.value());
    auto write_result = json_file::write(region_filename, j_umap);

    if (!write_result) {
        const auto& json_err = write_result.error();
        return std::unexpected(make_error(
            error::code::countries_write_failed, json_err.message, json_err.operation,
            "File: " + json_err.filename + "\n" + "Details: " + json_err.details));
    }

    return countries_result;
}

std::expected<void, graph_builder::error> graph_builder::impl::build(
    const std::string& region) {
    const std::string region_filename = region + ".json";
    std::unordered_map<std::string, country> countries;

    if (!std::filesystem::exists(region_filename)) {
        auto fetch_and_cache_result = fetch_and_cache_countries(region_filename, region);

        if (!fetch_and_cache_result) {
            return std::unexpected(fetch_and_cache_result.error());
        }

        countries = fetch_and_cache_result.value();
    } else {
        auto region_result = json_file::read(region_filename);

        if (!region_result) {
            const auto& json_err = region_result.error();
            return std::unexpected(make_error(
                error::code::read_region_file_error, json_err.message, json_err.operation,
                "File: " + json_err.filename + "\n" + "Details: " + json_err.details));
        }

        countries = region_result.value().get<std::unordered_map<std::string, country>>();
    }

    export_graph(countries, region + "graph.svg");
    return {};
}

graph_builder::graph_builder(std::string geo_data_api_key)
    : pimpl_(std::make_unique<impl>(std::move(geo_data_api_key))) {}

graph_builder::~graph_builder() = default;

graph_builder::graph_builder(graph_builder&&) noexcept = default;
graph_builder& graph_builder::operator=(graph_builder&&) noexcept = default;

std::expected<void, graph_builder::error> graph_builder::build(
    const std::string& region) {
    return pimpl_->build(region);
}

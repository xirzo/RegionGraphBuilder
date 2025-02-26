#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <expected>
#include <memory>
#include <string>

class graph_builder {
public:
    struct error {
        enum class code {
            region_codes_failed,
            countries_failed,
            countries_write_failed,
            read_region_file_error
        };

        code error_code;
        std::string message;   
        std::string operation;  
        std::string details;     
    };

    explicit graph_builder(std::string geo_data_api_key);

    ~graph_builder();

    graph_builder(graph_builder&& other) noexcept;
    graph_builder& operator=(graph_builder&& other) noexcept;

    graph_builder(const graph_builder&) = delete;
    graph_builder& operator=(const graph_builder&) = delete;

    std::expected<void, error> build(const std::string& region);

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

#endif  // !GRAPH_BUILDER_H

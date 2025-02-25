#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <expected>
#include <string>

class graph_builder {
private:
    const std::string geo_data_api_key_;

public:
    graph_builder(const std::string geo_data_api_key)
        : geo_data_api_key_(std::move(geo_data_api_key)) {}

    void build(const std::string& region);
};

#endif  // !GRAPH_BUILDER_H

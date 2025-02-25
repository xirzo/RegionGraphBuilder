#ifndef GRAPH_BUILDER_H
#define GRAPH_BUILDER_H

#include <string>
#include <vector>

struct named_country {
    std::string name;
    std::vector<std::string> neighbours;
};

struct edge_pair {
    std::string country1;
    std::string country2;

    edge_pair(const std::string& c1, const std::string& c2) {
        if (c1 < c2) {
            country1 = c1;
            country2 = c2;
        } else {
            country1 = c2;
            country2 = c1;
        }
    }

    bool operator<(const edge_pair& other) const {
        if (country1 != other.country1) {
            return country1 < other.country1;
        }

        return country2 < other.country2;
    }
};

class graph_builder {
public:
    graph_builder(std::string geo_data_source_api_key, std::string region_to_search_in)
        : geo_data_source_api_key_(std::move(geo_data_source_api_key)),
          region_to_search_in_(std::move(region_to_search_in)) {}

    int build();

private:
    const std::string geo_data_source_api_key_;
    const std::string region_to_search_in_;
    const std::string codes_file_name_ = region_to_search_in_ + "codes.json";
};

#endif

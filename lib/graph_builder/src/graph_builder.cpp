#include "graph_builder.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/graphics.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>

#include "code_parser.h"
#include "fetch.h"
#include "json_reader.h"
#include "json_writer.h"
#include "name_parser.h"
#include "neighbour_parser.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

using namespace ogdf;

bool graph_builder::build() {
    if (!std::filesystem::exists(codes_file_name_)) {
        const auto code_fetch_result = fetch::fetch_region_codes(region_to_search_in_);

        if (!code_fetch_result.has_value()) {
            std::cerr << code_fetch_result.error() << std::endl;
            return EXIT_FAILURE;
        }

        auto code_write_result =
            json_writer::write_to_file(code_fetch_result.value(), codes_file_name_);

        if (!code_write_result.has_value()) {
            std::cerr << code_write_result.error() << std::endl;
            return EXIT_FAILURE;
        }
    }

    const auto codes_result = json_reader::read_file(codes_file_name_);

    if (!codes_result.has_value()) {
        std::cerr << codes_result.error() << std::endl;
        return EXIT_FAILURE;
    }

    const auto code_parse_result = code_parser::parse(codes_result.value());

    if (!code_parse_result.has_value()) {
        std::cerr << code_parse_result.error() << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<neighbour_parser::country> all_countries;

    for (const auto& code : code_parse_result.value()) {
        if (!std::filesystem::exists(code.iso_3166_2)) {
            const auto neighboring_result = fetch::fetch_neighboring_countries(
                geo_data_source_api_key_, code.iso_3166_2);

            if (!neighboring_result.has_value()) {
                std::cerr << neighboring_result.error() << std::endl;
                continue;
            }

            auto neighbour_write_result =
                json_writer::write_to_file(neighboring_result.value(), code.iso_3166_2);

            if (!neighbour_write_result.has_value()) {
                std::cerr << neighbour_write_result.error() << std::endl;
                continue;
            }
        }

        const auto neighbour_read_result = json_reader::read_file(code.iso_3166_2);

        if (!neighbour_read_result.has_value()) {
            std::cerr << neighbour_read_result.error() << std::endl;
            continue;
        }

        const auto neighbour_parse_result =
            neighbour_parser::parse(code, neighbour_read_result.value());

        if (!neighbour_parse_result.has_value()) {
            std::cerr << neighbour_parse_result.error() << std::endl;
            continue;
        }

        all_countries.push_back(neighbour_parse_result.value());
    }

    const string code_to_country_name_filename =
        region_to_search_in_ + "code_to_country_name.json";

    nlohmann::json ctcn;

    if (!std::filesystem::exists(code_to_country_name_filename)) {
        for (const auto& country : all_countries) {
            auto fetch_country_result = fetch::fetch_country(country.code.iso_3166_2);

            if (!fetch_country_result) {
                std::cerr << fetch_country_result.error() << std::endl;
                continue;
            }

            auto parse_name_result = name_parser::parse(fetch_country_result.value());

            if (!parse_name_result) {
                std::cerr << parse_name_result.error() << std::endl;
                continue;
            }

            ctcn[country.code.iso_3166_2] = parse_name_result.value();
        }

        auto ctcn_write_result =
            json_writer::write_json_to_file(ctcn, code_to_country_name_filename);

        if (!ctcn_write_result) {
            std::cerr << ctcn_write_result.error() << std::endl;
            return EXIT_FAILURE;
        }
    }

    auto ctcn_read_result = json_reader::read_file(code_to_country_name_filename);

    if (!ctcn_read_result) {
        std::cerr << ctcn_read_result.error() << std::endl;
        return EXIT_FAILURE;
    }

    nlohmann::json code_to_country_name = nlohmann::json::parse(ctcn_read_result.value());

    std::vector<named_country> countries;

    for (const auto& country : all_countries) {
        named_country c;

        if (code_to_country_name.find(country.code.iso_3166_2) ==
            code_to_country_name.end()) {
            continue;
        }

        if (code_to_country_name[country.code.iso_3166_2].is_null()) {
            continue;
        }

        c.name = code_to_country_name[country.code.iso_3166_2].get<std::string>();

        for (const auto& neighbour : country.neighbours) {
            if (code_to_country_name.find(neighbour.iso_3166_2) ==
                code_to_country_name.end()) {
                continue;
            }

            if (code_to_country_name[neighbour.iso_3166_2].is_null()) {
                continue;
            }

            c.neighbours.emplace_back(
                code_to_country_name[neighbour.iso_3166_2].get<std::string>());
        }

        countries.push_back(c);
    }

    std::unordered_map<std::string, node> node_to_country;
    Graph graph;
    GraphAttributes graph_attribute(
        graph, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics |
                   GraphAttributes::nodeLabel | GraphAttributes::edgeLabel |
                   GraphAttributes::edgeStyle | GraphAttributes::edgeArrow |
                   GraphAttributes::nodeStyle

    );

    graph_attribute.directed() = false;

    for (const auto& country : countries) {
        if (!node_to_country.contains(country.name)) {
            node v = graph.newNode();
            graph_attribute.label(v) = country.name;

            graph_attribute.width(v) = 120.0;
            graph_attribute.height(v) = 80.0;

            graph_attribute.shape(v) = Shape::RoundedRect;

            node_to_country[country.name] = v;
        }
    }

    std::set<edge_pair> added_edges;

    for (const auto& country : countries) {
        auto source_it = node_to_country.find(country.name);

        if (source_it != node_to_country.end()) {
            for (const auto& neighbour : country.neighbours) {
                auto target_it = node_to_country.find(neighbour);

                if (target_it != node_to_country.end()) {
                    edge_pair edge_pair(country.name, neighbour);

                    if (added_edges.insert(edge_pair).second) {
                        edge e = graph.newEdge(source_it->second, target_it->second);
                        graph_attribute.strokeWidth(e) = 2.0;
                        graph_attribute.arrowType(e) = EdgeArrow::None;
                    }
                }
            }
        }
    }

    SugiyamaLayout sugiyama_layout;

    sugiyama_layout.setRanking(new OptimalRanking);
    sugiyama_layout.setCrossMin(new MedianHeuristic);

    OptimalHierarchyLayout* hierarchyLayout = new OptimalHierarchyLayout;
    hierarchyLayout->layerDistance(5.0);
    hierarchyLayout->nodeDistance(15.0);
    hierarchyLayout->weightBalancing(0.8);

    sugiyama_layout.setLayout(hierarchyLayout);

    sugiyama_layout.call(graph_attribute);

    GraphIO::write(graph_attribute, region_to_search_in_ + "graph.svg", GraphIO::drawSVG);

    std::cout << "Graph has been created and saved" << std::endl;
    return EXIT_SUCCESS;
}

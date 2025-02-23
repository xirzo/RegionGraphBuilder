#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <vector>

#include "code_parser.h"
#include "fetch.h"
#include "json_reader.h"
#include "json_writer.h"
#include "neighbour_parser.h"

using namespace ogdf;

int main(void) {
    const char* geo_data_source_api_key = std::getenv("geo_data_source_api_key");

    if (geo_data_source_api_key == nullptr || strlen(geo_data_source_api_key) == 0) {
        std::cerr << "Error: geodatasource api key is not set" << std::endl;
        return EXIT_FAILURE;
    }

    static constexpr std::string codes_file_name = "codes.json";

    if (!std::filesystem::exists(codes_file_name)) {
        const auto code_fetch_result = fetch::fetch_region_codes();

        if (!code_fetch_result.has_value()) {
            std::cerr << code_fetch_result.error() << std::endl;
            return EXIT_FAILURE;
        }

        auto code_write_result =
            json_writer::write_to_file(code_fetch_result.value(), codes_file_name);

        if (!code_write_result.has_value()) {
            std::cerr << code_write_result.error() << std::endl;
            return EXIT_FAILURE;
        }
    }

    const auto codes_result = json_reader::read_file(codes_file_name);

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
                geo_data_source_api_key, code.iso_3166_2);

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
            neighbour_parser::parse("name", code, neighbour_read_result.value());

        if (!neighbour_parse_result.has_value()) {
            std::cerr << neighbour_parse_result.error() << std::endl;
            continue;
        }

        all_countries.push_back(neighbour_parse_result.value());
    }

    std::unordered_map<std::string, node> node_to_country;
    Graph graph;
    GraphAttributes graph_attribute(
        graph, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics |
                   GraphAttributes::nodeLabel | GraphAttributes::edgeLabel |
                   GraphAttributes::edgeStyle | GraphAttributes::nodeStyle);

    for (const auto& country : all_countries) {
        if (!node_to_country.contains(country.code.iso_3166_2)) {
            node v = graph.newNode();
            graph_attribute.label(v) = country.code.iso_3166_2;

            graph_attribute.width(v) = 80.0;
            graph_attribute.height(v) = 40.0;

            node_to_country[country.code.iso_3166_2] = v;
        }
    }

    for (const auto& country : all_countries) {
        for (const auto& neighbour : country.neighbours) {
            if (node_to_country.contains(country.code.iso_3166_2) &&
                node_to_country.contains(neighbour.code.iso_3166_2)) {
                edge e = graph.newEdge(node_to_country[country.code.iso_3166_2],
                                       node_to_country[neighbour.code.iso_3166_2]);

                graph_attribute.strokeWidth(e) = 2.0;
            }
        }
    }

    SugiyamaLayout sugiyama_layout;

    sugiyama_layout.setRanking(new OptimalRanking);
    sugiyama_layout.setCrossMin(new MedianHeuristic);

    OptimalHierarchyLayout* hierarchyLayout = new OptimalHierarchyLayout;
    hierarchyLayout->layerDistance(1.0);
    hierarchyLayout->nodeDistance(10.0);
    hierarchyLayout->weightBalancing(0.1);

    sugiyama_layout.setLayout(hierarchyLayout);

    sugiyama_layout.call(graph_attribute);

    GraphIO::write(graph_attribute, "graph.svg", GraphIO::drawSVG);

    std::cout << "Graph has been created and saved as 'graph.svg'" << std::endl;

    return EXIT_SUCCESS;
}

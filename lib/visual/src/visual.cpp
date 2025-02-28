#include "visual.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/planarity/PlanarizationLayout.h>

#include <set>
#include <string>

#include "distance_math.cpp"
#include "metrics.h"

using namespace ogdf;

edge_pair::edge_pair(const std::string& c1, const std::string& c2) {
    if (c1 < c2) {
        country1 = c1;
        country2 = c2;
    } else {
        country1 = c2;
        country2 = c1;
    }
}

bool edge_pair::operator<(const edge_pair& other) const {
    if (country1 != other.country1) {
        return country1 < other.country1;
    }

    return country2 < other.country2;
}

void export_graph(std::unordered_map<std::string, country>& countries,
                  const std::string& filename) {
    Graph graph;

    GraphAttributes graph_attribute(
        graph, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics |
                   GraphAttributes::nodeLabel | GraphAttributes::edgeLabel |
                   GraphAttributes::edgeStyle | GraphAttributes::edgeArrow |
                   GraphAttributes::nodeStyle

    );

    graph_attribute.directed() = false;

    std::unordered_map<std::string, node> name_to_node;

    for (const auto& [_, country] : countries) {
        node v = graph.newNode();

        name_to_node[country.name] = v;

        graph_attribute.label(v) = country.name;

        graph_attribute.width(v) = 120.0;
        graph_attribute.height(v) = 80.0;

        graph_attribute.shape(v) = Shape::RoundedRect;
    }

    std::set<edge_pair> added_edges;

    for (const auto& [_, country] : countries) {
        for (const auto& neighbour_iso : country.neighboring_countries_iso) {
            auto neighbour = countries[neighbour_iso];

            if (name_to_node.contains(neighbour.name)) {
                edge_pair edge_pair(country.name, neighbour.name);

                if (added_edges.insert(edge_pair).second) {
                    auto v1 = name_to_node[country.name];
                    auto v2 = name_to_node[neighbour.name];

                    edge e = graph.newEdge(v1, v2);

                    graph_attribute.strokeWidth(e) = 2.0;
                    graph_attribute.arrowType(e) = EdgeArrow::None;

                    const std::string name = std::to_string(distance(
                        country.capital_coords.latitude, country.capital_coords.longitude,
                        neighbour.capital_coords.latitude,
                        neighbour.capital_coords.longitude));

                    graph_attribute.label(e) = name;
                }
            }
        }
    }

    PlanarizationLayout planar_layout;

    planar_layout.call(graph_attribute);

    auto m = calculate_metrics(graph, graph_attribute);

    print_metrics(m, graph_attribute);

    GraphIO::write(graph_attribute, filename, GraphIO::drawSVG);
}

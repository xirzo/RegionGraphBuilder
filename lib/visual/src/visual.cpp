#include "visual.h"

#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/planarity/PlanarizationLayout.h>

#include <set>
#include <string>

using namespace ogdf;

long double rad(const long double& degree) {
    long double one_deg = (M_PI) / 180;
    return (one_deg * degree);
}

long double distance(long double lat1, long double long1, long double lat2,
                     long double long2) {
    lat1 = rad(lat1);
    long1 = rad(long1);
    lat2 = rad(lat2);
    long2 = rad(long2);

    long double dlong = long2 - long1;
    long double dlat = lat2 - lat1;

    long double ans =
        pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlong / 2), 2);

    ans = 2 * asin(sqrt(ans));

    constexpr long double R = 6371;

    ans = ans * R;

    return ans;
}

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

    GraphIO::write(graph_attribute, filename, GraphIO::drawSVG);
}

#include "metrics.h"

#include <algorithm>
#include <ctime>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>

std::vector<int> graph_metrics::bfs(const ogdf::Graph& G, ogdf::node start) {
    std::vector<int> distances(G.numberOfNodes(), -1);
    std::vector<bool> visited(G.numberOfNodes(), false);
    std::queue<ogdf::node> q;

    std::unordered_map<ogdf::node, int> node_to_index;
    int idx = 0;
    for (ogdf::node v : G.nodes) {
        node_to_index[v] = idx++;
    }

    q.push(start);
    distances[node_to_index[start]] = 0;
    visited[node_to_index[start]] = true;

    while (!q.empty()) {
        ogdf::node current = q.front();
        q.pop();

        for (ogdf::adjEntry adj : current->adjEntries) {
            ogdf::node neighbor = adj->twinNode();
            if (!visited[node_to_index[neighbor]]) {
                visited[node_to_index[neighbor]] = true;
                distances[node_to_index[neighbor]] =
                    distances[node_to_index[current]] + 1;
                q.push(neighbor);
            }
        }
    }

    return distances;
}

int graph_metrics::calculateEccentricity(const ogdf::Graph& G, ogdf::node v) {
    std::vector<int> distances = bfs(G, v);

    int eccentricity = 0;
    for (int dist : distances) {
        if (dist > eccentricity) {
            eccentricity = dist;
        }
    }

    return eccentricity;
}

void graph_metrics::dfsComponent(const ogdf::Graph& G, ogdf::node v,
                                 std::vector<bool>& visited,
                                 std::vector<ogdf::node>& component) {
    std::unordered_map<ogdf::node, int> node_to_index;
    int idx = 0;
    for (ogdf::node u : G.nodes) {
        node_to_index[u] = idx++;
    }

    visited[node_to_index[v]] = true;
    component.push_back(v);

    for (ogdf::adjEntry adj : v->adjEntries) {
        ogdf::node neighbor = adj->twinNode();
        if (!visited[node_to_index[neighbor]]) {
            dfsComponent(G, neighbor, visited, component);
        }
    }
}

std::vector<graph_metrics::component_info> graph_metrics::findComponents(
    const ogdf::Graph& G) {
    std::vector<bool> visited(G.numberOfNodes(), false);
    std::vector<component_info> components;
    std::unordered_map<ogdf::node, int> node_to_index;
    int idx = 0;

    for (ogdf::node u : G.nodes) {
        node_to_index[u] = idx++;
    }

    for (ogdf::node v : G.nodes) {
        if (!visited[node_to_index[v]]) {
            component_info comp;
            dfsComponent(G, v, visited, comp.nodes);
            comp.size = comp.nodes.size();
            components.push_back(comp);
        }
    }

    std::sort(
        components.begin(), components.end(),
        [](const component_info& a, const component_info& b) { return a.size > b.size; });

    return components;
}

bool graph_metrics::isClique(const ogdf::Graph& G, const std::vector<ogdf::node>& nodes) {
    for (size_t i = 0; i < nodes.size(); ++i) {
        for (size_t j = i + 1; j < nodes.size(); ++j) {
            bool adjacent = false;
            for (ogdf::adjEntry adj : nodes[i]->adjEntries) {
                if (adj->twinNode() == nodes[j]) {
                    adjacent = true;
                    break;
                }
            }
            if (!adjacent) return false;
        }
    }
    return true;
}

std::vector<ogdf::node> graph_metrics::bronKerboschWithPivot(const ogdf::Graph& G,
                                                             std::vector<ogdf::node>& R,
                                                             std::vector<ogdf::node>& P,
                                                             std::vector<ogdf::node>& X) {
    static std::vector<ogdf::node> max_clique;

    if (P.empty() && X.empty()) {
        if (R.size() > max_clique.size()) {
            max_clique = R;
        }
        return max_clique;
    }

    ogdf::node pivot = P.empty() ? X[0] : P[0];
    std::vector<ogdf::node> P_minus_N_pivot;

    for (const auto& v : P) {
        bool is_neighbor = false;
        for (ogdf::adjEntry adj : pivot->adjEntries) {
            if (adj->twinNode() == v) {
                is_neighbor = true;
                break;
            }
        }
        if (!is_neighbor) {
            P_minus_N_pivot.push_back(v);
        }
    }

    for (const auto& v : P_minus_N_pivot) {
        std::vector<ogdf::node> new_R = R;
        new_R.push_back(v);

        std::vector<ogdf::node> new_P, new_X;
        std::unordered_set<ogdf::node> neighbors;

        for (ogdf::adjEntry adj : v->adjEntries) {
            neighbors.insert(adj->twinNode());
        }

        for (const auto& p : P) {
            if (neighbors.find(p) != neighbors.end()) {
                new_P.push_back(p);
            }
        }

        for (const auto& x : X) {
            if (neighbors.find(x) != neighbors.end()) {
                new_X.push_back(x);
            }
        }

        bronKerboschWithPivot(G, new_R, new_P, new_X);

        P.erase(std::remove(P.begin(), P.end(), v), P.end());
        X.push_back(v);
    }

    return max_clique;
}

std::vector<ogdf::node> graph_metrics::findLargestClique(
    const ogdf::Graph& G, const std::vector<ogdf::node>& component) {
    std::vector<ogdf::node> R;
    std::vector<ogdf::node> P = component;
    std::vector<ogdf::node> X;

    return bronKerboschWithPivot(G, R, P, X);
}

int graph_metrics::calculateChromaticNumber(const ogdf::Graph& G,
                                            const std::vector<ogdf::node>& component) {
    std::unordered_map<ogdf::node, int> colors;

    for (ogdf::node v : component) {
        std::vector<bool> used_colors(component.size(), false);

        for (ogdf::adjEntry adj : v->adjEntries) {
            ogdf::node neighbor = adj->twinNode();
            if (colors.find(neighbor) != colors.end()) {
                used_colors[colors[neighbor]] = true;
            }
        }

        int color = 0;
        while (color < used_colors.size() && used_colors[color]) {
            color++;
        }
        colors[v] = color;
    }

    int max_color = 0;
    for (const auto& pair : colors) {
        max_color = std::max(max_color, pair.second);
    }

    return max_color + 1;
}

graph_metrics::metrics_result graph_metrics::calculate_metrics(
    const ogdf::Graph& G, const ogdf::GraphAttributes& GA) {
    metrics_result result;
    result.radius = std::numeric_limits<int>::max();
    result.diameter = 0;
    result.maxDegree = 0;
    result.minDegree = std::numeric_limits<int>::max();
    result.numVertices = G.numberOfNodes();
    result.numEdges = G.numberOfEdges();

    auto components = findComponents(G);
    result.numComponents = components.size();
    result.cyclomaticNumber =
        G.numberOfEdges() - G.numberOfNodes() + result.numComponents;

    if (!components.empty()) {
        result.largestComponentSize = components[0].size;
        result.chromaticNumber = calculateChromaticNumber(G, components[0].nodes);
        result.largestClique = findLargestClique(G, components[0].nodes);
        result.largestCliqueSize = result.largestClique.size();

        for (const auto& node : result.largestClique) {
            result.largestCliqueLabels.push_back(GA.label(node).c_str());
        }
    }

    for (ogdf::node v : G.nodes) {
        int ecc = calculateEccentricity(G, v);

        result.radius = std::min(result.radius, ecc);
        result.diameter = std::max(result.diameter, ecc);

        int degree = v->degree();
        if (degree > result.maxDegree) {
            result.maxDegree = degree;
            result.maxDegreeNodeLabel = GA.label(v).c_str();
        }
        if (degree < result.minDegree) {
            result.minDegree = degree;
            result.minDegreeNodeLabel = GA.label(v).c_str();
        }

        if (ecc == result.radius) {
            result.center.push_back(v);
            result.centerLabels.push_back(GA.label(v).c_str());
        }
    }

    return result;
}

void graph_metrics::printMetrics(const metrics_result& metrics) {
    std::cout << "\n=== Graph Metrics Report ===" << std::endl;
    std::cout << "Generated on: 2025-02-24 09:40:09 UTC" << std::endl;
    std::cout << "Generated by: xirzo" << std::endl;

    std::cout << "\nBasic Graph Properties:" << std::endl;
    std::cout << "- Number of vertices: " << metrics.numVertices << std::endl;
    std::cout << "- Number of edges: " << metrics.numEdges << std::endl;
    std::cout << "- Number of components: " << metrics.numComponents << std::endl;
    std::cout << "- Cyclomatic number: " << metrics.cyclomaticNumber << std::endl;

    std::cout << "\nLargest Component Properties:" << std::endl;
    std::cout << "- Size: " << metrics.largestComponentSize << std::endl;
    std::cout << "- Chromatic number: " << metrics.chromaticNumber << std::endl;
    std::cout << "- Largest clique size: " << metrics.largestCliqueSize << std::endl;

    if (!metrics.largestCliqueLabels.empty()) {
        std::cout << "- Largest clique countries:" << std::endl;
        for (const auto& label : metrics.largestCliqueLabels) {
            std::cout << "  * " << label << std::endl;
        }
    }

    std::cout << "\nDegree Information:" << std::endl;
    std::cout << "- Maximum degree: " << metrics.maxDegree
              << " (Node: " << metrics.maxDegreeNodeLabel << ")" << std::endl;
    std::cout << "- Minimum degree: " << metrics.minDegree
              << " (Node: " << metrics.minDegreeNodeLabel << ")" << std::endl;

    std::cout << "\nDistance Metrics:" << std::endl;
    std::cout << "- Radius (rad(G)): " << metrics.radius << std::endl;
    std::cout << "- Diameter (diam(G)): " << metrics.diameter << std::endl;

    std::cout << "\nCenter Vertices (center(G)):" << std::endl;
    for (const auto& label : metrics.centerLabels) {
        std::cout << "- " << label << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

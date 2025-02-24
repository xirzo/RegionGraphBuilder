#include "metrics.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <queue>

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

graph_metrics::metrics_result graph_metrics::calculate_metrics(
    const ogdf::Graph& G, const ogdf::GraphAttributes& GA) {
    metrics_result result;
    result.radius = std::numeric_limits<int>::max();
    result.diameter = 0;
    result.maxDegree = 0;
    result.minDegree = std::numeric_limits<int>::max();
    result.numVertices = G.numberOfNodes();
    result.numEdges = G.numberOfEdges();

    std::vector<std::pair<ogdf::node, int>> eccentricities;

    for (ogdf::node v : G.nodes) {
        int ecc = calculateEccentricity(G, v);
        eccentricities.push_back({v, ecc});

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
    }

    for (const auto& pair : eccentricities) {
        if (pair.second == result.radius) {
            result.center.push_back(pair.first);
            result.centerLabels.push_back(GA.label(pair.first).c_str());
        }
    }

    return result;
}

void graph_metrics::printMetrics(const metrics_result& metrics) {
    std::cout << "\n=== Graph Metrics Report ===" << std::endl;
    std::cout << "Generated on: 2025-02-24 09:06:37 UTC" << std::endl;
    std::cout << "Generated by: xirzo" << std::endl;

    std::cout << "\nBasic Graph Properties:" << std::endl;
    std::cout << "- Number of vertices: " << metrics.numVertices << std::endl;
    std::cout << "- Number of edges: " << metrics.numEdges << std::endl;

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

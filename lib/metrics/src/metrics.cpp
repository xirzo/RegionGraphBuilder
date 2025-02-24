#include "metrics.h"

#include <algorithm>
#include <ctime>
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

bool graph_metrics::isConnected(const ogdf::Graph& G,
                                const std::vector<ogdf::node>& nodes) {
    if (nodes.empty()) return true;

    std::unordered_map<ogdf::node, bool> visited;
    for (const auto& node : nodes) {
        visited[node] = false;
    }

    std::queue<ogdf::node> q;
    q.push(nodes[0]);
    visited[nodes[0]] = true;
    int visitedCount = 1;

    while (!q.empty()) {
        ogdf::node current = q.front();
        q.pop();

        for (ogdf::adjEntry adj : current->adjEntries) {
            ogdf::node neighbor = adj->twinNode();
            if (std::find(nodes.begin(), nodes.end(), neighbor) != nodes.end() &&
                !visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
                visitedCount++;
            }
        }
    }

    return visitedCount == nodes.size();
}

bool graph_metrics::hasEvenDegrees(const ogdf::Graph& G,
                                   const std::vector<ogdf::node>& nodes) {
    std::unordered_set<ogdf::node> nodeSet(nodes.begin(), nodes.end());

    for (const auto& node : nodes) {
        int degree = 0;
        for (ogdf::adjEntry adj : node->adjEntries) {
            if (nodeSet.find(adj->twinNode()) != nodeSet.end()) {
                degree++;
            }
        }
        if (degree % 2 != 0) return false;
    }
    return true;
}

bool graph_metrics::isEulerian(const ogdf::Graph& G,
                               const std::vector<ogdf::node>& nodes) {
    return isConnected(G, nodes) && hasEvenDegrees(G, nodes);
}

bool graph_metrics::areNodesAdjacent(const ogdf::Graph& G, ogdf::node u, ogdf::node v) {
    for (ogdf::adjEntry adj : u->adjEntries) {
        if (adj->twinNode() == v) return true;
    }
    return false;
}

bool graph_metrics::hamiltonianCycle(const ogdf::Graph& G,
                                     const std::vector<ogdf::node>& nodes,
                                     std::vector<ogdf::node>& path,
                                     std::vector<bool>& visited, ogdf::node current,
                                     int count) {
    if (count == nodes.size()) {
        return areNodesAdjacent(G, current, path[0]);
    }

    for (const auto& next : nodes) {
        if (!visited[&next - &nodes[0]] && areNodesAdjacent(G, current, next)) {
            path[count] = next;
            visited[&next - &nodes[0]] = true;

            if (hamiltonianCycle(G, nodes, path, visited, next, count + 1)) {
                return true;
            }

            visited[&next - &nodes[0]] = false;
        }
    }
    return false;
}

bool graph_metrics::isHamiltonian(const ogdf::Graph& G,
                                  const std::vector<ogdf::node>& nodes) {
    if (nodes.size() < 3) return false;

    std::vector<ogdf::node> path(nodes.size());
    std::vector<bool> visited(nodes.size(), false);

    path[0] = nodes[0];
    visited[0] = true;

    return hamiltonianCycle(G, nodes, path, visited, nodes[0], 1);
}

graph_metrics::subgraph_info graph_metrics::findLargestEulerianSubgraph(
    const ogdf::Graph& G, const ogdf::GraphAttributes& GA,
    const std::vector<ogdf::node>& component) {
    subgraph_info result;
    size_t n = component.size();

    if (n < 3) {
        return result;
    }

    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    std::vector<int> degrees(n, 0);

    for (size_t i = 0; i < n; ++i) {
        for (ogdf::adjEntry adjE : component[i]->adjEntries) {
            auto it = std::find(component.begin(), component.end(), adjE->twinNode());

            if (it != component.end()) {
                size_t j = it - component.begin();
                if (!adj[i][j]) {
                    adj[i][j] = adj[j][i] = true;
                    degrees[i]++;
                    degrees[j]++;
                }
            }
        }
    }

    std::vector<size_t> candidates;

    for (size_t i = 0; i < n; ++i) {
        if (degrees[i] % 2 == 0 && degrees[i] >= 2) {
            candidates.push_back(i);
        }
    }

    std::sort(candidates.begin(), candidates.end(),
              [&degrees](size_t a, size_t b) { return degrees[a] > degrees[b]; });

    for (size_t start : candidates) {
        std::vector<bool> used(n, false);
        std::vector<size_t> current;
        std::vector<int> currentDegrees(n, 0);

        used[start] = true;
        current.push_back(start);

        bool changed;

        do {
            changed = false;
            for (size_t i = 0; i < n; ++i) {
                if (used[i]) continue;

                int connections = 0;

                for (size_t j : current) {
                    if (adj[i][j]) connections++;
                }

                if (connections >= 2 && connections % 2 == 0) {
                    used[i] = true;
                    current.push_back(i);
                    changed = true;
                }
            }
        } while (changed);

        std::vector<ogdf::node> currentNodes;

        currentNodes.reserve(current.size());
        for (size_t idx : current) {
            currentNodes.push_back(component[idx]);
        }

        if (currentNodes.size() > result.size && isConnected(G, currentNodes)) {
            result.nodes = std::move(currentNodes);
            result.nodeLabels.clear();
            result.nodeLabels.reserve(current.size());

            for (size_t idx : current) {
                result.nodeLabels.push_back(GA.label(component[idx]).c_str());
            }
            result.size = current.size();
            result.isEulerian = true;

            if (result.size >= n - 1) {
                break;
            }
        }
    }

    return result;
}

graph_metrics::subgraph_info graph_metrics::findLargestHamiltonianSubgraph(
    const ogdf::Graph& G, const ogdf::GraphAttributes& GA,
    const std::vector<ogdf::node>& component) {
    subgraph_info result;
    size_t n = component.size();

    if (n < 3) {
        return result;
    }

    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    std::vector<int> degrees(n, 0);

    for (size_t i = 0; i < n; ++i) {
        for (ogdf::adjEntry adjE : component[i]->adjEntries) {
            auto it = std::find(component.begin(), component.end(), adjE->twinNode());
            if (it != component.end()) {
                size_t j = it - component.begin();
                if (!adj[i][j]) {
                    adj[i][j] = adj[j][i] = true;
                    degrees[i]++;
                    degrees[j]++;
                }
            }
        }
    }

    std::vector<size_t> highDegNodes;

    for (size_t i = 0; i < n; ++i) {
        if (degrees[i] >= n / 2) {
            highDegNodes.push_back(i);
        }
    }

    std::sort(highDegNodes.begin(), highDegNodes.end(),
              [&degrees](size_t a, size_t b) { return degrees[a] > degrees[b]; });

    for (size_t start : highDegNodes) {
        std::vector<size_t> current;
        std::vector<bool> used(n, false);
        current.push_back(start);
        used[start] = true;

        for (size_t i = 0; i < n; ++i) {
            if (i == start || used[i]) continue;

            int connections = 0;

            for (const size_t j : current) {
                if (adj[i][j]) connections++;
            }

            if (connections >= current.size() / 2) {
                current.push_back(i);
                used[i] = true;
            }
        }

        if (current.size() > result.size) {
            std::vector<ogdf::node> currentNodes;
            currentNodes.reserve(current.size());
            for (size_t idx : current) {
                currentNodes.push_back(component[idx]);
            }

            if (isHamiltonian(G, currentNodes)) {
                result.nodes = std::move(currentNodes);
                result.size = current.size();
                result.isHamiltonian = true;

                result.nodeLabels.clear();
                result.nodeLabels.reserve(current.size());
                for (size_t idx : current) {
                    result.nodeLabels.push_back(GA.label(component[idx]).c_str());
                }

                if (result.size >= n - 1) {
                    break;
                }
            }
        }
    }

    return result;
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

        result.largestEulerianSubgraph =
            findLargestEulerianSubgraph(G, GA, components[0].nodes);
        result.largestHamiltonianSubgraph =
            findLargestHamiltonianSubgraph(G, GA, components[0].nodes);
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
    std::cout << "Generated on: 2025-02-24 09:59:29 UTC" << std::endl;
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

    std::cout << "\nEulerian Properties:" << std::endl;
    std::cout << "- Largest Eulerian subgraph size: "
              << metrics.largestEulerianSubgraph.size << std::endl;
    if (!metrics.largestEulerianSubgraph.nodeLabels.empty()) {
        std::cout << "- Countries in largest Eulerian subgraph:" << std::endl;
        for (const auto& label : metrics.largestEulerianSubgraph.nodeLabels) {
            std::cout << "  * " << label << std::endl;
        }
    }

    std::cout << "\nHamiltonian Properties:" << std::endl;
    std::cout << "- Largest Hamiltonian subgraph size: "
              << metrics.largestHamiltonianSubgraph.size << std::endl;
    if (!metrics.largestHamiltonianSubgraph.nodeLabels.empty()) {
        std::cout << "- Countries in largest Hamiltonian subgraph:" << std::endl;
        for (const auto& label : metrics.largestHamiltonianSubgraph.nodeLabels) {
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

#include "metrics.h"

#include <algorithm>
#include <ctime>
#include <queue>
#include <unordered_map>
#include <unordered_set>

std::vector<int> graph_metrics_calculator::bfs(const ogdf::Graph& G, ogdf::node start) {
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

int graph_metrics_calculator::calculate_eccentricity(const ogdf::Graph& G, ogdf::node v) {
    std::vector<int> distances = bfs(G, v);

    int eccentricity = 0;
    for (int dist : distances) {
        if (dist > eccentricity) {
            eccentricity = dist;
        }
    }

    return eccentricity;
}

void graph_metrics_calculator::dfs_component(const ogdf::Graph& G, ogdf::node v,
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
            dfs_component(G, neighbor, visited, component);
        }
    }
}

std::vector<graph_metrics_calculator::component_info>
graph_metrics_calculator::find_components(const ogdf::Graph& G) {
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
            dfs_component(G, v, visited, comp.nodes);
            comp.size = comp.nodes.size();
            components.push_back(comp);
        }
    }

    std::sort(
        components.begin(), components.end(),
        [](const component_info& a, const component_info& b) { return a.size > b.size; });

    return components;
}

bool graph_metrics_calculator::is_clique(const ogdf::Graph& G,
                                         const std::vector<ogdf::node>& nodes) {
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

std::vector<ogdf::node> graph_metrics_calculator::bron_kerbosch_with_pivot(
    const ogdf::Graph& G, std::vector<ogdf::node>& R, std::vector<ogdf::node>& P,
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

        bron_kerbosch_with_pivot(G, new_R, new_P, new_X);

        P.erase(std::remove(P.begin(), P.end(), v), P.end());
        X.push_back(v);
    }

    return max_clique;
}

std::vector<ogdf::node> graph_metrics_calculator::find_largest_clique(
    const ogdf::Graph& G, const std::vector<ogdf::node>& component) {
    std::vector<ogdf::node> R;
    std::vector<ogdf::node> P = component;
    std::vector<ogdf::node> X;

    return bron_kerbosch_with_pivot(G, R, P, X);
}

int graph_metrics_calculator::calculate_chromatic_number(
    const ogdf::Graph& G, const std::vector<ogdf::node>& component) {
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

bool graph_metrics_calculator::is_connected(const ogdf::Graph& G,
                                            const std::vector<ogdf::node>& nodes) {
    if (nodes.empty()) return true;

    std::unordered_map<ogdf::node, bool> visited;
    for (const auto& node : nodes) {
        visited[node] = false;
    }

    std::queue<ogdf::node> q;
    q.push(nodes[0]);
    visited[nodes[0]] = true;
    int visited_count = 1;

    while (!q.empty()) {
        ogdf::node current = q.front();
        q.pop();

        for (ogdf::adjEntry adj : current->adjEntries) {
            ogdf::node neighbor = adj->twinNode();
            if (std::find(nodes.begin(), nodes.end(), neighbor) != nodes.end() &&
                !visited[neighbor]) {
                visited[neighbor] = true;
                q.push(neighbor);
                visited_count++;
            }
        }
    }

    return visited_count == nodes.size();
}

bool graph_metrics_calculator::has_even_degrees(const ogdf::Graph& G,
                                                const std::vector<ogdf::node>& nodes) {
    std::unordered_set<ogdf::node> node_set(nodes.begin(), nodes.end());

    for (const auto& node : nodes) {
        int degree = 0;
        for (ogdf::adjEntry adj : node->adjEntries) {
            if (node_set.find(adj->twinNode()) != node_set.end()) {
                degree++;
            }
        }
        if (degree % 2 != 0) return false;
    }
    return true;
}

bool graph_metrics_calculator::is_eulerian(const ogdf::Graph& G,
                                           const std::vector<ogdf::node>& nodes) {
    return is_connected(G, nodes) && has_even_degrees(G, nodes);
}

bool graph_metrics_calculator::are_nodes_adjacent(const ogdf::Graph& G, ogdf::node u,
                                                  ogdf::node v) {
    for (ogdf::adjEntry adj : u->adjEntries) {
        if (adj->twinNode() == v) return true;
    }
    return false;
}

bool graph_metrics_calculator::hamiltonian_cycle(const ogdf::Graph& G,
                                                 const std::vector<ogdf::node>& nodes,
                                                 std::vector<ogdf::node>& path,
                                                 std::vector<bool>& visited,
                                                 ogdf::node current, int count) {
    if (count == nodes.size()) {
        return are_nodes_adjacent(G, current, path[0]);
    }

    for (const auto& next : nodes) {
        if (!visited[&next - &nodes[0]] && are_nodes_adjacent(G, current, next)) {
            path[count] = next;
            visited[&next - &nodes[0]] = true;

            if (hamiltonian_cycle(G, nodes, path, visited, next, count + 1)) {
                return true;
            }

            visited[&next - &nodes[0]] = false;
        }
    }
    return false;
}

bool graph_metrics_calculator::is_hamiltonian(const ogdf::Graph& G,
                                              const std::vector<ogdf::node>& nodes) {
    if (nodes.size() < 3) return false;

    std::vector<ogdf::node> path(nodes.size());
    std::vector<bool> visited(nodes.size(), false);

    path[0] = nodes[0];
    visited[0] = true;

    return hamiltonian_cycle(G, nodes, path, visited, nodes[0], 1);
}

graph_metrics_calculator::subgraph_info
graph_metrics_calculator::find_largest_eulerian_subgraph(
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
        std::vector<int> current_degrees(n, 0);

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

        std::vector<ogdf::node> current_nodes;

        current_nodes.reserve(current.size());
        for (size_t idx : current) {
            current_nodes.push_back(component[idx]);
        }

        if (current_nodes.size() > result.size && is_connected(G, current_nodes)) {
            result.nodes = std::move(current_nodes);
            result.node_labels.clear();
            result.node_labels.reserve(current.size());

            for (size_t idx : current) {
                result.node_labels.push_back(GA.label(component[idx]).c_str());
            }
            result.size = current.size();
            result.is_eulerian = true;

            if (result.size >= n - 1) {
                break;
            }
        }
    }

    return result;
}

graph_metrics_calculator::subgraph_info
graph_metrics_calculator::find_largest_hamiltonian_subgraph(
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

    std::vector<size_t> high_deg_nodes;

    for (size_t i = 0; i < n; ++i) {
        if (degrees[i] >= n / 2) {
            high_deg_nodes.push_back(i);
        }
    }

    std::sort(high_deg_nodes.begin(), high_deg_nodes.end(),
              [&degrees](size_t a, size_t b) { return degrees[a] > degrees[b]; });

    for (size_t start : high_deg_nodes) {
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
            std::vector<ogdf::node> current_nodes;
            current_nodes.reserve(current.size());
            for (size_t idx : current) {
                current_nodes.push_back(component[idx]);
            }

            if (is_hamiltonian(G, current_nodes)) {
                result.nodes = std::move(current_nodes);
                result.size = current.size();
                result.is_hamiltonian = true;

                result.node_labels.clear();
                result.node_labels.reserve(current.size());
                for (size_t idx : current) {
                    result.node_labels.push_back(GA.label(component[idx]).c_str());
                }

                if (result.size >= n - 1) {
                    break;
                }
            }
        }
    }

    return result;
}

graph_metrics_calculator::metrics graph_metrics_calculator::calculate_metrics(
    const ogdf::Graph& G, const ogdf::GraphAttributes& GA) {
    metrics result;
    result.radius = std::numeric_limits<int>::max();
    result.diameter = 0;
    result.max_degree = 0;
    result.min_degree = std::numeric_limits<int>::max();
    result.num_vertices = G.numberOfNodes();
    result.num_edges = G.numberOfEdges();

    auto components = find_components(G);
    result.number_components = components.size();
    result.cyclomatic_number =
        G.numberOfEdges() - G.numberOfNodes() + result.number_components;

    if (!components.empty()) {
        result.largest_component_size = components[0].size;
        result.chromatic_number = calculate_chromatic_number(G, components[0].nodes);
        result.largest_clique = find_largest_clique(G, components[0].nodes);
        result.largest_clique_size = result.largest_clique.size();

        for (const auto& node : result.largest_clique) {
            result.largest_clique_members.push_back(GA.label(node).c_str());
        }

        result.largest_eulerian_subgraph =
            find_largest_eulerian_subgraph(G, GA, components[0].nodes);
        result.largest_hamiltonian_subgraph =
            find_largest_hamiltonian_subgraph(G, GA, components[0].nodes);
    }

    for (ogdf::node v : G.nodes) {
        int ecc = calculate_eccentricity(G, v);

        result.radius = std::min(result.radius, ecc);
        result.diameter = std::max(result.diameter, ecc);

        int degree = v->degree();
        if (degree > result.max_degree) {
            result.max_degree = degree;
            result.max_degree_node_label = GA.label(v).c_str();
        }
        if (degree < result.min_degree) {
            result.min_degree = degree;
            result.min_degree_node_label = GA.label(v).c_str();
        }

        if (ecc == result.radius) {
            result.center.push_back(v);
            result.center_labels.push_back(GA.label(v).c_str());
        }
    }

    return result;
}

void graph_metrics_calculator::print_metrics(const metrics& metrics) {
    std::cout << "\n=== Graph Metrics Report ===" << std::endl;
    std::cout << "Generated on: 2025-02-24 09:59:29 UTC" << std::endl;
    std::cout << "Generated by: xirzo" << std::endl;

    std::cout << "\nBasic Graph Properties:" << std::endl;
    std::cout << "- Number of vertices: " << metrics.num_vertices << std::endl;
    std::cout << "- Number of edges: " << metrics.num_edges << std::endl;
    std::cout << "- Number of components: " << metrics.number_components << std::endl;
    std::cout << "- Cyclomatic number: " << metrics.cyclomatic_number << std::endl;

    std::cout << "\nLargest Component Properties:" << std::endl;
    std::cout << "- Size: " << metrics.largest_component_size << std::endl;
    std::cout << "- Chromatic number: " << metrics.chromatic_number << std::endl;
    std::cout << "- Largest clique size: " << metrics.largest_clique_size << std::endl;
    if (!metrics.largest_clique_members.empty()) {
        std::cout << "- Largest clique countries:" << std::endl;
        for (const auto& label : metrics.largest_clique_members) {
            std::cout << "  * " << label << std::endl;
        }
    }

    std::cout << "\nEulerian Properties:" << std::endl;
    std::cout << "- Largest Eulerian subgraph size: "
              << metrics.largest_eulerian_subgraph.size << std::endl;
    if (!metrics.largest_eulerian_subgraph.node_labels.empty()) {
        std::cout << "- Countries in largest Eulerian subgraph:" << std::endl;
        for (const auto& label : metrics.largest_eulerian_subgraph.node_labels) {
            std::cout << "  * " << label << std::endl;
        }
    }

    std::cout << "\nHamiltonian Properties:" << std::endl;
    std::cout << "- Largest Hamiltonian subgraph size: "
              << metrics.largest_hamiltonian_subgraph.size << std::endl;
    if (!metrics.largest_hamiltonian_subgraph.node_labels.empty()) {
        std::cout << "- Countries in largest Hamiltonian subgraph:" << std::endl;
        for (const auto& label : metrics.largest_hamiltonian_subgraph.node_labels) {
            std::cout << "  * " << label << std::endl;
        }
    }

    std::cout << "\nDegree Information:" << std::endl;
    std::cout << "- Maximum degree: " << metrics.max_degree
              << " (Node: " << metrics.max_degree_node_label << ")" << std::endl;
    std::cout << "- Minimum degree: " << metrics.min_degree
              << " (Node: " << metrics.min_degree_node_label << ")" << std::endl;

    std::cout << "\nDistance Metrics:" << std::endl;
    std::cout << "- Radius (rad(G)): " << metrics.radius << std::endl;
    std::cout << "- Diameter (diam(G)): " << metrics.diameter << std::endl;

    std::cout << "\nCenter Vertices (center(G)):" << std::endl;
    for (const auto& label : metrics.center_labels) {
        std::cout << "- " << label << std::endl;
    }

    std::cout << "=========================" << std::endl;
}

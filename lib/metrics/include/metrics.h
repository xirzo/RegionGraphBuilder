#ifndef METRICS_H
#define METRICS_H

#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/Graph_d.h>

#include <vector>

class graph_metrics_calculator {
public:
    struct component_info {
        std::vector<ogdf::node> nodes;
        size_t size;
    };

    struct subgraph_info {
        std::vector<ogdf::node> nodes;
        std::vector<std::string> node_labels;
        size_t size;
        bool is_eulerian;
        bool is_hamiltonian;
    };

    struct metrics {
        int radius;
        int diameter;
        std::vector<ogdf::node> center;
        std::vector<std::string> center_labels;
        int max_degree;
        int min_degree;
        std::string max_degree_node_label;
        std::string min_degree_node_label;
        size_t num_vertices;
        size_t num_edges;
        int cyclomatic_number;
        int number_components;
        int chromatic_number;
        std::vector<ogdf::node> largest_clique;
        std::vector<std::string> largest_clique_members;
        size_t largest_clique_size;
        size_t largest_component_size;
        subgraph_info largest_eulerian_subgraph;
        subgraph_info largest_hamiltonian_subgraph;
    };

    static metrics calculate_metrics(const ogdf::Graph& G,
                                     const ogdf::GraphAttributes& GA);

    static void print_metrics(const metrics& metrics);

private:
    static int calculate_eccentricity(const ogdf::Graph& G, ogdf::node v);
    static std::vector<int> bfs(const ogdf::Graph& G, ogdf::node start);
    static std::vector<component_info> find_components(const ogdf::Graph& G);
    static void dfs_component(const ogdf::Graph& G, ogdf::node v,
                              std::vector<bool>& visited,
                              std::vector<ogdf::node>& component);
    static int calculate_chromatic_number(const ogdf::Graph& G,
                                          const std::vector<ogdf::node>& component);
    static std::vector<ogdf::node> find_largest_clique(
        const ogdf::Graph& G, const std::vector<ogdf::node>& component);
    static bool is_clique(const ogdf::Graph& G, const std::vector<ogdf::node>& nodes);
    static std::vector<ogdf::node> bron_kerbosch_with_pivot(const ogdf::Graph& G,
                                                            std::vector<ogdf::node>& R,
                                                            std::vector<ogdf::node>& P,
                                                            std::vector<ogdf::node>& X);
    static bool is_connected(const ogdf::Graph& G, const std::vector<ogdf::node>& nodes);
    static bool has_even_degrees(const ogdf::Graph& G,
                                 const std::vector<ogdf::node>& nodes);
    static bool is_eulerian(const ogdf::Graph& G, const std::vector<ogdf::node>& nodes);
    static bool is_hamiltonian(const ogdf::Graph& G,
                               const std::vector<ogdf::node>& nodes);
    static subgraph_info find_largest_eulerian_subgraph(
        const ogdf::Graph& G, const ogdf::GraphAttributes& GA,
        const std::vector<ogdf::node>& component);
    static subgraph_info find_largest_hamiltonian_subgraph(
        const ogdf::Graph& G, const ogdf::GraphAttributes& GA,
        const std::vector<ogdf::node>& component);
    static bool hamiltonian_cycle(const ogdf::Graph& G,
                                  const std::vector<ogdf::node>& nodes,
                                  std::vector<ogdf::node>& path,
                                  std::vector<bool>& visited, ogdf::node current,
                                  int count);
    static bool are_nodes_adjacent(const ogdf::Graph& G, ogdf::node u, ogdf::node v);
};

#endif  // !METRICS_H

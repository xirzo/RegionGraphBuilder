#include "metrics.h"

#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/decomposition/BCTree.h>
#include <ogdf/graphalg/ShortestPathAlgorithms.h>

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

void extract_component(ogdf::Graph& subgraph, const ogdf::Graph& graph,
                       const ogdf::NodeArray<int>& components, int component_id) {
    ogdf::NodeArray<ogdf::node> nodeMap(graph);

    for (ogdf::node v : graph.nodes) {
        if (components[v] == component_id) {
            nodeMap[v] = subgraph.newNode();
        }
    }

    for (ogdf::edge e : graph.edges) {
        ogdf::node source = e->source();
        ogdf::node target = e->target();

        if (components[source] == component_id && components[target] == component_id) {
            subgraph.newEdge(nodeMap[source], nodeMap[target]);
        }
    }
}

std::vector<int> find_graph_centers(const ogdf::Graph& graph) {
    std::vector<int> centers;

    int min_eccentricity = std::numeric_limits<int>::max();

    for (ogdf::node v : graph.nodes) {
        ogdf::NodeArray<int> distance(graph, -1);

        ogdf::bfs_SPSS(v, graph, distance, 1);

        int eccentricity = 0;

        for (ogdf::node u : graph.nodes) {
            if (distance[u] > eccentricity) {
                eccentricity = distance[u];
            }
        }

        if (eccentricity < min_eccentricity) {
            min_eccentricity = eccentricity;
            centers.clear();
            centers.push_back(v->index());
        } else if (eccentricity == min_eccentricity) {
            centers.push_back(v->index());
        }
    }

    return centers;
}

int calculate_diameter(const ogdf::Graph& G) {
    int diameter = 0;

    for (ogdf::node v : G.nodes) {
        ogdf::NodeArray<int> distance(G, -1);
        ogdf::bfs_SPSS(v, G, distance, 1);

        for (ogdf::node u : G.nodes) {
            if (distance[u] > diameter) {
                diameter = distance[u];
            }
        }
    }

    return diameter;
}

int find_max_clique(const ogdf::Graph& G) {
    int max_clique_size = 0;

    for (ogdf::node v : G.nodes) {
        std::vector<ogdf::node> neighbors;
        for (ogdf::adjEntry adj = v->firstAdj(); adj; adj = adj->succ()) {
            neighbors.push_back(adj->twinNode());
        }

        std::vector<ogdf::node> clique = {v};

        for (ogdf::node u : neighbors) {
            bool canAdd = true;

            for (ogdf::node c : clique) {
                if (c != u && !G.searchEdge(u, c)) {
                    canAdd = false;
                    break;
                }
            }

            if (canAdd) {
                clique.push_back(u);
            }
        }

        max_clique_size = std::max(max_clique_size, (int)clique.size());
    }

    return max_clique_size;
}

metrics calculate_metrics(ogdf::Graph graph, ogdf::GraphAttributes graph_attributes) {
    metrics m;

    m.number_of_vertices = graph.numberOfNodes();
    m.number_of_edges = graph.numberOfEdges();

    ogdf::NodeArray<int> component_map(graph);

    int number_of_component = ogdf::connectedComponents(graph, component_map);

    m.components = number_of_component;

    std::vector<int> component_sizes(number_of_component, 0);

    for (ogdf::node v : graph.nodes) {
        component_sizes[component_map[v]]++;
    }

    int largestCompIdx =
        std::distance(component_sizes.begin(),
                      std::max_element(component_sizes.begin(), component_sizes.end()));
    m.biggest_component = component_sizes[largestCompIdx];

    ogdf::Graph largest_component;

    extract_component(largest_component, graph, component_map, largestCompIdx);

    int maxDegree = 0;
    int minDegree = std::numeric_limits<int>::max();
    for (ogdf::node v : largest_component.nodes) {
        int degree = v->degree();
        maxDegree = std::max(maxDegree, degree);
        minDegree = std::min(minDegree, degree);
    }

    m.biggest_component_max_degree = maxDegree;
    m.biggest_component_min_degree = minDegree;

    m.biggest_component_chromatic_number = maxDegree + 1;

    m.biggest_component_diameter = calculate_diameter(largest_component);

    m.biggest_component_centers = find_graph_centers(largest_component);

    m.cyclomatic_numbre = m.number_of_edges - m.number_of_vertices + m.components;

    ogdf::BCTree bcTree(graph);
    m.blocks = bcTree.numberOfBComps();

    m.largest_clique = find_max_clique(graph);

    m.max_induced_eulerian_subgraph = 0;
    m.max_induced_hamiltonian_subgraph = 0;

    return m;
}

void print_metrics(metrics m) {
    std::cout << "Number of vertices: " << m.number_of_vertices << std::endl;
    std::cout << "Number of edges: " << m.number_of_edges << std::endl;
    std::cout << "Number of connected components: " << m.components << std::endl;
    std::cout << "Size of biggest component: " << m.biggest_component << std::endl;
    std::cout << "Biggest component - chromatic number estimate: "
              << m.biggest_component_chromatic_number << std::endl;
    std::cout << "Biggest component - max degree: " << m.biggest_component_max_degree
              << std::endl;
    std::cout << "Biggest component - min degree: " << m.biggest_component_min_degree
              << std::endl;
    std::cout << "Biggest component - diameter: " << m.biggest_component_diameter
              << std::endl;

    std::cout << "Biggest component - centers: ";

    for (size_t i = 0; i < m.biggest_component_centers.size(); ++i) {
        std::cout << m.biggest_component_centers[i];

        if (i < m.biggest_component_centers.size() - 1) {
            std::cout << ", ";
        }
    }

    std::cout << std::endl;

    std::cout << "Cyclomatic number: " << m.cyclomatic_numbre << std::endl;
    std::cout << "Size of largest clique: " << m.largest_clique << std::endl;
    std::cout << "Maximum induced Eulerian subgraph size: "
              << m.max_induced_eulerian_subgraph << std::endl;
    std::cout << "Maximum induced Hamiltonian subgraph size: "
              << m.max_induced_hamiltonian_subgraph << std::endl;
    std::cout << "Number of blocks (biconnected components): " << m.blocks << std::endl;
}

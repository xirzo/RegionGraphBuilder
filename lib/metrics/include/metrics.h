#ifndef METRICS_H
#define METRICS_H

#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/Graph_d.h>

#include <cstddef>
#include <vector>

typedef struct {
    std::size_t number_of_vertices;
    std::size_t number_of_edges;
    int components;
    int biggest_component;
    int biggest_component_chromatic_number;
    int biggest_component_max_degree;
    int biggest_component_min_degree;
    int biggest_component_diameter;
    std::vector<int> biggest_component_centers;
    int cyclomatic_number;
    int largest_clique;
    int max_induced_eulerian_subgraph;
    int max_induced_hamiltonian_subgraph;
    int blocks;

} metrics;

metrics calculate_metrics(ogdf::Graph, ogdf::GraphAttributes);
void print_metrics(metrics);

#endif  // !METRICS_H

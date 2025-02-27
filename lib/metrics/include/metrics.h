#ifndef METRICS_H
#define METRICS_H

#include <cstddef>

typedef struct {
    std::size_t number_of_vertices;
    std::size_t number_of_edges;
} metrics;

metrics calculate_metrics();
void print_metrics();

#endif  // !METRICS_H

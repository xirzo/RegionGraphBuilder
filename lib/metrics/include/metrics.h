#ifndef METRICS_H
#define METRICS_H

#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/Graph_d.h>

typedef struct metrics metrics;

metrics* calculate_metrics(ogdf::Graph, ogdf::GraphAttributes);
void print_metrics(metrics*, ogdf::GraphAttributes&);
void delete_metrics(metrics*);

#endif  // !METRICS_H

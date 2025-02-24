#ifndef METRICS_H
#define METRICS_H

#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/Graph_d.h>

#include <vector>

class graph_metrics {
public:
    struct metrics_result {
        int radius;
        int diameter;
        std::vector<ogdf::node> center;
        std::vector<std::string> centerLabels;
        int maxDegree;
        int minDegree;
        std::string maxDegreeNodeLabel;
        std::string minDegreeNodeLabel;
        size_t numVertices;
        size_t numEdges;
    };

    static metrics_result calculate_metrics(const ogdf::Graph& G,
                                            const ogdf::GraphAttributes& GA);

    static void printMetrics(const metrics_result& metrics);

private:
    static int calculateEccentricity(const ogdf::Graph& G, ogdf::node v);
    static std::vector<int> bfs(const ogdf::Graph& G, ogdf::node start);
};

#endif  // !METRICS_H

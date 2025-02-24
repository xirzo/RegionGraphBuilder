#ifndef METRICS_H
#define METRICS_H

#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/basic/Graph_d.h>

#include <vector>

class graph_metrics {
public:
    struct component_info {
        std::vector<ogdf::node> nodes;
        size_t size;
    };

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
        int cyclomaticNumber;
        int numComponents;
        int chromaticNumber;
        std::vector<ogdf::node> largestClique;
        std::vector<std::string> largestCliqueLabels;  // Added labels for clique nodes
        size_t largestCliqueSize;
        size_t largestComponentSize;
    };

    static metrics_result calculate_metrics(const ogdf::Graph& G,
                                            const ogdf::GraphAttributes& GA);

    static void printMetrics(const metrics_result& metrics);

private:
    static int calculateEccentricity(const ogdf::Graph& G, ogdf::node v);
    static std::vector<int> bfs(const ogdf::Graph& G, ogdf::node start);
    static std::vector<component_info> findComponents(const ogdf::Graph& G);
    static void dfsComponent(const ogdf::Graph& G, ogdf::node v,
                             std::vector<bool>& visited,
                             std::vector<ogdf::node>& component);
    static int calculateChromaticNumber(const ogdf::Graph& G,
                                        const std::vector<ogdf::node>& component);
    static std::vector<ogdf::node> findLargestClique(
        const ogdf::Graph& G, const std::vector<ogdf::node>& component);
    static bool isClique(const ogdf::Graph& G, const std::vector<ogdf::node>& nodes);
    static std::vector<ogdf::node> bronKerboschWithPivot(
        const ogdf::Graph& G, std::vector<ogdf::node>& R, std::vector<ogdf::node>& P,
        std::vector<ogdf::node>& X);  // ... (rest of the header remains the same)
};

#endif  // !METRICS_H

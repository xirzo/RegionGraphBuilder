#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <ogdf/fileformats/GraphIO.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/SugiyamaLayout.h>

using namespace ogdf;

int main() {
    // Create a new graph
    Graph G;
    GraphAttributes GA(G, GraphAttributes::nodeGraphics | GraphAttributes::edgeGraphics |
                          GraphAttributes::nodeLabel | GraphAttributes::edgeLabel);

    // Add some nodes
    node v1 = G.newNode();
    node v2 = G.newNode();
    node v3 = G.newNode();
    node v4 = G.newNode();

    // Set node labels
    GA.label(v1) = "Node 1";
    GA.label(v2) = "Node 2";
    GA.label(v3) = "Node 3";
    GA.label(v4) = "Node 4";

    // Add edges and set their labels
    edge e1 = G.newEdge(v1, v2);
    edge e2 = G.newEdge(v2, v3);
    edge e3 = G.newEdge(v3, v4);
    edge e4 = G.newEdge(v4, v1);

    // Set edge labels
    GA.label(e1) = "Edge 1";
    GA.label(e2) = "Edge 2";
    GA.label(e3) = "Edge 3";
    GA.label(e4) = "Edge 4";

    // Create a Sugiyama layout
    SugiyamaLayout SL;
    SL.setRanking(new OptimalRanking);
    SL.setCrossMin(new MedianHeuristic);
    SL.setLayout(new OptimalHierarchyLayout);

    // Apply the layout
    SL.call(GA);

    // Save the graph as SVG
    GraphIO::write(GA, "test_graph.svg", GraphIO::drawSVG);

    std::cout << "Graph has been created and saved as 'test_graph.svg'" << std::endl;
    return 0;
}

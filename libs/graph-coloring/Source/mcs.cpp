
#include "../Header/mcs.hpp"

#include <algorithm>
#include <iostream>
#include <queue>
#include <set>

using std::cerr;
using std::cout;
using std::endl;
using std::queue;

// TODO: The following optimization can be made via a note in the paper
// The procedure MCS can be implemented to run in O(|V | + |E|) time. To
// see that, notice that the first loop executes |V | iterations. In the
// second loop, for each vertex of G, all its neighbors are visited. After
// a vertex is evaluated, it is removed from the remaining graph. Therefore,
// the weight λ is increased exactly |E| times. By keeping vertices in an
// array of buckets indexed by λ, the vertex of highest weight can be found
// in O(1) time.

// Maximum Cardinal Search
map<string, int> GraphColoring::Mcs::color()
{

    map<string, vector<string>> temp_graph = graph;
    map<string, int>            weight;
    queue<string>               ordering;

    // Initially set the weight of each node to 0
    for (auto& i : temp_graph) { weight[i.first] = 0; }

    // Work through all the nodes in the graph, choosing the node
    // with maximum weight, then add that node to the queue. Increase
    // the weight of the queued nodes neighbors by 1. Continue until
    // every node in the graph has been added to the queue
    for (int i = 0; i < this->graph.size(); i++)
    {
        int    max_weight = -1;
        string max_vertex;

        // Out of the remaining nodes, find the node with the highest weight
        for (auto& j : temp_graph)
        {
            if (weight[j.first] > max_weight)
            {
                max_weight = weight[j.first];
                max_vertex = j.first;
            }
        }
        if (max_vertex.empty())
        {
            cerr << "Error: Could not find a max weight node in the graph (reason unknown)" << endl;
            this->graph_colors = map<string, int>();
            return this->graph_colors;
        }

        // Add highest weight node to the queue and increment all of its
        // neighbors weights by 1
        ordering.push(max_vertex);
        for (unsigned j = 0; j < graph[max_vertex].size(); j++) { weight[temp_graph[max_vertex][j]] += 1; }

        // Remove the maximum weight node from the graph so that it won't
        // be accidentally added again
        temp_graph.erase(max_vertex);
    }

    // Work through the queue in order and color each node
    while (!ordering.empty())
    {
        int color = 0;

        // Find the lowest possible graph_colors for this node between
        // its neighbors
        string min = ordering.front();

        // Thanks to Michael Kochte @ Universitaet Stuttgart for the below speedup snippit

        // Collect color numbers of neighbors
        std::set<int> colorset;
        for (auto& i : graph[min])
        {
            int col = graph_colors[i];
            colorset.insert(col);
        }

        // Sort and uniquify
        vector<int> colorvec;
        std::copy(colorset.begin(), colorset.end(), std::back_inserter(colorvec));
        std::sort(colorvec.begin(), colorvec.end());

        // Pick the lowest color not contained
        int newcolor = 0;
        for (int i : colorvec)
        {
            if (i == newcolor)
            {
                newcolor++;
            }
        }
        color = newcolor;

        this->graph_colors[min] = color;
        ordering.pop();
    }
    return this->graph_colors;
}


#include "../Header/coloring_algorithm.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ostringstream;

GraphColoring::GraphColor::GraphColor()
{
    this->graph        = map<string, vector<string>>();
    this->graph_colors = map<string, int>();
}

GraphColoring::GraphColor::GraphColor(const map<string, vector<string>>& graph)
{
    this->graph = graph;
    for (auto& itr : graph) { this->graph_colors[itr.first] = -1; }
}

GraphColoring::GraphColor::~GraphColor() = default;

// Checks that no two adjacent nodes have the same color
bool GraphColoring::GraphColor::is_valid()
{
    if (this->graph_colors.empty() || this->graph.size() != this->graph_colors.size())
    {
        return false;
    }
    for (auto& i : this->graph)
    {
        if (this->graph_colors.find(i.first) == this->graph_colors.end())
        {
            return false;
        }
        for (unsigned j = 0; j < i.second.size(); j++)
        {
            if (this->graph_colors[i.first] == this->graph_colors[i.second[j]] || this->graph_colors[i.first] == -1)
            {
                return false;
            }
        }
    }
    return true;
}

// Returns the color of an individual node, if that node isn't colored it returns -1
int GraphColoring::GraphColor::get_color(const string& node)
{
    if (this->graph_colors.find(node) != this->graph_colors.end())
    {
        return this->graph_colors.at(node);
    }
    return -1;
}

// Used to print the Chromatic Color
void GraphColoring::GraphColor::print_chromatic()
{
    cout << this->get_algorithm() << " Chromatic Number: " << this->get_num_colors() << endl;
}

// Used to print the color of each node in the graph
void GraphColoring::GraphColor::print_coloring()
{
    std::cout << "----------" << this->get_algorithm() << " Colorings----------" << endl;
    for (auto& graph_color : graph_colors) { std::cout << graph_color.first << " " << graph_color.second << endl; }
}

int GraphColoring::GraphColor::get_num_colors()
{
    int max_color = 0;
    for (auto& graph_color : this->graph_colors)
    {
        if (graph_color.second > max_color)
        {
            max_color = graph_color.second;
        }
    }
    return max_color + 1;
}

bool GraphColoring::GraphColor::is_colored()
{
    if (this->graph_colors.empty())
    {
        return false;
    }
    else
    {
        for (auto& graph_color : this->graph_colors)
        {
            if (graph_color.second == -1)
            {
                return false;
            }
        }
        return true;
    }
}

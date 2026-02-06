#ifndef _GRAPH_COLOR_HPP_
#define _GRAPH_COLOR_HPP_

#include <map>
#include <string>
#include <utility>
#include <vector>

using std::map;
using std::string;
using std::vector;

namespace GraphColoring
{
class GraphColor
{
  protected:
    /* Internal Members */
    map<string, vector<string>> graph;
    map<string, int>            graph_colors;

  public:
    /* Constructors */
    GraphColor();
    explicit GraphColor(const map<string, vector<string>>& graph);
    virtual ~GraphColor();

    /* Mutators */
    virtual map<string, int> color() = 0;

    void set_graph(map<string, vector<string>> new_graph)
    {
        this->graph = std::move(new_graph);
    }

    void modify_graph(const string& node, const vector<string>& neighbors)
    {
        this->graph[node] = neighbors;
    }

    /* Accessors */
    virtual string get_algorithm() = 0;

    std::size_t size()
    {
        return this->graph.size();
    }

    bool is_colored();

    map<string, int> get_coloring()
    {
        return this->graph_colors;
    }

    int  get_color(const string& node);
    int  get_num_colors();
    bool is_valid();

    /* Print functions */
    void print_coloring();
    void print_chromatic();
};
}  // namespace GraphColoring

#endif  // _GRAPH_COLOR_HPP_

#ifndef _GRAPH_COLOR_HPP_
#define _GRAPH_COLOR_HPP_

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

namespace GraphColoring {
    class GraphColor {
        protected:
            /* Internal Members */
            map<string,vector<string>> graph;
            map<string,int> graph_colors;

        public:
            /* Constructors */
            GraphColor();
            explicit GraphColor(map<string, vector<string>> graph);

            /* Mutators */
            virtual map<string,int> color() = 0;
            void set_graph(map<string,vector<string>> new_graph) { this->graph = new_graph; }
            void modify_graph(string node, vector<string> neighbors) { this->graph[node] = neighbors; }

            /* Accessors */
            virtual string get_algorithm() = 0;
            unsigned size() { return this->graph.size(); }
            bool is_colored();
            map<string,int> get_coloring() { return this->graph_colors; }
            int get_color(string node);
            int get_num_colors();
            bool is_valid();

            /* Print functions */
            void print_coloring();
            void print_chromatic();
    };
}

#endif // _GRAPH_COLOR_HPP_

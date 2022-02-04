
#include <fstream>
#include <iostream>
#include <sstream>

#include "../Header/coloring_algorithm.hpp"

using std::cout;
using std::cerr;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ostringstream;

GraphColoring::GraphColor::GraphColor() {
    this->graph = map<string,vector<string>>();
    this->graph_colors = map<string,int>();
}

GraphColoring::GraphColor::GraphColor(map<string,vector<string>> graph) {
    this->graph = graph;
    for(map<string,vector<string>>::iterator itr = graph.begin(); itr != graph.end(); itr++) {
        this->graph_colors[itr->first] = -1;
    }
}

// Checks that no two adjacent nodes have the same color
bool GraphColoring::GraphColor::is_valid() {
    if(this->graph_colors.size() == 0 || this->graph.size() != this->graph_colors.size()) {
        return false;
    }
    for(map<string,vector<string>>::iterator i = this->graph.begin(); i != this->graph.end(); i++) {
        if(this->graph_colors.find(i->first) == this->graph_colors.end()) {
            return false;
        }
        for(unsigned j = 0; j < i->second.size(); j++) {
            if(this->graph_colors[i->first] == this->graph_colors[i->second[j]] || this->graph_colors[i->first] == -1) {
                return false;
            }
        }
    }
    return true;
}

// Returns the color of an individual node, if that node isn't colored it returns -1
int GraphColoring::GraphColor::get_color(string node) {
    if(this->graph_colors.find(node) != this->graph_colors.end()) {
        return this->graph_colors.at(node);
    }
    return -1;
}

// Used to print the Chromatic Color
void GraphColoring::GraphColor::print_chromatic() {
    cout << this->get_algorithm() << " Chromatic Number: " << this->get_num_colors() << endl;
}

// Used to print the color of each node in the graph
void GraphColoring::GraphColor::print_coloring() {
    std::cout << "----------" << this->get_algorithm() << " Colorings----------" << endl;
    for(map< string,int >::iterator itr = graph_colors.begin(); itr != graph_colors.end(); itr++) {
        std::cout << itr->first << " " << itr->second << endl;
    }
}

int GraphColoring::GraphColor::get_num_colors() {
    int max_color = 0;
    for(map<string,int>::iterator color_it = this->graph_colors.begin(); color_it != this->graph_colors.end(); color_it++) {
        if(color_it->second > max_color) {
            max_color = color_it->second;
        }
    }
    return max_color + 1;
}

bool GraphColoring::GraphColor::is_colored() {
    if(this->graph_colors.size() < 1) {
        return false;
    } else {
        for(map<string,int>::iterator itr = this->graph_colors.begin(); itr != this->graph_colors.end(); itr++) {
            if(itr->second == -1) {
                return false;
            }
        }
        return true;
    }
}

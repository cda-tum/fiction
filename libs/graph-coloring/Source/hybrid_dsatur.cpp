
#include "../Header/hybrid_dsatur.hpp"

map<string,int> GraphColoring::HybridDsatur::color() {
    if(this->graph.size() == 0) {
        this->graph_colors = map<string,int>();
        return map<string,int>();
    }
    Dsatur* temp_graph = new Dsatur(this->graph);
    this->graph_colors = temp_graph->color();
    if(!temp_graph->is_valid()) {
        // TODO(brrcrites): We probably want to de-color the graph if
        // its partially colored and return that instead
        this->graph_colors = map<string,int>();
        return map<string,int>();
    }
    int largest = this->get_num_colors();

    Tabucol* tabu_graph = new Tabucol(this->graph, largest);

    map<string,int> best = this->graph_colors;
    map<string,int> tabu_color = tabu_graph->color();
    while(tabu_graph->is_valid()) {
        best = tabu_color;
        largest -= 1;
        tabu_graph->set_condition(largest);
        tabu_color = tabu_graph->color();
    }
    this->graph_colors = best;
    return this->graph_colors;
}

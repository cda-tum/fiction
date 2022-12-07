
#include "../Header/hybrid_lmxrlf.hpp"

map<string,int> GraphColoring::HybridLmxrlf::color() {
    if(this->graph.size() == 0) {
        this->graph_colors = map<string,int>();
        return map<string,int>();
    }
    Lmxrlf* lmxrlf_graph = new Lmxrlf(this->graph, this->condition);
    this->graph_colors = lmxrlf_graph->color();
    if(!lmxrlf_graph->is_valid()) {
        // TODO(brrcrites): consider we should probably reset the graph to uncolored
        // and then return that rather than an empty color map
        this->graph_colors = map<string,int>();
        return map<string,int>();
    }
    int largest = this->get_num_colors();

    Tabucol* tabu_graph = new Tabucol(this->graph, largest);

    map<string,int> best = this->graph_colors;
    map<string,int> tabu_color = tabu_graph->color();
    while(tabu_graph->is_valid()) {
        best = tabu_color;
        // TODO(brrcrites): in theory this can go to 0 or negative, we should probably
        // put in some corrective measure to stop that
        largest -= 1;
        tabu_graph->set_condition(largest);
        tabu_color = tabu_graph->color();
    }
    this->graph_colors = best;
    return this->graph_colors;
}


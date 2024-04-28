#include "../Header/hybrid_dsatur.hpp"

map<string, int> GraphColoring::HybridDsatur::color()
{
    if (this->graph.empty())
    {
        this->graph_colors = map<string, int>();

        return {};
    }

    const auto temp_graph = std::make_unique<Dsatur>(this->graph);

    this->graph_colors = temp_graph->color();

    if (!temp_graph->is_valid())
    {
        // TODO(brrcrites): We probably want to de-color the graph if
        // its partially colored and return that instead
        this->graph_colors = map<string, int>();
        return {};
    }

    int largest = this->get_num_colors();

    const auto tabu_graph = std::make_unique<Tabucol>(this->graph, largest);

    map<string, int> best       = this->graph_colors;
    map<string, int> tabu_color = tabu_graph->color();

    while (tabu_graph->is_valid())
    {
        best = tabu_color;
        largest -= 1;
        tabu_graph->set_condition(largest);
        tabu_color = tabu_graph->color();
    }

    this->graph_colors = best;
    return this->graph_colors;
}

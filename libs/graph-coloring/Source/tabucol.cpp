
#include "../Header/tabucol.hpp"

#include <cstdlib>
#include <iostream>
#include <queue>

using std::cerr;
using std::endl;
using std::queue;

using std::cout;

GraphColoring::Tabucol::Tabucol(const map<string, vector<string>>& g, int condition, int tabu_size, int rep,
                                int nbmax) :
        GraphColor(g)
{
    if (condition > this->graph.size())
    {
        this->condition = this->graph.size();
    }
    else
    {
        this->condition = condition;
    }
    this->tabu_size = tabu_size;
    this->rep       = rep;
    this->nbmax     = nbmax;
}

int GraphColoring::Tabucol::f(map<string, int> graph_colors)
{
    int sum = 0;
    for (auto& i : this->graph)
    {
        for (unsigned j = 0; j < i.second.size(); j++)
        {
            if (graph_colors[i.first] == graph_colors[i.second[j]])
            {
                sum += 1;
            }
        }
    }
    return sum;
}

map<string, int> GraphColoring::Tabucol::make_colors_contiguous(map<string, int> coloring)
{
    map<int, int> recoloring;
    int           current_color = 0;
    for (const auto& coloring_tuple : coloring)
    {
        if (recoloring.find(coloring_tuple.second) != recoloring.end())
        {
            coloring[coloring_tuple.first] = recoloring[coloring_tuple.second];
        }
        else
        {
            recoloring[coloring_tuple.second] = current_color;
            coloring[coloring_tuple.first]    = current_color;
            current_color++;
        }
    }
    return coloring;
}

map<string, int> GraphColoring::Tabucol::color()
{
    if (this->condition <= 0)
    {
        this->graph_colors = map<string, int>();
        return {};
    }
    if (this->graph.empty())
    {
        this->graph_colors = map<string, int>();
        return {};
    }
    for (auto& adj_tuple : this->graph) { this->graph_colors[adj_tuple.first] = rand() % this->condition; }

    srand(time(NULL));
    queue<int>    tabu_color;
    queue<string> tabu_vertex;
    for (int i = 0; i < this->tabu_size; i++)
    {
        auto x = this->graph.begin();
        std::advance(x, (rand() % this->graph.size()));
        tabu_vertex.push(x->first);
        tabu_color.push(rand() % this->condition);
    }

    int nbiter = 0;
    while (f(this->graph_colors) > 0 && nbiter < this->nbmax)
    {
        int    best_color = -1;
        string best_vertex;
        int    x          = 0;
        int    original_f = f(this->graph_colors);
        while (x < this->rep)
        {
            int    flag = 0;
            int    move_color;
            string move_vertex;
            while (!flag)
            {
                move_color = rand() % this->condition;
                auto mv    = this->graph.begin();
                std::advance(mv, (rand() % this->graph.size()));
                move_vertex    = mv->first;
                int inner_flag = 0;
                for (unsigned i = 0; i < tabu_vertex.size(); i++)
                {
                    const string temp_vertex = tabu_vertex.front();
                    int          temp_color  = tabu_color.front();
                    if (temp_vertex == move_vertex && temp_color == move_color)
                    {
                        flag = 1;
                    }
                    tabu_vertex.pop();
                    tabu_color.pop();
                    tabu_vertex.push(temp_vertex);
                    tabu_color.push(temp_color);
                }
                if (!inner_flag)
                {
                    flag = 1;
                }
            }
            if (best_color == -1)
            {
                best_color  = move_color;
                best_vertex = move_vertex;
            }
            map<string, int> colors_move = this->graph_colors;
            colors_move[move_vertex]     = move_color;
            map<string, int> colors_best = this->graph_colors;
            colors_best[best_vertex]     = best_color;
            if (f(colors_move) < f(colors_best))
            {
                best_vertex = move_vertex;
                best_color  = move_color;
            }
            x += 1;
            if (f(colors_move) < original_f)
            {
                x = this->rep;
            }
        }
        if (best_color < 0)
        {
            cerr << "Best Color was never updated in the loop" << endl;
            this->graph_colors = map<string, int>();
            return this->graph_colors;
        }
        tabu_color.pop();
        tabu_color.push(best_color);
        tabu_vertex.pop();
        tabu_vertex.push(best_vertex);
        this->graph_colors[best_vertex] = best_color;
        nbiter += 1;
    }

    this->graph_colors = make_colors_contiguous(this->graph_colors);

    // TODO(brrcrites): Why are we checking at the end if its valid or not? Shouldn't we be
    // able to tell during the computation if it isn't going to be valid?
    if (!this->is_valid())
    {
        this->graph_colors = map<string, int>();
        return graph_colors;
    }

    return graph_colors;
}

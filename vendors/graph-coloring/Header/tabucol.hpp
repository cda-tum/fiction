#ifndef _TABUCOL_HPP_
#define _TABUCOL_HPP_

#include "coloring_algorithm.hpp"

using GraphColoring::GraphColor;

namespace GraphColoring
{
class Tabucol : public GraphColor
{
  private:
    int condition;
    int tabu_size;
    int rep;
    int nbmax;

    int f(map<string, int> coloring);

  public:
    /* Constructors */
    explicit Tabucol(const map<string, vector<string>>& g, int condition = 0, int tabu_size = 25, int rep = 100,
                     int nbmax = 1000);

    /* Mutators */
    map<string, int> color() override;

    void set_coloring(const map<string, int>& coloring)
    {
        this->graph_colors = coloring;
    }
    void set_condition(const int con)
    {
        this->condition = con;
    }
    void set_tabu_size(const int size)
    {
        this->tabu_size = size;
    }
    void set_rep(const int r)
    {
        this->rep = r;
    }
    void set_nbmax(const int nb)
    {
        this->nbmax = nb;
    }
    map<string, int> make_colors_contiguous(map<string, int>);

    /* Accessors */
    string get_algorithm() override
    {
        return "TABUCOL";
    }
};
}  // namespace GraphColoring

#endif  //_TABUCOL_HPP_

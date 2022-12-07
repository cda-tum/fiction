#ifndef _DSATUR_HPP_
#define _DSATUR_HPP_

#include "coloring_algorithm.hpp"

#include <utility>

using GraphColoring::GraphColor;

namespace GraphColoring
{
class Dsatur : public GraphColor
{
  public:
    /* Constructors */
    explicit Dsatur(const map<string, vector<string>>& g) : GraphColor(g){};

    /* Mutators */
    map<string, int> color() override;

    /* Accessors */
    string get_algorithm() override
    {
        return "DSATUR";
    }
};
}  // namespace GraphColoring

#endif  //_DSATUR_HPP_

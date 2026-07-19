#ifndef _MCS_HPP_
#define _MCS_HPP_

#include "coloring_algorithm.hpp"

using GraphColoring::GraphColor;

namespace GraphColoring
{
class Mcs : public GraphColor
{
  public:
    /* Constructors */
    explicit Mcs(const map<string, vector<string>>& g) : GraphColor(g) {}

    /* Mutators */
    map<string, int> color() override;

    /* Accessors */
    string get_algorithm() override
    {
        return "MCS";
    }
};
}  // namespace GraphColoring

#endif  //_MCS_HPP_

#ifndef _HYBRID_DSATUR_HPP_
#define _HYBRID_DSATUR_HPP_

#include "coloring_algorithm.hpp"
#include "dsatur.hpp"
#include "tabucol.hpp"

#include <memory>
#include <utility>

using GraphColoring::Dsatur;
using GraphColoring::GraphColor;
using GraphColoring::Tabucol;

namespace GraphColoring
{
class HybridDsatur : public GraphColor
{
  public:
    /* Constructors */
    explicit HybridDsatur(const map<string, vector<string>>& g) : GraphColor(g) {}

    /* Mutators */
    map<string, int> color() override;

    /* Accessors */
    string get_algorithm() override
    {
        return "HYBRID_DSATUR";
    }
};
}  // namespace GraphColoring

#endif  //_HYBRID_DSATUR_HPP_

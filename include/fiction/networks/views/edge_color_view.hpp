//
// Created by marcel on 18.01.22.
//

#ifndef FICTION_EDGE_COLOR_VIEW_HPP
#define FICTION_EDGE_COLOR_VIEW_HPP

#include "fiction/utils/network_utils.hpp"

#include <mockturtle/views/color_view.hpp>

#include <cstdint>
#include <unordered_map>

namespace fiction
{
/**
 * A view that extends `mockturtle::out_of_place_color_view` such that it can color edges, i.e., tuples of nodes as
 * well.
 *
 * @tparam Ntk mockturtle network type.
 */
template <typename Ntk>
class out_of_place_edge_color_view : public mockturtle::out_of_place_color_view<Ntk>
{
  public:
    using storage     = typename Ntk::storage;
    using node        = typename Ntk::node;
    using signal      = typename Ntk::signal;
    using edge        = typename mockturtle::edge<out_of_place_edge_color_view<Ntk>>;
    using parent_view = typename mockturtle::out_of_place_color_view<Ntk>;

    /**
     * Standard constructor.
     *
     * @param ntk Network to color.
     */
    explicit out_of_place_edge_color_view(const Ntk& ntk) : parent_view(ntk)
    {
        static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");
    }
    /**
     * Remove all assigned colors.
     */
    void clear_colors() const
    {
        edge_colors.clear();
        parent_view::clear_colors(0);
    }
    /**
     * Returns the assigned color of a given edge.
     *
     * @param e Edge whose color is desired.
     * @return Color of edge `e`.
     */
    [[nodiscard]] uint32_t edge_color(const edge& e) const
    {
        if (auto it = edge_colors.find(e); it != edge_colors.cend())
        {
            return it->second;
        }

        return 0u;
    }
    /**
     * Paint a given edge with the current color.
     *
     * @param e Edge to paint.
     */
    void paint_edge(const edge& e) const
    {
        edge_colors[e] = parent_view::value;
    }
    /**
     * Paint a given edge with the given color.
     *
     * @param e Edge to paint.
     * @param clr Color to paint `e` with.
     */
    void paint_edge(const edge& e, uint32_t clr) const
    {
        edge_colors[e] = clr;
    }
    /**
     * Paint a given edge with the color of another given edge.
     *
     * @param e Edge to paint.
     * @param other Edge whose color is to be used to paint `e`.
     */
    void paint_edge(const edge& e, const edge& other) const
    {
        edge_colors[e] = edge_colors[other];
    }
    /**
     * Evaluates a predicate on a given edge's color.
     *
     * @tparam Pred Predicate type.
     * @param e Edge whose color is used as a parameter for `pred`.
     * @param pred Predicate to evaluate on `e`'s color.
     * @return Return value of `pred(color(e))`.
     */
    template <typename Pred>
    [[nodiscard]] bool eval_edge_color(const edge& e, Pred&& pred) const
    {
        return pred(edge_color(e));
    }
    /**
     * Evaluates a predicate on two given edges' colors.
     *
     * @tparam Pred Predicate type.
     * @param a Edge whose color is used as the first parameter for `pred`.
     * @param b Edge whose color is used as the second parameter for `pred`.
     * @param pred Predicate to evaluate on `a`'s and `b`'s colors.
     * @return Return value of `pred(color(a), color(b))`.
     */
    template <typename Pred>
    [[nodiscard]] bool eval_edge_color(const edge& a, const edge& b, Pred&& pred) const
    {
        return pred(edge_color(a), edge_color(b));
    }

    // make functions from parent_view accessible in this scope
    using parent_view::clear_colors;
    using parent_view::color;
    using parent_view::current_color;
    using parent_view::eval_color;
    using parent_view::new_color;
    using parent_view::paint;

  protected:
    /**
     * Out-of-place storage for edge colors.
     */
    mutable std::unordered_map<edge, uint32_t> edge_colors{};
};

}  // namespace fiction

#endif  // FICTION_EDGE_COLOR_VIEW_HPP

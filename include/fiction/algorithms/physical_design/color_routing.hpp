//
// Created by marcel on 02.02.22.
//

#ifndef FICTION_COLOR_ROUTING_HPP
#define FICTION_COLOR_ROUTING_HPP

#include "fiction/algorithms/graph/generate_edge_intersection_graph.hpp"
#include "fiction/algorithms/graph/graph_coloring.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <mockturtle/traits.hpp>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

namespace fiction
{

struct color_routing_params
{
    /**
     * Do not abort if some objectives cannot be fulfilled, but partially route the layout as much as possible.
     */
    bool conduct_partial_routing = false;

    determine_vertex_coloring_params dvc_ps{};
};

struct color_routing_stats
{};

namespace detail
{

template <typename Lyt>
class color_routing_impl
{
  public:
    color_routing_impl(Lyt& lyt, const std::vector<routing_objective<Lyt>>& obj, const color_routing_params& p,
                       color_routing_stats& st) :
            layout{lyt},
            objectives{obj},
            ps{p},
            pst{st}
    {}

    bool run()
    {
        generate_edge_intersection_graph_stats geig_st{};
        const auto edge_intersection_graph = generate_edge_intersection_graph(layout, objectives, {}, &geig_st);

        // if no partial routing is allowed, abort if some objectives cannot be satisfied by path enumeration
        if (!ps.conduct_partial_routing && geig_st.number_of_unsatisfiable_objectives > 0)
        {
            return false;
        }

        determine_vertex_coloring_stats dvc_st{};
        const auto vertex_coloring = determine_vertex_coloring(edge_intersection_graph, ps.dvc_ps, &dvc_st);

        const auto [color, frequency] = determine_most_frequent_color(vertex_coloring);

        // if no partial routing is allowed, abort if the coloring does not satisfy all objectives
        if (!ps.conduct_partial_routing && !is_complete_routing(frequency, geig_st))
        {
            return false;
        }

        conduct_routing(edge_intersection_graph, vertex_coloring, color);

        return true;
    }

  private:
    Lyt& layout;

    const std::vector<routing_objective<Lyt>> objectives;

    const color_routing_params ps;

    color_routing_stats& pst;

    template <typename Graph, typename Color>
    [[nodiscard]] std::pair<Color, std::size_t>
    determine_most_frequent_color(const vertex_coloring<Graph, Color>& coloring) const noexcept
    {
        std::map<Color, std::size_t> color_counter{};

        std::for_each(coloring.cbegin(), coloring.cend(),
                      [&color_counter](const auto& v_clr_pair)
                      {
                          const auto clr = v_clr_pair.second;
                          color_counter[clr]++;
                      });

        const auto max_clr_it = std::max_element(color_counter.cbegin(), color_counter.cend(),
                                                 [](const auto& p1, const auto& p2) { return p1.second < p2.second; });

        if (max_clr_it != color_counter.cend())
        {
            return *max_clr_it;
        }

        return {};
    }
    /**
     * Checks if a valid, i.e., complete, routing can be conducted given the vertex coloring of the edge intersection
     * graph. To this end, the full coloring is not needed to answer this question. Instead, only the frequency of the
     * most used color is necessary. This realization is based on the assumption that the edge intersection graph is a
     * collection of strongly connected components (SCCs) whose size is known. If a color occurs in each SCC, the
     * respectively associated paths form a complete routing. Since a color cannot appear twice within the same SCC by
     * the definition of vertex coloring, it is sufficient to check whether the given frequency is equal to the number
     * of all SCCs.
     *
     * @param frequency Frequency of most used color in the colored edge intersection graph.
     * @param geig_st Statistics determined from the call to generate_edge_intersection_graph that store the SCCs.
     * @return True iff the frequency nodes in the edge intersection graph that are painted with the same color form a
     * complete routing.
     */
    [[nodiscard]] bool is_complete_routing(const std::size_t                             frequency,
                                           const generate_edge_intersection_graph_stats& geig_st) const noexcept
    {
        return frequency == geig_st.strongly_connected_components.size();
    }

    template <typename Graph, typename Color>
    void conduct_routing(const Graph& graph, const vertex_coloring<Graph, Color>& coloring, const Color& color) noexcept
    {
        std::for_each(graph.begin_vertices(), graph.end_vertices(),
                      [this, &graph, &coloring, &color](const auto& v_path_pair)
                      {
                          const auto& [vertex, path] = v_path_pair;
                          if (coloring.at(vertex) == color)
                          {
                              route_path(layout, path);
                          }
                      });
    }
};

}  // namespace detail

/**
 *
 * @tparam Lyt
 * @param lyt
 * @param ps
 * @param pst
 * @return True iff routing was successful, i.e., iff all objectives could be satisfied.
 */
template <typename Lyt>
bool color_routing(Lyt& lyt, const std::vector<routing_objective<Lyt>>& objectives, color_routing_params ps = {},
                   color_routing_stats* pst = nullptr)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    color_routing_stats st{};

    detail::color_routing_impl<Lyt> p{lyt, objectives, ps, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_COLOR_ROUTING_HPP

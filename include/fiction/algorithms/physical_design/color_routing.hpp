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
    /**
     * The engine to use.
     */
    graph_coloring_engine engine = graph_coloring_engine::MCS;
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

        determine_vertex_coloring_params<::fiction::edge_intersection_graph<Lyt>> dvc_ps{ps.engine};
        if (const auto biggest_scc = std::max_element(geig_st.strongly_connected_components.cbegin(),
                                                      geig_st.strongly_connected_components.cend());
            biggest_scc != geig_st.strongly_connected_components.cend())
        {
            dvc_ps.strongly_connected_component = *biggest_scc;
        }

        determine_vertex_coloring_stats dvc_st{};
        const auto vertex_coloring = determine_vertex_coloring(edge_intersection_graph, dvc_ps, &dvc_st);

        // if no partial routing is allowed, abort if the coloring does not satisfy all objectives
        if (!ps.conduct_partial_routing && dvc_st.color_frequency != geig_st.strongly_connected_components.size())
        {
            return false;
        }

        conduct_routing(edge_intersection_graph, vertex_coloring, dvc_st.most_frequent_color);

        return true;
    }

  private:
    Lyt& layout;

    const std::vector<routing_objective<Lyt>> objectives;

    const color_routing_params ps;

    color_routing_stats& pst;

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

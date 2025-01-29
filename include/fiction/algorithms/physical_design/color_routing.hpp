//
// Created by marcel on 02.02.22.
//

#ifndef FICTION_COLOR_ROUTING_HPP
#define FICTION_COLOR_ROUTING_HPP

#include "fiction/algorithms/graph/generate_edge_intersection_graph.hpp"
#include "fiction/algorithms/graph/graph_coloring.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <vector>

namespace fiction
{

/**
 * Parameters for the color routing algorithm.
 */
struct color_routing_params
{
    /**
     * Do not abort if some objectives cannot be fulfilled, but partially route the layout as much as possible.
     */
    bool conduct_partial_routing = false;
    /**
     * Enable crossings.
     */
    bool crossings = false;
    /**
     * If a value is given, for each objective, only up to the `path_limit` shortest paths will be enumerated (using
     * Yen's algorithm) instead of all paths.
     */
    std::optional<uint32_t> path_limit = std::nullopt;
    /**
     * The engine to use.
     */
    graph_coloring_engine engine = graph_coloring_engine::SAT;
    /**
     * Allow partial solutions when the SAT engine is used.
     */
    bool partial_sat = false;
};

struct color_routing_stats
{
    /**
     * Runtime measurement.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * For each routing objective that was not fulfilled by the coloring engine, this counter is incremented.
     */
    std::size_t number_of_unsatisfied_objectives{0};
    /**
     * Statistics of the edge intersection graph generation.
     */
    generate_edge_intersection_graph_stats epg_stats{};
    /**
     * Statistics of the vertex coloring.
     */
    determine_vertex_coloring_stats<> color_stats{};
};

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
        // measure runtime
        mockturtle::stopwatch stop{pst.time_total};

        generate_edge_intersection_graph_params epg_params{};
        epg_params.crossings  = ps.crossings;
        epg_params.path_limit = ps.path_limit;

        const auto edge_intersection_graph =
            generate_edge_intersection_graph(layout, objectives, epg_params, &pst.epg_stats);

        // if no partial routing is allowed, abort if some objectives cannot be satisfied by path enumeration
        if (!ps.conduct_partial_routing && pst.epg_stats.number_of_unroutable_objectives > 0)
        {
            return false;
        }

        determine_vertex_coloring_params<::fiction::edge_intersection_graph<Lyt>> dvc_ps{};
        dvc_ps.engine                                 = ps.engine;
        dvc_ps.sat_params.cliques                     = pst.epg_stats.cliques;
        dvc_ps.sat_params.clique_size_color_frequency = !ps.partial_sat;
        dvc_ps.sat_params.sat_search_tactic           = graph_coloring_sat_search_tactic::LINEARLY_ASCENDING;
        dvc_ps.sat_params.sat_engine                  = bill::solvers::glucose_41;

        const auto vertex_coloring = determine_vertex_coloring(edge_intersection_graph, dvc_ps, &pst.color_stats);

        // if no partial routing is allowed, abort if the coloring does not satisfy all objectives
        if (!ps.conduct_partial_routing && pst.color_stats.color_frequency != pst.epg_stats.cliques.size())
        {
            return false;
        }

        conduct_routing(edge_intersection_graph, vertex_coloring, pst.color_stats.most_frequent_color);

        return true;
    }

  private:
    /**
     * The layout to route.
     */
    Lyt& layout;
    /**
     * The routing objectives.
     */
    const std::vector<routing_objective<Lyt>> objectives;
    /**
     * Parameters.
     */
    const color_routing_params ps;
    /**
     * Statistics.
     */
    color_routing_stats& pst;

    /**
     * Extracts the routing paths of the given color from the vertex coloring of the generated edge intersection graph
     * and applies them to the stored layout.
     *
     * This function logs the number of unsatisfied objectives in the statistics.
     *
     * @tparam Graph The type of the edge intersection graph.
     * @tparam Color The type of the vertex colors.
     * @param graph The edge intersection graph.
     * @param coloring The vertex coloring.
     * @param color The color to extract and route.
     */
    template <typename Graph, typename Color>
    void conduct_routing(const Graph& graph, const vertex_coloring<Graph, Color>& coloring, const Color& color) noexcept
    {
        std::size_t num_satisfied_objectives{0};

        std::for_each(graph.begin_vertices(), graph.end_vertices(),
                      [this, &coloring, &color, &num_satisfied_objectives](const auto& v_path_pair)
                      {
                          const auto& [vertex, path] = v_path_pair;
                          if (coloring.at(vertex) == color)
                          {
                              route_path(layout, path);
                              ++num_satisfied_objectives;
                          }
                      });

        // log the number of unsatisfied objectives
        pst.number_of_unsatisfied_objectives = objectives.size() - num_satisfied_objectives;
    }
};

}  // namespace detail

/**
 * A multi-path signal routing approach based on coloring of edge intersection graphs as originally proposed in
 * \"Efficient Multi-Path Signal Routing for Field-coupled Nanotechnologies\" by M. Walter and R. Wille in NANOARCH
 * 2022.
 *
 * Given a gate-level layout and a set of routing objectives, this algorithm tries to fulfill all objectives by routing
 * several conflict-free wire paths. To this end, a plethora of possible paths are enumerated in the given layout and
 * an edge-intersection graph of paths on a grid (EPG) created from them. In an EPG, each vertex represents a path and
 * each edge conflicts between them. When two vertices are connected by an edge, they cannot be routed together
 * conflict-free in the layout. To determine a (maximum) set of routable paths, a vertex coloring is computed on the
 * EPG. Finally, all vertices that are colored identically can be routed together. The biggest such set is applied to
 * the layout.
 *
 * Multiple parameters can be set to specify the behavior of the algorithm. For instance, whether crossings should be
 * enabled and whether a partial routing should be conducted if not all objectives could be fulfilled. Furthermore, the
 * path enumeration and the coloring can be parameterized in the first place. By default, all paths are enumerated for
 * each objective. While this guarantees completeness on small layouts, it quickly becomes intractable. Therefore, a
 * path limit can be set that restricts the number of paths to the \f$k\f$ shortest. Additionally, for the coloring
 * process, SAT solving is used by default, which, again, guarantees completeness, but becomes infeasible rather
 * quickly. However, powerful symmetry breaking is applied that assists the solving process, e.g., it is known that all
 * vertices that are belonging to the same objective are forming a clique, which can be pre-colored. Additionally,
 * lexicographical orderings are enforced. For a more scalable (yet incomplete) approach, several coloring heuristics
 * are available, from which can be chosen (see determine_vertex_coloring).
 *
 * This function will return `true` if all objectives could be satisfied or if the partial routing parameter was set. In
 * the case of `true` being returned, all determined paths have been routed in the given layout.
 *
 * @tparam Lyt The gate-level layout type to route.
 * @param lyt A gate-level layout to route.
 * @param objectives The routing objectives as source-target pairs to fulfill.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return `true` iff routing was successful, i.e., iff all objectives could be satisfied.
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

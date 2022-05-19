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
#include <mockturtle/utils/stopwatch.hpp>

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
     * Enable crossings.
     */
    bool crossings = false;
    //    /**
    //     * For routing objectives that are more than distance_threshold units apart measured by the Manhattan
    //     distance, only
    //     * the shortest path is determined and included in the edge intersection graph via the A* algorithm.
    //     */
    //    uint32_t distance_threshold = 10;
    /**
     * If a value is given, for each objective, only up to the path_limit shortest paths will be enumerated (using Yen's
     * algorithm) instead of all paths.
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
        std::cout << fmt::format("Numer of routing objectives: {}", objectives.size()) << std::endl;

        // measure runtime
        mockturtle::stopwatch stop{pst.time_total};

        generate_edge_intersection_graph_params epg_params{};
        epg_params.crossings  = ps.crossings;
        epg_params.path_limit = ps.path_limit;

        const auto edge_intersection_graph =
            generate_edge_intersection_graph(layout, objectives, epg_params, &pst.epg_stats);

        std::cout << fmt::format("Generated EPG with {} vertices and {} edges in {} cliques",
                                 pst.epg_stats.num_vertices, pst.epg_stats.num_edges, pst.epg_stats.cliques.size())
                  << std::endl;

        // if no partial routing is allowed, abort if some objectives cannot be satisfied by path enumeration
        if (!ps.conduct_partial_routing && pst.epg_stats.number_of_unroutable_objectives > 0)
        {
            return false;
        }

        determine_vertex_coloring_params<::fiction::edge_intersection_graph<Lyt>> dvc_ps{};
        dvc_ps.engine                                 = ps.engine;
        dvc_ps.sat_params.cliques                     = pst.epg_stats.cliques;
        dvc_ps.sat_params.clique_size_color_frequency = !ps.partial_sat;
        dvc_ps.sat_params.sat_search_tactic           = graph_coloring_sat_search_tactic::BINARY_SEARCH;

        std::cout << "Started coloring..." << std::endl;

        const auto vertex_coloring = determine_vertex_coloring(edge_intersection_graph, dvc_ps, &pst.color_stats);

        std::cout << "... done!" << std::endl;

        // if no partial routing is allowed, abort if the coloring does not satisfy all objectives
        if (!ps.conduct_partial_routing && pst.color_stats.color_frequency != pst.epg_stats.cliques.size())
        {
            return false;
        }

        conduct_routing(edge_intersection_graph, vertex_coloring, pst.color_stats.most_frequent_color);

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
        std::size_t num_satisfied_objectives{0};

        std::for_each(graph.begin_vertices(), graph.end_vertices(),
                      [this, &graph, &coloring, &color, &num_satisfied_objectives](const auto& v_path_pair)
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

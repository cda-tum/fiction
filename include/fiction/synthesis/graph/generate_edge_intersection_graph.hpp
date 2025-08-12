//
// Created by marcel on 02.02.22.
//

#ifndef FICTION_GENERATE_EDGE_INTERSECTION_GRAPH_HPP
#define FICTION_GENERATE_EDGE_INTERSECTION_GRAPH_HPP

#include "fiction/algorithms/path_finding/enumerate_all_paths.hpp"
#include "fiction/algorithms/path_finding/k_shortest_paths.hpp"
#include "fiction/layouts/obstruction_layout.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"
#include "fiction/utils/stl_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>
#include <phmap.h>

#include <algorithm>
#include <cstdint>
#include <optional>
#include <vector>

#include <combinations.h>
#include <undirected_graph.h>
#include <undirected_pair.h>

namespace fiction
{

/**
 * Define an edge_intersection_graph as an undirected graph of layout_coordinate_path elements assigned to its nodes.
 * Each node (vertex) is identified via a unique ID whereas each edge is an undirected pair of such IDs. Since the
 * implementation demands it, each edge, additionally, is associated with a data field that represents its ID.
 */
template <typename Lyt>
using edge_intersection_graph =
    undirected_graph<std::size_t, layout_coordinate_path<Lyt>, undirected_pair<std::size_t>, std::size_t>;

/**
 * Parameters for the edge intersection graph generation algorithm.
 */
struct generate_edge_intersection_graph_params
{
    /**
     * Allow crossings by not creating edges between paths that only share single-coordinate sub-paths.
     */
    bool crossings = false;
    /**
     * If a value is given, for each objective, only up to the `path_limit` shortest paths will be enumerated (using
     * Yen's algorithm) instead of all paths.
     */
    std::optional<uint32_t> path_limit = std::nullopt;
};

struct generate_edge_intersection_graph_stats
{
    /**
     * Runtime measurement.
     */
    mockturtle::stopwatch<>::duration time_total{0};
    /**
     * For each routing objective that cannot be fulfilled in the given layout, this counter is incremented.
     */
    std::size_t number_of_unroutable_objectives{0};
    /**
     * Stores all cliques in the resulting graph that were created during path enumeration. There might be more cliques
     * in the overall graph but these ones correspond to one routing objective each, which could be useful information
     * to have in certain algorithms.
     */
    std::vector<std::vector<std::size_t>> cliques{};
    /**
     * Stores the size of the generated edge intersection graph.
     */
    std::size_t num_vertices, num_edges;
};

namespace detail
{

template <typename Lyt>
class generate_edge_intersection_graph_impl
{
  public:
    generate_edge_intersection_graph_impl(const Lyt& lyt, const std::vector<routing_objective<Lyt>>& obj,
                                          const generate_edge_intersection_graph_params& p,
                                          generate_edge_intersection_graph_stats&        st) :
            layout{lyt},
            objectives{obj},
            ps{p},
            pst{st}
    {}

    edge_intersection_graph<Lyt> run()
    {
        // measure runtime
        mockturtle::stopwatch stop{pst.time_total};

        std::for_each(objectives.cbegin(), objectives.cend(),
                      [this](const auto& obj)
                      {
                          path_collection<clk_path> obj_paths{};

                          if (!ps.path_limit.has_value())
                          {
                              // enumerate all paths for the current objective
                              obj_paths = enumerate_all_paths<clk_path>(obstruction_layout{layout},
                                                                        {obj.source, obj.target}, {ps.crossings});
                          }
                          else
                          {
                              // enumerate k paths for the current objective
                              obj_paths = yen_k_shortest_paths<clk_path>(
                                  obstruction_layout{layout}, {obj.source, obj.target}, *ps.path_limit, {ps.crossings});
                          }

                          // assign a unique label to each path and create a corresponding node in the graph
                          initiate_objective_nodes(obj_paths);

                          // if there are no paths, the objective could not be fulfilled
                          if (obj_paths.empty())
                          {
                              pst.number_of_unroutable_objectives++;
                          }
                          else if (obj_paths.size() > 1)
                          {
                              // since all paths of the same objective have intersections by definition, create
                              // edges between all of them by iterating over all possible combinations of size 2
                              connect_clique(obj_paths);
                          }
                          // for each previously stored path, create an edge if there is an intersection
                          create_intersection_edges(obj_paths);

                          // add the collection to all paths gathered thus far
                          all_paths.insert(all_paths.end(), obj_paths.cbegin(), obj_paths.cend());
                      });

        // store size of the generated graph
        pst.num_vertices = graph.size_vertices();
        pst.num_edges    = graph.size_edges();

        return graph;
    }

  private:
    /**
     * Reference to the layout.
     */
    const Lyt& layout;
    /**
     * The routing objectives.
     */
    const std::vector<routing_objective<Lyt>> objectives;
    /**
     * Parameters.
     */
    const generate_edge_intersection_graph_params ps;
    /**
     * Statistics.
     */
    generate_edge_intersection_graph_stats& pst;
    /**
     * The edge intersection graph to be created.
     */
    edge_intersection_graph<Lyt> graph{};
    /**
     * IDs for nodes and edges.
     */
    std::size_t node_id{0}, edge_id{0};
    /**
     * Extends the layout_coordinate_path to additionally to the vector representation of the path also hold a set that
     * allows fast lookup needed to find intersections (O(log n)). Additionally, a label is assigned to each path to
     * identify it in the edge intersection graph.
     */
    class labeled_layout_coordinate_lookup_path : public layout_coordinate_path<Lyt>
    {
      public:
        /**
         * Overwrites the append function to additionally store the given coordinate in a set.
         *
         * @param c Coordinate to append to the path.
         */
        void append(const coordinate<Lyt>& c) noexcept
        {
            path_elements.insert(c);
            layout_coordinate_path<Lyt>::append(c);
        }
        /**
         * Given another path, this function checks if they are not disjoint, i.e., it looks for at least one coordinate
         * that both paths share.
         *
         * If, at some point, the set approach is not to be used anymore, std::find_first_of offers the same
         * functionality on any kind of range.
         *
         * @tparam Path Type of other path.
         * @param other The other path.
         * @return `true` iff this path and the given one are not disjoint, i.e., share at least one coordinate.
         */
        template <typename Path>
        bool has_intersection_with(const Path& other) const noexcept
        {
            // if source and target are identical, an intersection was found
            if (this->source() == other.source() && this->target() == other.target())
            {
                return true;
            }

            // else, check if any of the remaining coordinates occur in the stored path
            return std::any_of(std::cbegin(other) + 1, std::cend(other) - 1,
                               [this](const auto& c) { return path_elements.count(c) > 0; });
        }
        /**
         * Like has_intersection_with but allows paths to share crossings, i.e., single-tile intersections.
         *
         * Similar to has_intersection_with, this function also returns `true` if source and target are matching in both
         * paths.
         *
         * @tparam Path Type of other path.
         * @param other The other path.
         * @return `true` iff this path and the given one are overlapping, i.e., share at least one coordinate segment
         * of size 2.
         */
        template <typename Path>
        bool has_overlap_with(const Path& other) const noexcept
        {
            // if source and target are identical, an intersection was found
            if (this->source() == other.source() && this->target() == other.target())
            {
                return true;
            }

            // else, check if any of the coordinates (including I/Os) form a shared segment in the stored path
            return find_first_two_of(std::cbegin(other), std::cend(other), std::cbegin(*this), std::cend(*this)) !=
                   std::cend(other);
        }
        /**
         * Label to identify the path in the edge intersection graph.
         */
        std::size_t label{};

      protected:
        using base = layout_coordinate_path<Lyt>;

      public:
        // make all inherited constructors available
        using base::base;

      private:
        /**
         * Uniquely identify path elements in a set to make them searchable in O(1).
         */
        phmap::flat_hash_set<coordinate<Lyt>> path_elements{};
    };
    /**
     * Alias for the path type.
     */
    using clk_path = labeled_layout_coordinate_lookup_path;
    /**
     * Stores a collection of all annotated paths (labeled_layout_coordinate_lookup_path objects) computed thus far to
     * find intersections with new ones. The edge intersection graph stores plain paths without the extra set and label.
     * Therefore, after the generate_edge_intersection_graph function terminates, the extra memory overhead is being
     * released again.
     */
    path_collection<clk_path> all_paths{};
    /**
     * Given a collection of paths belonging to the same objective, this function assigns them unique labels and
     * generates corresponding nodes in the edge intersection graph.
     *
     * Since each path of the same objective objective belongs to a clique in the final graph, this function
     * additionally stores their node IDs in the statistics.
     *
     * @param objective_paths Collection of paths belonging to the same objective.
     */
    void initiate_objective_nodes(path_collection<clk_path>& objective_paths) noexcept
    {
        std::vector<std::size_t> clique{};

        std::for_each(objective_paths.begin(), objective_paths.end(),
                      [this, &clique](auto& p)
                      {
                          p.label = node_id++;
                          graph.insert_vertex(p.label, p);
                          clique.push_back(p.label);
                      });

        if (!clique.empty())
        {
            pst.cliques.push_back(clique);
        }
    }
    /**
     * Given a collection of paths belonging to the same objective, this function creates edges in the edge intersection
     * graph between each pair of corresponding nodes, thus, forming a clique (complete sub-graph).
     *
     * @param objective_paths Collection of paths belonging to the same objective.
     */
    void connect_clique(path_collection<clk_path>& objective_paths) noexcept
    {
        combinations::for_each_combination(objective_paths.begin(), objective_paths.begin() + 2, objective_paths.end(),
                                           [this](const auto begin, [[maybe_unused]] const auto end)
                                           {
                                               graph.insert_edge(begin->label, (begin + 1)->label, edge_id++);

                                               return false;  // keep looping
                                           });
    }
    /**
     * Given a collection of paths belonging to the same objective, this function creates edges in the edge intersection
     * graph between each corresponding node and all of the already existing nodes that represent paths that intersect
     * with it, i.e., that share at least one coordinate.
     *
     * @param objective_paths Collection of paths belonging to the same objective.
     */
    void create_intersection_edges(path_collection<clk_path>& objective_paths) noexcept
    {
        std::for_each(objective_paths.cbegin(), objective_paths.cend(),
                      [this](const auto& obj_p)
                      {
                          std::for_each(all_paths.cbegin(), all_paths.cend(),
                                        [this, &obj_p](const auto& stored_p)
                                        {
                                            if (ps.crossings ? obj_p.has_overlap_with(stored_p) :
                                                               obj_p.has_intersection_with(stored_p))
                                            {
                                                graph.insert_edge(obj_p.label, stored_p.label, edge_id++);
                                            }
                                        });
                      });
    }
};

}  // namespace detail

/**
 * Creates an edge intersection graph of all paths that satisfy a given list of routing objectives. That is, this
 * function generates an undirected graph whose nodes represent paths in the given layout and whose edges represent
 * intersections of these paths. An intersection is understood as the non-disjunction of paths, i.e., they share at
 * least one coordinate. To generate the paths for the routing objectives, all possible paths from source to target in
 * the layout are enumerated while taking obstructions into consideration. The given layout must be clocked.
 *
 * @tparam Lyt Type of the clocked layout.
 * @param lyt The layout to generate the edge intersection graph for.
 * @param objectives A list of routing objectives given as source-target pairs.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return An edge intersection graph of paths satisfying the given routing objectives in `lyt`.
 */
template <typename Lyt>
edge_intersection_graph<Lyt> generate_edge_intersection_graph(const Lyt&                                 lyt,
                                                              const std::vector<routing_objective<Lyt>>& objectives,
                                                              generate_edge_intersection_graph_params    ps  = {},
                                                              generate_edge_intersection_graph_stats*    pst = nullptr)
{
    static_assert(is_clocked_layout_v<Lyt>, "Lyt is not a clocked layout");

    generate_edge_intersection_graph_stats st{};

    detail::generate_edge_intersection_graph_impl<Lyt> p{lyt, objectives, ps, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_GENERATE_EDGE_INTERSECTION_GRAPH_HPP

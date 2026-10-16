/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Builds the edge intersection graph of all paths satisfying given routing objectives.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/gate_layout.hpp"
#include "fiction/physical_design/path_finding/enumerate_all_paths.hpp"
#include "fiction/physical_design/path_finding/k_shortest_paths.hpp"
#include "fiction/physical_design/routing_utils.hpp"
#include "fiction/utils/stl/stl_utils.hpp"

#include <mockturtle/utils/stopwatch.hpp>

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

#include <combinations.h>
#include <undirected_graph.h>
#include <undirected_pair.h>

namespace fiction::physical_design
{

/**
 * Define an edge_intersection_graph as an undirected graph of layout_coordinate_path elements assigned to its nodes.
 * Each node (vertex) is identified via a unique ID whereas each edge is an undirected pair of such IDs. Since the
 * implementation demands it, each edge, additionally, is associated with a data field that represents its ID.
 */
using edge_intersection_graph =
    undirected_graph<std::size_t, layout_coordinate_path, undirected_pair<std::size_t>, std::size_t>;

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
     * @brief Routing objective index for each vertex ID.
     */
    std::vector<std::size_t> objective_indices{};
    /**
     * Stores the size of the generated edge intersection graph.
     */
    std::size_t num_vertices, num_edges;
};

namespace detail
{

/**
 * @brief Construct candidate paths and their intersection graph.
 * @tparam Lyt Gate layout type.
 */
template <typename Lyt>
class generate_edge_intersection_graph_impl
{
  public:
    /**
     * @brief Set the layout, objectives, parameters, and statistics.
     * @param lyt Gate layout.
     * @param obj Routing objectives.
     * @param p Parameters.
     * @param st Statistics.
     */
    generate_edge_intersection_graph_impl(const Lyt& lyt, const std::vector<routing_objective>& obj,
                                          const generate_edge_intersection_graph_params& p,
                                          generate_edge_intersection_graph_stats&        st) :
            layout{lyt},
            objectives{obj},
            ps{p},
            pst{st}
    {}

    /**
     * @brief Builds and transfers the completed intersection graph.
     * @return Graph of candidate paths and their intersections.
     */
    edge_intersection_graph run()
    {
        // measure runtime
        mockturtle::stopwatch stop{pst.time_total};

        std::ranges::for_each(
            objectives,
            [this](const auto& obj)
            {
                /** @brief Candidate coordinate paths for the current routing objective. */
                path_collection paths{};

                if (!ps.path_limit.has_value())
                {
                    // enumerate all paths for the current objective
                    paths = physical_design::path_finding::enumerate_all_paths(layout, {obj.source, obj.target},
                                                                               {ps.crossings});
                }
                else
                {
                    // enumerate k paths for the current objective
                    paths = physical_design::path_finding::yen_k_shortest_paths(layout, {obj.source, obj.target},
                                                                                *ps.path_limit, {ps.crossings});
                }

                /** @brief Candidate paths with storage for graph vertex labels. */
                std::vector<clk_path> obj_paths{std::make_move_iterator(paths.begin()),
                                                std::make_move_iterator(paths.end())};

                // assign a unique label to each path and create a corresponding node in the graph
                initiate_objective_nodes(obj_paths, static_cast<std::size_t>(&obj - objectives.data()));

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
                all_paths.insert(all_paths.end(), std::make_move_iterator(obj_paths.begin()),
                                 std::make_move_iterator(obj_paths.end()));
            });

        // store size of the generated graph
        pst.num_vertices = graph.size_vertices();
        pst.num_edges    = graph.size_edges();

        // The one-shot implementation transfers its completed graph.
        return std::move(graph);
    }

  private:
    /**
     * Reference to the layout.
     */
    const Lyt& layout;
    /**
     * The routing objectives.
     */
    const std::vector<routing_objective> objectives;
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
    edge_intersection_graph graph{};
    /**
     * IDs for nodes and edges.
     */
    std::size_t node_id{0}, edge_id{0};
    /** @brief A coordinate path with its graph vertex label. */
    class labeled_layout_coordinate_path : public layout_coordinate_path
    {
      public:
        /**
         * @brief Check whether this path shares an interior coordinate of the other path.
         *
         * @param other The other path.
         * @return `true` iff this path contains an interior coordinate of the other path.
         */
        [[nodiscard]] bool has_intersection_with(const layout_coordinate_path& other) const noexcept
        {
            if (other.size() <= 2)
            {
                return false;
            }
            // Pairwise scanning costs O(|p||q|); long-route workloads can use a query-local coordinate index.
            return std::ranges::find_first_of(std::cbegin(other) + 1, std::cend(other) - 1, std::cbegin(*this),
                                              std::cend(*this)) != std::cend(other) - 1;
        }
        /**
         * @brief Check for a shared segment, allowing single-tile intersections as crossings.
         *
         * @param other The other path.
         * @return `true` iff this path and the given one are overlapping, i.e., share at least one coordinate segment
         * of size 2.
         */
        [[nodiscard]] bool has_overlap_with(const layout_coordinate_path& other) const noexcept
        {
            // Include endpoints when checking for a shared segment.
            return fiction::utils::stl::find_first_two_of(std::cbegin(other), std::cend(other), std::cbegin(*this),
                                                          std::cend(*this)) != std::cend(other);
        }
        /**
         * Label to identify the path in the edge intersection graph.
         */
        std::size_t label{};

        /**
         * @brief Takes ownership of a coordinate path.
         * @param path Path to label.
         */
        explicit labeled_layout_coordinate_path(layout_coordinate_path path) : layout_coordinate_path{std::move(path)}
        {}
    };
    /**
     * Alias for the path type.
     */
    using clk_path = labeled_layout_coordinate_path;
    /**
     * Stores labeled paths to find intersections with each new objective. The graph stores plain paths.
     */
    std::vector<clk_path> all_paths{};
    /**
     * Given a collection of paths belonging to the same objective, this function assigns them unique labels and
     * generates corresponding nodes in the edge intersection graph.
     *
     * Since each path of the same objective belongs to a clique in the final graph, this function
     * additionally stores their node IDs in the statistics.
     *
     * @param objective_paths Collection of paths belonging to the same objective.
     * @param objective_index Index of the routing objective.
     * @throws std::bad_alloc If storage allocation fails.
     */
    void initiate_objective_nodes(std::vector<clk_path>& objective_paths, const std::size_t objective_index)
    {
        std::vector<std::size_t> clique{};

        std::ranges::for_each(objective_paths,
                              [this, &clique, objective_index](auto& p)
                              {
                                  p.label = node_id++;
                                  graph.insert_vertex(p.label, p);
                                  pst.objective_indices.push_back(objective_index);
                                  clique.push_back(p.label);
                              });

        if (!clique.empty())
        {
            pst.cliques.push_back(std::move(clique));
        }
    }
    /**
     * Given a collection of paths belonging to the same objective, this function creates edges in the edge intersection
     * graph between each pair of corresponding nodes, thus, forming a clique (complete sub-graph).
     *
     * @param objective_paths Collection of paths belonging to the same objective.
     * @throws std::bad_alloc If storage allocation fails.
     */
    void connect_clique(std::vector<clk_path>& objective_paths)
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
     * @throws std::bad_alloc If storage allocation fails.
     */
    void create_intersection_edges(std::vector<clk_path>& objective_paths)
    {
        std::ranges::for_each(objective_paths,
                              [this](const auto& obj_p)
                              {
                                  std::ranges::for_each(all_paths,
                                                        [this, &obj_p](const auto& stored_p)
                                                        {
                                                            if (ps.crossings ? obj_p.has_overlap_with(stored_p) :
                                                                               obj_p.has_intersection_with(stored_p))
                                                            {
                                                                graph.insert_edge(obj_p.label, stored_p.label,
                                                                                  edge_id++);
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
 * An explicit zero path limit creates no candidate paths and counts each objective as unroutable without checking
 * endpoint bounds.
 *
 * @tparam Lyt Type of the clocked layout.
 * @param lyt The layout to generate the edge intersection graph for.
 * @param objectives Source coordinates and indexed destination inputs.
 * @param ps Parameters.
 * @param pst Statistics.
 * @return An edge intersection graph of paths satisfying the given routing objectives in `lyt`.
 * @throws std::invalid_argument If an objective endpoint lies outside the layout extent when ps.path_limit is absent
 * or nonzero.
 */
template <typename Lyt>
    requires requires { requires std::same_as<typename Lyt::object_id, layouts::layout_object_id>; }
edge_intersection_graph generate_edge_intersection_graph(const Lyt&                              lyt,
                                                         const std::vector<routing_objective>&   objectives,
                                                         generate_edge_intersection_graph_params ps  = {},
                                                         generate_edge_intersection_graph_stats* pst = nullptr)
{
    generate_edge_intersection_graph_stats st{};

    detail::generate_edge_intersection_graph_impl<Lyt> p{lyt, objectives, ps, st};

    /** @brief Completed intersection graph retained while statistics are copied. */
    auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction::physical_design

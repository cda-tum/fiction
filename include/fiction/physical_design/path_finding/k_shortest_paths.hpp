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
 * @brief Yen's algorithm for the k shortest loop-less paths in a layout.
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/obstructions.hpp"
#include "fiction/physical_design/path_finding/a_star.hpp"
#include "fiction/physical_design/path_finding/cost.hpp"
#include "fiction/physical_design/path_finding/distance.hpp"
#include "fiction/physical_design/routing_utils.hpp"
#include "fiction/traits.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>

namespace fiction::physical_design::path_finding
{

/**
 * Parameters for Yen's \f$k\f$-shortest paths algorithm.
 */
struct yen_k_shortest_paths_params
{
    /**
     * Parameters for the internal A* algorithm.
     */
    a_star_params astar_params{};
};

namespace detail
{

template <typename Path, typename Lyt>
class yen_k_shortest_paths_impl
{
  public:
    yen_k_shortest_paths_impl(const Lyt& lyt, const routing_objective<Lyt>& obj, const uint32_t k,
                              const yen_k_shortest_paths_params&            p,
                              const layouts::obstructions<coordinate<Lyt>>& extra) :
            layout{lyt},
            initial_obstructions{extra},
            search_obstructions{extra},
            objective{obj},
            num_shortest_paths{k},
            params{p}
    {
        // start by determining the shortest path between source and target
        k_shortest_paths.push_back(a_star<Path>(layout, objective, manhattan_distance_functor<Lyt, uint64_t>(),
                                                unit_cost_functor<Lyt, uint8_t>(), params.astar_params,
                                                search_obstructions));
    }

    /**
     * Enumerate up to k shortest paths in a layout that start at `objective.source` and lead to `objective.target`.
     *
     * @return A collection of up to k shortest paths in `layout` from `objective.source` to `objective.target`.
     */
    [[nodiscard]] path_collection<Path> run() noexcept
    {
        assert(!objective.source.is_dead() && !objective.target.is_dead() &&
               "Neither source nor target coordinate can be dead");

        assert(layout.is_within_bounds(objective.source) && layout.is_within_bounds(objective.target) &&
               "Both source and target coordinate have to be within the layout bounds");

        // if there was no path to begin with, abort
        if (k_shortest_paths.back().empty())
        {
            return {};
        }

        // for the number of shortest paths k
        for (uint32_t k = 1; k < num_shortest_paths; ++k)
        {
            const auto& latest_path = k_shortest_paths[k - 1];

            // for all coordinates of the latest path except the last one
            for (uint32_t i = 0; i < static_cast<int64_t>(latest_path.size()) - 1; ++i)
            {
                // create a spur, which is the ith coordinate of the latest path
                const auto spur = latest_path[i];

                // the root path is the path from the source to the spur coordinate
                Path root_path{latest_path.cbegin(), latest_path.cbegin() + i};

                // for all previous paths
                for (const auto& p : k_shortest_paths)
                {
                    // if the root path is equal to a previous partial path
                    // p[i] and p[i + 1] are accessed below, so p must hold at least i + 2 coordinates
                    if (p.size() > static_cast<std::size_t>(i) + 1 &&
                        std::ranges::equal(latest_path.cbegin(), latest_path.cbegin() + i + 1, p.cbegin(),
                                           p.cbegin() + i + 1))
                    {
                        // block the connection that was already used in the previous shortest path
                        search_obstructions.obstruct_connection(p[i], p[i + 1]);
                    }
                }

                // the root path excludes the spur coordinate
                for (const auto& root : root_path)
                {
                    search_obstructions.obstruct_coordinate(root);
                }

                // find an alternative path from the spur coordinate to the target and check that it is not empty
                if (const auto spur_path =
                        a_star<Path>(layout, {spur, objective.target}, manhattan_distance_functor<Lyt, uint64_t>(),
                                     unit_cost_functor<Lyt, uint8_t>(), params.astar_params, search_obstructions);
                    !spur_path.empty())
                {
                    // the final path will be a concatenation of the root path and the spur path
                    auto& final_path = root_path;
                    // allocate more memory for the final path (prepare concatenation)
                    final_path.reserve(root_path.size() + spur_path.size());
                    // concatenate root path and spur path to get the final path
                    final_path.insert(final_path.end(), std::make_move_iterator(spur_path.begin()),
                                      std::make_move_iterator(spur_path.end()));

                    shortest_path_candidates.add(final_path);
                }

                // clear obstructions again (prepare for the next potential path)
                reset_temporary_obstructions();
            }

            // if there were no spur paths or if all spur paths have been added to k_shortest_paths already
            if (shortest_path_candidates.empty())
            {
                break;
            }

            // fetch and remove the lowest cost path from the candidates and add it to k_shortest_paths
            if (const auto lowest_cost_path_it =
                    std::ranges::min_element(shortest_path_candidates, [](const auto& p1, const auto& p2)
                                             { return path_cost(p1) < path_cost(p2); });
                lowest_cost_path_it != shortest_path_candidates.cend())
            {
                k_shortest_paths.add(*lowest_cost_path_it);
                shortest_path_candidates.erase(lowest_cost_path_it);
            }
        }

        return k_shortest_paths;
    }

  private:
    /**
     * The caller's layout, queried without modification.
     */
    const Lyt& layout;
    /** @brief Caller constraints retained across spur searches. */
    const layouts::obstructions<coordinate<Lyt>>& initial_obstructions;
    /** @brief Independent constraints for the current spur search. */
    layouts::obstructions<coordinate<Lyt>> search_obstructions;
    /**
     * Source and target coordinates.
     */
    const routing_objective<Lyt> objective;
    /**
     * The number of paths to determine, i.e., k.
     */
    const uint32_t num_shortest_paths;
    /**
     * Routing parameters.
     */
    const yen_k_shortest_paths_params params;
    /**
     * The list of k shortest paths that is created during the algorithm.
     */
    path_collection<Path> k_shortest_paths{};
    /**
     * A set of potential shortest paths.
     */
    path_set<Path> shortest_path_candidates{};
    /**
     * Computes the cost of a path. This function can be adjusted to fetch paths of differing costs.
     *
     * Currently, the cost is equal to its length.
     *
     * @param p Path whose costs are to be calculated.
     * @return Costs of path p.
     */
    static std::size_t path_cost(const Path& p) noexcept
    {
        return p.size();
    }
    /**
     * Resets all temporary obstructions.
     */
    void reset_temporary_obstructions() noexcept
    {
        search_obstructions = initial_obstructions;
    }
};

}  // namespace detail

/**
 * Yen's algorithm for finding up to \f$k\f$ shortest paths without loops from a source to a target coordinate. If
 * \f$k\f$ is larger than the number of possible paths from source to target, the size of the returned path collection
 * will be smaller than \f$k\f$.
 *
 * This implementation uses the A* algorithm with the Manhattan distance function internally.
 *
 * This function automatically detects whether the given layout implements a clocking interface (see
 * `gate_level_layout`) and respects the underlying information flow imposed by `layout`'s clocking scheme. This
 * algorithm does neither generate duplicate nor looping paths, even in a cyclic clocking scheme. That is, along each
 * path, each coordinate can occur at maximum once.
 *
 * If the given layout implements the obstruction interface (see `obstructions`), paths will not be routed via
 * obstructed coordinates or connections.
 *
 * If the given layout is a gate-level layout and implements the obstruction interface (see `obstructions`), paths
 * may contain wire crossings if specified in the parameters. Wire crossings are only allowed over other wires and only
 * if the crossing layer is not obstructed. Furthermore, it is ensured that crossings do not run along another wire but
 * cross only in a single point (orthogonal crossings + knock-knees/double wires).
 *
 * In certain cases it might be desirable to enumerate regular coordinate paths even if the layout implements a clocking
 * interface. This can be achieved by static-casting the layout to a coordinate layout when calling this function:
 * @code{.cpp}
 * using clk_lyt = gate_level_layout<cartesian_layout<>>;
 * using path = layout_coordinate_path<cartesian_layout<>>;
 * clk_lyt layout = ...;
 * auto k_paths = yen_k_shortest_paths<path>(static_cast<cartesian_layout<>>(layout), {source, target}, k);
 * @endcode
 *
 * The algorithm was originally described in \"An algorithm for finding shortest routes from all source nodes to a given
 * destination in general networks\" by Jin Y. Yen in Quarterly of Applied Mathematics, 1970.
 *
 * @tparam Path Type of the returned individual paths.
 * @tparam Lyt Type of the layout to perform path finding on.
 * @param layout The layout in which the \f$k\f$ shortest paths are to be found.
 * @param objective Source-target coordinate pair.
 * @param k Maximum number of shortest paths to find.
 * @param params Parameters.
 * @param obstructions Additional constraints; caller and layout obstructions remain unchanged.
 * @return A collection of up to \f$k\f$ shortest loop-less paths in `layout` from `objective.source` to
 * `objective.target`.
 */
template <typename Path, typename Lyt>
[[nodiscard]] path_collection<Path>
yen_k_shortest_paths(const Lyt& layout, const routing_objective<Lyt>& objective, const uint32_t k,
                     const yen_k_shortest_paths_params&            params       = {},
                     const layouts::obstructions<coordinate<Lyt>>& obstructions = {}) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    if (k == 0)
    {
        return {};
    }

    return detail::yen_k_shortest_paths_impl<Path, Lyt>{layout, objective, k, params, obstructions}.run();
}

}  // namespace fiction::physical_design::path_finding

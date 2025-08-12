//
// Created by marcel on 17.02.22.
//

#ifndef FICTION_A_STAR_HPP
#define FICTION_A_STAR_HPP

#include "fiction/algorithms/path_finding/cost.hpp"
#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"
#include "fiction/utils/stl_utils.hpp"

#include <phmap.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>
#include <vector>

namespace fiction
{

/**
 * Parameters for the A* algorithm.
 */
struct a_star_params
{
    /**
     * Allow paths to cross over obstructed tiles if they are occupied by wire segments.
     */
    bool crossings = false;
};

namespace detail
{

template <typename Path, typename Lyt, typename Dist, typename Cost>
class a_star_impl
{
  public:
    a_star_impl(const Lyt& lyt, const routing_objective<Lyt>& obj, const distance_functor<Lyt, Dist>& dist_fn,
                const cost_functor<Lyt, Cost>& cost_fn, const a_star_params& p) :
            layout{lyt},
            objective{obj},
            distance{dist_fn},
            cost{cost_fn},
            params{p}
    {
        open_list.push(coordinate_f{objective.source, 0});
    }

    /**
     * Determine the shortest path in a layout that starts at `objective.source` and leads to `objective.target`.
     *
     * @return The shortest path in `layout` from `objective.source` to `objective.target`.
     */
    [[nodiscard]] Path run() noexcept
    {
        assert(!objective.source.is_dead() && !objective.target.is_dead() &&
               "Neither source nor target coordinate can be dead");

        assert(layout.is_within_bounds(objective.source) && layout.is_within_bounds(objective.target) &&
               "Both source and target coordinate have to be within the layout bounds");

        do {
            // get coordinate with lowest f-value
            const auto current = get_lowest_f_coord();

            // if coord is the target, a path has been found
            if (current == objective.target)
            {
                return reconstruct_path();
            }
            // don't examine the current coordinate again
            closed_list.insert(current);

            // expand from current coordinate
            expand(current);

        } while (!open_list.empty());  // until the open list is empty

        return {};  // open list is empty, no path has been found
    }

  private:
    /**
     * The layout in which the shortest path between `source` and `target` is to be found.
     */
    const Lyt& layout;
    /**
     * The source-target coordinate pair.
     */
    const routing_objective<Lyt> objective;
    /**
     * The distance functor that implements the heuristic estimation function.
     */
    const distance_functor<Lyt, Dist> distance;
    /**
     * The cost functor that implements the cost function.
     */
    const cost_functor<Lyt, Cost> cost;
    /**
     * Routing parameters.
     */
    const a_star_params params;

    /**
     * The values used for g and f have a type in accordance with Dist and Cost.
     */
    using g_f_type = std::common_type_t<Dist, Cost>;
    /**
     * Stores a coordinate and its f-value.
     */
    struct coordinate_f
    {
        /**
         * Coordinate.
         */
        coordinate<Lyt> coord;  // cannot be const because operator= is required by std::priority_queue
        /**
         * f-value of the coordinate.
         */
        g_f_type f;

        /**
         * Comparator for the priority queue. Compares only the f-values.
         *
         * @param other Other coordinate_f to compare with.
         * @return `true` iff this f-value is greater than the other's.
         */
        bool operator>(const coordinate_f& other) const
        {
            return this->f > other.f;
        }
        /**
         * Equality operator that only compares the stored coordinates. This allows for a convenient lookup in the
         * priority queue.
         *
         * @param other Other coordinate_f to compare with.
         * @return `true` iff this coord value is equal to the other's.
         */
        bool operator==(const coordinate_f& other) const
        {
            return this->coord == other.coord;
        }
    };

    /**
     * Priority queue for coordinates sorted by their f-values. Uses std::greater to get the smallest f-value on top...
     * C++ is weird at times...
     */
    using a_star_priority_queue =
        searchable_priority_queue<coordinate_f, std::vector<coordinate_f>, std::greater<coordinate_f>>;
    /**
     * Open list that contains all coordinates to process next sorted by their f-value.
     */
    a_star_priority_queue open_list{};
    /**
     * Closed list that acts as a set of already visited coordinates.
     */
    phmap::flat_hash_set<coordinate<Lyt>> closed_list{};
    /**
     * Stores the g-cost for each coordinate c, i.e., the length of the shortest path from source to c found so far.
     */
    phmap::parallel_flat_hash_map<coordinate<Lyt>, g_f_type> g_values{};
    /**
     * Tracks the origin of each coordinate to recreate the path.
     */
    phmap::parallel_flat_hash_map<coordinate<Lyt>, coordinate<Lyt>> came_from{};
    /**
     * Fetches and pops the coordinate with the lowest f-value from the open list priority queue.
     *
     * @return Coordinate with the lowest f-value from the open list.
     */
    coordinate<Lyt> get_lowest_f_coord() noexcept
    {
        const auto current = open_list.top().coord;
        open_list.pop();

        return current;
    }
    /**
     * Expands the frontier of coordinates to visit next in the direction of the heuristic cost function.
     *
     * @param current Coordinate that is currently examined.
     */
    void expand(const coordinate<Lyt>& current) noexcept
    {
        const auto explore_successor = [this, current](auto successor)  // make a copy
            noexcept
        {
            // return to ground layer to avoid getting stuck in crossing layer
            successor = layout.below(successor);

            // check if successor is obstructed
            if constexpr (has_is_obstructed_coordinate_v<Lyt>)
            {
                if (layout.is_obstructed_coordinate(successor) && successor != objective.target)
                {
                    // if crossings are enabled, check if it is possible to switch to the crossing layer
                    if (params.crossings &&
                        (is_crossable_wire(layout, current, successor) || layout.above(successor) == objective.target))
                    {
                        // if the crossing layer is not obstructed
                        if (const auto above_successor = layout.above(successor);
                            above_successor != successor &&
                            (!layout.is_obstructed_coordinate(above_successor) || above_successor == objective.target))
                        {
                            // allow exploring the crossing layer
                            successor = above_successor;
                        }
                        else
                        {
                            return;  // skip the obstructed coordinate and keep looping
                        }
                    }
                    else
                    {
                        return;  // skip the obstructed coordinate and keep looping
                    }
                }
            }

            // check if the connection to the successor is obstructed
            if constexpr (has_is_obstructed_connection_v<Lyt>)
            {
                if (layout.is_obstructed_connection(current, successor))
                {
                    return;  // skip the obstructed connection and keep looping
                }
            }

            if (is_visited(successor))
            {
                return;  // skip any coordinate that is already in the closed list
            }

            // compute the g-value of cz. In this implementation, the costs of each 'step' are given by a function
            const g_f_type tentative_g = g(current) + cost(current, successor);

            // f-value does not matter because the comparator compares only the coordinates
            const auto it = open_list.find({successor, 0});
            if (it != open_list.end() && no_improvement(successor, tentative_g))
            {
                return;  // skip the coordinate if it does not offer improvement
            }

            // track origin
            came_from[successor] = current;
            set_g(successor, tentative_g);

            // compute new f-value
            const auto f = tentative_g + static_cast<g_f_type>(distance(layout, successor, objective.target));

            // if successor is contained in the open list (frontier)
            if (it != open_list.end())
            {
                // update its f-value
                it->f = f;
            }
            else
            {
                // add successor to the open list
                open_list.push({successor, f});
            }
        };

        if constexpr (is_clocked_layout_v<Lyt>)
        {
            // recurse for all outgoing clock zones
            layout.foreach_outgoing_clocked_zone(current, explore_successor);
        }
        else
        {
            // recurse for all adjacent coordinates
            layout.foreach_adjacent_coordinate(current, explore_successor);
        }
    }
    /**
     * Checks if a coordinate has been visited already.
     *
     * @param c Coordinate to check.
     * @return `true` iff c has already been visited.
     */
    bool is_visited(const coordinate<Lyt>& c) const noexcept
    {
        return closed_list.count(c) > 0;
    }
    /**
     * Returns the g-value of a coordinate by accessing the g_values map. Returns 0 if no entry exists.
     *
     * @param c Coordinate whose g-value is desired.
     * @return g-value of coordinate c or 0 if no value has been stored.
     */
    g_f_type g(const coordinate<Lyt>& c) const noexcept
    {
        if (const auto it = g_values.find(c); it != g_values.cend())
        {
            return it->second;
        }

        return 0ul;
    }
    /**
     * Updates the g-value of the given coordinate to the given value.
     *
     * @param c Coordinate whose g-value is to be updated to g_val.
     * @param g_val New g-value for c.
     */
    void set_g(const coordinate<Lyt>& c, const g_f_type g_val) noexcept
    {
        g_values.insert_or_assign(c, g_val);
    }
    /**
     * Checks if the given g-value is greater or equal than the stored g-value of the given coordinate. If that is the
     * case, the new g-value does not improve the path.
     *
     * @param c Coordinate to whose g-value is to be checked.
     * @param g_val g-value to compare to c's.
     * @return `true` iff the given g-value does not mean an improvement for the given coordinate.
     */
    bool no_improvement(const coordinate<Lyt>& c, const g_f_type g_val) noexcept
    {
        return g_val >= g(c);
    }
    /**
     * Reconstruct the final path from the origin map that was created during the path finding algorithm.
     *
     * @return The shortest path connecting source and target.
     */
    Path reconstruct_path() const noexcept
    {
        Path path{};

        // iterate backwards over the found connections and add them to the path
        for (auto current = objective.target; current != objective.source; current = came_from.at(current))
        {
            path.push_back(current);
        }
        // finally, add the source coordinate
        path.push_back(objective.source);
        // and reverse the path to bring it in proper order
        std::reverse(std::begin(path), std::end(path));

        return path;
    }
};
}  // namespace detail

/**
 * The A* path finding algorithm for shortest loop-less paths between a given source and target coordinate in a layout.
 * This function automatically detects whether the given layout implements a clocking interface (see `clocked_layout`)
 * and respects the underlying information flow imposed by `layout`'s clocking scheme.
 *
 * A* is an extension of Dijkstra's algorithm for shortest paths but offers better average complexity. It uses a
 * heuristic distance function that estimates the remaining costs towards the target in every step. Thus, this heuristic
 * function should neither be complex to calculate nor overestimating the remaining costs. Common heuristics to be used
 * are the Manhattan and the Euclidean distance functions. See `distance_functor` for implementations.
 *
 * If the given layout implements the obstruction interface (see `obstruction_layout`), paths will not be routed via
 * obstructed coordinates and connections.
 *
 * If the given layout is a gate-level layout and implements the obstruction interface (see `obstruction_layout`), paths
 * may contain wire crossings if specified in the parameters. Wire crossings are only allowed over other wires and only
 * if the crossing layer is not obstructed. Furthermore, it is ensured that crossings do not run along another wire but
 * cross only in a single point (orthogonal crossings + knock-knees/double wires).
 *
 * In certain cases it might be desirable to determine regular coordinate paths even if the layout implements a clocking
 * interface. This can be achieved by static-casting the layout to a coordinate layout when calling this function:
 * @code{.cpp}
 * using clk_lyt = clocked_layout<cartesian_layout<>>;
 * using path = layout_coordinate_path<cartesian_layout<>>;
 * clk_lyt layout = ...;
 * auto shortest_path = a_star<path>(static_cast<cartesian_layout<>>(layout), {source, target});
 * @endcode
 *
 * A* was introduced in \"A Formal Basis for the Heuristic Determination of Minimum Cost Paths\" by Peter E. Hart, Nils
 * J. Nilsson, and Bertram Raphael in IEEE Transactions on Systems Science and Cybernetics 1968, Volume 4, Issue 2.
 *
 * This implementation is based on the pseudocode from https://en.wikipedia.org/wiki/A_star_search_algorithm.
 *
 * @tparam Path Type of the returned path.
 * @tparam Lyt Type of the layout to perform path finding on.
 * @tparam Dist Distance value type to be used in the heuristic estimation function.
 * @tparam Cost Cost value type to be used when determining moving cost between coordinates.
 * @param layout The layout in which the shortest path between a source and target coordinate is to be found.
 * @param objective Source-target coordinate pair.
 * @param dist_fn A distance functor that implements the desired heuristic estimation function.
 * @param cost_fn A cost functor that implements the desired cost function.
 * @param params Parameters.
 * @return The shortest loop-less path in `layout` from `objective.source` to `objective.target`.
 */
template <typename Path, typename Lyt, typename Dist = uint64_t, typename Cost = uint8_t>
[[nodiscard]] Path a_star(const Lyt& layout, const routing_objective<Lyt>& objective,
                          const distance_functor<Lyt, Dist>& dist_fn = manhattan_distance_functor<Lyt, uint64_t>(),
                          const cost_functor<Lyt, Cost>&     cost_fn = unit_cost_functor<Lyt, uint8_t>(),
                          const a_star_params&               params  = {}) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");

    return detail::a_star_impl<Path, Lyt, Dist, Cost>{layout, objective, dist_fn, cost_fn, params}.run();
}
/**
 * A distance function that does not approximate but compute the actual minimum path length on the given layout via A*
 * traversal. Naturally, this function cannot be evaluated in \f$\mathcal{O}(1)\f$, but has the polynomial complexity
 * of A*.
 *
 * If no path between `source` and `target` exists in `layout`, the returned distance is
 * `std::numeric_limits<Dist>::infinity()` if that value is supported by `Dist`, or `std::numeric_limits<Dist>::max()`,
 * otherwise.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Distance type.
 * @param layout The layout in which the distance between `source` and `target` is to be determined.
 * @param source Source coordinate.
 * @param target Target coordinate.
 * @return Minimum path length between `source` and `target` in `layout`.
 */
template <typename Lyt, typename Dist = uint64_t>
[[nodiscard]] Dist a_star_distance(const Lyt& layout, const coordinate<Lyt>& source,
                                   const coordinate<Lyt>& target) noexcept
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(std::is_arithmetic_v<Dist>, "Dist is not an arithmetic type");

    const auto path_length = a_star<layout_coordinate_path<Lyt>>(layout, {source, target}).size();

    if (path_length == 0ul)
    {
        if constexpr (std::numeric_limits<Dist>::has_infinity)
        {
            return std::numeric_limits<Dist>::infinity();
        }

        return std::numeric_limits<Dist>::max();
    }

    return static_cast<Dist>(path_length - 1);
}
/**
 * A pre-defined distance functor that uses the A* distance.
 *
 * @tparam Lyt Coordinate layout type.
 * @tparam Dist Distance type.
 */
template <typename Lyt, typename Dist = uint64_t>
class a_star_distance_functor : public distance_functor<Lyt, Dist>
{
  public:
    a_star_distance_functor() : distance_functor<Lyt, Dist>(&a_star_distance<Lyt, Dist>) {}
};

}  // namespace fiction

#endif  // FICTION_A_STAR_HPP

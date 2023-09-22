//
// Created by marcel on 19.05.22.
//

#ifndef FICTION_JUMP_POINT_SEARCH_HPP
#define FICTION_JUMP_POINT_SEARCH_HPP

#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"
#include "fiction/utils/stl_utils.hpp"

#include <phmap.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <optional>
#include <type_traits>
#include <vector>

namespace fiction
{

namespace detail
{

template <typename Path, typename Lyt, typename Dist>
class jump_point_search_impl
{
  public:
    jump_point_search_impl(const Lyt& lyt, const routing_objective<Lyt>& obj,
                           const distance_functor<Lyt, Dist>& dist_fn) :
            layout{lyt},
            objective{obj},
            distance{dist_fn}
    {
        open_list.push(coordinate_f{objective.source, 0});
    }

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
     * Stores a coordinate and its f-value.
     */
    struct coordinate_f
    {
        /**
         * Coordinate.
         */
        coordinate<Lyt> coord;
        /**
         * f-value of the coordinate.
         */
        Dist f;

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
    using jump_point_search_priority_queue =
        searchable_priority_queue<coordinate_f, std::vector<coordinate_f>, std::greater<coordinate_f>>;
    /**
     * Open list that contains all coordinates to process next sorted by their f-value.
     */
    jump_point_search_priority_queue open_list{};
    /**
     * Closed list that acts as a set of already visited coordinates.
     */
    phmap::flat_hash_set<coordinate<Lyt>> closed_list{};
    /**
     * Stores the g-cost for each coordinate c, i.e., the length of the shortest path from source to c found so far.
     */
    phmap::parallel_flat_hash_map<coordinate<Lyt>, Dist> g_values{};
    /**
     * Tracks the origin of each coordinate to recreate the path.
     */
    phmap::parallel_flat_hash_map<coordinate<Lyt>, coordinate<Lyt>> came_from{};
    /**
     * Fetches and pops the coordinate with the lowest f-value from the open list priority queue.
     *
     * @return Coordinate with the lowest f-value from the open list.
     */
    [[nodiscard]] coordinate<Lyt> get_lowest_f_coord() noexcept
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
        const auto explore_successor = [this, current](const auto& successor) noexcept
        {
            // check if the connection to the successor is obstructed
            if constexpr (has_is_obstructed_connection_v<Lyt>)
            {
                if (layout.is_obstructed_connection(current, successor))
                {
                    return;  // skip the obstructed connection and keep looping
                }
            }

            // check if successor is obstructed
            if constexpr (has_is_obstructed_coordinate_v<Lyt>)
            {
                if (layout.is_obstructed_coordinate(successor) && successor != objective.target)
                {
                    return;  // skip the obstructed coordinate and keep looping
                }
            }

            // compute the next jump point to prune the search space
            if (const auto jump_point = jump(current, successor); jump_point.has_value())
            {
                if (is_visited(*jump_point))
                {
                    return;  // skip any jump point that is already in the closed list
                }

                // compute the g-value of current. Add the distance to the jump point as it might not be adjacent
                const auto tentative_g = g(current) + distance(layout, *jump_point, current);

                // f-value does not matter because the comparator compares only the coordinates
                const auto it = open_list.find({*jump_point, 0});
                if (it != open_list.end() && no_improvement(*jump_point, tentative_g))
                {
                    return;  // skip the coordinate if it does not offer improvement
                }

                // track origin
                came_from[*jump_point] = current;
                set_g(*jump_point, tentative_g);

                // compute new f-value
                const auto f = tentative_g + distance(layout, *jump_point, objective.target);

                // if successor is contained in the open list (frontier)
                if (it != open_list.end())
                {
                    // update its f-value
                    it->f = f;
                }
                else
                {
                    // add successor to the open list
                    open_list.push({*jump_point, f});
                }
            }

            return;  // keep looping
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
     * Compute the next jump point starting from successor with current as its parent. The vector (current, successor)
     * thus defines the direction in which to look for the jump point. A jump point has to be stepwise traversable from
     * successor. That is, there must be a valid path through non-obstructed tiles.
     *
     * @param current Parent coordinate.
     * @param successor Coordinate from which to look for the next jump point.
     * @return The next jump point or `std::nullopt` if no such coordinate is available.
     */
    [[nodiscard]] std::optional<coordinate<Lyt>> jump(const coordinate<Lyt>& current,
                                                      const coordinate<Lyt>& successor) const noexcept
    {
        if (!is_traversable(current, successor))
        {
            return std::nullopt;
        }

        if (successor == objective.target)
        {
            return successor;
        }

        const auto cx = static_cast<int64_t>(current.x), cy = static_cast<int64_t>(current.y),
                   sx = static_cast<int64_t>(successor.x), sy = static_cast<int64_t>(successor.y);

        const int64_t dx = sx - cx, dy = sy - cy;

        if (dx != 0)
        {
            if ((is_traversable(successor, {sx, sy - 1}) && !is_traversable(successor, {sx - dx, sy - 1})) ||
                (is_traversable(successor, {sx, sy + 1}) && !is_traversable(successor, {sx - dx, sy + 1})))
            {
                return successor;
            }
        }
        else if (dy != 0)
        {
            if ((is_traversable(successor, {sx - 1, sy}) && !is_traversable(successor, {sx - 1, sy - dy})) ||
                (is_traversable(successor, {sx + 1, sy}) && !is_traversable(successor, {sx + 1, sy - dy})))
            {
                return successor;
            }
            // check for horizontal jump points when traversing in vertical direction
            if (jump({sx + 1, sy}, successor).has_value() || jump({sx - 1, sy}, successor).has_value())
            {
                return successor;
            }
        }

        return jump({sx + dx, sy + dy}, successor);
    }
    /**
     * Checks if coordinate c2 is directly reachable from coordinate c1. That is if the coordinates are not identical,
     * if they are adjacent or if the clocking checks out, and if there is no obstacle in the way.
     *
     * @param c1 Start coordinate.
     * @param c2 Goal coordinate.
     * @return `true` iff c2 is directly reachable from c1.
     */
    [[nodiscard]] bool is_traversable(const coordinate<Lyt>& c1, const coordinate<Lyt>& c2) const noexcept
    {
        if (c1 == c2)
        {
            return false;
        }

        if constexpr (is_clocked_layout_v<Lyt>)
        {
            if (!layout.is_outgoing_clocked(c1, c2))
            {
                return false;
            }
        }
        else
        {
            if (!layout.is_adjacent_of(c1, c2))
            {
                return false;
            }
        }

        if constexpr (has_is_obstructed_connection_v<Lyt>)
        {
            if (layout.is_obstructed_connection(c1, c2))
            {
                return false;
            }
        }

        if constexpr (has_is_obstructed_coordinate_v<Lyt>)
        {
            if (layout.is_obstructed_coordinate(c2) && c2 != objective.target)
            {
                return false;
            }
        }

        return true;
    }
    /**
     * Checks if a coordinate has been visited already.
     *
     * @param c Coordinate to check.
     * @return `true` iff c has already been visited.
     */
    [[nodiscard]] bool is_visited(const coordinate<Lyt>& c) const noexcept
    {
        return closed_list.count(c) > 0;
    }
    /**
     * Returns the g-value of a coordinate by accessing the g_values map. Returns 0 if no entry exists.
     *
     * @param c Coordinate whose g-value is desired.
     * @return g-value of coordinate c or 0 if no value has been stored.
     */
    [[nodiscard]] Dist g(const coordinate<Lyt>& c) const noexcept
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
    void set_g(const coordinate<Lyt>& c, const Dist g_val) noexcept
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
    [[nodiscard]] bool no_improvement(const coordinate<Lyt>& c, const Dist g_val) noexcept
    {
        return g_val >= g(c);
    }
    /**
     * Reconstruct the final path from the origin map that was created during the path finding algorithm.
     *
     * @return The shortest path connecting source and target.
     */
    [[nodiscard]] Path reconstruct_path() const noexcept
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

        return fill_in_jumps(path);  // fill in the blanks that were skipped via jump points
    }
    /**
     * Completes a reconstructed path by filling in the jumps that were made during JPS.
     *
     * @param p Incomplete path with jump gaps.
     * @return Completed interpolated path.
     */
    [[nodiscard]] Path fill_in_jumps(const Path& p) const noexcept
    {
        Path expanded{};

        if (p.size() < 2)
        {
            return p;
        }

        for (auto i = 0ull; i < p.size() - 1; ++i)
        {
            const auto c1 = p[i], c2 = p[i + 1];

            const auto interpolated = interpolate(c1, c2);
            for (auto j = 0ull; j < interpolated.size() - 1; ++j)
            {
                expanded.push_back(interpolated[j]);
            }
        }
        expanded.push_back(p.back());

        return expanded;
    }
    /**
     * Interpolates a straight line between two given coordinates using Bresenham's algorithm:
     * http://en.wikipedia.org/wiki/Bresenham's_line_algorithm
     *
     * This function is repeatedly used in the fill_in_jumps function to interpolate the jump gaps.
     *
     * @param c1 Start coordinate.
     * @param c2 Goal coordinate.
     * @return A straight path between c1 and c2.
     */
    [[nodiscard]] Path interpolate(coordinate<Lyt> c1, coordinate<Lyt> c2) const noexcept
    {
        Path line{};

        const int64_t dx = std::abs(static_cast<std::make_signed_t<decltype(c1.x)>>(c2.x - c1.x)),
                      dy = std::abs(static_cast<std::make_signed_t<decltype(c1.y)>>(c2.y - c1.y));

        const int64_t sx = (c1.x < c2.x) ? 1 : -1, sy = (c1.y < c2.y) ? 1 : -1;

        int64_t err = dx - dy;

        while (true)
        {
            line.push_back(c1);

            if (c1.x == c2.x && c1.y == c2.y)
            {
                break;
            }

            const auto e2 = 2 * err;

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"
            if (e2 > -dy)
            {
                err  = err - dy;
                c1.x = static_cast<decltype(c1.x)>(c1.x + sx);
            }
            if (e2 < dx)
            {
                err  = err + dx;
                c1.y = static_cast<decltype(c1.y)>(c1.y + sy);
            }
#pragma GCC diagnostic pop
        }

        return line;
    }
};

}  // namespace detail

/**
 * The Jump Point Search (JPS) path finding algorithm for shortest loop-less paths between a given source and target
 * coordinate in a Cartesian layout. This function automatically detects whether the given layout implements a clocking
 * interface (see `clocked_layout`) and respects the underlying information flow imposed by `layout`'s clocking scheme.
 *
 * JPS was proposed as an optimization of A* for shortest paths and offers better average complexity on uniform-cost
 * grids that allow diagonal connections. It uses a heuristic distance function that estimates the remaining costs
 * towards the target in every step. Thus, this heuristic function should neither be complex to calculate nor
 * overestimating the remaining costs. Common heuristics to be used are the Manhattan and the Euclidean distance
 * functions. See `distance_functor` for implementations. Since JPS assumes a unit-cost grid, the use of cost functions
 * together with JPS is not possible.
 *
 * If the given layout implements the obstruction interface (see `obstruction_layout`), paths will not be routed via
 * obstructed coordinates and connections.
 *
 * In certain cases it might be desirable to determine regular coordinate paths even if the layout implements a clocking
 * interface. This can be achieved by static-casting the layout to a coordinate layout when calling this function:
 * @code{.cpp}
 * using clk_lyt = clocked_layout<cartesian_layout<>>;
 * using path = layout_coordinate_path<cartesian_layout<>>;
 * clk_lyt layout = ...;
 * auto shortest_path = jump_point_search<path>(static_cast<cartesian_layout<>>(layout), {source, target});
 * @endcode
 *
 * JPS was introduced in \"Online Graph Pruning for Pathfinding on Grid Maps\" by Daniel Harabor and Alban Grastien in
 * AAAI 2011.
 *
 * Parts of this implementation are based on https://github.com/qiao/PathFinding.js.
 *
 * @note The original JPS highly relies on diagonal paths in the grid which are not possible in most Cartesian
 * grid-based FCN technologies. Therefore, this implementation disallows diagonal paths. Consequently, and due to
 * non-uniform clocking schemes, JPS might perform worse than A* in terms of runtime. It is recommended to use A* (see
 * `a_star`).
 *
 * @note JPS does not support wire crossings.
 *
 * @tparam Path Type of the returned path.
 * @tparam Lyt Type of the layout to perform path finding on.
 * @tparam Dist Distance value type to be used in the heuristic estimation function.
 * @param layout The layout in which the shortest path between a source and target is to be found.
 * @param objective Source-target coordinate pair.
 * @param dist_fn A distance functor that implements the desired heuristic estimation function.
 * @return The shortest loop-less path in `layout` from `objective.source` to `objective.target`.
 */
template <typename Path, typename Lyt, typename Dist = uint64_t>
[[nodiscard]] Path
jump_point_search(const Lyt& layout, const routing_objective<Lyt>& objective,
                  const distance_functor<Lyt, Dist>& dist_fn = manhattan_distance_functor<Lyt, Dist>()) noexcept
{
    static_assert(is_cartesian_layout_v<Lyt>, "Lyt is not a Cartesian layout");

    return detail::jump_point_search_impl<Path, Lyt, Dist>{layout, objective, dist_fn}.run();
}

}  // namespace fiction

#endif  // FICTION_JUMP_POINT_SEARCH_HPP

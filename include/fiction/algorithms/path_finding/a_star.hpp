//
// Created by marcel on 17.02.22.
//

#ifndef FICTION_A_STAR_HPP
#define FICTION_A_STAR_HPP

#include "fiction/algorithms/path_finding/distance.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/stl_utils.hpp"

#include <cassert>
#include <unordered_set>
#include <utility>
#include <vector>

namespace fiction
{

struct a_star_params
{
    // should paths be considered that extend into the crossing layer?
    bool consider_crossing_layer = false;
};

namespace detail
{

template <typename Path, typename Lyt, typename Dist>
class a_star_impl
{
  public:
    a_star_impl(const Lyt& lyt, const coordinate<Lyt>& src, const coordinate<Lyt>& tgt,
                const distance_functor<Lyt, Dist>& dist_fn, const a_star_params p) :
            layout{lyt},
            source{src},
            target{tgt},
            distance{dist_fn},
            ps{p}
    {
        open_list.push(coordinate_f{source, 0});
    }

    Path run()
    {
        assert(!source.is_dead() && !target.is_dead() && "Neither source nor target coordinate can be dead");

        assert(layout.is_within_bounds(source) && layout.is_within_bounds(target) &&
               "Both source and target coordinate have to be within the layout bounds");

        do {
            // get coordinate with lowest f-value
            const auto current = get_lowest_f_coord();

            // if coord is the target, a path has been found
            if (current == target)
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
    const Lyt& layout;

    const coordinate<Lyt>&source, target;

    const distance_functor<Lyt, Dist> distance;

    a_star_params ps;

    /**
     * Stores a coordinate and its f-value.
     */
    struct coordinate_f
    {
        coordinate<Lyt> coord;

        Dist f;

        /**
         * Comparator for the priority queue. Compares only the f-values.
         *
         * @param other Other coordinate_f to compare with.
         * @return True iff this f-value is greater than the other's.
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
         * @return True iff this coord value is equal to the other's.
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
    std::unordered_set<coordinate<Lyt>> closed_list{};
    /**
     * Stores the g-cost for each coordinate c, i.e., the length of the shortest path from source to c found so far.
     */
    std::unordered_map<coordinate<Lyt>, uint32_t> g_values{};
    /**
     * Tracks the origin of each coordinate to recreate the path.
     */
    std::unordered_map<coordinate<Lyt>, coordinate<Lyt>> came_from{};
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
        layout.foreach_outgoing_clocked_zone(
            current,
            [this, current](const auto& successor)
            {
                if constexpr (has_is_obstructed_v<Lyt>)
                {
                    if (layout.is_obstructed(successor) && successor != target)
                    {
                        return true;  // skip the obstructed coordinate and keep looping
                    }
                }

                if (is_visited(successor))
                {
                    return true;  // skip any coordinate that is already in the closed list
                }

                // compute the g-value of cz. In this implementation, each 'step' has uniform costs of 1
                const auto tentative_g = g(current) + 1;

                // f-value does not matter because the comparator compares only the coordinates
                if (const auto it = open_list.find({successor, 0});
                    it != open_list.end() && no_improvement(successor, tentative_g))
                {
                    return true;  // skip the coordinate if it does not offer improvement
                }
                else
                {
                    // track origin
                    came_from[successor] = current;
                    set_g(successor, tentative_g);

                    // compute new f-value
                    const auto f = tentative_g + distance(successor, target);

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

                    return true;  // keep looping
                }
            });
    }
    /**
     * Checks if a coordinate has been visited already.
     *
     * @param c Coordinate to check.
     * @return True iff c has already been visited.
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
    uint32_t g(const coordinate<Lyt>& c) const noexcept
    {
        if (const auto it = g_values.find(c); it != g_values.cend())
        {
            return it->second;
        }
        else
        {
            return 0ul;
        }
    }
    /**
     * Updates the g-value of the given coordinate to the given value.
     *
     * @param c Coordinate whose g-value is to be updated to g_val.
     * @param g_val New g-value for c.
     */
    void set_g(const coordinate<Lyt>& c, const uint32_t g_val) noexcept
    {
        g_values.insert_or_assign(c, g_val);
    }
    /**
     * Checks if the given g-value is greater or equal than the stored g-value of the given coordinate. If that is the
     * case, the new g-value does not improve the path.
     *
     * @param c Coordinate to whose g-value is to be checked.
     * @param g_val g-value to compare to c's.
     * @return True iff the given g-value does not mean an improvement for the given coordinate.
     */
    bool no_improvement(const coordinate<Lyt>& c, const uint32_t g_val) noexcept
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
        for (auto current = target; current != source; current = came_from.at(current)) { path.push_back(current); }
        // finally, add the source coordinate
        path.push_back(source);
        // and reverse the path to bring it in proper order
        std::reverse(std::begin(path), std::end(path));

        return path;
    }
};
}  // namespace detail

/**
 * The A* path finding algorithm for shortest paths between a given source and target coordinate in a clocked layout. A*
 * is an extension of Dijkstra's algorithm for shortest paths but offers better average complexity. It uses a heuristic
 * distance function that estimates the remaining costs towards the target in every step. Thus, this heuristic function
 * should neither be complex to calculate nor overestimating the remaining costs. Common heuristics to be used are the
 * Manhattan and the Euclidean distance functions. See distance.hpp for implementations.
 *
 * If the given layout implements the is_obstructed interface (see obstruction_layout.hpp), paths will not be routed via
 * obstructed coordinates.
 *
 * @tparam Path Path type to create.
 * @tparam Lyt Clocked layout type.
 * @tparam Dist Distance value type to be used in the heuristic cost function.
 * @param layout The clocked layout in which the shortest path between source and target is to be found.
 * @param source Starting coordinate.
 * @param target Goal coordinate.
 * @param dist_fn A distance functor that implements the desired heuristic cost function.
 * @param ps Parameters.
 * @return The shortest path from source to target.
 */
template <typename Path, typename Lyt, typename Dist = uint64_t>
[[nodiscard]] Path a_star(const Lyt& layout, const coordinate<Lyt>& source, const coordinate<Lyt>& target,
                          const distance_functor<Lyt, Dist>& dist_fn = manhattan_distance_functor<Lyt, Dist>(),
                          a_star_params                      ps      = {}) noexcept
{
    static_assert(is_clocked_layout_v<Lyt>, "Lyt is not a clocked layout");

    return detail::a_star_impl<Path, Lyt, Dist>{layout, source, target, dist_fn, ps}.run();
}

}  // namespace fiction

#endif  // FICTION_A_STAR_HPP

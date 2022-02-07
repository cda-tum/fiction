//
// Created by marcel on 01.02.22.
//

#ifndef FICTION_ENUMERATE_ALL_PATHS_HPP
#define FICTION_ENUMERATE_ALL_PATHS_HPP

#include "fiction/traits.hpp"
#include "fiction/utils/routing_utils.hpp"

#include <vector>

namespace fiction
{

struct enumerate_all_clocking_paths_params
{
    // should paths be considered that extend into the crossing layer?
    bool consider_crossing_layer = false;
};

namespace detail
{

template <typename Path, typename Lyt>
class enumerate_all_clocking_paths_impl
{
  public:
    enumerate_all_clocking_paths_impl(const Lyt& lyt, const coordinate<Lyt>& src, const coordinate<Lyt>& tgt,
                                      const enumerate_all_clocking_paths_params p) :
            layout{lyt},
            source{src},
            target{tgt},
            ps{p},
            visited(layout.area(), false)
    {}

    [[nodiscard]] path_collection<Path> run()
    {
        Path p{};
        recursively_enumerate_all_paths(source, target, p);

        return collection;
    }

  private:
    const Lyt& layout;

    const coordinate<Lyt>&source, target;

    enumerate_all_clocking_paths_params ps;

    std::vector<bool> visited;

    path_collection<Path> collection{};

    /**
     * Calculates the index position of a coordinate. Indices are not a reliable way to associate information with
     * coordinates because they shift whenever the layout is resized.
     *
     * @param c Coordinate whose index is desired.
     * @return Index of coordinate c.
     */
    [[nodiscard]] std::size_t to_index(const coordinate<Lyt>& c) const noexcept
    {
        return c.x + (layout.y() + 1) * c.y;
    }

    void mark_visited(const coordinate<Lyt>& c) noexcept
    {
        visited[to_index(c)] = true;
    }

    void mark_unvisited(const coordinate<Lyt>& c) noexcept
    {
        visited[to_index(c)] = false;
    }

    [[nodiscard]] bool is_visited(const coordinate<Lyt>& c) const noexcept
    {
        return visited[to_index(c)];
    }

    void recursively_enumerate_all_paths(const coordinate<Lyt>& src, const coordinate<Lyt>& tgt, Path p) noexcept
    {
        // mark source coordinate as visited and append it to the path
        mark_visited(src);
        p.append(src);

        // if source and target are identical, a path has been found
        if (src == tgt)
        {
            collection.add(p);
        }
        else  // destination is not reached yet
        {
            // recurse for all outgoing clock zones
            layout.foreach_outgoing_clocked_zone(
                src,
                [this, &tgt, &p](const auto& cz)
                {
                    if constexpr (has_is_obstructed_v<Lyt>)
                    {
                        if (layout.is_obstructed(cz))
                        {
                            return true;  // skip the obstructed coordinate and keep looping
                        }
                    }

                    // if it has not yet been visited
                    if (!is_visited(cz))
                    {
                        // recurse
                        recursively_enumerate_all_paths(cz, tgt, p);
                    }

                    return true;  // keep looping
                });
        }

        // after recursion, remove current coordinate from path and mark it as unvisited to allow it in other paths
        p.pop_back();
        mark_unvisited(src);
    }
};

}  // namespace detail

/**
 * Enumerates all possible paths in a clocked layout that start at coordinate source and lead to coordinate target while
 * respecting the information flow imposed by the clocking scheme. This algorithm does neither generate duplicate nor
 * looping paths, even in a cyclic clocking scheme. That is, along each path, each coordinate can occur at maximum once.
 *
 * @tparam Path Type of the returned individual paths.
 * @tparam Lyt Type of the clocked layout to perform path finding on.
 * @param layout The clocked layout whose paths are to be enumerated.
 * @param source Starting coordinate.
 * @param target Goal coordinate.
 * @param ps Parameters.
 * @return A collection of all unique paths in layout from source to target.
 */
template <typename Path, typename Lyt>
[[nodiscard]] path_collection<Path> enumerate_all_clocking_paths(const Lyt& layout, const coordinate<Lyt>& source,
                                                                 const coordinate<Lyt>&              target,
                                                                 enumerate_all_clocking_paths_params ps = {}) noexcept
{
    static_assert(is_clocked_layout_v<Lyt>, "Lyt is not a clocked layout");

    return detail::enumerate_all_clocking_paths_impl<Path, Lyt>{layout, source, target, ps}.run();
}

}  // namespace fiction

#endif  // FICTION_ENUMERATE_ALL_PATHS_HPP

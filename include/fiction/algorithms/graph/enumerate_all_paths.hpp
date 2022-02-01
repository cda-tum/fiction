//
// Created by marcel on 01.02.22.
//

#ifndef FICTION_ENUMERATE_ALL_PATHS_HPP
#define FICTION_ENUMERATE_ALL_PATHS_HPP

#include "fiction/traits.hpp"

#include <algorithm>
#include <vector>

namespace fiction
{

template <typename Lyt>
struct layout_coordinate_path : std::vector<coordinate<Lyt>>
{
    void append(const coordinate<Lyt>& c) noexcept
    {
        this->push_back(c);
    }

    [[nodiscard]] coordinate<Lyt> source() const noexcept
    {
        return this->empty() ? coordinate<Lyt>{} : this->front();
    }

    [[nodiscard]] coordinate<Lyt> target() const noexcept
    {
        return this->empty() ? coordinate<Lyt>{} : this->back();
    }
};

template <typename Path>
struct path_collection : std::vector<Path>
{
    void add(const Path& p) noexcept
    {
        this->push_back(p);
    }

    [[nodiscard]] bool contains(const Path& p) const noexcept
    {
        return std::find(std::cbegin(*this), std::cend(*this), p) != std::cend(*this);
    }
};

struct enumerate_all_clocking_paths_params
{
    // should paths be considered that extend into the crossing layer?
    bool consider_crossing_layer = false;
};

namespace detail
{

template <typename Lyt, typename Path>
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
            layout.foreach_outgoing_clocked_zone(src,
                                                 [this, &tgt, &p](const auto& cz)
                                                 {
                                                     // if it has not yet been visited
                                                     if (!is_visited(cz))
                                                     {
                                                         // recurse
                                                         recursively_enumerate_all_paths(cz, tgt, p);
                                                     }
                                                 });
        }

        // after recursion, remove current coordinate from path and mark it as unvisited to allow it in other paths
        p.pop_back();
        mark_unvisited(src);
    }
};

}  // namespace detail

template <typename Path, typename Lyt>
[[nodiscard]] path_collection<Path> enumerate_all_clocking_paths(const Lyt& layout, const coordinate<Lyt>& source,
                                                                 const coordinate<Lyt>&              target,
                                                                 enumerate_all_clocking_paths_params ps = {}) noexcept
{
    static_assert(is_clocked_layout_v<Lyt>, "Lyt is not a clocked layout");

    detail::enumerate_all_clocking_paths_impl<Lyt, Path> p{layout, source, target, ps};

    auto result = p.run();

    return result;
}

}  // namespace fiction

#endif  // FICTION_ENUMERATE_ALL_PATHS_HPP

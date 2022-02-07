//
// Created by marcel on 07.02.22.
//

#ifndef FICTION_ROUTING_UTILS_HPP
#define FICTION_ROUTING_UTILS_HPP

#include "fiction/traits.hpp"

#include <algorithm>
#include <vector>

namespace fiction
{

/**
 * Routing objectives are source-target pairs.
 *
 * @tparam Lyt Layout type whose coordinates are to be used.
 */
template <typename Lyt>
struct routing_objective
{
    const coordinate<Lyt>&source, target;
};
/**
 * A path in a layout defined as a ordered sequence of coordinates.
 *
 * @tparam Lyt Coordinate layout type.
 */
template <typename Lyt>
class layout_coordinate_path : public std::vector<coordinate<Lyt>>
{
  public:
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
/**
 * A collection of multiple paths in a layout.
 *
 * @tparam Path Path type.
 */
template <typename Path>
class path_collection : public std::vector<Path>
{
  public:
    void add(const Path& p) noexcept
    {
        this->push_back(p);
    }

    [[nodiscard]] bool contains(const Path& p) const noexcept
    {
        return std::find(std::cbegin(*this), std::cend(*this), p) != std::cend(*this);
    }
};
/**
 * Establishes a wire routing along the given path in the given layout. To this end, the given path's source and target
 * coordinates are assumed to be populated by other gates or wires that the new path shall connect to.
 *
 * This function overwrites any nodes that might already be placed at any of the path's coordinates other than source
 * and target. If path contains exactly source and target, no wires are created but source and target are connected.
 *
 * @tparam Lyt Gate-level layout type.
 * @tparam Path Path type.
 * @param lyt Gate-level layout in which a wire path is to be established.
 * @param path Path to route wires along.
 */
template <typename Lyt, typename Path>
void route_path(Lyt& lyt, const Path& path) noexcept
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    auto incoming_signal = static_cast<mockturtle::signal<Lyt>>(path.source());

    // exclude source and target
    std::for_each(path.cbegin() + 1, path.cend() - 1,
                  [&lyt, &incoming_signal](const auto& coord)
                  { incoming_signal = lyt.create_buf(incoming_signal, coord); });

    // establish final connection to target node
    lyt.connect(incoming_signal, lyt.get_node(path.target()));
}

}  // namespace fiction

#endif  // FICTION_ROUTING_UTILS_HPP

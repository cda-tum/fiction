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
 * Routing objectives are source-target pairs.
 *
 * @tparam Lyt Layout type whose coordinates are to be used.
 */
template <typename Lyt>
struct routing_objective
{
    const coordinate<Lyt>&source, target;
};

}  // namespace fiction

#endif  // FICTION_ROUTING_UTILS_HPP

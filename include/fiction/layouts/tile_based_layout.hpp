//
// Created by marcel on 21.07.21.
//

#ifndef FICTION_TILE_BASED_LAYOUT_HPP
#define FICTION_TILE_BASED_LAYOUT_HPP

#include "coordinate_layout.hpp"

#include <memory>

namespace fiction
{

class tile_based_layout : public coordinate_layout
{
  public:
#pragma region Types and constructors

    using tile = coordinate_layout::coordinate;

    using base_type = tile_based_layout;

    explicit tile_based_layout(const aspect_ratio& ar = {}) : coordinate_layout(ar) {}

    explicit tile_based_layout(std::shared_ptr<coordinate_layout_storage> s) : coordinate_layout(s) {}

#pragma endregion

#pragma region Iteration

    [[nodiscard]] auto tiles(const coord_t& start = {}, const coord_t& stop = {}) const
    {
        return coordinate_layout::coordinates(start, stop);
    }

    template <typename Fn>
    void foreach_tile(Fn&& fn, const coord_t& start = {}, const coord_t& stop = {}) const
    {
        coordinate_layout::foreach_coordinate(fn, start, stop);
    }

    [[nodiscard]] auto ground_tiles(const coord_t& start = {}, const coord_t& stop = {}) const
    {
        return coordinate_layout::ground_coordinates(start, stop);
    }

    template <typename Fn>
    void foreach_ground_tile(Fn&& fn, const coord_t& start = {}, const coord_t& stop = {}) const
    {
        coordinate_layout::foreach_ground_coordinate(fn, start, stop);
    }

    template <typename Container>
    Container adjacent_tiles(const tile& t) const noexcept
    {
        return coordinate_layout::adjacent_coordinates<Container>(t);
    }

#pragma endregion
};

}  // namespace fiction

#endif  // FICTION_TILE_BASED_LAYOUT_HPP

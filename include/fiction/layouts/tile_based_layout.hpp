//
// Created by marcel on 21.07.21.
//

#ifndef FICTION_TILE_BASED_LAYOUT_HPP
#define FICTION_TILE_BASED_LAYOUT_HPP

#include "fiction/traits.hpp"

#include <memory>

namespace fiction
{
/**
 * This class provides a tile-based naming scheme for coordinate-based functions. It does not add any functionality,
 * but it might be useful to adopt tile_based_layout to provide an intuition of abstraction in the code. For instance,
 * in a gate_level_layout, the coordinates, in fact, refer to groups of coordinates in lower-level abstractions. These
 * are called 'tiles' in the literature. Therefore, it might be helpful for a reader of the code to provide this
 * abstraction level-dependent information.
 *
 * @tparam CoordinateLayout A coordinate layout type.
 */
template <typename CoordinateLayout>
class tile_based_layout : public CoordinateLayout
{
  public:
#pragma region Types and constructors

    using tile = typename CoordinateLayout::coordinate;

    using base_type = tile_based_layout;

    explicit tile_based_layout(const typename CoordinateLayout::aspect_ratio_type& ar = {}) : CoordinateLayout(ar)
    {
        static_assert(is_coordinate_layout_v<CoordinateLayout>, "CoordinateLayout is not a coordinate layout type");
        static_assert(!is_clocked_layout_v<CoordinateLayout>, "CoordinateLayout cannot be a clocked layout type");
    }

    template <typename Storage>
    explicit tile_based_layout(std::shared_ptr<Storage> s) : CoordinateLayout(s)
    {
        static_assert(is_coordinate_layout_v<CoordinateLayout>, "CoordinateLayout is not a coordinate layout type");
        static_assert(!is_clocked_layout_v<CoordinateLayout>, "CoordinateLayout cannot be a clocked layout type");
    }

    explicit tile_based_layout(const CoordinateLayout& lyt) : CoordinateLayout(lyt)
    {
        static_assert(is_coordinate_layout_v<CoordinateLayout>, "CoordinateLayout is not a coordinate layout type");
        static_assert(!is_clocked_layout_v<CoordinateLayout>, "CoordinateLayout cannot be a clocked layout type");
    }

    [[nodiscard]] tile_based_layout clone() const noexcept
    {
        return tile_based_layout{CoordinateLayout::clone()};
    }

#pragma endregion

#pragma region Iteration

    [[nodiscard]] auto tiles(const tile& start = {}, const tile& stop = {}) const
    {
        return CoordinateLayout::coordinates(start, stop);
    }

    template <typename Fn>
    void foreach_tile(Fn&& fn, const tile& start = {}, const tile& stop = {}) const
    {
        CoordinateLayout::foreach_coordinate(std::forward<Fn>(fn), start, stop);
    }

    [[nodiscard]] auto ground_tiles(const tile& start = {}, const tile& stop = {}) const
    {
        return CoordinateLayout::ground_coordinates(start, stop);
    }

    template <typename Fn>
    void foreach_ground_tile(Fn&& fn, const tile& start = {}, const tile& stop = {}) const
    {
        CoordinateLayout::foreach_ground_coordinate(std::forward<Fn>(fn), start, stop);
    }

    std::vector<tile> adjacent_tiles(const tile& t) const noexcept
    {
        return CoordinateLayout::adjacent_coordinates(t);
    }

    template <typename Fn>
    void foreach_adjacent_tile(const tile& t, Fn&& fn) const
    {
        CoordinateLayout::foreach_adjacent_coordinate(t, std::forward<Fn>(fn));
    }

    std::vector<std::pair<tile, tile>> adjacent_opposite_tiles(const tile& t) const noexcept
    {
        return CoordinateLayout::adjacent_opposite_coordinates(t);
    }

    template <typename Fn>
    void foreach_adjacent_opposite_tiles(const tile& t, Fn&& fn) const
    {
        CoordinateLayout::foreach_adjacent_opposite_coordinates(t, std::forward<Fn>(fn));
    }

#pragma endregion
};

}  // namespace fiction

#endif  // FICTION_TILE_BASED_LAYOUT_HPP

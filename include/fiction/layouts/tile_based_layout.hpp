//
// Created by marcel on 21.07.21.
//

#ifndef FICTION_TILE_BASED_LAYOUT_HPP
#define FICTION_TILE_BASED_LAYOUT_HPP

#include <memory>

namespace fiction
{

template <typename CoordinateLayout>
class tile_based_layout : public CoordinateLayout
{
  public:
#pragma region Types and constructors

    using tile = typename CoordinateLayout::coordinate;

    using base_type = tile_based_layout;

    explicit tile_based_layout(const typename CoordinateLayout::aspect_ratio& ar = {}) : CoordinateLayout(ar) {}

    template <typename Storage>
    explicit tile_based_layout(std::shared_ptr<Storage> s) : CoordinateLayout(s)
    {}

#pragma endregion

#pragma region Iteration

    [[nodiscard]] auto tiles(const coord_t& start = {}, const coord_t& stop = {}) const
    {
        return CoordinateLayout::coordinates(start, stop);
    }

    template <typename Fn>
    void foreach_tile(Fn&& fn, const coord_t& start = {}, const coord_t& stop = {}) const
    {
        CoordinateLayout::foreach_coordinate(fn, start, stop);
    }

    [[nodiscard]] auto ground_tiles(const coord_t& start = {}, const coord_t& stop = {}) const
    {
        return CoordinateLayout::ground_coordinates(start, stop);
    }

    template <typename Fn>
    void foreach_ground_tile(Fn&& fn, const coord_t& start = {}, const coord_t& stop = {}) const
    {
        CoordinateLayout::foreach_ground_coordinate(fn, start, stop);
    }

    template <typename Container>
    Container adjacent_tiles(const tile& t) const noexcept
    {
        return CoordinateLayout::template adjacent_coordinates<Container>(t);
    }

    template <typename Fn>
    void foreach_adjacent_tile(const tile& t, Fn&& fn) const
    {
        CoordinateLayout::foreach_adjacent_coordinate(t, fn);
    }

#pragma endregion
};

}  // namespace fiction

#endif  // FICTION_TILE_BASED_LAYOUT_HPP

//
// Created by marcel on 15.09.21.
//

#ifndef FICTION_HEXAGONAL_LAYOUT_HPP
#define FICTION_HEXAGONAL_LAYOUT_HPP

#include "../utils/range.hpp"
#include "coordinates.hpp"

#include <mockturtle/networks/detail/foreach.hpp>

#include <algorithm>
#include <array>
#include <memory>
#include <set>
#include <type_traits>

namespace fiction
{

/**
 * \verbatim
 *
 *    / \
 *  /     \
 * |       |
 * |       |
 *  \     /
 *    \ /
 *
 */
struct pointy_top
{
    using orientation = pointy_top;
};

/**
 * \verbatim
 *    _____
 *   /     \
 *  /       \
 *  \       /
 *   \_____/
 *
 */
struct flat_top
{
    using orientation = flat_top;
};

/**
 * \verbatim
 *
 *        / \     / \     / \
 *      /     \ /     \ /     \
 *     | (0,0) | (1,0) | (2,0) |
 *     |       |       |       |
 *      \     / \     / \     /  \
 *        \ /     \ /     \ /     \
 *         | (0,1) | (1,1) | (2,1) |
 *         |       |       |       |
 *        / \     / \     /  \    /
 *      /     \ /     \ /     \ /
 *     | (0,2) | (1,2) | (2,2) |
 *     |       |       |       |
 *      \     / \     / \     /
 *        \ /     \ /     \ /
 */
struct odd_row : pointy_top
{};
/**
 * \verbatim
 *
 *        / \     / \     / \
 *      /     \ /     \ /     \
 *     | (0,0) | (1,0) | (2,0) |
 *     |       |       |       |
 *    / \     / \     / \     /
 *  /     \ /     \ /     \ /
 * | (0,1) | (1,1) | (2,1) |
 * |       |       |       |
 *  \     / \     / \     /  \
 *    \ /     \ /     \ /     \
 *     | (0,2) | (1,2) | (2,2) |
 *     |       |       |       |
 *      \     / \     / \     /
 *        \ /     \ /     \ /
 */
struct even_row : pointy_top
{};
/**
 * \verbatim
 *    _____         _____
 *   /     \       /     \
 *  / (0,0) \_____/ (2,0) \_____
 *  \       /     \       /     \
 *   \_____/ (1,0) \_____/ (3,0) \
 *   /     \       /     \       /
 *  / (0,1) \_____/ (2,1) \_____/
 *  \       /     \       /     \
 *   \_____/ (1,1) \_____/ (3,1) \
 *   /     \       /     \       /
 *  / (0,2) \_____/ (2,2) \_____/
 *  \       /     \       /
 *   \_____/       \_____/
 *
 */
struct odd_column : flat_top
{};

/**
 * \verbatim
 *           _____         _____
 *          /     \       /     \
 *    _____/ (1,0) \_____/ (3,0) \
 *   /     \       /     \       /
 *  / (0,0) \_____/ (2,0) \_____/
 *  \       /     \       /     \
 *   \_____/ (1,1) \_____/ (3,1) \
 *   /     \       /     \       /
 *  / (0,1) \_____/ (2,1) \_____/
 *  \       /     \       /     \
 *   \_____/ (1,2) \_____/ (3,2) \
 *         \       /     \       /
 *          \_____/       \_____/
 *
 */
struct even_column : flat_top
{};

/**
 * A layout type that utilizes offset coordinates to represent a hexagonal grid. It's faces are organized in a offset
 * coordinate system as provided. Hexagons can be in the 'pointy top' or 'flat top' orientation. Based on that, two
 * respectively possible coordinate systems emerge accordingly: 'odd row' and 'even row' for pointy tops and 'odd
 * column' and 'even column' for flat tops. Both are sketched in ASCII above.
 *
 * Other representations would be using cube or axial coordinates for instance, but since we want the layouts to be
 * rectangular-ish, offset coordinates make the most sense here.
 *
 * https://www.redblobgames.com/grids/hexagons/ is a wonderful resource on the topic.
 *
 * @tparam OffsetCoordinateType The coordinate implementation to be used. Offset coordinates are required.
 * @tparam OffsetCoordinateSystem One of the following: odd_row, even_row, odd_column, even_column.
 * @tparam CubeCoordinateType Internally, cube coordinates are needed for certain algorithms or calculations.
 */
template <typename OffsetCoordinateType, typename OffsetCoordinateSystem, typename CubeCoordinateType = cube::coord_t>
class hexagonal_layout
{
  public:
#pragma region Types and constructors

    using coordinate   = OffsetCoordinateType;
    using aspect_ratio = OffsetCoordinateType;

    struct hexagonal_layout_storage
    {
        explicit hexagonal_layout_storage(const aspect_ratio& ar) noexcept : dimension{ar} {};

        aspect_ratio dimension;
    };

    static constexpr auto min_fanin_size = 0;
    static constexpr auto max_fanin_size = 5;

    using base_type = hexagonal_layout;

    using hex_arrangement = OffsetCoordinateSystem;

    using storage = std::shared_ptr<hexagonal_layout_storage>;

    /**
     * Standard constructor. The given aspect ratio points to the highest possible coordinate in the layout. That means
     * in the even_column ASCII layout representation above ar = (3,2). Consequently, with ar = (0,0), the layout has
     * exactly one coordinate.
     *
     * @param ar Highest possible position in the layout.
     */
    explicit hexagonal_layout(const aspect_ratio& ar = {}) : strg{std::make_shared<hexagonal_layout_storage>(ar)}
    {
        static_assert(
            std::is_same_v<OffsetCoordinateSystem, odd_row> || std::is_same_v<OffsetCoordinateSystem, even_row> ||
                std::is_same_v<OffsetCoordinateSystem, odd_column> ||
                std::is_same_v<OffsetCoordinateSystem, even_column>,
            "OffsetCoordinateSystem has to be one of the following: odd_row, even_row, odd_column, even_column");
    }

    explicit hexagonal_layout(std::shared_ptr<hexagonal_layout_storage> s) : strg{std::move(s)} {}

#pragma endregion

#pragma region Structural properties

    [[nodiscard]] uint64_t x() const noexcept
    {
        return strg->dimension.x;
    }

    [[nodiscard]] uint64_t y() const noexcept
    {
        return strg->dimension.y;
    }

    [[nodiscard]] uint64_t z() const noexcept
    {
        return strg->dimension.z;
    }

    [[nodiscard]] uint64_t area() const noexcept
    {
        return x() + 1 * y() + 1;
    }

    void resize(const aspect_ratio& ar) noexcept
    {
        strg->dimension = ar;
    }

#pragma endregion

#pragma region Cardinal operations

    [[nodiscard]] constexpr OffsetCoordinateType north(const OffsetCoordinateType& c) const noexcept
    {
        if (c.y == 0ull)
            return c;

        auto nc = c;
        --nc.y;

        return nc;
    }

    [[nodiscard]] OffsetCoordinateType east(const OffsetCoordinateType& c) const noexcept
    {
        auto ec = c;

        if (c.x > x())
            ec.d = 1;
        else if (c.x < x())
            ++ec.x;

        return ec;
    }

    [[nodiscard]] OffsetCoordinateType south(const OffsetCoordinateType& c) const noexcept
    {
        auto sc = c;

        if (c.y > y())
            sc.d = 1;
        else if (c.y < y())
            ++sc.y;

        return sc;
    }

    [[nodiscard]] constexpr OffsetCoordinateType west(const OffsetCoordinateType& c) const noexcept
    {
        if (c.x == 0ull)
            return c;

        auto wc = c;
        --wc.x;

        return wc;
    }

    [[nodiscard]] OffsetCoordinateType above(const OffsetCoordinateType& c) const noexcept
    {
        auto ac = c;

        if (c.z > z())
            ac.d = 1;
        else if (c.z < z())
            ++ac.z;

        return ac;
    }

    [[nodiscard]] constexpr OffsetCoordinateType below(const OffsetCoordinateType& c) const noexcept
    {
        if (c.z == 0ull)
            return c;

        auto bc = c;
        --bc.z;

        return bc;
    }

//    [[nodiscard]] constexpr bool is_north_of(const OffsetCoordinateType& c1,
//                                             const OffsetCoordinateType& c2) const noexcept
//    {
//        return c1 != c2 && north(c1) == c2;
//    }
//
//    [[nodiscard]] bool is_east_of(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
//    {
//        return c1 != c2 && east(c1) == c2;
//    }
//
//    [[nodiscard]] bool is_south_of(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
//    {
//        return c1 != c2 && south(c1) == c2;
//    }
//
//    [[nodiscard]] constexpr bool is_west_of(const OffsetCoordinateType& c1,
//                                            const OffsetCoordinateType& c2) const noexcept
//    {
//        return c1 != c2 && west(c1) == c2;
//    }
//
//    [[nodiscard]] bool is_adjacent_of(const OffsetCoordinateType& c1,
//                                      const OffsetCoordinateType& c2) const noexcept
//    {
//        return is_north_of(c1, c2) || is_east_of(c1, c2) || is_south_of(c1, c2) || is_west_of(c1, c2);
//    }
//
//    [[nodiscard]] bool is_adjacent_elevation_of(const OffsetCoordinateType& c1,
//                                                const OffsetCoordinateType& c2) const noexcept
//    {
//        return is_adjacent_of(c1, c2) || is_north_of(c1, above(c2)) || is_east_of(c1, above(c2)) ||
//               is_south_of(c1, above(c2)) || is_west_of(c1, above(c2)) || is_north_of(c1, below(c2)) ||
//               is_east_of(c1, below(c2)) || is_south_of(c1, below(c2)) || is_west_of(c1, below(c2));
//    }
//
//    [[nodiscard]] bool is_above_of(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
//    {
//        return c1 != c2 && above(c1) == c2;
//    }
//
//    [[nodiscard]] constexpr bool is_below_of(const OffsetCoordinateType& c1,
//                                             const OffsetCoordinateType& c2) const noexcept
//    {
//        return c1 != c2 && below(c1) == c2;
//    }
//
//    [[nodiscard]] constexpr bool is_northwards_of(const OffsetCoordinateType& c1,
//                                                  const OffsetCoordinateType& c2) const noexcept
//    {
//        return (c1.z == c2.z) && (c1.y > c2.y) && (c1.x == c2.x);
//    }
//
//    [[nodiscard]] constexpr bool is_eastwards_of(const OffsetCoordinateType& c1,
//                                                 const OffsetCoordinateType& c2) const noexcept
//    {
//        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x < c2.x);
//    }
//
//    [[nodiscard]] constexpr bool is_southwards_of(const OffsetCoordinateType& c1,
//                                                  const OffsetCoordinateType& c2) const noexcept
//    {
//        return (c1.z == c2.z) && (c1.y < c2.y) && (c1.x == c2.x);
//    }
//
//    [[nodiscard]] constexpr bool is_westwards_of(const OffsetCoordinateType& c1,
//                                                 const OffsetCoordinateType& c2) const noexcept
//    {
//        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x > c2.x);
//    }
//
//    [[nodiscard]] constexpr bool is_northern_border(const OffsetCoordinateType& c) const noexcept
//    {
//        return c.y == 0ull;
//    }
//
//    [[nodiscard]] bool is_eastern_border(const OffsetCoordinateType& c) const noexcept
//    {
//        return c.x == x();
//    }
//
//    [[nodiscard]] bool is_southern_border(const OffsetCoordinateType& c) const noexcept
//    {
//        return c.y == y();
//    }
//
//    [[nodiscard]] constexpr bool is_western_border(const OffsetCoordinateType& c) const noexcept
//    {
//        return c.x == 0ull;
//    }
//
//    [[nodiscard]] bool is_border(const OffsetCoordinateType& c) const noexcept
//    {
//        return is_northern_border(c) || is_eastern_border(c) || is_southern_border(c) || is_western_border(c);
//    }
//
//    [[nodiscard]] OffsetCoordinateType northern_border_of(const OffsetCoordinateType& c) const noexcept
//    {
//        return {c.x, 0ull, c.z};
//    }
//
//    [[nodiscard]] OffsetCoordinateType eastern_border_of(const OffsetCoordinateType& c) const noexcept
//    {
//        return {x(), c.y, c.z};
//    }
//
//    [[nodiscard]] OffsetCoordinateType southern_border_of(const OffsetCoordinateType& c) const noexcept
//    {
//        return {c.x, y(), c.z};
//    }
//
//    [[nodiscard]] OffsetCoordinateType western_border_of(const OffsetCoordinateType& c) const noexcept
//    {
//        return {0ull, c.y, c.z};
//    }
//
//    [[nodiscard]] constexpr bool is_ground_layer(const OffsetCoordinateType& c) const noexcept
//    {
//        return c.z == 0ull;
//    }
//
//    [[nodiscard]] constexpr bool is_crossing_layer(const OffsetCoordinateType& c) const noexcept
//    {
//        return c.z > 0ull;
//    }

#pragma endregion

#pragma region Iteration

    [[nodiscard]] auto coordinates(const OffsetCoordinateType& start = {}, const OffsetCoordinateType& stop = {}) const
    {
        return range_t{std::make_pair(
            offset::coord_iterator{strg->dimension, start.is_dead() ? OffsetCoordinateType{0, 0} : start},
            offset::coord_iterator{strg->dimension, stop.is_dead() ? strg->dimension.get_dead() : stop})};
    }

    template <typename Fn>
    void foreach_coordinate(Fn&& fn, const OffsetCoordinateType& start = {},
                            const OffsetCoordinateType& stop = {}) const
    {
        mockturtle::detail::foreach_element(
            offset::coord_iterator{strg->dimension, start.is_dead() ? OffsetCoordinateType{0, 0} : start},
            offset::coord_iterator{strg->dimension, stop.is_dead() ? strg->dimension.get_dead() : stop}, fn);
    }

    [[nodiscard]] auto ground_coordinates(const OffsetCoordinateType& start = {},
                                          const OffsetCoordinateType& stop  = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        auto ground_layer = aspect_ratio{x(), y(), 0};

        return range_t{
            std::make_pair(offset::coord_iterator{ground_layer, start.is_dead() ? OffsetCoordinateType{0, 0} : start},
                           offset::coord_iterator{ground_layer, stop.is_dead() ? ground_layer.get_dead() : stop})};
    }

    template <typename Fn>
    void foreach_ground_coordinate(Fn&& fn, const OffsetCoordinateType& start = {},
                                   const OffsetCoordinateType& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        auto ground_layer = aspect_ratio{x(), y(), 0};

        mockturtle::detail::foreach_element(
            offset::coord_iterator{ground_layer, start.is_dead() ? OffsetCoordinateType{0, 0} : start},
            offset::coord_iterator{ground_layer, stop.is_dead() ? ground_layer.get_dead() : stop}, fn);
    }

    template <typename Container>
    Container adjacent_coordinates(const OffsetCoordinateType& c) const noexcept
    {
        Container cnt{};

        // six possible directions in cube coordinates
        constexpr const std::array<CubeCoordinateType, 6> cube_directions{
            {{+1, -1, 0}, {+1, 0, -1}, {0, +1, -1}, {-1, +1, 0}, {-1, 0, +1}, {0, -1, +1}}};

        // for each direction
        std::for_each(cube_directions.cbegin(), cube_directions.cend(),
                      [this, &cnt, &c](const auto& dir)
                      {
                          // convert given coordinate to the cube system, add direction, and convert back to offset
                          auto neighbor = to_offset_coordinate(to_cube_coordinate(c) + dir);
                          // since cube coordinates don't carry the layer information, it has to be manually added
                          neighbor.z = c.z;

                          // add neighboring coordinate if there was no over-/underflow
                          if (!(neighbor.x > strg->dimension.x || neighbor.y > strg->dimension.y))
                          {
                              cnt.insert(cnt.end(), neighbor);
                          }
                      });

        return cnt;
    }

    template <typename Fn>
    void foreach_adjacent_coordinate(const OffsetCoordinateType& c, Fn&& fn) const
    {
        const auto adj = adjacent_coordinates<std::set<OffsetCoordinateType>>(c);

        mockturtle::detail::foreach_element(adj.cbegin(), adj.cend(), fn);
    }

#pragma endregion

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wconversion"

#pragma region coordinates

    // adapted from https://www.redblobgames.com/grids/hexagons/codegen/output/lib.cpp
    [[nodiscard]] constexpr CubeCoordinateType
    to_cube_coordinate(const OffsetCoordinateType& offset_coord) const noexcept
    {
        CubeCoordinateType cube_coord{0, 0, 0};

        constexpr auto offset =
            std::is_same_v<OffsetCoordinateSystem, odd_row> || std::is_same_v<OffsetCoordinateSystem, odd_column> ? -1 :
                                                                                                                    1;

        if constexpr (std::is_same_v<OffsetCoordinateSystem, odd_row> ||
                      std::is_same_v<OffsetCoordinateSystem, even_row>)
        {
            cube_coord.x = offset_coord.x -
                           static_cast<decltype(cube_coord.x)>((offset_coord.y + offset * (offset_coord.y & 1)) / 2);
            cube_coord.y = offset_coord.y;
            cube_coord.z = -cube_coord.x - cube_coord.y;
        }
        else if constexpr (std::is_same_v<OffsetCoordinateSystem, odd_column> ||
                           std::is_same_v<OffsetCoordinateSystem, even_column>)
        {
            cube_coord.x = offset_coord.x;
            cube_coord.y = offset_coord.y -
                           static_cast<decltype(cube_coord.y)>((offset_coord.x + offset * (offset_coord.x & 1)) / 2);
            cube_coord.z = -cube_coord.x - cube_coord.y;
        }

        return cube_coord;
    }

    // adapted from https://www.redblobgames.com/grids/hexagons/codegen/output/lib.cpp
    [[nodiscard]] constexpr OffsetCoordinateType
    to_offset_coordinate(const CubeCoordinateType& cube_coord) const noexcept
    {
        // the generated coordinate will be in ground layer
        OffsetCoordinateType offset_coord{0, 0};

        constexpr auto offset =
            std::is_same_v<OffsetCoordinateSystem, odd_row> || std::is_same_v<OffsetCoordinateSystem, odd_column> ? -1 :
                                                                                                                    1;

        if constexpr (std::is_same_v<OffsetCoordinateSystem, odd_row> ||
                      std::is_same_v<OffsetCoordinateSystem, even_row>)
        {
            offset_coord.x = static_cast<decltype(offset_coord.x)>(cube_coord.x) +
                             static_cast<decltype(offset_coord.x)>((cube_coord.y + offset * (cube_coord.y & 1)) / 2);
            offset_coord.y = static_cast<decltype(offset_coord.y)>(cube_coord.y);
        }
        else if constexpr (std::is_same_v<OffsetCoordinateSystem, odd_column> ||
                           std::is_same_v<OffsetCoordinateSystem, even_column>)
        {
            offset_coord.x = static_cast<decltype(offset_coord.x)>(cube_coord.x);
            offset_coord.y = static_cast<decltype(offset_coord.y)>(cube_coord.y) +
                             static_cast<decltype(offset_coord.y)>((cube_coord.x + offset * (cube_coord.x & 1)) / 2);
        }

        return offset_coord;
    }

#pragma endregion

#pragma GCC diagnostic pop

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_HEXAGONAL_LAYOUT_HPP

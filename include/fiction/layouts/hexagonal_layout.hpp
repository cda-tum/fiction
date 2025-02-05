//
// Created by marcel on 15.09.21.
//

#ifndef FICTION_HEXAGONAL_LAYOUT_HPP
#define FICTION_HEXAGONAL_LAYOUT_HPP

#include "fiction/layouts/coordinates.hpp"
#include "fiction/utils/range.hpp"

#include <mockturtle/networks/detail/foreach.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * \verbatim
     / \
   /     \
  |       |
  |       |
   \     /
     \ /
  \endverbatim
 */
struct pointy_top_hex
{
    using orientation = pointy_top_hex;
};

/**
 * \verbatim
     _____
    /     \
   /       \
   \       /
    \_____/
  \endverbatim
 */
struct flat_top_hex
{
    using orientation = flat_top_hex;
};

/**
 * \verbatim
         / \     / \     / \
       /     \ /     \ /     \
      | (0,0) | (1,0) | (2,0) |
      |       |       |       |
       \     / \     / \     / \
         \ /     \ /     \ /     \
          | (0,1) | (1,1) | (2,1) |
          |       |       |       |
         / \     / \     / \     /
       /     \ /     \ /     \ /
      | (0,2) | (1,2) | (2,2) |
      |       |       |       |
       \     / \     / \     /
         \ /     \ /     \ /
  \endverbatim
 */
struct odd_row_hex : pointy_top_hex
{};
/**
 * \verbatim
         / \     / \     / \
       /     \ /     \ /     \
      | (0,0) | (1,0) | (2,0) |
      |       |       |       |
     / \     / \     / \     /
   /     \ /     \ /     \ /
  | (0,1) | (1,1) | (2,1) |
  |       |       |       |
   \     / \     / \     / \
     \ /     \ /     \ /     \
      | (0,2) | (1,2) | (2,2) |
      |       |       |       |
       \     / \     / \     /
         \ /     \ /     \ /
  \endverbatim
 */
struct even_row_hex : pointy_top_hex
{};
/**
 * \verbatim
     _____         _____
    /     \       /     \
   / (0,0) \_____/ (2,0) \_____
   \       /     \       /     \
    \_____/ (1,0) \_____/ (3,0) \
    /     \       /     \       /
   / (0,1) \_____/ (2,1) \_____/
   \       /     \       /     \
    \_____/ (1,1) \_____/ (3,1) \
    /     \       /     \       /
   / (0,2) \_____/ (2,2) \_____/
   \       /     \       /
    \_____/       \_____/
  \endverbatim
 */
struct odd_column_hex : flat_top_hex
{};

/**
 * \verbatim
            _____         _____
           /     \       /     \
     _____/ (1,0) \_____/ (3,0) \
    /     \       /     \       /
   / (0,0) \_____/ (2,0) \_____/
   \       /     \       /     \
    \_____/ (1,1) \_____/ (3,1) \
    /     \       /     \       /
   / (0,1) \_____/ (2,1) \_____/
   \       /     \       /     \
    \_____/ (1,2) \_____/ (3,2) \
          \       /     \       /
           \_____/       \_____/
  \endverbatim
 */
struct even_column_hex : flat_top_hex
{};

/**
 * A layout type that utilizes offset coordinates to represent a hexagonal grid. Its faces are organized in an offset
 * coordinate system as provided. Hexagons can be in the pointy_top_hex or flat_top_hex orientation. Based on that, two
 * respectively possible coordinate systems emerge accordingly: odd_row_hex and even_row_hex for pointy tops and
 * odd_column_hex and even_column_hex for flat tops. All are sketched in ASCII above.
 *
 * Other representations would be using cube or axial coordinates for instance, but since we want the layouts to be
 * rectangular-ish, offset coordinates make the most sense here.
 *
 * https://www.redblobgames.com/grids/hexagons/ is a wonderful resource on the topic.
 *
 * @tparam OffsetCoordinateType The coordinate implementation to be used. Offset coordinates are required.
 * @tparam HexagonalCoordinateSystem One of the following: odd_row_hex, even_row_hex, odd_column_hex, even_column_hex.
 * @tparam CubeCoordinateType Internally, cube coordinates are needed for certain algorithms or calculations.
 */
template <typename OffsetCoordinateType = offset::ucoord_t, typename HexagonalCoordinateSystem = even_row_hex,
          typename CubeCoordinateType = cube::coord_t>
class hexagonal_layout
{
  public:
#pragma region Types and constructors

    using coordinate   = OffsetCoordinateType;
    using aspect_ratio_type = aspect_ratio<OffsetCoordinateType>;

    using cube_coordinate = CubeCoordinateType;

    struct hexagonal_layout_storage
    {
        explicit hexagonal_layout_storage(const aspect_ratio_type& ar) noexcept : dimension{ar} {};

        aspect_ratio_type dimension;
    };

    static constexpr auto min_fanin_size = 0u;  // NOLINT(readability-identifier-naming): mockturtle requirement
    static constexpr auto max_fanin_size = 5u;  // NOLINT(readability-identifier-naming): mockturtle requirement

    using base_type = hexagonal_layout;

    using hex_arrangement = HexagonalCoordinateSystem;

    using storage = std::shared_ptr<hexagonal_layout_storage>;

    /**
     * Standard constructor. The given aspect ratio points to the highest possible coordinate in the layout. That means
     * in the even_column_hex ASCII layout representation above `ar = (3,2)`. Consequently, with `ar = (0,0)`, the
     * layout has exactly one coordinate.
     *
     * @param ar Highest possible position in the layout.
     */
    explicit hexagonal_layout(const aspect_ratio_type& ar = {}) : strg{std::make_shared<hexagonal_layout_storage>(ar)}
    {
        static_assert(std::is_same_v<HexagonalCoordinateSystem, odd_row_hex> ||
                          std::is_same_v<HexagonalCoordinateSystem, even_row_hex> ||
                          std::is_same_v<HexagonalCoordinateSystem, odd_column_hex> ||
                          std::is_same_v<HexagonalCoordinateSystem, even_column_hex>,
                      "HexagonalCoordinateSystem has to be one of the following: odd_row_hex, even_row_hex, "
                      "odd_column_hex, even_column_hex");
    }

    explicit hexagonal_layout(std::shared_ptr<hexagonal_layout_storage> s) : strg{std::move(s)}
    {
        static_assert(std::is_same_v<HexagonalCoordinateSystem, odd_row_hex> ||
                          std::is_same_v<HexagonalCoordinateSystem, even_row_hex> ||
                          std::is_same_v<HexagonalCoordinateSystem, odd_column_hex> ||
                          std::is_same_v<HexagonalCoordinateSystem, even_column_hex>,
                      "HexagonalCoordinateSystem has to be one of the following: odd_row_hex, even_row_hex, "
                      "odd_column_hex, even_column_hex");
    }
    /**
     * Clones the layout returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] hexagonal_layout clone() const noexcept
    {
        return hexagonal_layout(std::make_shared<hexagonal_layout_storage>(*strg));
    }
    /**
     * Creates and returns a coordinate in the layout from the given x-, y-, and z-values.
     *
     * @note This function is equivalent to calling `OffsetCoordinateType(x, y, z)`.
     *
     * @tparam X x-type.
     * @tparam Y y-type.
     * @tparam Z z-type.
     * @param x x-value.
     * @param y y-value.
     * @param z z-value.
     * @return A coordinate in the layout of type `OffsetCoordinateType`.
     */
    template <typename X, typename Y, typename Z = uint64_t>
    constexpr OffsetCoordinateType coord(const X x, const Y y, const Z z = 0ul) const noexcept
    {
        return OffsetCoordinateType(x, y, z);
    }

#pragma endregion

#pragma region Structural properties
    /**
     * Returns the layout's x-dimension, i.e., returns the biggest x-value that still belongs to the layout.
     *
     * @return x-dimension.
     */
    [[nodiscard]] uint64_t x() const noexcept
    {
        return strg->dimension.x();
    }
    /**
     * Returns the layout's y-dimension, i.e., returns the biggest y-value that still belongs to the layout.
     *
     * @return y-dimension.
     */
    [[nodiscard]] uint64_t y() const noexcept
    {
        return strg->dimension.y();
    }
    /**
     * Returns the layout's z-dimension, i.e., returns the biggest z-value that still belongs to the layout.
     *
     * @return z-dimension.
     */
    [[nodiscard]] uint64_t z() const noexcept
    {
        return strg->dimension.z();
    }
    /**
     * Returns the layout's number of faces depending on the coordinate type.
     *
     * @return Area of layout.
     */
    [[nodiscard]] uint64_t area() const noexcept
    {
        return strg->dimension.area();
    }
    /**
     * Updates the layout's dimensions, effectively resizing it.
     *
     * @param ar New aspect ratio.
     */
    void resize(const aspect_ratio_type& ar) noexcept
    {
        strg->dimension = ar;
    }
    /**
     * Overloaded resize method to accept a coordinate.
     *
     * This method resizes the layout by creating an aspect_ratio_type from the provided max coordinate,
     * with the origin remaining unchanged.
     *
     * @param max The new end coordinate defining the layout's size.
     */
    //    void resize(const coordinate& max) noexcept
    //    {
    //        resize(aspect_ratio_type{max});
    //    }
    //    void resize(const aspect_ratio_type& ar) noexcept
    //    {
    //        strg->dimension = ar;
    //    }

#pragma endregion

#pragma region row / column detection
    /**
     * Checks if the given coordinate is located in a row with an odd index.
     *
     * @param c Coordinate to check.
     * @return `true` iff `c` is located in an odd row.
     */
    [[nodiscard]] bool is_in_odd_row(const OffsetCoordinateType& c) const noexcept
    {
        return c.y % 2 == 1;
    }
    /**
     * Checks if the given coordinate is located in a row with an even index.
     *
     * @param c Coordinate to check.
     * @return `true` iff `c` is located in an even row.
     */
    [[nodiscard]] bool is_in_even_row(const OffsetCoordinateType& c) const noexcept
    {
        return c.y % 2 == 0;
    }
    /**
     * Checks if the given coordinate is located in a column with an odd index.
     *
     * @param c Coordinate to check.
     * @return `true` iff `c` is located in an odd column.
     */
    [[nodiscard]] bool is_in_odd_column(const OffsetCoordinateType& c) const noexcept
    {
        return c.x % 2 == 1;
    }
    /**
     * Checks if the given coordinate is located in a column with an even index.
     *
     * @param c Coordinate to check.
     * @return `true` iff `c` is located in an even column.
     */
    [[nodiscard]] bool is_in_even_column(const OffsetCoordinateType& c) const noexcept
    {
        return c.x % 2 == 0;
    }

#pragma endregion

#pragma region Cardinal operations
    /**
     * Returns the coordinate that is directly adjacent in northern direction of a given coordinate `c`, i.e., the face
     * whose y-dimension is lower by 1. If `c`'s y-dimension is already at minimum, `c` is returned instead.
     *
     * @param c Coordinate whose northern counterpart is desired.
     * @return Coordinate adjacent and north of `c`.
     */
    [[nodiscard]] constexpr OffsetCoordinateType north(const OffsetCoordinateType& c) const noexcept
    {
        if (c.y == 0ull)
        {
            return c;
        }

        auto nc = c;
        --nc.y;

        return nc;
    }
    /**
     * Returns the coordinate that is located in north-eastern direction of a given coordinate `c`. Depending on the
     * hexagonal orientation of the layout, the dimension values of the returned coordinate may differ.
     *
     * @param c Coordinate whose north-eastern counterpart is desired.
     * @return Coordinate directly north-eastern of `c`.
     */
    [[nodiscard]] constexpr OffsetCoordinateType north_east(const OffsetCoordinateType& c) const noexcept
    {
        auto ne = to_offset_coordinate(to_cube_coordinate(c) + CubeCoordinateType{+1, 0, -1});

        ne.z = c.z;

        return is_within_bounds(ne) ? ne : c;
    }
    /**
     * Returns the coordinate that is directly adjacent in eastern direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is higher by 1. If `c`'s x-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose eastern counterpart is desired.
     * @return Coordinate adjacent and east of `c`.
     */
    [[nodiscard]] OffsetCoordinateType east(const OffsetCoordinateType& c) const noexcept
    {
        auto ec = c;

        if (c.x > x())
        {
            ec.d = 1;
        }
        else if (c.x < x())
        {
            ++ec.x;
        }

        return ec;
    }
    /**
     * Returns the coordinate that is located in south-eastern direction of a given coordinate `c`. Depending on the
     * hexagonal orientation of the layout, the dimension values of the returned coordinate may differ.
     *
     * @param c Coordinate whose south-eastern counterpart is desired.
     * @return Coordinate directly south-eastern of `c`.
     */
    [[nodiscard]] constexpr OffsetCoordinateType south_east(const OffsetCoordinateType& c) const noexcept
    {
        if constexpr (std::is_same_v<typename hex_arrangement::orientation, pointy_top_hex>)
        {
            auto se = to_offset_coordinate(to_cube_coordinate(c) + CubeCoordinateType{0, -1, +1});

            se.z = c.z;

            return is_within_bounds(se) ? se : c;
        }
        else
        {
            auto se = to_offset_coordinate(to_cube_coordinate(c) + CubeCoordinateType{+1, -1, 0});

            se.z = c.z;

            return is_within_bounds(se) ? se : c;
        }
    }
    /**
     * Returns the coordinate that is directly adjacent in southern direction of a given coordinate `c`, i.e., the face
     * whose y-dimension is higher by 1. If `c`'s y-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose southern counterpart is desired.
     * @return Coordinate adjacent and south of `c`.
     */
    [[nodiscard]] OffsetCoordinateType south(const OffsetCoordinateType& c) const noexcept
    {
        auto sc = c;

        if (c.y > y())
        {
            sc.d = 1;
        }
        else if (c.y < y())
        {
            ++sc.y;
        }

        return sc;
    }
    /**
     * Returns the coordinate that is located in south-western direction of a given coordinate `c`. Depending on the
     * hexagonal orientation of the layout, the dimension values of the returned coordinate may differ.
     *
     * @param c Coordinate whose south-western counterpart is desired.
     * @return Coordinate directly south-western of `c`.
     */
    [[nodiscard]] constexpr OffsetCoordinateType south_west(const OffsetCoordinateType& c) const noexcept
    {
        auto sw = to_offset_coordinate(to_cube_coordinate(c) + CubeCoordinateType{-1, 0, +1});

        sw.z = c.z;

        return is_within_bounds(sw) ? sw : c;
    }
    /**
     * Returns the coordinate that is directly adjacent in western direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is lower by 1. If `c`'s x-dimension is already at minimum, `c` is returned instead.
     *
     * @param c Coordinate whose western counterpart is desired.
     * @return Coordinate adjacent and west of `c`.
     */
    [[nodiscard]] constexpr OffsetCoordinateType west(const OffsetCoordinateType& c) const noexcept
    {
        if (c.x == 0ull)
        {
            return c;
        }

        auto wc = c;
        --wc.x;

        return wc;
    }
    /**
     * Returns the coordinate that is located in north-western direction of a given coordinate `c`. Depending on the
     * hexagonal orientation of the layout, the dimension values of the returned coordinate may differ.
     *
     * @param c Coordinate whose north-western counterpart is desired.
     * @return Coordinate directly north-western of `c`.
     */
    [[nodiscard]] constexpr OffsetCoordinateType north_west(const OffsetCoordinateType& c) const noexcept
    {
        if constexpr (std::is_same_v<typename hex_arrangement::orientation, pointy_top_hex>)
        {
            auto nw = to_offset_coordinate(to_cube_coordinate(c) + CubeCoordinateType{0, +1, -1});

            nw.z = c.z;

            return is_within_bounds(nw) ? nw : c;
        }
        else
        {
            auto nw = to_offset_coordinate(to_cube_coordinate(c) + CubeCoordinateType{-1, +1, 0});

            nw.z = c.z;

            return is_within_bounds(nw) ? nw : c;
        }
    }
    /**
     * Returns the coordinate that is directly above a given coordinate `c`, i.e., the face whose z-dimension is higher
     * by 1. If `c`'s z-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose above counterpart is desired.
     * @return Coordinate directly above `c`.
     */
    [[nodiscard]] OffsetCoordinateType above(const OffsetCoordinateType& c) const noexcept
    {
        auto ac = c;

        if (c.z > z())
        {
            ac.d = 1;
        }
        else if (c.z < z())
        {
            ++ac.z;
        }

        return ac;
    }
    /**
     * Returns the coordinate that is directly below a given coordinate `c`, i.e., the face whose z-dimension is lower
     * by 1. If `c`'s z-dimension is already at minimum, `c` is returned instead.
     *
     * @param c Coordinate whose below counterpart is desired.
     * @return Coordinate directly below `c`.
     */
    [[nodiscard]] constexpr OffsetCoordinateType below(const OffsetCoordinateType& c) const noexcept
    {
        if (c.z == 0ull)
        {
            return c;
        }

        auto bc = c;
        --bc.z;

        return bc;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly north of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly north of `c1`.
     */
    [[nodiscard]] constexpr bool is_north_of(const OffsetCoordinateType& c1,
                                             const OffsetCoordinateType& c2) const noexcept
    {
        return c1 != c2 && north(c1) == c2;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly east of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly east of `c1`.
     */
    [[nodiscard]] bool is_east_of(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
    {
        return c1 != c2 && east(c1) == c2;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly south of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly south of `c1`.
     */
    [[nodiscard]] bool is_south_of(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
    {
        return c1 != c2 && south(c1) == c2;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly west of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly west of `c1`.
     */
    [[nodiscard]] constexpr bool is_west_of(const OffsetCoordinateType& c1,
                                            const OffsetCoordinateType& c2) const noexcept
    {
        return c1 != c2 && west(c1) == c2;
    }
    /**
     * Returns `true` iff coordinate `c2` is either north, north-east, east, south-east, south, south-west, west, or
     * north-west of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly adjacent to `c1` in one of the six different ordinal directions possible for
     * the layout's hexagonal orientation.
     */
    [[nodiscard]] bool is_adjacent_of(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
    {
        bool is_adjacent = false;

        foreach_adjacent_coordinate(c1,
                                    [&c2, &is_adjacent](const auto& ac1)
                                    {
                                        if (ac1 == c2)
                                        {
                                            is_adjacent = true;
                                        }
                                    });

        return is_adjacent;
    }
    /**
     * Similar to is_adjacent_of but also considers `c1`'s elevation, i.e., if `c2` is adjacent to `above(c1)` or
     * `below(c1)`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is either adjacent of `c1` or `c1`'s elevations.
     */
    [[nodiscard]] bool is_adjacent_elevation_of(const OffsetCoordinateType& c1,
                                                const OffsetCoordinateType& c2) const noexcept
    {
        return is_adjacent_of(c1, c2) || is_adjacent_of(above(c1), c2) || is_adjacent_of(below(c1), c2);
    }
    /**
     * Returns `true` iff coordinate `c2` is directly above coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly above `c1`.
     */
    [[nodiscard]] bool is_above(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
    {
        return c1 != c2 && above(c1) == c2;
    }
    /**
     * Returns `true` iff coordinate `c2` is directly below coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is directly below `c1`.
     */
    [[nodiscard]] constexpr bool is_below(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
    {
        return c1 != c2 && below(c1) == c2;
    }
    /**
     * Returns `true` iff coordinate `c2` is somewhere north of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is somewhere north of `c1`.
     */
    [[nodiscard]] constexpr bool is_northwards_of(const OffsetCoordinateType& c1,
                                                  const OffsetCoordinateType& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y > c2.y) && (c1.x == c2.x);
    }
    /**
     * Returns `true` iff coordinate `c2` is somewhere east of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is somewhere east of `c1`.
     */
    [[nodiscard]] constexpr bool is_eastwards_of(const OffsetCoordinateType& c1,
                                                 const OffsetCoordinateType& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x < c2.x);
    }
    /**
     * Returns `true` iff coordinate `c2` is somewhere south of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is somewhere south of `c1`.
     */
    [[nodiscard]] constexpr bool is_southwards_of(const OffsetCoordinateType& c1,
                                                  const OffsetCoordinateType& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y < c2.y) && (c1.x == c2.x);
    }
    /**
     * Returns `true` iff coordinate `c2` is somewhere west of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is somewhere west of `c1`.
     */
    [[nodiscard]] constexpr bool is_westwards_of(const OffsetCoordinateType& c1,
                                                 const OffsetCoordinateType& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x > c2.x);
    }
    /**
     * Returns whether the given coordinate is located at the layout's northern border where y is minimal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's northern border.
     */
    [[nodiscard]] constexpr bool is_at_northern_border(const OffsetCoordinateType& c) const noexcept
    {
        return c.y == 0ull;
    }
    /**
     * Returns whether the given coordinate is located at the layout's eastern border where x is maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's northern border.
     */
    [[nodiscard]] bool is_at_eastern_border(const OffsetCoordinateType& c) const noexcept
    {
        return c.x == x();
    }
    /**
     * Returns whether the given coordinate is located at the layout's southern border where y is maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's southern border.
     */
    [[nodiscard]] bool is_at_southern_border(const OffsetCoordinateType& c) const noexcept
    {
        return c.y == y();
    }
    /**
     * Returns whether the given coordinate is located at the layout's western border where x is minimal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's western border.
     */
    [[nodiscard]] constexpr bool is_at_western_border(const OffsetCoordinateType& c) const noexcept
    {
        return c.x == 0ull;
    }
    /**
     * Returns whether the given coordinate is located at any of the layout's borders where x or y are either minimal or
     * maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at any of the layout's borders.
     */
    [[nodiscard]] bool is_at_any_border(const OffsetCoordinateType& c) const noexcept
    {
        return is_at_northern_border(c) || is_at_eastern_border(c) || is_at_southern_border(c) ||
               is_at_western_border(c);
    }
    /**
     * Returns the coordinate with the same x and z values as a given coordinate but that is located at the layout's
     * northern border.
     *
     * @param c Coordinate whose border counterpart is desired.
     * @return The northern border equivalent of `c`.
     */
    [[nodiscard]] OffsetCoordinateType northern_border_of(const OffsetCoordinateType& c) const noexcept
    {
        return {c.x, 0ull, c.z};
    }
    /**
     * Returns the coordinate with the same y and z values as a given coordinate but that is located at the layout's
     * eastern border.
     *
     * @param c Coordinate whose border counterpart is desired.
     * @return The eastern border equivalent of `c`.
     */
    [[nodiscard]] OffsetCoordinateType eastern_border_of(const OffsetCoordinateType& c) const noexcept
    {
        return {x(), c.y, c.z};
    }
    /**
     * Returns the coordinate with the same x and z values as a given coordinate but that is located at the layout's
     * southern border.
     *
     * @param c Coordinate whose border counterpart is desired.
     * @return The southern border equivalent of `c`.
     */
    [[nodiscard]] OffsetCoordinateType southern_border_of(const OffsetCoordinateType& c) const noexcept
    {
        return {c.x, y(), c.z};
    }
    /**
     * Returns the coordinate with the same y and z values as a given coordinate but that is located at the layout's
     * western border.
     *
     * @param c Coordinate whose border counterpart is desired.
     * @return The western border equivalent of `c`.
     */
    [[nodiscard]] OffsetCoordinateType western_border_of(const OffsetCoordinateType& c) const noexcept
    {
        return {0ull, c.y, c.z};
    }
    /**
     * Returns whether the given coordinate is located in the ground layer where z is minimal.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c` is in ground layer.
     */
    [[nodiscard]] constexpr bool is_ground_layer(const OffsetCoordinateType& c) const noexcept
    {
        return c.z == 0ull;
    }
    /**
     * Returns whether the given coordinate is located in a crossing layer where z is not minimal.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c` is in a crossing layer.
     */
    [[nodiscard]] constexpr bool is_crossing_layer(const OffsetCoordinateType& c) const noexcept
    {
        return c.z > 0;
    }
    /**
     * Returns whether the given coordinate is located within the layout bounds.
     *
     * @param c Coordinate to check for boundary.
     * @return `true` iff `c` is located within the layout bounds.
     */
    [[nodiscard]] constexpr bool is_within_bounds(const OffsetCoordinateType& c) const noexcept
    {
        return c.x <= x() && c.y <= y() && c.z <= z();
    }

#pragma endregion

#pragma region Iteration
    /**
     * Returns a range of all coordinates accessible in the layout between `start` and `stop`. If no values are
     * provided, all coordinates in the layout will be included. The returned iterator range points to the first and
     * last coordinate, respectively. The range object can be used within a for-each loop. Incrementing the iterator is
     * equivalent to nested for loops in the order z, y, x. Consequently, the iteration will happen inside out, i.e., x
     * will be iterated first, then y, then z.
     *
     * @param start First coordinate to include in the range of all coordinates.
     * @param stop Last coordinate (exclusive) to include in the range of all coordinates.
     * @return An iterator range from `start` to `stop`. If they are not provided, the first/last coordinate is used as
     * a default.
     */
    [[nodiscard]] auto coordinates(const OffsetCoordinateType& start = {}, const OffsetCoordinateType& stop = {}) const
    {
        return range_t{std::make_pair(
            coord_iterator{strg->dimension.max, start.is_dead() ? OffsetCoordinateType{0, 0} : start},
            coord_iterator{strg->dimension.max, stop.is_dead() ? strg->dimension.max.get_dead() : stop})};
    }
    /**
     * Applies a function to all coordinates accessible in the layout between `start` and `stop`. The iteration order is
     * the same as for the coordinates function.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by `mockturtle::foreach_element`.
     * @param fn Functor to apply to each coordinate in the range.
     * @param start First coordinate to include in the range of all coordinates.
     * @param stop Last coordinate (exclusive) to include in the range of all coordinates.
     */
    template <typename Fn>
    void foreach_coordinate(Fn&& fn, const OffsetCoordinateType& start = {},
                            const OffsetCoordinateType& stop = {}) const
    {
        mockturtle::detail::foreach_element(
            coord_iterator{strg->dimension.max, start.is_dead() ? OffsetCoordinateType{0, 0} : start},
            coord_iterator{strg->dimension.max, stop.is_dead() ? strg->dimension.max.get_dead() : stop}, fn);
    }
    /**
     * Returns a range of all coordinates accessible in the layout's ground layer between `start` and `stop`. The
     * iteration order is the same as for the coordinates function but without the z dimension.
     *
     * @param start First coordinate to include in the range of all ground coordinates.
     * @param stop Last coordinate (exclusive) to include in the range of all ground coordinates.
     * @return An iterator range from `start` to `stop`. If they are not provided, the first/last coordinate in the
     * ground layer is used as a default.
     */
    [[nodiscard]] auto ground_coordinates(const OffsetCoordinateType& start = {},
                                          const OffsetCoordinateType& stop  = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        auto ground_layer = aspect_ratio_type{x(), y(), 0};

        return range_t{
            std::make_pair(coord_iterator{ground_layer.max, start.is_dead() ? OffsetCoordinateType{0, 0} : start},
                           coord_iterator{ground_layer.max, stop.is_dead() ? ground_layer.max.get_dead() : stop})};
    }
    /**
     * Applies a function to all coordinates accessible in the layout's ground layer between `start` and `stop`. The
     * iteration order is the same as for the ground_coordinates function.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by `mockturtle::foreach_element`.
     * @param fn Functor to apply to each coordinate in the range.
     * @param start First coordinate to include in the range of all ground coordinates.
     * @param stop Last coordinate (exclusive) to include in the range of all ground coordinates.
     */
    template <typename Fn>
    void foreach_ground_coordinate(Fn&& fn, const OffsetCoordinateType& start = {},
                                   const OffsetCoordinateType& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        auto ground_layer = aspect_ratio_type{x(), y(), 0};

        mockturtle::detail::foreach_element(
            coord_iterator{ground_layer.max, start.is_dead() ? OffsetCoordinateType{0, 0} : start},
            coord_iterator{ground_layer.max, stop.is_dead() ? ground_layer.max.get_dead() : stop}, fn);
    }
    /**
     * Returns a container that contains all coordinates that are adjacent to a given one. Thereby, cardinal and ordinal
     * directions are being considered, i.e., the container will contain all coordinates `ac` for which `is_adjacent(c,
     * ac)` returns `true`.
     *
     * Coordinates that are outside of the layout bounds are not considered. Thereby, the size of the returned container
     * is at most 6.
     *
     * @param c Coordinate whose adjacent ones are desired.
     * @return A container that contains all of `c`'s adjacent coordinates.
     */
    auto adjacent_coordinates(const OffsetCoordinateType& c) const noexcept
    {
        std::vector<OffsetCoordinateType> cnt{};
        cnt.reserve(max_fanin_size + 1);  // reserve memory

        foreach_adjacent_coordinate(c, [&cnt](const auto& ac) { cnt.push_back(ac); });

        return cnt;
    }
    /**
     * Applies a function to all coordinates adjacent to a given one in accordance with `adjacent_coordinates`. Thereby,
     * cardinal and ordinal directions are being considered, i.e., the given function is applied to all coordinates ac
     * for which `is_adjacent(c, ac)` returns `true`.
     *
     * Coordinates that are outside of the layout bounds are not considered. Thereby, at most 6 coordinates are touched.
     *
     * @tparam Fn Functor type.
     * @param c Coordinate whose adjacent ones are desired.
     * @param fn Functor to apply to each of `c`'s adjacent coordinates.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate(const OffsetCoordinateType& c, Fn&& fn) const
    {
        // six possible directions in cube coordinates
        static constexpr const std::array<CubeCoordinateType, 6> cube_directions{
            {{+1, -1, 0}, {+1, 0, -1}, {0, +1, -1}, {-1, +1, 0}, {-1, 0, +1}, {0, -1, +1}}};

        // for each direction
        std::for_each(cube_directions.cbegin(), cube_directions.cend(),
                      [this, &c, &fn](const auto& dir)
                      {
                          // convert given coordinate to the cube system, add direction, and convert back to offset
                          auto neighbor = to_offset_coordinate(to_cube_coordinate(c) + dir);
                          // since cube coordinates don't carry the layer information, it has to be manually added
                          neighbor.z = c.z;

                          // add neighboring coordinate if there was no over-/underflow
                          if (is_within_bounds(neighbor))
                          {
                              std::invoke(std::forward<Fn>(fn), std::move(neighbor));
                          }
                      });
    }
    /**
     * Returns a container that contains all coordinates pairs of opposing adjacent coordinates with
     * respect to a given one. In this hexagonal layout, the container content depends on the hexagonal orientation.
     *
     * In case of a pointy_top_hex orientation, the container will contain (`east(c)`, `west(c)`), (`north_east(c)`,
     * `south_west(c)`), (`north_west(c)`, `south_east(c)`). In case of a flat_top_hex orientation, the container will
     * contain (`north(c)`, `south(c)`), (`north_east(c)`, `south_west(c)`), (`north_west(c)`, `south_east(c)`) instead.
     *
     * This function comes in handy when straight lines on the layout are to be examined.
     *
     * Coordinates outside of the layout bounds are not being considered.
     *
     * @param c Coordinate whose opposite ones are desired.
     * @return A container that contains pairs of `c`'s opposing coordinates.
     */
    auto adjacent_opposite_coordinates(const OffsetCoordinateType& c) const noexcept
    {
        std::vector<std::pair<OffsetCoordinateType, OffsetCoordinateType>> cnt{};
        cnt.reserve((max_fanin_size + 1) / 2);  // reserve memory

        foreach_adjacent_opposite_coordinates(c, [&cnt](const auto& cp) { cnt.push_back(cp); });

        return cnt;
    }
    /**
     * Applies a function to all opposing coordinate pairs adjacent to a given one. In this hexagonal layout, the
     * function application depends on the hexagonal orientation.
     *
     * In case of a pointy_top_hex orientation, the function will apply to (`east(c)`, `west(c)`), (`north_east(c)`,
     * `south_west(c)`), (`north_west(c)`, `south_east(c)`). In case of a flat_top_hex orientation, the function will
     * apply to (`north(c)`, `south(c)`), (`north_east(c)`, `south_west(c)`), (`north_west(c)`, `south_east(c)`)
     * instead.
     *
     * This function comes in handy when straight lines on the layout are to be examined.
     *
     * Coordinates outside of the layout bounds are not being considered.
     *
     * @tparam Fn Functor type.
     * @param c Coordinate whose opposite adjacent ones are desired.
     * @param fn Functor to apply to each of `c`'s opposite adjacent coordinate pairs.
     */
    template <typename Fn>
    void foreach_adjacent_opposite_coordinates(const OffsetCoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](auto cardinal1, auto cardinal2) noexcept
        {
            if (cardinal1 != c && cardinal2 != c)
            {
                std::invoke(std::forward<Fn>(fn), std::make_pair(std::move(cardinal1), std::move(cardinal2)));
            }
        };

        if constexpr (std::is_same_v<typename hex_arrangement::orientation, pointy_top_hex>)
        {
            apply_if_not_c(east(c), west(c));
        }
        else  // flat top
        {
            apply_if_not_c(north(c), south(c));
        }

        apply_if_not_c(north_east(c), south_west(c));
        apply_if_not_c(north_west(c), south_east(c));
    }

#pragma endregion

// data types cannot properly be converted to bit field types
#pragma GCC diagnostic push
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wuseless-cast"
#endif
#pragma GCC diagnostic ignored "-Wconversion"

#pragma region coordinates
    /**
     * Converts an offset coordinate to a cube coordinate.
     *
     * This implementation is adapted from https://www.redblobgames.com/grids/hexagons/codegen/output/lib.cpp
     *
     * @param offset_coord Offset coordinate to convert.
     * @return Cube coordinate representing `offset_coord` in the layout's hexagonal orientation.
     */
    [[nodiscard]] constexpr CubeCoordinateType
    to_cube_coordinate(const OffsetCoordinateType& offset_coord) const noexcept
    {
        CubeCoordinateType cube_coord{0, 0, 0};

        constexpr const auto offset = std::is_same_v<HexagonalCoordinateSystem, odd_row_hex> ||
                                              std::is_same_v<HexagonalCoordinateSystem, odd_column_hex> ?
                                          -1 :
                                          1;

        if constexpr (std::is_same_v<typename hex_arrangement::orientation, pointy_top_hex>)
        {
            cube_coord.x = offset_coord.x -
                           static_cast<decltype(cube_coord.x)>((offset_coord.y + offset * (offset_coord.y & 1)) / 2);
            cube_coord.z = offset_coord.y;
            cube_coord.y = -cube_coord.x - cube_coord.z;
        }
        else if constexpr (std::is_same_v<typename hex_arrangement::orientation, flat_top_hex>)
        {
            cube_coord.x = offset_coord.x;
            cube_coord.z = offset_coord.y -
                           static_cast<decltype(cube_coord.z)>((offset_coord.x + offset * (offset_coord.x & 1)) / 2);
            cube_coord.y = -cube_coord.x - cube_coord.z;
        }

        return cube_coord;
    }
    /**
     * Converts a cube coordinate to an offset coordinate.
     *
     * This implementation is adapted from https://www.redblobgames.com/grids/hexagons/codegen/output/lib.cpp
     *
     * @param cube_coord Cube coordinate to convert.
     * @return Offset coordinate representing `cube_coord` in the layout's hexagonal orientation.
     */
    [[nodiscard]] constexpr OffsetCoordinateType
    to_offset_coordinate(const CubeCoordinateType& cube_coord) const noexcept
    {
        // the generated coordinate will be in ground layer
        OffsetCoordinateType offset_coord{0, 0};

        constexpr const auto offset = std::is_same_v<HexagonalCoordinateSystem, odd_row_hex> ||
                                              std::is_same_v<HexagonalCoordinateSystem, odd_column_hex> ?
                                          -1 :
                                          1;
        if constexpr (std::is_same_v<typename hex_arrangement::orientation, pointy_top_hex>)
        {
            offset_coord.x = static_cast<decltype(offset_coord.x)>(
                cube_coord.x + static_cast<int64_t>((cube_coord.z + offset * (cube_coord.z & 1)) / 2));
            offset_coord.y = static_cast<decltype(offset_coord.y)>(cube_coord.z);
        }
        else if constexpr (std::is_same_v<typename hex_arrangement::orientation, flat_top_hex>)
        {
            offset_coord.x = static_cast<decltype(offset_coord.x)>(cube_coord.x);
            offset_coord.y = static_cast<decltype(offset_coord.y)>(
                cube_coord.z + static_cast<int64_t>((cube_coord.x + offset * (cube_coord.x & 1)) / 2));
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

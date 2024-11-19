//
// Created by marcel on 31.03.21.
//

#ifndef FICTION_CARTESIAN_LAYOUT_HPP
#define FICTION_CARTESIAN_LAYOUT_HPP

#include "fiction/layouts/coordinates.hpp"
#include "fiction/utils/range.hpp"

#include <mockturtle/networks/detail/foreach.hpp>

#include <algorithm>
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
 * A layout type that utilizes offset coordinates to represent a Cartesian grid. Its faces are organized in the
 * following way:
 *
 * \verbatim
   +-------+-------+-------+-------+
   |       |       |       |       |
   | (0,0) | (1,0) | (2,0) | (3,0) |
   |       |       |       |       |
   +-------+-------+-------+-------+
   |       |       |       |       |
   | (0,1) | (1,1) | (2,1) | (3,1) |
   |       |       |       |       |
   +-------+-------+-------+-------+
   |       |       |       |       |
   | (0,2) | (1,2) | (2,2) | (3,2) |
   |       |       |       |       |
   +-------+-------+-------+-------+
   \endverbatim
 *
 * @tparam CoordinateType The coordinate implementation to be used.
 */
template <typename CoordinateType = offset::ucoord_t>
class cartesian_layout
{
  public:
#pragma region Types and constructors

    using coordinate   = CoordinateType;
    using aspect_ratio = CoordinateType;

    struct cartesian_layout_storage
    {
        explicit cartesian_layout_storage(const aspect_ratio& ar) noexcept : dimension{ar} {};

        aspect_ratio dimension;
    };

    static constexpr auto min_fanin_size = 0u;  // NOLINT(readability-identifier-naming): mockturtle requirement
    static constexpr auto max_fanin_size = 3u;  // NOLINT(readability-identifier-naming): mockturtle requirement

    using base_type = cartesian_layout;

    using storage = std::shared_ptr<cartesian_layout_storage>;

    /**
     * Standard constructor. The given aspect ratio points to the highest possible coordinate in the layout. That means
     * in the ASCII layout above `ar = (3,2)`. Consequently, with `ar = (0,0)`, the layout has exactly one coordinate.
     *
     * @param ar Highest possible position in the layout.
     */
    explicit cartesian_layout(const aspect_ratio& ar = {}) :
            strg{std::make_shared<cartesian_layout_storage>(initialize_dimension(ar))}
    {}
    /**
     * Copy constructor from another layout's storage.
     *
     * @param s Storage of another cartesian_layout.
     */
    explicit cartesian_layout(std::shared_ptr<cartesian_layout_storage> s) : strg{std::move(s)} {}
    /**
     * Clones the layout returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] cartesian_layout clone() const noexcept
    {
        return cartesian_layout(std::make_shared<cartesian_layout_storage>(*strg));
    }
    /**
     * Creates and returns a coordinate in the layout from the given x-, y-, and z-values.
     *
     * @note This function is equivalent to calling `CoordinateType(x, y, z)`.
     *
     * @tparam X x-type.
     * @tparam Y y-type.
     * @tparam Z z-type.
     * @param x x-value.
     * @param y y-value.
     * @param z z-value.
     * @return A coordinate in the layout of type `CoordinateType`.
     */
    template <typename X, typename Y, typename Z = uint64_t>
    constexpr CoordinateType coord(const X x, const Y y, const Z z = 0ul) const noexcept
    {
        return CoordinateType(x, y, z);
    }

#pragma endregion

#pragma region Structural properties
    /**
     * Returns the layout's x-dimension, i.e., returns the biggest x-value that still belongs to the layout.
     *
     * @return x-dimension.
     */
    [[nodiscard]] auto x() const noexcept
    {
        return strg->dimension.x;
    }
    /**
     * Returns the layout's y-dimension, i.e., returns the biggest y-value that still belongs to the layout.
     *
     * @return y-dimension.
     */
    [[nodiscard]] auto y() const noexcept
    {
        return strg->dimension.y;
    }
    /**
     * Returns the layout's z-dimension, i.e., returns the biggest z-value that still belongs to the layout.
     *
     * @return z-dimension.
     */
    [[nodiscard]] auto z() const noexcept
    {
        return strg->dimension.z;
    }
    /**
     * Returns the layout's number of faces depending on the coordinate type.
     *
     * @return Area of layout.
     */
    [[nodiscard]] auto area() const noexcept
    {
        return fiction::area(strg->dimension);
    }
    /**
     * Updates the layout's dimensions, effectively resizing it.
     *
     * @param ar New aspect ratio.
     */
    void resize(const aspect_ratio& ar) noexcept
    {
        strg->dimension = initialize_dimension(ar);
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
    [[nodiscard]] constexpr CoordinateType north(const CoordinateType& c) const noexcept
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
     * Returns the coordinate that is located in north-eastern direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is higher by 1 and whose y-dimension is lower by 1. If `c`'s x-dimension is already at maximum
     * or `c`'s y-dimension is already at minimum, `c` is returned instead.
     *
     * @param c Coordinate whose north-eastern counterpart is desired.
     * @return Coordinate directly north-eastern of `c`.
     */
    [[nodiscard]] constexpr CoordinateType north_east(const CoordinateType& c) const noexcept
    {
        if (c.x == x() || c.y == 0ull)
        {
            return c;
        }

        auto nec = c;
        ++nec.x;
        --nec.y;

        return nec;
    }
    /**
     * Returns the coordinate that is directly adjacent in eastern direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is higher by 1. If `c`'s x-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose eastern counterpart is desired.
     * @return Coordinate adjacent and east of `c`.
     */
    [[nodiscard]] CoordinateType east(const CoordinateType& c) const noexcept
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
     * Returns the coordinate that is located in south-eastern direction of a given coordinate `c`, i.e., the face
     * whose x-dimension and y-dimension are higher by 1. If `c`'s x-dimension or y-dimension are already at maximum,
     * `c` is returned instead.
     *
     * @param c Coordinate whose south-eastern counterpart is desired.
     * @return Coordinate directly south-eastern of `c`.
     */
    [[nodiscard]] CoordinateType south_east(const CoordinateType& c) const noexcept
    {
        auto sec = c;

        if (c.x > x() || c.y > y())
        {
            sec.d = 1;
        }
        else if (c.x < x() && c.y < y())
        {
            ++sec.x;
            ++sec.y;
        }

        return sec;
    }
    /**
     * Returns the coordinate that is directly adjacent in southern direction of a given coordinate `c`, i.e., the face
     * whose y-dimension is higher by 1. If `c`'s y-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose southern counterpart is desired.
     * @return Coordinate adjacent and south of `c`.
     */
    [[nodiscard]] CoordinateType south(const CoordinateType& c) const noexcept
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
     * Returns the coordinate that is located in south-western direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is lower by 1 and whose y-dimension is higher by 1. If `c`'s x-dimension is already at minimum
     * or `c`'s y-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose south-western counterpart is desired.
     * @return Coordinate directly south-western of `c`.
     */
    [[nodiscard]] CoordinateType south_west(const CoordinateType& c) const noexcept
    {
        auto swc = c;

        if (c.y > y())
        {
            swc.d = 1;
        }
        else if (c.x > static_cast< decltype(c.x)>(0) && c.y < y())
        {
            --swc.x;
            ++swc.y;
        }

        return swc;
    }
    /**
     * Returns the coordinate that is directly adjacent in western direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is lower by 1. If `c`'s x-dimension is already at minimum, `c` is returned instead.
     *
     * @param c Coordinate whose western counterpart is desired.
     * @return Coordinate adjacent and west of `c`.
     */
    [[nodiscard]] constexpr CoordinateType west(const CoordinateType& c) const noexcept
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
     * Returns the coordinate that is located in north-western direction of a given coordinate `c`, i.e., the face
     * whose x-dimension and y-dimension are lower by 1. If `c`'s x-dimension or y-dimension are already at minimum, `c`
     * is returned instead.
     *
     * @param c Coordinate whose north-western counterpart is desired.
     * @return Coordinate directly north-western of `c`.
     */
    [[nodiscard]] constexpr CoordinateType north_west(const CoordinateType& c) const noexcept
    {
        if (c.x == 0ull || c.y == 0ull)
        {
            return c;
        }

        auto nwc = c;
        --nwc.x;
        --nwc.y;

        return nwc;
    }
    /**
     * Returns the coordinate that is directly above a given coordinate `c`, i.e., the face whose z-dimension is higher
     * by 1. If `c`'s z-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose above counterpart is desired.
     * @return Coordinate directly above `c`.
     */
    [[nodiscard]] CoordinateType above(const CoordinateType& c) const noexcept
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
    [[nodiscard]] constexpr CoordinateType below(const CoordinateType& c) const noexcept
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
    [[nodiscard]] constexpr bool is_north_of(const CoordinateType& c1,
                                             const CoordinateType& c2) const noexcept
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
    [[nodiscard]] bool is_east_of(const CoordinateType& c1, const CoordinateType& c2) const noexcept
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
    [[nodiscard]] bool is_south_of(const CoordinateType& c1, const CoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_west_of(const CoordinateType& c1,
                                            const CoordinateType& c2) const noexcept
    {
        return c1 != c2 && west(c1) == c2;
    }
    /**
     * Returns `true` iff coordinate `c2` is either directly north, east, south, or west of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is either directly north, east, south, or west of `c1`.
     */
    [[nodiscard]] bool is_adjacent_of(const CoordinateType& c1, const CoordinateType& c2) const noexcept
    {
        return is_north_of(c1, c2) || is_east_of(c1, c2) || is_south_of(c1, c2) || is_west_of(c1, c2);
    }
    /**
     * Similar to `is_adjacent_of` but also considers `c1`'s elevation, i.e., if `c2` is adjacent to `above(c1)` or
     * `below(c1)`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is either directly north, east, south, or west of `c1` or `c1`'s elevations.
     */
    [[nodiscard]] bool is_adjacent_elevation_of(const CoordinateType& c1,
                                                const CoordinateType& c2) const noexcept
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
    [[nodiscard]] bool is_above(const CoordinateType& c1, const CoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_below(const CoordinateType& c1, const CoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_northwards_of(const CoordinateType& c1,
                                                  const CoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_eastwards_of(const CoordinateType& c1,
                                                 const CoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_southwards_of(const CoordinateType& c1,
                                                  const CoordinateType& c2) const noexcept
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
    [[nodiscard]] constexpr bool is_westwards_of(const CoordinateType& c1,
                                                 const CoordinateType& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x > c2.x);
    }
    /**
     * Returns whether the given coordinate is located at the layout's northern border where y is minimal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's northern border.
     */
    [[nodiscard]] constexpr bool is_at_northern_border(const CoordinateType& c) const noexcept
    {
        return c.y == 0ull;
    }
    /**
     * Returns whether the given coordinate is located at the layout's eastern border where x is maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's northern border.
     */
    [[nodiscard]] bool is_at_eastern_border(const CoordinateType& c) const noexcept
    {
        return c.x == x();
    }
    /**
     * Returns whether the given coordinate is located at the layout's southern border where y is maximal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's southern border.
     */
    [[nodiscard]] bool is_at_southern_border(const CoordinateType& c) const noexcept
    {
        return c.y == y();
    }
    /**
     * Returns whether the given coordinate is located at the layout's western border where x is minimal.
     *
     * @param c Coordinate to check for border location.
     * @return `true` iff `c` is located at the layout's western border.
     */
    [[nodiscard]] constexpr bool is_at_western_border(const CoordinateType& c) const noexcept
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
    [[nodiscard]] bool is_at_any_border(const CoordinateType& c) const noexcept
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
    [[nodiscard]] CoordinateType northern_border_of(const CoordinateType& c) const noexcept
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
    [[nodiscard]] CoordinateType eastern_border_of(const CoordinateType& c) const noexcept
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
    [[nodiscard]] CoordinateType southern_border_of(const CoordinateType& c) const noexcept
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
    [[nodiscard]] CoordinateType western_border_of(const CoordinateType& c) const noexcept
    {
        return {0ull, c.y, c.z};
    }
    /**
     * Returns whether the given coordinate is located in the ground layer where z is minimal.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c` is in ground layer.
     */
    [[nodiscard]] constexpr bool is_ground_layer(const CoordinateType& c) const noexcept
    {
        return c.z == decltype(c.z){0};
    }
    /**
     * Returns whether the given coordinate is located in a crossing layer where z is not minimal.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c` is in a crossing layer.
     */
    [[nodiscard]] constexpr bool is_crossing_layer(const CoordinateType& c) const noexcept
    {
        return c.z > decltype(c.z){0};
    }
    /**
     * Returns whether the given coordinate is located within the layout bounds.
     *
     * @param c Coordinate to check for boundary.
     * @return `true` iff `c` is located within the layout bounds.
     */
    [[nodiscard]] constexpr bool is_within_bounds(const CoordinateType& c) const noexcept
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
    [[nodiscard]] auto coordinates(const CoordinateType& start = {}, const CoordinateType& stop = {}) const
    {
        return range_t{
            std::make_pair(coord_iterator{strg->dimension, start.is_dead() ? CoordinateType{0, 0} : start},
                           coord_iterator{strg->dimension, stop.is_dead() ? strg->dimension.get_dead() : stop})};
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
    void foreach_coordinate(Fn&& fn, const CoordinateType& start = {},
                            const CoordinateType& stop = {}) const
    {
        mockturtle::detail::foreach_element(
            coord_iterator{strg->dimension, start.is_dead() ? CoordinateType{0, 0} : start},
            coord_iterator{strg->dimension, stop.is_dead() ? strg->dimension.get_dead() : stop}, fn);
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
    [[nodiscard]] auto ground_coordinates(const CoordinateType& start = {},
                                          const CoordinateType& stop  = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        const auto ground_layer = aspect_ratio{x(), y(), 0};

        return range_t{
            std::make_pair(coord_iterator{ground_layer, start.is_dead() ? CoordinateType{0, 0} : start},
                           coord_iterator{ground_layer, stop.is_dead() ? ground_layer.get_dead() : stop})};
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
    void foreach_ground_coordinate(Fn&& fn, const CoordinateType& start = {},
                                   const CoordinateType& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        const auto ground_layer = aspect_ratio{x(), y(), 0};

        mockturtle::detail::foreach_element(
            coord_iterator{ground_layer, start.is_dead() ? CoordinateType{0, 0} : start},
            coord_iterator{ground_layer, stop.is_dead() ? ground_layer.get_dead() : stop}, fn);
    }
    /**
     * Returns a container that contains all coordinates that are adjacent to a given one. Thereby, only cardinal
     * directions are being considered, i.e., the container contains all coordinates `ac` for which `is_adjacent(c, ac)`
     * returns `true`.
     *
     * Coordinates that are outside of the layout bounds are not considered. Thereby, the size of the returned container
     * is at most 4.
     *
     * @param c Coordinate whose adjacent ones are desired.
     * @return A container that contains all of `c`'s adjacent coordinates.
     */
    auto adjacent_coordinates(const CoordinateType& c) const noexcept
    {
        std::vector<CoordinateType> cnt{};
        cnt.reserve(max_fanin_size + 1);  // reserve memory

        foreach_adjacent_coordinate(c, [&cnt](const auto& ac) noexcept { cnt.push_back(ac); });

        return cnt;
    }
    /**
     * Applies a function to all coordinates adjacent to a given one. Thereby, only cardinal directions are being
     * considered, i.e., the function is applied to all coordinates `ac` for which `is_adjacent(c, ac)` returns `true`.
     *
     * Coordinates that are outside of the layout bounds are not considered. Thereby, at most 4 coordinates are touched.
     *
     * @tparam Fn Functor type.
     * @param c Coordinate whose adjacent ones are desired.
     * @param fn Functor to apply to each of `c`'s adjacent coordinates.
     */
    template <typename Fn>
    void foreach_adjacent_coordinate(const CoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](const auto& cardinal) noexcept
        {
            if (cardinal != c)
            {
                std::invoke(std::forward<Fn>(fn), cardinal);
            }
        };

        apply_if_not_c(north(c));
        apply_if_not_c(east(c));
        apply_if_not_c(south(c));
        apply_if_not_c(west(c));
    }
    /**
     * Returns a container that contains all coordinates pairs of opposing adjacent coordinates with respect to a given
     * one. In this Cartesian layout, the container will contain (`north(c)`, `south(c)`) and (`east(c)`, `west(c)`).
     *
     * This function comes in handy when straight lines on the layout are to be examined.
     *
     * Coordinates outside of the layout bounds are not being considered.
     *
     * @param c Coordinate whose opposite ones are desired.
     * @return A container that contains pairs of `c`'s opposing coordinates.
     */
    auto adjacent_opposite_coordinates(const CoordinateType& c) const noexcept
    {
        std::vector<std::pair<CoordinateType, CoordinateType>> cnt{};
        cnt.reserve((max_fanin_size + 1) / 2);  // reserve memory

        foreach_adjacent_opposite_coordinates(c, [&cnt](const auto& cp) noexcept { cnt.push_back(cp); });

        return cnt;
    }
    /**
     * Applies a function to all opposing coordinate pairs adjacent to a given one. In this Cartesian layout, the
     * function will be applied to (`north(c)`, `south(c)`) and (`east(c)`, `west(c)`).
     *
     * @tparam Fn Functor type.
     * @param c Coordinate whose opposite adjacent ones are desired.
     * @param fn Functor to apply to each of `c`'s opposite adjacent coordinate pairs.
     */
    template <typename Fn>
    void foreach_adjacent_opposite_coordinates(const CoordinateType& c, Fn&& fn) const
    {
        const auto apply_if_not_c = [&c, &fn](auto cardinal1, auto cardinal2) noexcept
        {
            if (cardinal1 != c && cardinal2 != c)
            {
                std::invoke(std::forward<Fn>(fn), std::make_pair(std::move(cardinal1), std::move(cardinal2)));
            }
        };

        apply_if_not_c(north(c), south(c));
        apply_if_not_c(east(c), west(c));
    }

#pragma endregion

  private:
    storage strg;
    /*
     * Initializer for a cartesian layout dimension. When using SiQAD coordinates, it will default the z value to 1 if
     * the y value is greater than 0.
     */
    constexpr CoordinateType initialize_dimension(const CoordinateType& coord) const
    {
        if constexpr (std::is_same_v<CoordinateType, siqad::coord_t>)
        {
            return CoordinateType{coord.x, coord.y, 1};
        }

        return coord;
    }
};

}  // namespace fiction

#endif  // FICTION_CARTESIAN_LAYOUT_HPP

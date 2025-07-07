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
 * @tparam OffsetCoordinateType The coordinate implementation to be used.
 */
template <typename OffsetCoordinateType = offset::ucoord_t>
class cartesian_layout
{
  public:
#pragma region Types and constructors

    using coordinate        = OffsetCoordinateType;
    using aspect_ratio_type = aspect_ratio<OffsetCoordinateType>;

    /**
     * Struct representing the storage for a cartesian_layout.
     *
     * The `cartesian_layout_storage` struct holds the dimensions and origin of the layout.
     */
    struct cartesian_layout_storage
    {
        /**
         * Constructs a cartesian_layout_storage with specified dimensions and origin.
         *
         * @param dim The dimensions of the layout (width, height, etc.).
         * @param org The origin coordinate. Defaults to (0, 0, 0).
         */
        explicit cartesian_layout_storage(const aspect_ratio_type& ar) : ar{ar} {}

        aspect_ratio_type ar;  // width,height, etc.
    };

    static constexpr auto min_fanin_size = 0u;  // NOLINT(readability-identifier-naming): mockturtle requirement
    static constexpr auto max_fanin_size = 3u;  // NOLINT(readability-identifier-naming): mockturtle requirement

    using base_type = cartesian_layout;

    using storage = std::shared_ptr<cartesian_layout_storage>;

    /**
     * Standard constructor.
     *
     * Initializes the layout with the highest possible coordinate at (0, 0, 0), effectively creating
     * a layout with a single coordinate.
     */
    explicit cartesian_layout(const aspect_ratio_type& ar = {}) :
            strg{std::make_shared<cartesian_layout_storage>(initialize_dimension(ar))} {};

    /**
     * Constructs a cartesian_layout from an aspect_ratio_type.
     *
     * This constructor initializes the layout's dimensions based on the provided aspect_ratio_type,
     * and sets the origin to the min coordinate of the aspect_ratio_type.
     *
     * @param ar The aspect_ratio_type defining the layout's size and origin.
     */
    //    explicit cartesian_layout(const aspect_ratio_type& ar) : strg{std::make_shared<cartesian_layout_storage>(ar)}
    //    {}

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
     * Returns the layout's size in the x-dimension, i.e., the distance between x_min() and x()).
     *
     * @return x-dimension.
     */
    [[nodiscard]] auto x_size() const noexcept
    {
        return strg->ar.x_size();
    }
    /**
     * Returns the layout's size in the y-dimension, i.e., the distance between y_min() and y().
     *
     * @return y-dimension.
     */
    [[nodiscard]] auto y_size() const noexcept
    {
        return strg->ar.y_size();
    }
    /**
     * Returns the layout's size in the z-dimension, i.e., the distance between z() and z().
     *
     * @return z-dimension.
     */
    [[nodiscard]] auto z_size() const noexcept
    {
        return strg->ar.z_size();
    }
    /**
     * Returns the layout's minimum x-coordinate.
     *
     * @return The x_min coordinate of the layout.
     */
    [[nodiscard]] auto x_min() const noexcept
    {
        return strg->ar.x_min();
    }
    /**
     * Returns the layout's minimum y-coordinate.
     *
     * @return The y_min coordinate of the layout.
     */
    [[nodiscard]] auto y_min() const noexcept
    {
        return strg->ar.y_min();
    }
    /**
     * Returns the layout's minimum z-coordinate.
     *
     * @return The z_min coordinate of the layout.
     */
    [[nodiscard]] auto z_min() const noexcept
    {
        return strg->ar.z_min();
    }
    /**
     * Returns the layout's maximum x-coordinate.
     *
     * @return The xmaximum x-coordinate of the layout.
     */
    [[nodiscard]] auto x() const noexcept
    {
        return strg->ar.x();
    }
    /**
     * Returns the layout's maximum y-coordinate.
     *
     * @return The maximum y-coordinate of the layout.
     */
    [[nodiscard]] auto y() const noexcept
    {
        return strg->ar.y();
    }
    /**
     * Returns the layout's maximum z-coordinate.
     *
     * @return The maximum z-coordinate of the layout.
     */
    [[nodiscard]] auto z() const noexcept
    {
        return strg->ar.z();
    }
    /**
     * Returns the layout's number of faces depending on the coordinate type.
     *
     * @return Area of the layout.
     */
    [[nodiscard]] auto area() const noexcept
    {
        return strg->ar.area();
    }
    /**
     * Returns the layout's volume depending on the coordinate type.
     *
     * @return Volume of the layout.
     */
    [[nodiscard]] auto volume() const noexcept
    {
        return strg->ar.volume();
    }
    /**
     * Updates the layout's dimensions and origin based on a new aspect_ratio.
     *
     * This method effectively resizes the layout by adjusting its dimensions to match
     * the provided aspect_ratio. The origin is also updated to the start coordinate of the aspect_ratio.
     *
     * @param ar The new aspect_ratio to apply to the layout.
     */
    void resize(const aspect_ratio_type& ar) noexcept
    {
        strg->ar = ar;
    }
    /**
     * Updates the layout's dimensions based on a new coordinate.
     *
     * This method effectively resizes the layout by adjusting its dimensions to match
     * the provided coordinate. The origin is also updated to (0, 0, 0).
     *
     * @param ar The new max coordinate of the layout.
     */
    void resize(const OffsetCoordinateType& ar) noexcept
    {
        strg->ar = aspect_ratio_type(ar);
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
        auto nc = c;

        if (c.y <= y_min())
        {
            if (c.y < y_min())
            {
                nc.d = 1;
            }
            return nc;
        }

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
    [[nodiscard]] constexpr OffsetCoordinateType north_east(const OffsetCoordinateType& c) const noexcept
    {
        auto nec = c;

        if (c.x >= x() || c.y <= y_min())
        {
            if (c.x > x() || c.y < y_min())
            {
                nec.d = 1;
            }
            return nec;
        }

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
     * Returns the coordinate that is located in south-eastern direction of a given coordinate `c`, i.e., the face
     * whose x-dimension and y-dimension are higher by 1. If `c`'s x-dimension or y-dimension are already at maximum,
     * `c` is returned instead.
     *
     * @param c Coordinate whose south-eastern counterpart is desired.
     * @return Coordinate directly south-eastern of `c`.
     */
    [[nodiscard]] OffsetCoordinateType south_east(const OffsetCoordinateType& c) const noexcept
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
     * Returns the coordinate that is located in south-western direction of a given coordinate `c`, i.e., the face
     * whose x-dimension is lower by 1 and whose y-dimension is higher by 1. If `c`'s x-dimension is already at minimum
     * or `c`'s y-dimension is already at maximum, `c` is returned instead.
     *
     * @param c Coordinate whose south-western counterpart is desired.
     * @return Coordinate directly south-western of `c`.
     */
    [[nodiscard]] OffsetCoordinateType south_west(const OffsetCoordinateType& c) const noexcept
    {
        auto swc = c;

        if (c.x <= x_min() || c.y >= y())
        {
            if (c.x < x_min() || c.y > y())
            {
                swc.d = 1;
            }
            return swc;
        }

        --swc.x;
        ++swc.y;

        return swc;
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
        auto wc = c;

        if (c.x <= x_min())
        {
            if (c.x < x_min())
            {
                wc.d = 1;
            }
            return wc;
        }

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
    [[nodiscard]] constexpr OffsetCoordinateType north_west(const OffsetCoordinateType& c) const noexcept
    {
        auto nwc = c;

        if (c.x <= x_min() || c.y <= y_min())
        {
            if (c.x < x_min() || c.y < y_min())
            {
                nwc.d = 1;
            }
            return nwc;
        }

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
     * Returns `true` iff coordinate `c2` is either directly north, east, south, or west of coordinate `c1`.
     *
     * @param c1 Base coordinate.
     * @param c2 Coordinate to test for its location in relation to `c1`.
     * @return `true` iff `c2` is either directly north, east, south, or west of `c1`.
     */
    [[nodiscard]] bool is_adjacent_of(const OffsetCoordinateType& c1, const OffsetCoordinateType& c2) const noexcept
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
        return c.y == y_min();
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
        return c.x == x_min();
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
        return {c.x, y_min(), c.z};
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
        return {x_min(), c.y, c.z};
    }
    /**
     * Returns whether the given coordinate is located in the ground layer where z is minimal.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c` is in ground layer.
     */
    [[nodiscard]] constexpr bool is_ground_layer(const OffsetCoordinateType& c) const noexcept
    {
        return c.z == decltype(c.z){0};
    }
    /**
     * Returns whether the given coordinate is located in a crossing layer where z is not minimal.
     *
     * @param c Coordinate to check for elevation.
     * @return `true` iff `c` is in a crossing layer.
     */
    [[nodiscard]] constexpr bool is_crossing_layer(const OffsetCoordinateType& c) const noexcept
    {
        return c.z > decltype(c.z){0};
    }
    /**
     * Returns whether the given coordinate is located within the layout bounds.
     *
     * @param c Coordinate to check for boundary.
     * @return `true` iff `c` is located within the layout bounds.
     */
    [[nodiscard]] constexpr bool is_within_bounds(const OffsetCoordinateType& c) const noexcept
    {
        return x_min() <= c.x && c.x <= x() && y_min() <= c.y && c.y <= y() && z_min() <= c.z && c.z <= z();
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
            coord_iterator{strg->ar.max, start.is_dead() ? strg->ar.min : start, strg->ar.min},
            coord_iterator{strg->ar.max, stop.is_dead() ? strg->ar.max.get_dead() : stop, strg->ar.min})};
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
            coord_iterator{strg->ar.max, start.is_dead() ? strg->ar.min : start, strg->ar.min},
            coord_iterator{strg->ar.max, stop.is_dead() ? strg->ar.max.get_dead() : stop, strg->ar.min}, fn);
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

        const auto ground_layer_start = coordinate{strg->ar.min.x, strg->ar.min.y, 0};
        const auto ground_layer       = coordinate{strg->ar.max.x, strg->ar.max.y, 0};

        return range_t{std::make_pair(
            coord_iterator{ground_layer, start.is_dead() ? ground_layer_start : start, strg->ar.min},
            coord_iterator{ground_layer, stop.is_dead() ? ground_layer.get_dead() : stop, strg->ar.min})};
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

        const auto ground_layer_start = coordinate{strg->ar.min.x, strg->ar.min.y, 0};
        const auto ground_layer       = coordinate{strg->ar.max.x, strg->ar.max.y, 0};

        mockturtle::detail::foreach_element(
            coord_iterator{ground_layer, start.is_dead() ? ground_layer_start : start, strg->ar.min},
            coord_iterator{ground_layer, stop.is_dead() ? ground_layer.get_dead() : stop, strg->ar.min}, fn);
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
    auto adjacent_coordinates(const OffsetCoordinateType& c) const noexcept
    {
        std::vector<OffsetCoordinateType> cnt{};
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
    void foreach_adjacent_coordinate(const OffsetCoordinateType& c, Fn&& fn) const
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
    auto adjacent_opposite_coordinates(const OffsetCoordinateType& c) const noexcept
    {
        std::vector<std::pair<OffsetCoordinateType, OffsetCoordinateType>> cnt{};
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
    void foreach_adjacent_opposite_coordinates(const OffsetCoordinateType& c, Fn&& fn) const
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
     * Initializer for a cartesian layout dimension. When using SiQAD coordinates, it will default the z value to 1,
     * such that only complete dimer rows are considered.
     */
    constexpr aspect_ratio_type initialize_dimension(const aspect_ratio_type& coord) const
    {
        if constexpr (std::is_same_v<OffsetCoordinateType, siqad::coord_t>)
        {
            return aspect_ratio<OffsetCoordinateType>{coord.max.x, coord.max.y, 1};
        }

        return coord;
    }
};

//// Deduction guide for the constructor with aspect_ratio_type
// template <typename OffsetCoordinateType>
// cartesian_layout(const aspect_ratio<OffsetCoordinateType>& = {})
//     -> cartesian_layout<OffsetCoordinateType>;
//
//// Deduction guide for the constructor with OffsetCoordinateType
// template <typename OffsetCoordinateType>
// cartesian_layout(const OffsetCoordinateType& coord)
//     -> cartesian_layout<OffsetCoordinateType>;

}  // namespace fiction

#endif  // FICTION_CARTESIAN_LAYOUT_HPP

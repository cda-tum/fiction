//
// Created by marcel on 31.03.21.
//

#ifndef FICTION_CARTESIAN_LAYOUT_HPP
#define FICTION_CARTESIAN_LAYOUT_HPP

#include "../utils/range.hpp"
#include "coordinates.hpp"

#include <mockturtle/networks/detail/foreach.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <set>

namespace fiction
{
/**
 * A layout type that utilizes Cartesian coordinates. It's faces are organized in the following way:
 *
 * \verbatim
 *
 *  +-------+-------+-------+-------+
 *  |       |       |       |       |
 *  | (0,0) | (1,0) | (2,0) | (3,0) |
 *  |       |       |       |       |
 *  +-------+-------+-------+-------+
 *  |       |       |       |       |
 *  | (0,1) | (1,1) | (2,1) | (3,1) |
 *  |       |       |       |       |
 *  +-------+-------+-------+-------+
 *  |       |       |       |       |
 *  | (0,2) | (1,2) | (2,2) | (3,2) |
 *  |       |       |       |       |
 *  +-------+-------+-------+-------+
 *  \endverbatim
 *
 * @tparam CartesianCoordinateType The coordinate implementation to be used. Cartesian coordinates are required.
 */
template <typename CartesianCoordinateType>
class cartesian_layout
{
  public:
#pragma region Types and constructors

    using coordinate   = CartesianCoordinateType;
    using aspect_ratio = CartesianCoordinateType;

    struct cartesian_layout_storage
    {
        explicit cartesian_layout_storage(const aspect_ratio& ar) noexcept : dimension{ar} {};

        aspect_ratio dimension;
    };

    static constexpr auto min_fanin_size = 0;
    static constexpr auto max_fanin_size = 3;

    using base_type = cartesian_layout;

    using storage = std::shared_ptr<cartesian_layout_storage>;

    /**
     * Standard constructor. The given aspect ratio points to the highest possible coordinate in the layout. That means
     * in the ASCII layout above ar = (3,2). Consequently, with ar = (0,0), the layout has exactly one coordinate.
     *
     * @param ar Highest possible position in the layout.
     */
    explicit cartesian_layout(const aspect_ratio& ar = {}) : strg{std::make_shared<cartesian_layout_storage>(ar)} {}

    explicit cartesian_layout(std::shared_ptr<cartesian_layout_storage> s) : strg{std::move(s)} {}

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

    [[nodiscard]] constexpr CartesianCoordinateType north(const CartesianCoordinateType& c) const noexcept
    {
        if (c.y == 0ull)
            return c;

        auto nc = c;
        --nc.y;

        return nc;
    }

    [[nodiscard]] CartesianCoordinateType east(const CartesianCoordinateType& c) const noexcept
    {
        auto ec = c;

        if (c.x > x())
            ec.d = 1;
        else if (c.x < x())
            ++ec.x;

        return ec;
    }

    [[nodiscard]] CartesianCoordinateType south(const CartesianCoordinateType& c) const noexcept
    {
        auto sc = c;

        if (c.y > y())
            sc.d = 1;
        else if (c.y < y())
            ++sc.y;

        return sc;
    }

    [[nodiscard]] constexpr CartesianCoordinateType west(const CartesianCoordinateType& c) const noexcept
    {
        if (c.x == 0ull)
            return c;

        auto wc = c;
        --wc.x;

        return wc;
    }

    [[nodiscard]] CartesianCoordinateType above(const CartesianCoordinateType& c) const noexcept
    {
        auto ac = c;

        if (c.z > z())
            ac.d = 1;
        else if (c.z < z())
            ++ac.z;

        return ac;
    }

    [[nodiscard]] constexpr CartesianCoordinateType below(const CartesianCoordinateType& c) const noexcept
    {
        if (c.z == 0ull)
            return c;

        auto bc = c;
        --bc.z;

        return bc;
    }

    [[nodiscard]] constexpr bool is_north_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return c1 != c2 && north(c1) == c2;
    }

    [[nodiscard]] bool is_east_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return c1 != c2 && east(c1) == c2;
    }

    [[nodiscard]] bool is_south_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return c1 != c2 && south(c1) == c2;
    }

    [[nodiscard]] constexpr bool is_west_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return c1 != c2 && west(c1) == c2;
    }

    [[nodiscard]] bool is_adjacent_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return is_north_of(c1, c2) || is_east_of(c1, c2) || is_south_of(c1, c2) || is_west_of(c1, c2);
    }

    [[nodiscard]] bool is_adjacent_elevation_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return is_adjacent_of(c1, c2) || is_north_of(c1, above(c2)) || is_east_of(c1, above(c2)) ||
               is_south_of(c1, above(c2)) || is_west_of(c1, above(c2)) || is_north_of(c1, below(c2)) ||
               is_east_of(c1, below(c2)) || is_south_of(c1, below(c2)) || is_west_of(c1, below(c2));
    }

    [[nodiscard]] bool is_above_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return c1 != c2 && above(c1) == c2;
    }

    [[nodiscard]] constexpr bool is_below_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return c1 != c2 && below(c1) == c2;
    }

    [[nodiscard]] constexpr bool is_northwards_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y > c2.y) && (c1.x == c2.x);
    }

    [[nodiscard]] constexpr bool is_eastwards_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x < c2.x);
    }

    [[nodiscard]] constexpr bool is_southwards_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y < c2.y) && (c1.x == c2.x);
    }

    [[nodiscard]] constexpr bool is_westwards_of(const CartesianCoordinateType& c1, const CartesianCoordinateType& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x > c2.x);
    }

    [[nodiscard]] constexpr bool is_northern_border(const CartesianCoordinateType& c) const noexcept
    {
        return c.y == 0ull;
    }

    [[nodiscard]] bool is_eastern_border(const CartesianCoordinateType& c) const noexcept
    {
        return c.x == x();
    }

    [[nodiscard]] bool is_southern_border(const CartesianCoordinateType& c) const noexcept
    {
        return c.y == y();
    }

    [[nodiscard]] constexpr bool is_western_border(const CartesianCoordinateType& c) const noexcept
    {
        return c.x == 0ull;
    }

    [[nodiscard]] bool is_border(const CartesianCoordinateType& c) const noexcept
    {
        return is_northern_border(c) || is_eastern_border(c) || is_southern_border(c) || is_western_border(c);
    }

    [[nodiscard]] CartesianCoordinateType northern_border_of(const CartesianCoordinateType& c) const noexcept
    {
        return {c.x, 0ull, c.z};
    }

    [[nodiscard]] CartesianCoordinateType eastern_border_of(const CartesianCoordinateType& c) const noexcept
    {
        return {x(), c.y, c.z};
    }

    [[nodiscard]] CartesianCoordinateType southern_border_of(const CartesianCoordinateType& c) const noexcept
    {
        return {c.x, y(), c.z};
    }

    [[nodiscard]] CartesianCoordinateType western_border_of(const CartesianCoordinateType& c) const noexcept
    {
        return {0ull, c.y, c.z};
    }

    [[nodiscard]] constexpr bool is_ground_layer(const CartesianCoordinateType& c) const noexcept
    {
        return c.z == 0ull;
    }

    [[nodiscard]] constexpr bool is_crossing_layer(const CartesianCoordinateType& c) const noexcept
    {
        return c.z > 0ull;
    }

#pragma endregion

#pragma region Iteration

    [[nodiscard]] auto coordinates(const CartesianCoordinateType& start = {}, const CartesianCoordinateType& stop = {}) const
    {
        return range_t{std::make_pair(
            cartesian::coord_iterator{strg->dimension, start.is_dead() ? CartesianCoordinateType{0, 0} : start},
            cartesian::coord_iterator{strg->dimension, stop.is_dead() ? strg->dimension.get_dead() : stop})};
    }

    template <typename Fn>
    void foreach_coordinate(Fn&& fn, const CartesianCoordinateType& start = {}, const CartesianCoordinateType& stop = {}) const
    {
        mockturtle::detail::foreach_element(
            cartesian::coord_iterator{strg->dimension, start.is_dead() ? CartesianCoordinateType{0, 0} : start},
            cartesian::coord_iterator{strg->dimension, stop.is_dead() ? strg->dimension.get_dead() : stop}, fn);
    }

    [[nodiscard]] auto ground_coordinates(const CartesianCoordinateType& start = {}, const CartesianCoordinateType& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        const auto ground_layer = aspect_ratio{x(), y(), 0};

        return range_t{
            std::make_pair(cartesian::coord_iterator{ground_layer, start.is_dead() ? CartesianCoordinateType{0, 0} : start},
                           cartesian::coord_iterator{ground_layer, stop.is_dead() ? ground_layer.get_dead() : stop})};
    }

    template <typename Fn>
    void foreach_ground_coordinate(Fn&& fn, const CartesianCoordinateType& start = {}, const CartesianCoordinateType& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        const auto ground_layer = aspect_ratio{x(), y(), 0};

        mockturtle::detail::foreach_element(
            cartesian::coord_iterator{ground_layer, start.is_dead() ? CartesianCoordinateType{0, 0} : start},
            cartesian::coord_iterator{ground_layer, stop.is_dead() ? ground_layer.get_dead() : stop}, fn);
    }

    template <typename Container>
    Container adjacent_coordinates(const CartesianCoordinateType& c) const noexcept
    {
        Container cnt{};

        const auto add_if_not_t = [&c, &cnt](const auto& cardinal)
        {
            if (cardinal != c)
                cnt.insert(cnt.end(), cardinal);
        };

        add_if_not_t(north(c));
        add_if_not_t(east(c));
        add_if_not_t(south(c));
        add_if_not_t(west(c));

        return cnt;
    }

    template <typename Fn>
    void foreach_adjacent_coordinate(const CartesianCoordinateType& c, Fn&& fn) const
    {
        const auto adj = adjacent_coordinates<std::set<CartesianCoordinateType>>(c);

        mockturtle::detail::foreach_element(adj.cbegin(), adj.cend(), fn);
    }

#pragma endregion

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_CARTESIAN_LAYOUT_HPP

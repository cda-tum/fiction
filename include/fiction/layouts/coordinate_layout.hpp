//
// Created by marcel on 31.03.21.
//

#ifndef FICTION_COORDINATE_LAYOUT_HPP
#define FICTION_COORDINATE_LAYOUT_HPP

#include "../utils/range.hpp"
#include "coordinate.hpp"

#include <mockturtle/networks/detail/foreach.hpp>

#include <algorithm>
#include <cstdint>

namespace fiction
{

class coordinate_layout
{
  public:
#pragma region Types and constructors

    using coordinate   = coord_t;
    using aspect_ratio = coord_t;

    struct coordinate_layout_storage
    {
        explicit coordinate_layout_storage(const aspect_ratio& ar) noexcept : dimension{ar} {};

        aspect_ratio dimension;
    };

    using base_type = coordinate_layout;

    using storage = std::shared_ptr<coordinate_layout_storage>;

    explicit coordinate_layout(const aspect_ratio& ar = {}) : strg{std::make_shared<coordinate_layout_storage>(ar)} {}

    explicit coordinate_layout(std::shared_ptr<coordinate_layout_storage> s) : strg{std::move(s)} {}

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

    [[nodiscard]] constexpr coordinate north(const coordinate& c) const noexcept
    {
        if (c.y == 0ull)
            return c;

        auto nc = c;
        --nc.y;

        return nc;
    }

    [[nodiscard]] coordinate east(const coordinate& c) const noexcept
    {
        auto ec = c;

        if (c.x > x())
            ec.d = 1;
        else if (c.x < x())
            ++ec.x;

        return ec;
    }

    [[nodiscard]] coordinate south(const coordinate& c) const noexcept
    {
        auto sc = c;

        if (c.y > y())
            sc.d = 1;
        else if (c.y < y())
            ++sc.y;

        return sc;
    }

    [[nodiscard]] constexpr coordinate west(const coordinate& c) const noexcept
    {
        if (c.x == 0ull)
            return c;

        auto wc = c;
        --wc.x;

        return wc;
    }

    [[nodiscard]] coordinate above(const coordinate& c) const noexcept
    {
        auto ac = c;

        if (c.z > z())
            ac.d = 1;
        else if (c.z < z())
            ++ac.z;

        return ac;
    }

    [[nodiscard]] constexpr coordinate below(const coordinate& c) const noexcept
    {
        if (c.z == 0ull)
            return c;

        auto bc = c;
        --bc.z;

        return bc;
    }

    [[nodiscard]] constexpr bool is_north_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return c1 != c2 && north(c1) == c2;
    }

    [[nodiscard]] bool is_east_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return c1 != c2 && east(c1) == c2;
    }

    [[nodiscard]] bool is_south_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return c1 != c2 && south(c1) == c2;
    }

    [[nodiscard]] constexpr bool is_west_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return c1 != c2 && west(c1) == c2;
    }

    [[nodiscard]] bool is_adjacent_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return is_north_of(c1, c2) || is_east_of(c1, c2) || is_south_of(c1, c2) || is_west_of(c1, c2);
    }

    [[nodiscard]] bool is_surrounding_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return is_adjacent_of(c1, c2) || is_north_of(c1, above(c2)) || is_east_of(c1, above(c2)) ||
               is_south_of(c1, above(c2)) || is_west_of(c1, above(c2)) || is_north_of(c1, below(c2)) ||
               is_east_of(c1, below(c2)) || is_south_of(c1, below(c2)) || is_west_of(c1, below(c2));
    }

    [[nodiscard]] bool is_above_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return c1 != c2 && above(c1) == c2;
    }

    [[nodiscard]] constexpr bool is_below_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return c1 != c2 && below(c1) == c2;
    }

    [[nodiscard]] constexpr bool is_northwards_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y > c2.y) && (c1.x == c2.x);
    }

    [[nodiscard]] constexpr bool is_eastwards_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x < c2.x);
    }

    [[nodiscard]] constexpr bool is_southwards_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y < c2.y) && (c1.x == c2.x);
    }

    [[nodiscard]] constexpr bool is_westwards_of(const coordinate& c1, const coordinate& c2) const noexcept
    {
        return (c1.z == c2.z) && (c1.y == c2.y) && (c1.x > c2.x);
    }

    [[nodiscard]] constexpr bool is_northern_border(const coordinate& c) const noexcept
    {
        return c.y == 0ull;
    }

    [[nodiscard]] bool is_eastern_border(const coordinate& c) const noexcept
    {
        return c.x == x();
    }

    [[nodiscard]] bool is_southern_border(const coordinate& c) const noexcept
    {
        return c.y == y();
    }

    [[nodiscard]] constexpr bool is_western_border(const coordinate& c) const noexcept
    {
        return c.x == 0ull;
    }

    [[nodiscard]] bool is_border(const coordinate& c) const noexcept
    {
        return is_northern_border(c) || is_eastern_border(c) || is_southern_border(c) || is_western_border(c);
    }

    [[nodiscard]] coordinate northern_border_of(const coordinate& c) const noexcept
    {
        return {c.x, 0ull, c.z};
    }

    [[nodiscard]] coordinate eastern_border_of(const coordinate& c) const noexcept
    {
        return {x(), c.y, c.z};
    }

    [[nodiscard]] coordinate southern_border_of(const coordinate& c) const noexcept
    {
        return {c.x, y(), c.z};
    }

    [[nodiscard]] coordinate western_border_of(const coordinate& c) const noexcept
    {
        return {0ull, c.y, c.z};
    }

    [[nodiscard]] constexpr bool is_ground_layer(const coordinate& c) const noexcept
    {
        return c.z == 0ull;
    }

    [[nodiscard]] constexpr bool is_crossing_layer(const coordinate& c) const noexcept
    {
        return c.z > 0ull;
    }

#pragma endregion

#pragma region Iteration

    [[nodiscard]] auto coordinates(const coordinate& start = {}, const coordinate& stop = {}) const
    {
        return range_t{
            std::make_pair(coord_iterator{strg->dimension, start.is_dead() ? coordinate{0, 0} : start},
                           coord_iterator{strg->dimension, stop.is_dead() ? strg->dimension.get_dead() : stop})};
    }

    template <typename Fn>
    void foreach_coordinate(Fn&& fn, const coordinate& start = {}, const coordinate& stop = {}) const
    {
        mockturtle::detail::foreach_element(
            coord_iterator{strg->dimension, start.is_dead() ? coordinate{0, 0} : start},
            coord_iterator{strg->dimension, stop.is_dead() ? strg->dimension.get_dead() : stop}, fn);
    }

    [[nodiscard]] auto ground_coordinates(const coordinate& start = {}, const coordinate& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        auto ground_layer = aspect_ratio{x(), y(), 0};

        return range_t{std::make_pair(coord_iterator{ground_layer, start.is_dead() ? coordinate{0, 0} : start},
                                      coord_iterator{ground_layer, stop.is_dead() ? ground_layer.get_dead() : stop})};
    }

    template <typename Fn>
    void foreach_ground_coordinate(Fn&& fn, const coordinate& start = {}, const coordinate& stop = {}) const
    {
        assert(start.z == 0 && stop.z == 0);

        auto ground_layer = aspect_ratio{x(), y(), 0};

        mockturtle::detail::foreach_element(
            coord_iterator{ground_layer, start.is_dead() ? coordinate{0, 0} : start},
            coord_iterator{ground_layer, stop.is_dead() ? ground_layer.get_dead() : stop}, fn);
    }

    template <typename Container>
    Container adjacent_coordinates(const coordinate& c) const noexcept
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

#pragma endregion

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_COORDINATE_LAYOUT_HPP

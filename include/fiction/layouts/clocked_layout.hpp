//
// Created by marcel on 14.05.21.
//

#ifndef FICTION_CLOCKED_LAYOUT_HPP
#define FICTION_CLOCKED_LAYOUT_HPP

#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"

#include <mockturtle/networks/detail/foreach.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

namespace fiction
{

/**
 * A layout type to layer on top of a coordinate layout, e.g., `cartesian_layout`, `hexagonal_layout`, or
 * `tile_based_layout`. This type extends the layout by providing a notion of FCN clocking. To this end, it utilizes a
 * clocking scheme that assigns each coordinate in the extended coordinate layout a clock number. These clock numbers
 * can be manually overwritten if necessary.
 *
 * In the context of this layout type, coordinates are renamed as clock zones.
 *
 * @tparam CoordinateLayout The coordinate layout type whose coordinates should be clocked.
 */
template <typename CoordinateLayout>
class clocked_layout : public CoordinateLayout
{
  public:
#pragma region Types and constructors

    using clock_zone = typename CoordinateLayout::coordinate;

    using clocking_scheme_t = clocking_scheme<clock_zone>;
    using clock_number_t    = typename clocking_scheme_t::clock_number;

    using degree_t = uint8_t;

    struct clocked_layout_storage
    {
        explicit clocked_layout_storage(const clocking_scheme_t& scheme) noexcept :
                clocking{std::make_shared<clocking_scheme_t>(scheme)}
        {}

        std::shared_ptr<clocking_scheme_t> clocking;
    };

    using base_type = clocked_layout;

    using storage = std::shared_ptr<clocked_layout_storage>;

    /**
     * Standard constructor. Creates a clocked layout of the given aspect ratio and clocks it via the irregular 'open'
     * clocking scheme. This scheme is intended to be used if all clock zones are to be manually assigned.
     *
     * @param ar Highest possible position in the layout.
     */
    explicit clocked_layout(const typename CoordinateLayout::aspect_ratio_type& ar = {}) :
            CoordinateLayout(ar),
            strg{std::make_shared<clocked_layout_storage>(
                open_clocking<clocked_layout<CoordinateLayout>>(num_clks::FOUR))}
    {
        static_assert(is_coordinate_layout_v<CoordinateLayout>, "CoordinateLayout is not a coordinate layout type");
    }
    /**
     * Standard constructor. Creates a clocked layout of the given aspect ratio and clocks it via the given clocking
     * scheme.
     *
     * @param ar Highest possible position in the layout.
     * @param scheme Clocking scheme to apply to this layout.
     */
    clocked_layout(const typename CoordinateLayout::aspect_ratio_type& ar, const clocking_scheme_t& scheme) :
            CoordinateLayout(ar),
            strg{std::make_shared<clocked_layout_storage>(scheme)}
    {
        static_assert(is_coordinate_layout_v<CoordinateLayout>, "CoordinateLayout is not a coordinate layout type");
    }
    /**
     * Copy constructor from another layout's storage.
     *
     * @param s Storage of another clocked_layout.
     */
    explicit clocked_layout(std::shared_ptr<clocked_layout_storage> s) : strg{std::move(s)}
    {
        static_assert(is_coordinate_layout_v<CoordinateLayout>, "CoordinateLayout is not a coordinate layout type");
    }
    /**
     * Copy constructor from another `CoordinateLayout`.
     *
     * @param lyt Coordinate layout.
     */
    explicit clocked_layout(const CoordinateLayout& lyt) :
            CoordinateLayout(lyt),
            strg{std::make_shared<clocked_layout_storage>(
                open_clocking<clocked_layout<CoordinateLayout>>(num_clks::FOUR))}
    {
        static_assert(is_coordinate_layout_v<CoordinateLayout>, "CoordinateLayout is not a coordinate layout type");
    }
    /**
     * Clones the layout returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] clocked_layout clone() const noexcept
    {
        auto copy = clocked_layout(CoordinateLayout::clone());
        copy.strg = std::make_shared<clocked_layout_storage>(*strg);

        return copy;
    }

#pragma endregion

#pragma region Clocking
    /**
     * Replaces the stored clocking scheme with the provided one.
     *
     * @param scheme New clocking scheme.
     */
    void replace_clocking_scheme(const clocking_scheme_t& scheme) noexcept
    {
        strg->clocking = std::make_shared<clocking_scheme_t>(scheme);
    }
    /**
     * Overrides a clock number in the stored scheme with the provided one.
     *
     * @param cz Clock zone to override.
     * @param cn New clock number for `cz`.
     */
    void assign_clock_number(const clock_zone& cz, const clock_number_t cn) noexcept
    {
        strg->clocking->override_clock_number(cz, cn);
    }
    /**
     * Returns the clock number for the given clock zone.
     *
     * @param cz Clock zone.
     * @return Clock number of `cz`.
     */
    [[nodiscard]] clock_number_t get_clock_number(const clock_zone& cz) const noexcept
    {
        return (*strg->clocking)(cz);
    }
    /**
     * Returns the number of clock phases in the layout. Each clock cycle is divided into n phases. In QCA, the number
     * of phases is usually 4. In iNML it is 3. However, theoretically, any number >= 3 can be utilized.
     *
     * @return The number of different clock signals in the layout.
     */
    [[nodiscard]] clock_number_t num_clocks() const noexcept
    {
        return strg->clocking->num_clocks;
    }
    /**
     * Returns whether the layout is clocked by a regular clocking scheme with no overwritten zones.
     *
     * @return `true` iff the layout is clocked by a regular scheme and no zones have been overwritten.
     */
    [[nodiscard]] bool is_regularly_clocked() const noexcept
    {
        return strg->clocking->is_regular();
    }
    /**
     * Compares the stored clocking scheme against the provided name. Names of pre-defined clocking schemes are given in
     * the `clock_name` namespace.
     *
     * @param name Clocking scheme name.
     * @return `true` iff the layout is clocked by a clocking scheme of name `name`.
     */
    [[nodiscard]] bool is_clocking_scheme(const std::string_view& name) const noexcept
    {
        return *strg->clocking == name.data();
    }
    /**
     * Returns a copy of the stored clocking scheme object.
     *
     * @return A copy of the stored clocking scheme object.
     */
    [[nodiscard]] clocking_scheme_t get_clocking_scheme() const noexcept
    {
        return *strg->clocking;
    }
    /**
     * Evaluates whether clock zone `cz2` feeds information to clock zone `cz1`, i.e., whether `cz2` is clocked with a
     * clock number that is lower by 1 modulo `num_clocks()`.
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether its clock number is lower by 1.
     * @return `true` iff `cz2` can feed information to `cz1`.
     */
    [[nodiscard]] bool is_incoming_clocked(const clock_zone& cz1, const clock_zone& cz2) const noexcept
    {
        if (cz1 == cz2)
        {
            return false;
        }

        return static_cast<clock_number_t>((get_clock_number(cz2) + static_cast<clock_number_t>(1)) % num_clocks()) ==
               get_clock_number(cz1);
    }
    /**
     * Evaluates whether clock zone `cz2` accepts information from clock zone `cz1`, i.e., whether `cz2` is clocked with
     * a clock number that is higher by 1 modulo `num_clocks()`.
     *
     * @param cz1 Base clock zone.
     * @param cz2 Clock zone to check whether its clock number is higher by 1.
     * @return `true` iff `cz2` can accept information from `cz1`.
     */
    [[nodiscard]] bool is_outgoing_clocked(const clock_zone& cz1, const clock_zone& cz2) const noexcept
    {
        if (cz1 == cz2)
        {
            return false;
        }

        return static_cast<clock_number_t>((get_clock_number(cz1) + static_cast<clock_number_t>(1)) % num_clocks()) ==
               get_clock_number(cz2);
    }

#pragma endregion

#pragma region Iteration

    /**
     * Returns a container with all clock zones that are incoming to the given one.
     *
     * @param cz Base clock zone.
     * @return A container with all clock zones that are incoming to `cz`.
     */
    [[nodiscard]] auto incoming_clocked_zones(const clock_zone& cz) const noexcept
    {
        std::vector<clock_zone> incoming{};
        incoming.reserve(strg->clocking->max_in_degree);  // reserve memory

        foreach_incoming_clocked_zone(cz, [&incoming](const auto& ct) { incoming.push_back(ct); });

        return incoming;
    }
    /**
     * Applies a function to all incoming clock zones of a given one.
     *
     * @tparam Fn Functor type.
     * @param cz Base clock zone.
     * @param fn Functor to apply to each of `cz`'s incoming clock zones.
     */
    template <typename Fn>
    void foreach_incoming_clocked_zone(const clock_zone& cz, Fn&& fn) const
    {
        CoordinateLayout::foreach_adjacent_coordinate(cz,
                                                      [this, &cz, &fn](const auto& ct)
                                                      {
                                                          if (is_incoming_clocked(cz, ct))
                                                          {
                                                              std::invoke(std::forward<Fn>(fn), ct);
                                                          }
                                                      });
    }
    /**
     * Returns a container with all clock zones that are outgoing from the given one.
     *
     * @param cz Base clock zone.
     * @return A container with all clock zones that are outgoing from `cz`.
     */
    [[nodiscard]] auto outgoing_clocked_zones(const clock_zone& cz) const noexcept
    {
        std::vector<clock_zone> outgoing{};
        outgoing.reserve(strg->clocking->max_out_degree);  // reserve memory

        foreach_outgoing_clocked_zone(cz, [&outgoing](const auto& ct) { outgoing.push_back(ct); });

        return outgoing;
    }
    /**
     * Applies a function to all outgoing clock zones of a given one.
     *
     * @tparam Fn Functor type.
     * @param cz Base clock zone.
     * @param fn Functor to apply to each of `cz`'s outgoing clock zones.
     */
    template <typename Fn>
    void foreach_outgoing_clocked_zone(const clock_zone& cz, Fn&& fn) const
    {
        CoordinateLayout::foreach_adjacent_coordinate(cz,
                                                      [this, &cz, &fn](const auto& ct)
                                                      {
                                                          if (is_outgoing_clocked(cz, ct))
                                                          {
                                                              std::invoke(std::forward<Fn>(fn), ct);
                                                          }
                                                      });
    }

#pragma endregion

#pragma region Structural properties
    /**
     * Returns the number of incoming clock zones to the given one.
     *
     * @param cz Base clock zone.
     * @return Number of `cz`'s incoming clock zones.
     */
    [[nodiscard]] degree_t in_degree(const clock_zone& cz) const noexcept
    {
        degree_t idg{0};
        foreach_incoming_clocked_zone(cz, [&idg](const auto&) { ++idg; });

        return idg;
    }
    /**
     * Returns the number of outgoing clock zones from the given one.
     *
     * @param cz Base clock zone.
     * @return Number of `cz`'s outgoing clock zones.
     */
    [[nodiscard]] degree_t out_degree(const clock_zone& cz) const noexcept
    {
        degree_t odg{0};
        foreach_outgoing_clocked_zone(cz, [&odg](const auto&) { ++odg; });

        return odg;
    }
    /**
     * Returns the number of incoming plus outgoing clock zones of the given one.
     *
     * @param cz Base clock zone.
     * @return Number of `cz`'s incoming plus outgoing clock zones.
     */
    [[nodiscard]] degree_t degree(const clock_zone& cz) const noexcept
    {
        return static_cast<degree_t>(in_degree(cz) + out_degree(cz));
    }

#pragma endregion

  private:
    storage strg;
};

}  // namespace fiction

#endif  // FICTION_CLOCKED_LAYOUT_HPP

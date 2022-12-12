//
// Created by Jan Drewniok on 23.11.22.
//

#ifndef FICTION_CHARGE_DISTRIBUTION_SURFACE_HPP
#define FICTION_CHARGE_DISTRIBUTION_SURFACE_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/traits.hpp"

#include <unordered_map>

namespace fiction
{

/**
 * A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access
 * SiDBs' charge states.
 *
 * @tparam Lyt SiDB cell-level layout.
 * @tparam has_sidb_charge_distribution Automatically determines whether a charge distribution interface is already
 * present.
 */
template <typename Lyt, bool has_charge_distribution_interface =
                            std::conjunction_v<has_assign_charge_state<Lyt>, has_get_charge_state<Lyt>>>
class charge_distribution_surface : public Lyt
{};

template <typename Lyt>
class charge_distribution_surface<Lyt, true> : public Lyt
{
  public:
    explicit charge_distribution_surface(const Lyt& lyt) : Lyt(lyt) {}
};

template <typename Lyt>
class charge_distribution_surface<Lyt, false> : public Lyt
{
  public:
    struct charge_distribution_storage
    {
        std::unordered_map<typename Lyt::coordinate, sidb_charge_state> charge_coordinates{};
    };

    using storage = std::shared_ptr<charge_distribution_storage>;

    /**
     * Standard constructor for empty layouts.
     */
    explicit charge_distribution_surface() : Lyt(), strg{std::make_shared<charge_distribution_storage>()}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }
    /**
     * Standard constructor for existing layouts.
     */
    explicit charge_distribution_surface(const Lyt& lyt) :
            Lyt(lyt),
            strg{std::make_shared<charge_distribution_storage>()}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    };
    /**
     * Assigns a given charge state to the given coordinate.
     *
     * @param c Coordinate to assign charge state cs.
     * @param cs Charge state to assign to coordinate c.
     */
    void assign_charge_state(const coordinate<Lyt>& c, const sidb_charge_state& cs) noexcept
    {
        if (!Lyt::is_empty_cell(c))
        {
            if (cs != sidb_charge_state::NONE)
            {
                strg->charge_coordinates.insert_or_assign(c, cs);
            }
            else
            {
                strg->charge_coordinates.erase(c);
            }
        }
    }
    /**
     * Returns the given coordinate's assigned charge state.
     *
     * @param c Coordinate to check.
     * @return Charge state previously assigned to c or NONE if cell owns emtpy cell_type.
     */
    [[nodiscard]] sidb_charge_state get_charge_state(const coordinate<Lyt>& c) const noexcept
    {
        if (const auto it = strg->charge_coordinates.find(c); it != strg->charge_coordinates.cend())
        {
            return it->second;
        }
        return sidb_charge_state::NONE;
    }
    /**
     * Applies a function to all SiDBs' charge states on the surface. Since the charge states are fetched directly from
     * the storage map, the given function has to receive a pair of a coordinate and a charge state as its parameter.
     *
     * @tparam Fn Functor type that has to comply with the restrictions imposed by mockturtle::foreach_element.
     * @param fn Functor to apply to each charge coordinate.
     */
    template <typename Fn>
    void foreach_charge_state(Fn&& fn) const
    {
        mockturtle::detail::foreach_element(strg->charge_coordinates.cbegin(), strg->charge_coordinates.cend(),
                                            std::forward<Fn>(fn));
    }

  private:
    storage strg;
};

template <class T>
charge_distribution_surface(const T&) -> charge_distribution_surface<T>;

}  // namespace fiction

#endif  // CHARGE_DISTRIBUTION_SURFACE

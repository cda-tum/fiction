//
// Created by marcel on 07.03.22.
//

#ifndef FICTION_SIDB_SURFACE_HPP
#define FICTION_SIDB_SURFACE_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"

#include <set>
#include <type_traits>
#include <unordered_map>

namespace fiction
{

/**
 * A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access
 * fabrication defects on the H-Si(100) surface.
 *
 * @tparam Lyt SiDB cell-level layout.
 * @tparam has_sidb_defect_interface Automatically determines whether a defect interface is already present.
 */
template <typename Lyt,
          bool has_sidb_defect_interface = std::conjunction_v<has_assign_sidb_defect<Lyt>, has_get_sidb_defect<Lyt>>>
class sidb_surface : public Lyt
{};

template <typename Lyt>
class sidb_surface<Lyt, true> : public Lyt
{
  public:
    explicit sidb_surface(const Lyt& lyt) : Lyt(lyt) {}
};

template <typename Lyt>
class sidb_surface<Lyt, false> : public Lyt
{
  public:
    struct sidb_surface_storage
    {
        std::unordered_map<coordinate<Lyt>, sidb_defect_type> defective_coordinates{};
    };

    using storage = std::shared_ptr<sidb_surface_storage>;

    /**
     * Standard constructor for empty layouts.
     */
    sidb_surface() : Lyt(), strg{std::make_shared<sidb_surface_storage>()}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(std::is_same_v<technology<Lyt>, sidb_technology>, "Lyt is not an SiDB layout");
    }
    /**
     * Standard constructor that layers the SiDB defect interface onto an existing layout.
     *
     * @param lyt Existing layout that is to be extended by an SiDB defect interface.
     */
    explicit sidb_surface(const Lyt& lyt) : Lyt(lyt), strg{std::make_shared<sidb_surface_storage>()}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(std::is_same_v<technology<Lyt>, sidb_technology>, "Lyt is not an SiDB layout");
    }
    /**
     * Assigns a given defect type to the given coordinate.
     *
     * @param c Coordinate to assign defect d to.
     * @param d Defect to assign to coordinate c.
     */
    void assign_sidb_defect(const coordinate<Lyt>& c, const sidb_defect_type d) noexcept
    {
        if (d == sidb_defect_type::NONE)
        {
            strg->defective_coordinates.erase(c);
        }
        else
        {
            strg->defective_coordinates[c] = d;
        }
    }
    /**
     * Returns the given coordinate's assigned defect type. If no defect type has been assigned, NONE is returned.
     *
     * @param c Coordinate to check.
     * @return Defect type previously assigned to c or NONE if no defect was yet assigned.
     */
    [[nodiscard]] sidb_defect_type get_sidb_defect(const coordinate<Lyt>& c) const noexcept
    {
        if (auto it = strg->defective_coordinates.find(c); it != strg->defective_coordinates.cend())
        {
            return it->second;
        }
        else
        {
            return sidb_defect_type::NONE;
        }
    }
    /**
     * Returns all SiDB positions affected by the defect at the given coordinate. This function relies on the
     * defect_extent function defined in sidb_defects.hpp that computes the extent of charged and neutral defect types.
     *
     * If the given coordinate is defect-free, the empty set is returned.
     *
     * @param c Coordinate whose defect extent is to be determined.
     * @return All SiDB positions affected by the defect at coordinate c.
     */
    [[nodiscard]] std::set<coordinate<Lyt>> affected_sidbs(const coordinate<Lyt>& c) const noexcept
    {
        std::set<coordinate<Lyt>> influenced_sidbs{};

        if (const auto d = get_sidb_defect(c); d != sidb_defect_type::NONE)
        {
            const auto [horizontal_extent, vertical_extent] = defect_extent(d);

            for (auto y = c.y - vertical_extent; y <= c.y + vertical_extent; ++y)
            {
                for (auto x = c.x - horizontal_extent; x <= c.x + horizontal_extent; ++x)
                {
                    if (const auto affected = coordinate<Lyt>{x, y, c.z}; Lyt::is_within_bounds(affected))
                    {
                        influenced_sidbs.insert(affected);
                    }
                }
            }
        }

        return influenced_sidbs;
    }
    /**
     * Returns all SiDB positions affected by any defect on the surface. This function relies on the defect_extent
     * function defined in sidb_defects.hpp that computes the extent of charged and neutral defect types.
     *
     * If the given surface is defect-free, the empty set is returned.
     *
     * @return All SiDB positions affected by any defect on the surface.
     */
    [[nodiscard]] std::set<coordinate<Lyt>> all_affected_sidbs() const noexcept
    {
        std::set<coordinate<Lyt>> influenced_sidbs{};

        std::for_each(strg->defective_coordinates.cbegin(), strg->defective_coordinates.cend(),
                      [&influenced_sidbs, this](const auto& it)
                      {
                          const auto& [c, d] = it;
                          influenced_sidbs.merge(affected_sidbs(c));
                      });

        return influenced_sidbs;
    }

  private:
    storage strg;
};

template <class T>
sidb_surface(const T&) -> sidb_surface<T>;

}  // namespace fiction

#endif  // FICTION_SIDB_SURFACE_HPP

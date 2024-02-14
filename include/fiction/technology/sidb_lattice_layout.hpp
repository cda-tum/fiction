//
// Created by Jan Drewniok on 12.02.24.
//

#ifndef FICTION_SIDB_LATTICE_LAYOUT_HPP
#define FICTION_SIDB_LATTICE_LAYOUT_HPP

#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/technology/sidb_lattice_properties.hpp"
#include "fiction/traits.hpp"

#include <memory>
#include <utility>

namespace fiction
{
/**
 * A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access the
 * lattice orientation of the H-Si.
 *
 * @tparam Lyt SiDB cell-level layout.
 * @tparam LatticeOrientation Si-H lattice orientation.
 * @tparam has_sidb_lattice_interface Automatically determines whether am SiDB lattice interface is already present.
 */
template <typename Lyt, typename LatticeOrientation = si_lattice_orientations,
          bool has_sidb_lattice_interface = std::conjunction_v<has_assign_lattice_orientation<Lyt, LatticeOrientation>,
                                                               has_get_lattice_orientation<Lyt>>>
class sidb_lattice_layout : public Lyt
{};

template <typename Lyt, typename LatticeOrientation>
class sidb_lattice_layout<Lyt, LatticeOrientation, true> : public Lyt
{
  public:
    explicit sidb_lattice_layout(const Lyt& lyt, [[maybe_unused]] const LatticeOrientation& lattice_orientation = {}) :
            Lyt(lyt)
    {}
};

template <typename Lyt, typename LatticeOrientation>
class sidb_lattice_layout<Lyt, LatticeOrientation, false> : public Lyt
{
  public:
    struct sidb_lattice_layout_storage
    {
        explicit sidb_lattice_layout_storage(const LatticeOrientation&   orientation,
                                             const si_lattice_constants& lattice_consts = si_lattice_constants{}) :
                lattice_orientation{orientation},
                lattice_constants{std::move(lattice_consts)}
        {}
        /**
         * Lattice orientation of the H-Si surface.
         */
        LatticeOrientation lattice_orientation{};
        /**
         * Lattice constants of the H-Si surface.
         */
        si_lattice_constants lattice_constants{};
    };

    using storage = std::shared_ptr<sidb_lattice_layout_storage>;

    explicit sidb_lattice_layout() :
            Lyt(),
            strg{std::make_shared<sidb_lattice_layout_storage>(si_lattice_orientations::SI_100, si_lattice_constants{})}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }

    explicit sidb_lattice_layout(const Lyt& layout) :
            Lyt(layout),
            strg{std::make_shared<sidb_lattice_layout_storage>(si_lattice_orientations::SI_100, si_lattice_constants{})}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }

    explicit sidb_lattice_layout(const LatticeOrientation& orientation) :
            Lyt(),
            strg{std::make_shared<sidb_lattice_layout_storage>(orientation, si_lattice_constants{})}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }

    explicit sidb_lattice_layout(const Lyt& layout, const LatticeOrientation& orientation) :
            Lyt(layout),
            strg{std::make_shared<sidb_lattice_layout_storage>(orientation, si_lattice_constants{})}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }

    /**
     * Clones the layout returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] sidb_lattice_layout clone() const noexcept
    {
        sidb_lattice_layout copy{Lyt::clone()};
        copy.strg = std::make_shared<sidb_lattice_layout_storage>(*strg);

        return copy;
    }
    /**
     * Assigns the lattice orientation to the layout.
     *
     * @param orientation Lattice orientation which is assigned to the layout.
     */
    void assign_lattice_orientation(const LatticeOrientation& orientation) noexcept
    {
        strg->lattice_orientation = orientation;
    }
    /**
     * Gets the lattice orientation of the layout.
     *
     * @return Lattice orientation of the layout.
     */
    [[nodiscard]] LatticeOrientation get_lattice_orientation() const noexcept
    {
        return strg->lattice_orientation;
    }
    /**
     * Assigns the lattice constants to the layout.
     */
    void assign_lattice_constants(const si_lattice_constants& constants) noexcept
    {
        return strg->sidb_lattice_constants = constants;
    }
    /**
     * Gets the lattice constants of the layout.
     *
     * @return Lattice constants of the layout.
     */
    [[nodiscard]] si_lattice_constants get_lattice_constants() const noexcept
    {
        return strg->lattice_constants;
    }

  private:
    storage strg;
};

template <class T>
sidb_lattice_layout(const T&) -> sidb_lattice_layout<T, si_lattice_orientations>;

template <class T, class U>
sidb_lattice_layout(const T&, const U&) -> sidb_lattice_layout<T, U>;

}  // namespace fiction

#endif  // FICTION_SIDB_LATTICE_LAYOUT_HPP

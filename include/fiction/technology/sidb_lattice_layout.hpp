//
// Created by Jan Drewniok on 12.02.24.
//

#ifndef FICTION_SIDB_LATTICE_LAYOUT_HPP
#define FICTION_SIDB_LATTICE_LAYOUT_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/traits.hpp"

#include <memory>

namespace fiction
{
/**
 * A layout type to layer on top of any SiDB cell-level layout. It implements an interface to store and access the
 * lattice orientation of the H-Si.
 *
 * @tparam Lyt SiDB cell-level layout.
 * @tparam LatticeOrientation Si-H lattice orientation.
 * @tparam has_sidb_lattice_interface Automatically determines whether a sidb lattice interface is already present.
 */
template <typename Lyt, typename LatticeOrientation = lattice_orientation,
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
        explicit sidb_lattice_layout_storage(const LatticeOrientation& orientation) : lattice_orientation{orientation}
        {}
        /**
         * Lattice orientation of the H-Si.
         */
        LatticeOrientation lattice_orientation{};
    };

    using storage = std::shared_ptr<sidb_lattice_layout_storage>;

    explicit sidb_lattice_layout(const Lyt& layout) :
            Lyt(layout),
            strg{std::make_shared<sidb_lattice_layout_storage>(lattice_orientation::SI_100)}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }

    explicit sidb_lattice_layout(const LatticeOrientation& orientation) :
            Lyt(),
            strg{std::make_shared<sidb_lattice_layout_storage>(orientation)}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }

    explicit sidb_lattice_layout(const Lyt& layout, const LatticeOrientation& orientation) :
            Lyt(layout),
            strg{std::make_shared<sidb_lattice_layout_storage>(orientation)}
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }
    /**
     * Assign the lattice orientation to the layout.
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

  private:
    storage strg;
};

template <class T>
sidb_lattice_layout(const T&) -> sidb_lattice_layout<T, lattice_orientation>;

template <class T, class U>
sidb_lattice_layout(const T&, const U&) -> sidb_lattice_layout<T, U>;

}  // namespace fiction

#endif  // FICTION_SIDB_LATTICE_LAYOUT_HPP

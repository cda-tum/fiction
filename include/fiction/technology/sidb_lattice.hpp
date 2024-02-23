//
// Created by Jan Drewniok on 12.02.24.
//

#ifndef FICTION_SIDB_LATTICE_HPP
#define FICTION_SIDB_LATTICE_HPP

#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/technology/sidb_lattice_types.hpp"
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
 */
template <typename Lyt, typename LatticeOrientation = sidb_100_lattice>
class sidb_lattice : public Lyt
{
  public:
    using orientation = LatticeOrientation;

    explicit sidb_lattice() : Lyt()
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }
    /**
     * Constructor. Creates a named sidb lattice layout of the given cell-level layout.
     *
     * @param layout Cell-level layout.
     */
    explicit sidb_lattice(const Lyt& layout) : Lyt(layout)
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }
    /**
     * Constructor. Creates a named sidb lattice layout of the given aspect ratio.
     *
     * @param ar Highest possible position in the layout.
     * @param name Layout name.
     */
    explicit sidb_lattice(const aspect_ratio<Lyt>& ar, const std::string& name = "") : Lyt(ar, name)
    {
        static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
        static_assert(has_sidb_technology_v<Lyt>, "Lyt is not an SiDB layout");
    }
    /**
     * Clones the layout returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] sidb_lattice clone() const noexcept
    {
        sidb_lattice copy{Lyt::clone()};

        return copy;
    }
};

template <class T>
sidb_lattice(const T&) -> sidb_lattice<T, typename T::orientation>;

template <class T, class U>
sidb_lattice(const T&, const U&) -> sidb_lattice<T, U>;

}  // namespace fiction

#endif  // FICTION_SIDB_LATTICE_HPP

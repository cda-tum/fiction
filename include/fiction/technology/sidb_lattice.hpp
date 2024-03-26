//
// Created by Jan Drewniok on 12.02.24.
//

#ifndef FICTION_SIDB_LATTICE_HPP
#define FICTION_SIDB_LATTICE_HPP

#include "fiction/traits.hpp"

#include <string>

namespace fiction
{
/**
 * A layout type to layer on top of any SiDB cell-level layout. It implements an interface for different lattice
 * orientations of the H-Si crystal.
 *
 * @tparam Lyt SiDB cell-level layout.
 * @tparam LatticeOrientation Si-H lattice orientation.
 */
template <typename LatticeOrientation, typename Lyt>
class sidb_lattice : public Lyt
{
  public:
    /**
     * This type alias defines the orientation of the lattice using the LatticeOrientation enumeration.
     */
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
     * Clones the layout, returning a deep copy.
     *
     * @return Deep copy of the layout.
     */
    [[nodiscard]] sidb_lattice clone() const noexcept
    {
        sidb_lattice copy{Lyt::clone()};

        return copy;
    }
};

template <class T, class U>
sidb_lattice(const T&, const U&) -> sidb_lattice<T, U>;
}  // namespace fiction

#endif  // FICTION_SIDB_LATTICE_HPP

//
// Created by Jan Drewniok on 28.01.23.
//

#ifndef FICTION_NM_POSITION_HPP
#define FICTION_NM_POSITION_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_parameters.hpp"
#include "fiction/layouts/cell_level_layout.hpp"
#include "fiction/traits.hpp"

#include <utility>

namespace fiction
{

/**
 * Computes the position of a cell in nanometers from the layout origin.
 *
 * @tparam Lyt The layout type.
 * @param sp The simulation parameters (required for the lattice constants).
 * @param c The cell to compute the position for.
 * @return A pair representing the `(x,y)` position of `c` in nanometers from the layout origin.
 */
template <typename Lyt>
constexpr std::pair<units::length::nanometer_t, units::length::nanometer_t>
sidb_nm_position(const sidb_simulation_parameters& sp, const cell<Lyt>& c) noexcept
{
    static_assert(has_siqad_coord_v<Lyt>, "Lyt is not based on SiQAD coordinates");

    const auto x = units::length::nanometer_t(
        units::convert<units::length::angstrom, units::length::nanometer>((c.x * sp.lat_a).value()));
    const auto y = units::length::nanometer_t(
        units::convert<units::length::angstrom, units::length::nanometer>((c.y * sp.lat_b + c.z * sp.lat_c).value()));

    return std::make_pair(x, y);
}
}  // namespace fiction

#endif  // FICTION_NM_POSITION_HPP

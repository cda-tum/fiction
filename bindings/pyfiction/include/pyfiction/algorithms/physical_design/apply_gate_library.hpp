//
// Created by marcel on 20.06.22.
//

#ifndef PYFICTION_APPLY_GATE_LIBRARY_HPP
#define PYFICTION_APPLY_GATE_LIBRARY_HPP

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/cell_level_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/layouts/gate_level_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/technology/fcn_gate_library.hpp>
#include <fiction/technology/inml_topolinano_library.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>

#include <fmt/format.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename GateLibrary, typename GateLyt>
void apply_fcn_gate_library(pybind11::module& m, const std::string& lib_name)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using cell_clk_cart_lyt = fiction::cell_level_layout<
        fiction::technology<GateLibrary>,
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

    m.def(fmt::format("apply_{}_library", lib_name).c_str(),
          &fiction::apply_gate_library<cell_clk_cart_lyt, GateLibrary, GateLyt>, "gate_layout"_a);
}

}  // namespace detail

inline void apply_gate_library(pybind11::module& m)
{
    using gate_clk_cart_lyt = fiction::gate_level_layout<
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;
    detail::apply_fcn_gate_library<fiction::qca_one_library, gate_clk_cart_lyt>(m, "qca_one");

    using gate_clk_hex_lyt = fiction::gate_level_layout<fiction::clocked_layout<
        fiction::tile_based_layout<fiction::hexagonal_layout<fiction::offset::ucoord_t, fiction::even_row_hex>>>>;
    detail::apply_fcn_gate_library<fiction::sidb_bestagon_library, gate_clk_hex_lyt>(m, "bestagon");
}

}  // namespace pyfiction

#endif  // PYFICTION_APPLY_GATE_LIBRARY_HPP

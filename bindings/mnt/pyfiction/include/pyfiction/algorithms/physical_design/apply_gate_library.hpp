//
// Created by marcel on 20.06.22.
//

#ifndef PYFICTION_APPLY_GATE_LIBRARY_HPP
#define PYFICTION_APPLY_GATE_LIBRARY_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/technology/fcn_gate_library.hpp>
#include <fiction/technology/inml_topolinano_library.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/types.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename GateLibrary, typename GateLyt, typename CoordType>
void apply_fcn_gate_library(pybind11::module& m, const std::string& lib_name)
{
    namespace py = pybind11;

    using py_cartesian_technology_cell_layout = py_cartesian_cell_layout<fiction::technology<GateLibrary>, CoordType>;

    m.def(fmt::format("apply_{}_library", lib_name).c_str(),
          &fiction::apply_gate_library<py_cartesian_technology_cell_layout, GateLibrary, GateLyt>, py::arg("layout"),
          DOC(fiction_apply_gate_library));
}

}  // namespace detail

inline void apply_gate_library(pybind11::module& m)
{
    detail::apply_fcn_gate_library<fiction::qca_one_library, py_cartesian_gate_layout<py_offset_coordinate>,
                                   py_offset_coordinate>(m, "qca_one");
    detail::apply_fcn_gate_library<fiction::qca_one_library, py_cartesian_gate_layout<py_cube_coordinate>,
                                   py_cube_coordinate>(m, "qca_one");
    detail::apply_fcn_gate_library<fiction::inml_topolinano_library, py_shifted_cartesian_gate_layout,
                                   py_offset_coordinate>(m, "topolinano");
    detail::apply_fcn_gate_library<fiction::sidb_bestagon_library, py_hexagonal_gate_layout, py_offset_coordinate>(
        m, "bestagon");
}

}  // namespace pyfiction

#endif  // PYFICTION_APPLY_GATE_LIBRARY_HPP

//
// Created by marcel on 08.06.22.
//

#ifndef PYFICTION_EXACT_HPP
#define PYFICTION_EXACT_HPP

#if (FICTION_Z3_SOLVER)

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <fiction/algorithms/physical_design/exact.hpp>
#include <fiction/types.hpp>

#include <sstream>

namespace pyfiction
{

/**
 * SMT-based exact physical design.
 */
inline void exact(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using gate_clk_cart_lyt = fiction::gate_level_layout<
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

    using gate_clk_hex_lyt = fiction::gate_level_layout<fiction::clocked_layout<
        fiction::tile_based_layout<fiction::hexagonal_layout<fiction::offset::ucoord_t, fiction::even_row_hex>>>>;

    py::class_<fiction::exact_physical_design_params>(m, "exact_params")
        .def(py::init<>())
        .def_readwrite("scheme", &fiction::exact_physical_design_params::scheme)
        .def_readwrite("upper_bound", &fiction::exact_physical_design_params::upper_bound)
        .def_readwrite("fixed_size", &fiction::exact_physical_design_params::fixed_size)
        .def_readwrite("num_threads", &fiction::exact_physical_design_params::num_threads)
        .def_readwrite("crossings", &fiction::exact_physical_design_params::crossings)
        .def_readwrite("border_io", &fiction::exact_physical_design_params::border_io)
        .def_readwrite("straight_inverters", &fiction::exact_physical_design_params::straight_inverters)
        .def_readwrite("desynchronize", &fiction::exact_physical_design_params::desynchronize)
        .def_readwrite("minimize_wires", &fiction::exact_physical_design_params::minimize_wires)
        .def_readwrite("minimize_crossings", &fiction::exact_physical_design_params::minimize_crossings)
        .def_readwrite("timeout", &fiction::exact_physical_design_params::timeout)

        ;

    py::class_<fiction::exact_physical_design_stats>(m, "exact_stats")
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::exact_physical_design_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })

        ;

    m.def("exact_cartesian", &fiction::exact<gate_clk_cart_lyt, fiction::tec_nt>, "network"_a,
          "parameters"_a = fiction::exact_physical_design_params{}, "statistics"_a = nullptr);

    m.def("exact_hexagonal", &fiction::exact<gate_clk_hex_lyt, fiction::tec_nt>, "network"_a,
          "parameters"_a = fiction::exact_physical_design_params{}, "statistics"_a = nullptr);
}

}  // namespace pyfiction

#else  // FICTION_Z3_SOLVER

#include "pybind11/pybind11.h"

namespace pyfiction
{

/**
 * Disable SMT-based exact physical design.
 */
inline void exact([[maybe_unused]] pybind11::module& m) {}

}  // namespace pyfiction

#endif  // FICTION_Z3_SOLVER

#endif  // PYFICTION_EXACT_HPP

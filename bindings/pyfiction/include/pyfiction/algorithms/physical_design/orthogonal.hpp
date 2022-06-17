//
// Created by marcel on 08.06.22.
//

#ifndef PYFICTION_ORTHOGONAL_HPP
#define PYFICTION_ORTHOGONAL_HPP

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <fiction/algorithms/physical_design/orthogonal.hpp>
#include <fiction/types.hpp>

#include <sstream>

namespace pyfiction
{

/**
 * OGD-based physical design.
 */
inline void orthogonal(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using gate_clk_cart_lyt = fiction::gate_level_layout<fiction::synchronization_element_layout<
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>>;

    py::class_<fiction::orthogonal_physical_design_params>(m, "orthogonal_params").def(py::init<>())

        ;

    py::class_<fiction::orthogonal_physical_design_stats>(m, "orthogonal_stats")
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::orthogonal_physical_design_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })

        ;

    m.def("orthogonal", &fiction::orthogonal<gate_clk_cart_lyt, fiction::tec_nt>, "network"_a,
          "parameters"_a = fiction::orthogonal_physical_design_params{}, "statistics"_a = nullptr);
}

}  // namespace pyfiction

#endif  // PYFICTION_ORTHOGONAL_HPP

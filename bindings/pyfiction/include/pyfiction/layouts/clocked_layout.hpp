//
// Created by marcel on 03.06.22.
//

#ifndef PYFICTION_CLOCKED_LAYOUT_HPP
#define PYFICTION_CLOCKED_LAYOUT_HPP

#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include <fiction/layouts/cartesian_layout.hpp>
#include <fiction/layouts/clocked_layout.hpp>
#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/traits.hpp>

#include <set>

namespace pyfiction
{

inline void clocked_cartesian_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using cart_clk_lyt = fiction::clocked_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>;

    /**
     * Clocked Cartesian layout.
     */
    py::class_<cart_clk_lyt, fiction::cartesian_layout<fiction::offset::ucoord_t>>(m, "clocked_cartesian_layout")
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<cart_clk_lyt>&>(), "dimension"_a)
        .def(py::init(
            [](const fiction::aspect_ratio<cart_clk_lyt>& dimension, const std::string& scheme_name)
            {
                if (const auto scheme = fiction::get_clocking_scheme<cart_clk_lyt>(scheme_name); scheme.has_value())
                {
                    return cart_clk_lyt{dimension, *scheme};
                }
                else
                {
                    throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                }
            }))

        .def("assign_clock_number", &cart_clk_lyt::assign_clock_number)
        .def("get_clock_number", &cart_clk_lyt::get_clock_number)
        .def("num_clocks", &cart_clk_lyt::num_clocks)
        .def("is_regularly_clocked", &cart_clk_lyt::is_regularly_clocked)

        .def("is_incoming_clocked", &cart_clk_lyt::is_incoming_clocked)
        .def("is_outgoing_clocked", &cart_clk_lyt::is_outgoing_clocked)

        .def("incoming_clocked_zones",
             &cart_clk_lyt::incoming_clocked_zones<std::set<fiction::clock_zone<cart_clk_lyt>>>)
        .def("outgoing_clocked_zones",
             &cart_clk_lyt::outgoing_clocked_zones<std::set<fiction::clock_zone<cart_clk_lyt>>>)

        .def("in_degree", &cart_clk_lyt::in_degree)
        .def("out_degree", &cart_clk_lyt::out_degree)
        .def("degree", &cart_clk_lyt::degree)

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_CLOCKED_LAYOUT_HPP

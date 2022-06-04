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
#include <fiction/layouts/synchronization_element_layout.hpp>
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/traits.hpp>

#include <set>

namespace pyfiction
{

inline void clocked_cartesian_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using clk_cart_lyt = fiction::synchronization_element_layout<
        fiction::clocked_layout<fiction::tile_based_layout<fiction::cartesian_layout<fiction::offset::ucoord_t>>>>;

    /**
     * Clocked Cartesian layout.
     */
    py::class_<clk_cart_lyt, fiction::cartesian_layout<fiction::offset::ucoord_t>>(m, "clocked_cartesian_layout")
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<clk_cart_lyt>&>(), "dimension"_a)
        .def(py::init(
                 [](const fiction::aspect_ratio<clk_cart_lyt>& dimension, const std::string& scheme_name)
                 {
                     if (const auto scheme = fiction::get_clocking_scheme<clk_cart_lyt>(scheme_name);
                         scheme.has_value())
                     {
                         return clk_cart_lyt{dimension, *scheme};
                     }
                     else
                     {
                         throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                     }
                 }),
             "dimension"_a, "clocking_scheme"_a = "2DDWave")

        .def("assign_clock_number", &clk_cart_lyt::assign_clock_number)
        .def("get_clock_number", &clk_cart_lyt::get_clock_number)
        .def("num_clocks", &clk_cart_lyt::num_clocks)
        .def("is_regularly_clocked", &clk_cart_lyt::is_regularly_clocked)

        .def("assign_synchronization_element", &clk_cart_lyt::assign_synchronization_element)
        .def("is_synchronization_element", &clk_cart_lyt::is_synchronization_element)
        .def("get_synchronization_element", &clk_cart_lyt::get_synchronization_element)

        .def("is_incoming_clocked", &clk_cart_lyt::is_incoming_clocked)
        .def("is_outgoing_clocked", &clk_cart_lyt::is_outgoing_clocked)

        .def("incoming_clocked_zones",
             &clk_cart_lyt::incoming_clocked_zones<std::set<fiction::clock_zone<clk_cart_lyt>>>)
        .def("outgoing_clocked_zones",
             &clk_cart_lyt::outgoing_clocked_zones<std::set<fiction::clock_zone<clk_cart_lyt>>>)

        .def("in_degree", &clk_cart_lyt::in_degree)
        .def("out_degree", &clk_cart_lyt::out_degree)
        .def("degree", &clk_cart_lyt::degree)
        .def("num_se", &clk_cart_lyt::num_se)

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_CLOCKED_LAYOUT_HPP

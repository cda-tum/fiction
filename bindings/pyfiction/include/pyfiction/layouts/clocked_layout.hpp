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
#include <fiction/layouts/tile_based_layout.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

#include <set>
#include <string>

namespace pyfiction
{

namespace detail
{

template <typename LytBase>
void clocked_layout(pybind11::module& m, const std::string& topology)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    using clocked_layout = fiction::clocked_layout<fiction::tile_based_layout<LytBase>>;

    /**
     * Clocked Cartesian layout.
     */
    py::class_<clocked_layout, LytBase>(m, fmt::format("clocked_{}_layout", topology).c_str())
        .def(py::init<>())
        .def(py::init<const fiction::aspect_ratio<clocked_layout>&>(), "dimension"_a)
        .def(py::init(
                 [](const fiction::aspect_ratio<clocked_layout>& dimension, const std::string& scheme_name)
                 {
                     if (const auto scheme = fiction::get_clocking_scheme<clocked_layout>(scheme_name);
                         scheme.has_value())
                     {
                         return clocked_layout{dimension, *scheme};
                     }
                     else
                     {
                         throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                     }
                 }),
             "dimension"_a, "clocking_scheme"_a = "2DDWave")

        .def("assign_clock_number", &clocked_layout::assign_clock_number)
        .def("get_clock_number", &clocked_layout::get_clock_number)
        .def("num_clocks", &clocked_layout::num_clocks)
        .def("is_regularly_clocked", &clocked_layout::is_regularly_clocked)

        .def("is_incoming_clocked", &clocked_layout::is_incoming_clocked)
        .def("is_outgoing_clocked", &clocked_layout::is_outgoing_clocked)

        .def("incoming_clocked_zones",
             &clocked_layout::template incoming_clocked_zones<std::set<fiction::clock_zone<clocked_layout>>>)
        .def("outgoing_clocked_zones",
             &clocked_layout::template outgoing_clocked_zones<std::set<fiction::clock_zone<clocked_layout>>>)

        .def("in_degree", &clocked_layout::in_degree)
        .def("out_degree", &clocked_layout::out_degree)
        .def("degree", &clocked_layout::degree)

        ;
}

}  // namespace detail

void clocked_layouts(pybind11::module& m)
{
    using clk_cart_lyt_base = fiction::cartesian_layout<fiction::offset::ucoord_t>;
    detail::clocked_layout<clk_cart_lyt_base>(m, "cartesian");

    using clk_hex_lyt_base = fiction::hexagonal_layout<fiction::offset::ucoord_t, fiction::even_row_hex>;
    detail::clocked_layout<clk_hex_lyt_base>(m, "hexagonal");
}

}  // namespace pyfiction

#endif  // PYFICTION_CLOCKED_LAYOUT_HPP

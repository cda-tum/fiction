//
// Created by Jan Drewniok on 05.11.24.
//

#ifndef PYFICTION_WRITE_SIDB_LAYOUT_TO_SVG_IN_SIQAD_STYLE_HPP
#define PYFICTION_WRITE_SIDB_LAYOUT_TO_SVG_IN_SIQAD_STYLE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_sidb_layout_to_svg_in_siqad_style.hpp>

#include <pybind11/pybind11.h>

#include <string_view>
#include <ostream>

namespace pyfiction
{

namespace detail
{
template <typename Lyt>
inline void write_sidb_layout_to_svg_in_siqad_style_impl(pybind11::module& m)
{
    using namespace pybind11::literals;

    void (*write_sidb_layout_svg_in_siqad_style_pointer)(const Lyt&, const std::string_view&,
                                                         const fiction::write_sidb_layout_svg_in_siqad_style_params&) =
        &fiction::write_sidb_layout_to_svg_in_siqad_style<Lyt>;

    m.def("write_sidb_layout_svg_in_siqad_style", write_sidb_layout_svg_in_siqad_style_pointer, "layout"_a,
          "filename"_a, "ps"_a = fiction::write_sidb_layout_svg_in_siqad_style_params{});

    m.def("write_sidb_layout_to_svg_and_return_as_string", fiction::write_sidb_layout_to_svg_and_return_as_string<Lyt>,
          "layout"_a, "ps"_a = fiction::write_sidb_layout_svg_in_siqad_style_params{});
}

}  // namespace detail

void write_sidb_layout_to_svg_in_siqad_style(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::enum_<fiction::color_mode>(m, "color_mode")
        .value("LIGHT", fiction::color_mode::LIGHT)
        .value("DARK", fiction::color_mode::DARK);

    py::class_<fiction::write_sidb_layout_svg_in_siqad_style_params>(m, "write_sidb_layout_svg_in_siqad_style_params")
        .def(py::init<>())
        .def_readwrite("lattice_point_size", &fiction::write_sidb_layout_svg_in_siqad_style_params::lattice_point_size)
        .def_readwrite("sidb_size", &fiction::write_sidb_layout_svg_in_siqad_style_params::sidb_size)
        .def_readwrite("border_width", &fiction::write_sidb_layout_svg_in_siqad_style_params::border_width)
        .def_readwrite("cmode", &fiction::write_sidb_layout_svg_in_siqad_style_params::cmode);

    detail::write_sidb_layout_to_svg_in_siqad_style_impl<py_charge_distribution_surface_100>(m);
    detail::write_sidb_layout_to_svg_in_siqad_style_impl<py_charge_distribution_surface_100>(m);
    detail::write_sidb_layout_to_svg_in_siqad_style_impl<py_charge_distribution_surface>(m);

    detail::write_sidb_layout_to_svg_in_siqad_style_impl<py_sidb_111_lattice>(m);
    detail::write_sidb_layout_to_svg_in_siqad_style_impl<py_sidb_100_lattice>(m);
    detail::write_sidb_layout_to_svg_in_siqad_style_impl<py_sidb_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_SIDB_LAYOUT_TO_SVG_IN_SIQAD_STYLE_HPP

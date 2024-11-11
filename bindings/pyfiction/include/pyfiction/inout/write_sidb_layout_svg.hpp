//
// Created by Jan Drewniok on 05.11.24.
//

#ifndef PYFICTION_WRITE_SIDB_LAYOUT_SVG_HPP
#define PYFICTION_WRITE_SIDB_LAYOUT_SVG_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_sidb_layout_svg.hpp>

#include <pybind11/pybind11.h>

#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

namespace pyfiction
{

namespace detail
{
template <typename Lyt>
inline void write_sidb_layout_svg_impl(pybind11::module& m)
{
    using namespace pybind11::literals;

    // Pointers to the original functions
    void (*write_sidb_layout_svg_pointer)(const Lyt&, const std::string_view&,
                                          const fiction::write_sidb_layout_svg_params&) =
        &fiction::write_sidb_layout_svg<Lyt>;

    void (*write_sidb_layout_svg_pointer_stream)(
        const Lyt&, std::ostream&, const fiction::write_sidb_layout_svg_params&) = &fiction::write_sidb_layout_svg<Lyt>;

    // Register the original function to write to a file
    m.def("write_sidb_layout_svg", write_sidb_layout_svg_pointer, "layout"_a, "filename"_a,
          "ps"_a = fiction::write_sidb_layout_svg_params{});

    // Register the function to return an SVG as a string
    m.def(
        "write_sidb_layout_svg_and_return_as_string",
        [](const Lyt& layout, const fiction::write_sidb_layout_svg_params& params) -> std::string
        {
            std::ostringstream oss;                               // Create an in-memory output stream
            fiction::write_sidb_layout_svg(layout, oss, params);  // Write to the stream
            return oss.str();                                     // Return the string content
        },
        "layout"_a, "ps"_a = fiction::write_sidb_layout_svg_params{});
}

}  // namespace detail

void write_sidb_layout_svg(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::enum_<fiction::color_mode>(m, "color_mode")
        .value("LIGHT", fiction::color_mode::LIGHT)
        .value("DARK", fiction::color_mode::DARK);

    py::class_<fiction::write_sidb_layout_svg_params>(m, "write_sidb_layout_svg_params")
        .def(py::init<>())
        .def_readwrite("lattice_point_size", &fiction::write_sidb_layout_svg_params::lattice_point_size)
        .def_readwrite("sidb_size", &fiction::write_sidb_layout_svg_params::sidb_size)
        .def_readwrite("sidb_border_width", &fiction::write_sidb_layout_svg_params::sidb_border_width)
        .def_readwrite("color_background", &fiction::write_sidb_layout_svg_params::color_background);

    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface_100>(m);
    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface_100>(m);
    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface>(m);

    detail::write_sidb_layout_svg_impl<py_sidb_111_lattice>(m);
    detail::write_sidb_layout_svg_impl<py_sidb_100_lattice>(m);
    detail::write_sidb_layout_svg_impl<py_sidb_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_SIDB_LAYOUT_SVG_HPP

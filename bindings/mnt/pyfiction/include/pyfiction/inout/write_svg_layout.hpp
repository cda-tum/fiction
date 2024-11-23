//
// Created by marcel on 08.07.22.
//

#ifndef PYFICTION_WRITE_SVG_LAYOUT_HPP
#define PYFICTION_WRITE_SVG_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_svg_layout.hpp>

#include <pybind11/pybind11.h>

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

    // SiDB plot
    m.def("write_sidb_layout_svg", write_sidb_layout_svg_pointer, "layout"_a, "filename"_a,
          "ps"_a = fiction::write_sidb_layout_svg_params{}, DOC(fiction_write_sidb_layout_svg));

    // Register the function to return an SVG as a string
    m.def(
        "write_sidb_layout_svg_to_string",
        [](const Lyt& layout, const fiction::write_sidb_layout_svg_params& params) -> std::string
        {
            std::ostringstream oss;                               // Create an in-memory output stream
            fiction::write_sidb_layout_svg(layout, oss, params);  // Write to the stream
            return oss.str();                                     // Return the string content
        },
        "layout"_a, "ps"_a = fiction::write_sidb_layout_svg_params{}, DOC(fiction_write_sidb_layout_svg));
}

template <typename Lyt>
inline void write_qca_layout_svg_impl(pybind11::module& m)
{
    using namespace pybind11::literals;

    // QCA plot
    void (*write_qca_layout_svg_pointer)(const py_qca_layout&, const std::string_view&,
                                         const fiction::write_qca_layout_svg_params&) =
        &fiction::write_qca_layout_svg<py_qca_layout>;

    m.def("write_qca_layout_svg", write_qca_layout_svg_pointer, "layout"_a, "filename"_a,
          "params"_a = fiction::write_qca_layout_svg_params{}, DOC(fiction_write_qca_layout_svg));
}

}  // namespace detail

void write_svg_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::enum_<fiction::write_sidb_layout_svg_params::color_mode>(m, "color_mode",
                                                                 DOC(fiction_write_sidb_layout_svg_params_color_mode))
        .value("LIGHT", fiction::write_sidb_layout_svg_params::color_mode::LIGHT,
               DOC(fiction_write_sidb_layout_svg_params_color_mode_LIGHT))
        .value("DARK", fiction::write_sidb_layout_svg_params::color_mode::DARK,
               DOC(fiction_write_sidb_layout_svg_params_color_mode_DARK));

    py::enum_<fiction::write_sidb_layout_svg_params::sidb_lattice_mode>(
        m, "sidb_lattice_mode", DOC(fiction_write_sidb_layout_svg_params_sidb_lattice_mode))
        .value("SHOW_LATTICE", fiction::write_sidb_layout_svg_params::sidb_lattice_mode::SHOW_LATTICE,
               DOC(fiction_write_sidb_layout_svg_params_sidb_lattice_mode_SHOW_LATTICE))
        .value("HIDE_LATTICE", fiction::write_sidb_layout_svg_params::sidb_lattice_mode::HIDE_LATTICE,
               DOC(fiction_write_sidb_layout_svg_params_sidb_lattice_mode_HIDE_LATTICE));

    py::class_<fiction::write_sidb_layout_svg_params>(m, "write_sidb_layout_svg_params",
                                                      DOC(fiction_write_sidb_layout_svg_params))
        .def(py::init<>())
        .def_readwrite("lattice_point_size", &fiction::write_sidb_layout_svg_params::lattice_point_size,
                       DOC(fiction_write_sidb_layout_svg_params_lattice_point_size))
        .def_readwrite("sidb_size", &fiction::write_sidb_layout_svg_params::sidb_size,
                       DOC(fiction_write_sidb_layout_svg_params_sidb_size))
        .def_readwrite("sidb_border_width", &fiction::write_sidb_layout_svg_params::sidb_border_width,
                       DOC(fiction_write_sidb_layout_svg_params_sidb_border_width))
        .def_readwrite("color_background", &fiction::write_sidb_layout_svg_params::color_background,
                       DOC(fiction_write_sidb_layout_svg_params_color_background));

    py::class_<fiction::write_qca_layout_svg_params>(m, "write_qca_layout_svg_params",
                                                     DOC(fiction_write_qca_layout_svg_params))
        .def(py::init<>())
        .def_readwrite("simple", &fiction::write_qca_layout_svg_params::simple,
                       DOC(fiction_write_qca_layout_svg_params_simple));
    ;

    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface_111>(m);
    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface_100>(m);
    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface>(m);

    detail::write_sidb_layout_svg_impl<py_sidb_111_lattice>(m);
    detail::write_sidb_layout_svg_impl<py_sidb_100_lattice>(m);
    detail::write_sidb_layout_svg_impl<py_sidb_layout>(m);
    detail::write_qca_layout_svg_impl<py_sidb_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_SVG_LAYOUT_HPP

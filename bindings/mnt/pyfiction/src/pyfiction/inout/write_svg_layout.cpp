//
// Created by marcel on 08.07.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_svg_layout.hpp>

#include <sstream>
#include <string>
#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/tuple.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{
template <typename Lyt>
void write_sidb_layout_svg_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // Pointers to the original functions
    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_sidb_layout_svg_pointer)(const Lyt&, const std::string_view&,
                                                const fiction::write_sidb_layout_svg_params&) =
        &fiction::write_sidb_layout_svg<Lyt>;

    // SiDB plot
    m.def("write_sidb_layout_svg", write_sidb_layout_svg_pointer, py::arg("layout"), py::arg("filename"),
          py::arg("ps") = fiction::write_sidb_layout_svg_params{}, DOC(fiction_write_sidb_layout_svg));

    // Register the function to return an SVG as a string
    m.def(
        "write_sidb_layout_svg_to_string",
        [](const Lyt& layout, const fiction::write_sidb_layout_svg_params& params) -> std::string
        {
            std::ostringstream oss;                               // Create an in-memory output stream
            fiction::write_sidb_layout_svg(layout, oss, params);  // Write to the stream
            return oss.str();                                     // Return the string content
        },
        py::arg("layout"), py::arg("ps") = fiction::write_sidb_layout_svg_params{}, DOC(fiction_write_sidb_layout_svg));
}

template <typename Lyt>
void write_qca_layout_svg_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // QCA plot
    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_qca_layout_svg_pointer)(const py_qca_layout&, const std::string_view&,
                                               const fiction::write_qca_layout_svg_params&) =
        &fiction::write_qca_layout_svg<py_qca_layout>;

    m.def("write_qca_layout_svg", write_qca_layout_svg_pointer, py::arg("layout"), py::arg("filename"),
          py::arg("params") = fiction::write_qca_layout_svg_params{}, DOC(fiction_write_qca_layout_svg));
}

template <typename Lyt>
void write_mol_qca_layout_svg_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // MolQCA plot
    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_mol_qca_layout_svg_pointer)(const py_mol_qca_layout&, const std::string_view&,
                                                   const fiction::write_qca_layout_svg_params&) =
        &fiction::write_mol_qca_layout_svg<py_mol_qca_layout>;

    m.def("write_mol_qca_layout_svg", write_mol_qca_layout_svg_pointer, py::arg("layout"), py::arg("filename"),
          py::arg("params") = fiction::write_qca_layout_svg_params{}, DOC(fiction_write_mol_qca_layout_svg));
}

}  // namespace detail

void write_svg_layout(nanobind::module_& m)
{
    namespace py = nanobind;

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
        .def(py::init<>(), "Default constructor.")
        .def_rw("lattice_point_size", &fiction::write_sidb_layout_svg_params::lattice_point_size,
                DOC(fiction_write_sidb_layout_svg_params_lattice_point_size))
        .def_rw("sidb_size", &fiction::write_sidb_layout_svg_params::sidb_size,
                DOC(fiction_write_sidb_layout_svg_params_sidb_size))
        .def_rw("sidb_border_width", &fiction::write_sidb_layout_svg_params::sidb_border_width,
                DOC(fiction_write_sidb_layout_svg_params_sidb_border_width))
        .def_rw("color_background", &fiction::write_sidb_layout_svg_params::color_background,
                DOC(fiction_write_sidb_layout_svg_params_color_background))
        .def_rw("lattice_mode", &fiction::write_sidb_layout_svg_params::lattice_mode,
                DOC(fiction_write_sidb_layout_svg_params_lattice_mode))

        ;

    py::class_<fiction::write_qca_layout_svg_params>(m, "write_qca_layout_svg_params",
                                                     DOC(fiction_write_qca_layout_svg_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("simple", &fiction::write_qca_layout_svg_params::simple,
                DOC(fiction_write_qca_layout_svg_params_simple));
    ;

    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface_111>(m);
    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface_100>(m);
    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface>(m);

    detail::write_sidb_layout_svg_impl<py_sidb_111_lattice>(m);
    detail::write_sidb_layout_svg_impl<py_sidb_100_lattice>(m);
    detail::write_sidb_layout_svg_impl<py_sidb_layout>(m);
    detail::write_qca_layout_svg_impl<py_sidb_layout>(m);
    detail::write_mol_qca_layout_svg_impl<py_mol_qca_layout>(m);
}

}  // namespace pyfiction

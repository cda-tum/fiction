//
// Created by marcel on 08.07.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/io/write_sidb_layout_svg.hpp>

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
{
    template <typename Lyt>
    void write_sidb_layout_svg_impl(nanobind::module_ & m)
    {
        namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

        // Pointers to the original functions
        // NOLINTNEXTLINE(misc-const-correctness)
        void (*const write_sidb_layout_svg_pointer)(const Lyt&, const std::string_view&,
                                                    const fiction::sidb::io::write_sidb_layout_svg_params&) =
            &fiction::sidb::io::write_sidb_layout_svg<Lyt>;

        // SiDB plot
        m.def("write_sidb_layout_svg", write_sidb_layout_svg_pointer, py::arg("layout"), py::arg("filename"),
              py::arg("ps") = fiction::sidb::io::write_sidb_layout_svg_params{},
              DOC(fiction_sidb_io_write_sidb_layout_svg));

        // Register the function to return an SVG as a string
        m.def(
            "write_sidb_layout_svg_to_string",
            [](const Lyt& layout, const fiction::sidb::io::write_sidb_layout_svg_params& params) -> std::string
            {
                std::ostringstream oss;                                         // Create an in-memory output stream
                fiction::sidb::io::write_sidb_layout_svg(layout, oss, params);  // Write to the stream
                return oss.str();                                               // Return the string content
            },
            py::arg("layout"), py::arg("ps") = fiction::sidb::io::write_sidb_layout_svg_params{},
            DOC(fiction_sidb_io_write_sidb_layout_svg));
    }

}  // namespace detail

void write_sidb_layout_svg(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::sidb::io::write_sidb_layout_svg_params::color_mode>(
        m, "color_mode", DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_mode))
        .value("LIGHT", fiction::sidb::io::write_sidb_layout_svg_params::color_mode::LIGHT,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_mode_LIGHT))
        .value("DARK", fiction::sidb::io::write_sidb_layout_svg_params::color_mode::DARK,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_mode_DARK));

    py::enum_<fiction::sidb::io::write_sidb_layout_svg_params::sidb_lattice_mode>(
        m, "sidb_lattice_mode", DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_lattice_mode))
        .value("SHOW_LATTICE", fiction::sidb::io::write_sidb_layout_svg_params::sidb_lattice_mode::SHOW_LATTICE,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_lattice_mode_SHOW_LATTICE))
        .value("HIDE_LATTICE", fiction::sidb::io::write_sidb_layout_svg_params::sidb_lattice_mode::HIDE_LATTICE,
               DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_lattice_mode_HIDE_LATTICE));

    py::class_<fiction::sidb::io::write_sidb_layout_svg_params>(m, "write_sidb_layout_svg_params",
                                                                DOC(fiction_sidb_io_write_sidb_layout_svg_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("lattice_point_size", &fiction::sidb::io::write_sidb_layout_svg_params::lattice_point_size,
                DOC(fiction_sidb_io_write_sidb_layout_svg_params_lattice_point_size))
        .def_rw("sidb_size", &fiction::sidb::io::write_sidb_layout_svg_params::sidb_size,
                DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_size))
        .def_rw("sidb_border_width", &fiction::sidb::io::write_sidb_layout_svg_params::sidb_border_width,
                DOC(fiction_sidb_io_write_sidb_layout_svg_params_sidb_border_width))
        .def_rw("color_background", &fiction::sidb::io::write_sidb_layout_svg_params::color_background,
                DOC(fiction_sidb_io_write_sidb_layout_svg_params_color_background))
        .def_rw("lattice_mode", &fiction::sidb::io::write_sidb_layout_svg_params::lattice_mode,
                DOC(fiction_sidb_io_write_sidb_layout_svg_params_lattice_mode))

        ;

    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface_111>(m);
    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface_100>(m);
    detail::write_sidb_layout_svg_impl<py_charge_distribution_surface>(m);

    detail::write_sidb_layout_svg_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/orthogonal.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void orthogonal(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::physical_design::orthogonal_physical_design_params>(
        m, "orthogonal_params", DOC(fiction_physical_design_orthogonal_physical_design_params))
        .def(py::init<>(), "Default constructor.");

    py::class_<fiction::physical_design::orthogonal_physical_design_stats>(
        m, "orthogonal_stats", DOC(fiction_physical_design_orthogonal_physical_design_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::physical_design::orthogonal_physical_design_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def("report", &fiction::physical_design::orthogonal_physical_design_stats::report,
             DOC(fiction_physical_design_orthogonal_physical_design_stats_report))
        .def_ro("time_total", &fiction::physical_design::orthogonal_physical_design_stats::time_total,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_time_total))
        .def_ro("x_size", &fiction::physical_design::orthogonal_physical_design_stats::x_size,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_x_size))
        .def_ro("y_size", &fiction::physical_design::orthogonal_physical_design_stats::y_size,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_y_size))
        .def_ro("num_gates", &fiction::physical_design::orthogonal_physical_design_stats::num_gates,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_num_gates))
        .def_ro("num_wires", &fiction::physical_design::orthogonal_physical_design_stats::num_wires,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_num_wires))
        .def_ro("num_crossings", &fiction::physical_design::orthogonal_physical_design_stats::num_crossings,
                DOC(fiction_physical_design_orthogonal_physical_design_stats_num_crossings));

    m.def("orthogonal", &fiction::physical_design::orthogonal<py_cartesian_gate_layout, py_logic_network>,
          py::arg("network"), py::arg("parameters") = fiction::physical_design::orthogonal_physical_design_params{},
          py::arg("statistics") = nullptr, DOC(fiction_physical_design_orthogonal));
}

}  // namespace pyfiction

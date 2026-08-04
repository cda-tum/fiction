

#if (FICTION_Z3_SOLVER)

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/exact.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

void exact(nanobind::module_& m)
{
    namespace py = nanobind;

    py::enum_<fiction::technology_constraints>(m, "technology_constraints", DOC(fiction_technology_constraints))
        .value("NONE", fiction::technology_constraints::NONE, DOC(fiction_technology_constraints_NONE))
        .value("TOPOLINANO", fiction::technology_constraints::TOPOLINANO,
               DOC(fiction_technology_constraints_TOPOLINANO));

    py::class_<fiction::exact_physical_design_params>(m, "exact_params", DOC(fiction_exact_physical_design_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("scheme", &fiction::exact_physical_design_params::scheme,
                DOC(fiction_exact_physical_design_params_scheme))
        .def_rw("upper_bound_x", &fiction::exact_physical_design_params::upper_bound_x,
                DOC(fiction_exact_physical_design_params_upper_bound_x))
        .def_rw("upper_bound_y", &fiction::exact_physical_design_params::upper_bound_y,
                DOC(fiction_exact_physical_design_params_upper_bound_y))
        .def_rw("fixed_size", &fiction::exact_physical_design_params::fixed_size,
                DOC(fiction_exact_physical_design_params_fixed_size))
        .def_rw("num_threads", &fiction::exact_physical_design_params::num_threads,
                DOC(fiction_exact_physical_design_params_num_threads))
        .def_rw("crossings", &fiction::exact_physical_design_params::crossings,
                DOC(fiction_exact_physical_design_params_crossings))
        .def_rw("border_io", &fiction::exact_physical_design_params::border_io,
                DOC(fiction_exact_physical_design_params_border_io))
        .def_rw("straight_inverters", &fiction::exact_physical_design_params::straight_inverters,
                DOC(fiction_exact_physical_design_params_straight_inverters))
        .def_rw("desynchronize", &fiction::exact_physical_design_params::desynchronize,
                DOC(fiction_exact_physical_design_params_desynchronize))
        .def_rw("minimize_wires", &fiction::exact_physical_design_params::minimize_wires,
                DOC(fiction_exact_physical_design_params_minimize_wires))
        .def_rw("minimize_crossings", &fiction::exact_physical_design_params::minimize_crossings,
                DOC(fiction_exact_physical_design_params_minimize_crossings))
        .def_rw("timeout", &fiction::exact_physical_design_params::timeout,
                DOC(fiction_exact_physical_design_params_timeout))
        .def_rw("technology_specifics", &fiction::exact_physical_design_params::technology_specifics,
                DOC(fiction_exact_physical_design_params_technology_specifics));

    py::class_<fiction::exact_physical_design_stats>(m, "exact_stats", DOC(fiction_exact_physical_design_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::exact_physical_design_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def("report", &fiction::exact_physical_design_stats::report, DOC(fiction_exact_physical_design_stats_report))
        .def_ro("time_total", &fiction::exact_physical_design_stats::time_total,
                DOC(fiction_exact_physical_design_stats_duration))
        .def_ro("x_size", &fiction::exact_physical_design_stats::x_size,
                DOC(fiction_exact_physical_design_stats_x_size))
        .def_ro("y_size", &fiction::exact_physical_design_stats::y_size,
                DOC(fiction_exact_physical_design_stats_y_size))
        .def_ro("num_gates", &fiction::exact_physical_design_stats::num_gates,
                DOC(fiction_exact_physical_design_stats_num_gates))
        .def_ro("num_wires", &fiction::exact_physical_design_stats::num_wires,
                DOC(fiction_exact_physical_design_stats_num_wires))
        .def_ro("num_crossings", &fiction::exact_physical_design_stats::num_crossings,
                DOC(fiction_exact_physical_design_stats_num_crossings))
        .def_ro("num_aspect_ratios", &fiction::exact_physical_design_stats::num_aspect_ratios,
                DOC(fiction_exact_physical_design_stats_num_aspect_ratios));

    m.def("exact_cartesian", &fiction::exact<py_cartesian_gate_layout, py_logic_network>, py::arg("network"),
          py::arg("parameters") = fiction::exact_physical_design_params{}, py::arg("statistics") = nullptr,
          DOC(fiction_exact));

    m.def("exact_shifted_cartesian", &fiction::exact<py_shifted_cartesian_gate_layout, py_logic_network>,
          py::arg("network"), py::arg("parameters") = fiction::exact_physical_design_params{},
          py::arg("statistics") = nullptr, DOC(fiction_exact));

    m.def("exact_hexagonal", &fiction::exact<py_hexagonal_gate_layout, py_logic_network>, py::arg("network"),
          py::arg("parameters") = fiction::exact_physical_design_params{}, py::arg("statistics") = nullptr,
          DOC(fiction_exact));
}

}  // namespace pyfiction

#else  // FICTION_Z3_SOLVER

namespace pyfiction
{

/**
 * Disable SMT-based exact physical design.
 */
void exact([[maybe_unused]] nanobind::module_& m) {}

}  // namespace pyfiction

#endif  // FICTION_Z3_SOLVER

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/wiring_reduction.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/chrono.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

void wiring_reduction(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::wiring_reduction_params>(m, "wiring_reduction_params", DOC(fiction_wiring_reduction_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("timeout", &fiction::wiring_reduction_params::timeout, DOC(fiction_wiring_reduction_params_timeout));

    py::class_<fiction::wiring_reduction_stats>(m, "wiring_reduction_stats", DOC(fiction_wiring_reduction_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::wiring_reduction_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def("report", &fiction::wiring_reduction_stats::report, DOC(fiction_wiring_reduction_stats_report))
        .def_ro("time_total", &fiction::wiring_reduction_stats::time_total,
                DOC(fiction_wiring_reduction_stats_duration))
        .def_ro("x_size_before", &fiction::wiring_reduction_stats::x_size_before,
                DOC(fiction_wiring_reduction_stats_x_size_before))
        .def_ro("y_size_before", &fiction::wiring_reduction_stats::y_size_before,
                DOC(fiction_wiring_reduction_stats_y_size_before))
        .def_ro("x_size_after", &fiction::wiring_reduction_stats::x_size_after,
                DOC(fiction_wiring_reduction_stats_x_size_after))
        .def_ro("y_size_after", &fiction::wiring_reduction_stats::y_size_after,
                DOC(fiction_wiring_reduction_stats_y_size_after))
        .def_ro("num_wires_before", &fiction::wiring_reduction_stats::num_wires_before,
                DOC(fiction_wiring_reduction_stats_num_wires_before))
        .def_ro("num_wires_after", &fiction::wiring_reduction_stats::num_wires_after,
                DOC(fiction_wiring_reduction_stats_num_wires_after))
        .def_ro("wiring_improvement", &fiction::wiring_reduction_stats::wiring_improvement,
                DOC(fiction_wiring_reduction_stats_wiring_improvement))
        .def_ro("area_improvement", &fiction::wiring_reduction_stats::area_improvement,
                DOC(fiction_wiring_reduction_stats_area_improvement))

        ;

    m.def("wiring_reduction", &fiction::wiring_reduction<py_cartesian_gate_layout>, py::arg("layout"),
          py::arg("parameters") = fiction::wiring_reduction_params{}, py::arg("statistics") = nullptr,
          DOC(fiction_wiring_reduction));
}

}  // namespace pyfiction

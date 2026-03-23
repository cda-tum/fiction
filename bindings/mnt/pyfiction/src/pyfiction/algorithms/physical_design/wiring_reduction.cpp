#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/wiring_reduction.hpp>

#include <pybind11/chrono.h>
#include <pybind11/pybind11.h>

#include <sstream>

namespace pyfiction
{

void wiring_reduction(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;

    py::class_<fiction::wiring_reduction_params>(m, "wiring_reduction_params", DOC(fiction_wiring_reduction_params))
        .def(py::init<>())
        .def_readwrite("timeout", &fiction::wiring_reduction_params::timeout,
                       DOC(fiction_wiring_reduction_params_timeout));

    py::class_<fiction::wiring_reduction_stats>(m, "wiring_reduction_stats", DOC(fiction_wiring_reduction_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::wiring_reduction_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })
        .def("report", &fiction::wiring_reduction_stats::report, DOC(fiction_wiring_reduction_stats_report))
        .def_readonly("time_total", &fiction::wiring_reduction_stats::time_total,
                      DOC(fiction_wiring_reduction_stats_duration))
        .def_readonly("x_size_before", &fiction::wiring_reduction_stats::x_size_before,
                      DOC(fiction_wiring_reduction_stats_x_size_before))
        .def_readonly("y_size_before", &fiction::wiring_reduction_stats::y_size_before,
                      DOC(fiction_wiring_reduction_stats_y_size_before))
        .def_readonly("x_size_after", &fiction::wiring_reduction_stats::x_size_after,
                      DOC(fiction_wiring_reduction_stats_x_size_after))
        .def_readonly("y_size_after", &fiction::wiring_reduction_stats::y_size_after,
                      DOC(fiction_wiring_reduction_stats_y_size_after))
        .def_readonly("num_wires_before", &fiction::wiring_reduction_stats::num_wires_before,
                      DOC(fiction_wiring_reduction_stats_num_wires_before))
        .def_readonly("num_wires_after", &fiction::wiring_reduction_stats::num_wires_after,
                      DOC(fiction_wiring_reduction_stats_num_wires_after))
        .def_readonly("wiring_improvement", &fiction::wiring_reduction_stats::wiring_improvement,
                      DOC(fiction_wiring_reduction_stats_wiring_improvement))
        .def_readonly("area_improvement", &fiction::wiring_reduction_stats::area_improvement,
                      DOC(fiction_wiring_reduction_stats_area_improvement))

        ;

    m.def("wiring_reduction", &fiction::wiring_reduction<py_cartesian_gate_layout>, py::arg("layout"),
          py::arg("parameters") = fiction::wiring_reduction_params{}, py::arg("statistics") = nullptr,
          DOC(fiction_wiring_reduction));
}

}  // namespace pyfiction

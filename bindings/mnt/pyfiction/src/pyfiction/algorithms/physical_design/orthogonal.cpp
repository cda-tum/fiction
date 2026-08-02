#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/orthogonal.hpp>

#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
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

void orthogonal(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::orthogonal_physical_design_params>(m, "orthogonal_params",
                                                           DOC(fiction_orthogonal_physical_design_params))
        .def(py::init<>(), "Default constructor.");

    py::class_<fiction::orthogonal_physical_design_stats>(m, "orthogonal_stats",
                                                          DOC(fiction_orthogonal_physical_design_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::orthogonal_physical_design_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def("report", &fiction::orthogonal_physical_design_stats::report,
             DOC(fiction_orthogonal_physical_design_stats_report))
        .def_ro("time_total", &fiction::orthogonal_physical_design_stats::time_total,
                DOC(fiction_orthogonal_physical_design_stats_duration))
        .def_ro("x_size", &fiction::orthogonal_physical_design_stats::x_size,
                DOC(fiction_orthogonal_physical_design_stats_x_size))
        .def_ro("y_size", &fiction::orthogonal_physical_design_stats::y_size,
                DOC(fiction_orthogonal_physical_design_stats_y_size))
        .def_ro("num_gates", &fiction::orthogonal_physical_design_stats::num_gates,
                DOC(fiction_orthogonal_physical_design_stats_num_gates))
        .def_ro("num_wires", &fiction::orthogonal_physical_design_stats::num_wires,
                DOC(fiction_orthogonal_physical_design_stats_num_wires))
        .def_ro("num_crossings", &fiction::orthogonal_physical_design_stats::num_crossings,
                DOC(fiction_orthogonal_physical_design_stats_num_crossings));

    m.def("orthogonal", &fiction::orthogonal<py_cartesian_gate_layout, py_logic_network>, py::arg("network"),
          py::arg("parameters") = fiction::orthogonal_physical_design_params{}, py::arg("statistics") = nullptr,
          DOC(fiction_orthogonal));
}

}  // namespace pyfiction

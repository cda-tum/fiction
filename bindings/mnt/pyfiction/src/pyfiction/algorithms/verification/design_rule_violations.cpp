#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/verification/design_rule_violations.hpp>

#include <cstddef>
#include <sstream>
#include <utility>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void gate_level_drvs_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "gate_level_drvs",
        [](const Lyt& lyt, fiction::gate_level_drv_params params = {},
           const bool print_report = false) -> std::pair<std::size_t, std::size_t>
        {
            std::ostringstream report_stream{};
            params.out = &report_stream;

            fiction::gate_level_drv_stats stats{};

            fiction::gate_level_drvs(lyt, params, &stats);

            if (print_report)
            {
                nanobind::print(report_stream.str().c_str());
            }

            return {stats.warnings, stats.drvs};
        },
        py::arg("layout"), py::arg("params") = fiction::gate_level_drv_params{}, py::arg("print_report") = false,
        DOC(fiction_gate_level_drvs));
}

}  // namespace detail

void design_rule_violations(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::gate_level_drv_params>(m, "gate_level_drv_params", DOC(fiction_gate_level_drv_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("unplaced_nodes", &fiction::gate_level_drv_params::unplaced_nodes,
                DOC(fiction_gate_level_drv_params_unplaced_nodes))
        .def_rw("placed_dead_nodes", &fiction::gate_level_drv_params::placed_dead_nodes,
                DOC(fiction_gate_level_drv_params_placed_dead_nodes))
        .def_rw("non_adjacent_connections", &fiction::gate_level_drv_params::non_adjacent_connections,
                DOC(fiction_gate_level_drv_params_non_adjacent_connections))
        .def_rw("missing_connections", &fiction::gate_level_drv_params::missing_connections,
                DOC(fiction_gate_level_drv_params_missing_connections))
        .def_rw("crossing_gates", &fiction::gate_level_drv_params::crossing_gates,
                DOC(fiction_gate_level_drv_params_crossing_gates))
        .def_rw("clocked_data_flow", &fiction::gate_level_drv_params::clocked_data_flow,
                DOC(fiction_gate_level_drv_params_clocked_data_flow))
        .def_rw("has_io", &fiction::gate_level_drv_params::has_io, DOC(fiction_gate_level_drv_params_has_io))
        .def_rw("empty_io", &fiction::gate_level_drv_params::empty_io, DOC(fiction_gate_level_drv_params_empty_io))
        .def_rw("io_pins", &fiction::gate_level_drv_params::io_pins, DOC(fiction_gate_level_drv_params_io_pins))
        .def_rw("border_io", &fiction::gate_level_drv_params::border_io, DOC(fiction_gate_level_drv_params_border_io))

        ;

    detail::gate_level_drvs_impl<py_cartesian_gate_layout>(m);
    detail::gate_level_drvs_impl<py_shifted_cartesian_gate_layout>(m);
    detail::gate_level_drvs_impl<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

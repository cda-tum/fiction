#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/critical_temperature.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp>
#include <fiction/algorithms/simulation/sidb/detect_bdl_wires.hpp>

#include <sstream>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void critical_temperature_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "critical_temperature_gate_based",
        py::overload_cast<const Lyt&, const std::vector<py_tt>&, const fiction::critical_temperature_params&,
                          fiction::critical_temperature_stats*>(&fiction::critical_temperature_gate_based<Lyt, py_tt>),
        py::arg("lyt"), py::arg("spec"), py::arg("params") = fiction::critical_temperature_params{},
        py::arg("stats") = nullptr, DOC(fiction_critical_temperature_gate_based));

    m.def("critical_temperature_gate_based",
          py::overload_cast<
              const std::vector<Lyt>&, const std::vector<py_tt>&, const fiction::critical_temperature_params&,
              const std::vector<fiction::bdl_pair<fiction::cell<Lyt>>>&, const std::vector<fiction::bdl_wire<Lyt>>&,
              const std::vector<fiction::bdl_wire<Lyt>>&, fiction::critical_temperature_stats*>(
              &fiction::critical_temperature_gate_based<Lyt, py_tt>),
          py::arg("input_pattern_layouts"), py::arg("spec"), py::arg("params"), py::arg("output_bdl_pairs"),
          py::arg("input_bdl_wires"), py::arg("output_bdl_wires"), py::arg("stats") = nullptr,
          DOC(fiction_critical_temperature_gate_based_2));

    m.def("critical_temperature_non_gate_based", &fiction::critical_temperature_non_gate_based<Lyt>, py::arg("lyt"),
          py::arg("params") = fiction::critical_temperature_params{}, py::arg("stats") = nullptr,
          DOC(fiction_critical_temperature_non_gate_based));
}

}  // namespace detail

void critical_temperature(nanobind::module_& m)
{
    namespace py = nanobind;

    /**
     * Critical temperature statistics.
     */
    py::class_<fiction::critical_temperature_stats>(m, "critical_temperature_stats",
                                                    DOC(fiction_critical_temperature_stats))
        .def(py::init<>(), "Default constructor.")
        .def(
            "__repr__",
            [](const fiction::critical_temperature_stats& stats)
            {
                std::stringstream stream{};
                stats.report(stream);
                return stream.str();
            },
            "Returns a string representation of the statistics.")
        .def("report", &fiction::critical_temperature_stats::report, DOC(fiction_critical_temperature_stats_report))
        .def_ro("algorithm_name", &fiction::critical_temperature_stats::algorithm_name,
                DOC(fiction_critical_temperature_stats_algorithm_name))
        .def_ro("num_valid_lyt", &fiction::critical_temperature_stats::num_valid_lyt,
                DOC(fiction_critical_temperature_stats_num_valid_lyt))
        .def_ro("is_ground_state_transparent",
                &fiction::critical_temperature_stats::energy_between_ground_state_and_first_erroneous,
                DOC(fiction_critical_temperature_stats_energy_between_ground_state_and_first_erroneous))

        ;

    /**
     * Critical temperature parameters.
     */
    py::class_<fiction::critical_temperature_params>(m, "critical_temperature_params",
                                                     DOC(fiction_critical_temperature_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("operational_params", &fiction::critical_temperature_params::operational_params,
                DOC(fiction_critical_temperature_params))
        .def_rw("confidence_level", &fiction::critical_temperature_params::confidence_level,
                DOC(fiction_critical_temperature_params_confidence_level))
        .def_rw("max_temperature", &fiction::critical_temperature_params::max_temperature,
                DOC(fiction_critical_temperature_params_max_temperature));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::critical_temperature_impl<py_sidb_100_lattice>(m);
    detail::critical_temperature_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

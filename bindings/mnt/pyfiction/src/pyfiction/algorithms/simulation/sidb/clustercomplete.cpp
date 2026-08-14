#if (FICTION_ALGLIB_ENABLED)

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/clustercomplete.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/map.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void clustercomplete_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("clustercomplete", &fiction::clustercomplete<Lyt>, py::arg("lyt"),
          py::arg("params") = fiction::clustercomplete_params<>{}, DOC(fiction_clustercomplete));
}

}  // namespace detail

void clustercomplete(nanobind::module_& m)
{
    namespace py = nanobind;

    /**
     * Report *Ground State Space* stats.
     */
    py::enum_<fiction::clustercomplete_params<>::ground_state_space_reporting>(
        m, "ground_state_space_reporting", DOC(fiction_clustercomplete_params_ground_state_space_reporting))
        .value("ON", fiction::clustercomplete_params<>::ground_state_space_reporting::ON,
               DOC(fiction_clustercomplete_params_ground_state_space_reporting_ON))
        .value("OFF", fiction::clustercomplete_params<>::ground_state_space_reporting::OFF,
               DOC(fiction_clustercomplete_params_ground_state_space_reporting_OFF));

    /**
     * ClusterComplete parameters.
     */
    py::class_<fiction::clustercomplete_params<>>(m, "clustercomplete_params", DOC(fiction_clustercomplete_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("simulation_parameters", &fiction::clustercomplete_params<>::simulation_parameters,
                DOC(fiction_clustercomplete_params_simulation_parameters))
        .def_rw("local_external_potential", &fiction::clustercomplete_params<>::local_external_potential,
                DOC(fiction_clustercomplete_params_local_external_potential))
        .def_rw("global_potential", &fiction::clustercomplete_params<>::global_potential,
                DOC(fiction_clustercomplete_params_global_potential))
        .def_rw("validity_witness_partitioning_max_cluster_size_gss",
                &fiction::clustercomplete_params<>::validity_witness_partitioning_max_cluster_size_gss,
                DOC(fiction_clustercomplete_params_validity_witness_partitioning_max_cluster_size_gss))
        .def_rw("num_overlapping_witnesses_limit_gss",
                &fiction::clustercomplete_params<>::num_overlapping_witnesses_limit_gss,
                DOC(fiction_clustercomplete_params_num_overlapping_witnesses_limit_gss))
        .def_rw("available_threads", &fiction::clustercomplete_params<>::available_threads,
                DOC(fiction_clustercomplete_params_available_threads))
        .def_rw("report_gss_stats", &fiction::clustercomplete_params<>::report_gss_stats,
                DOC(fiction_clustercomplete_params_report_gss_stats));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::clustercomplete_impl<py_sidb_100_lattice>(m);
    detail::clustercomplete_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#else  // FICTION_ALGLIB_ENABLED

namespace pyfiction
{

/**
 * Disable ClusterComplete.
 */
void clustercomplete([[maybe_unused]] nanobind::module_& m) {}

}  // namespace pyfiction

#endif  // FICTION_ALGLIB_ENABLED

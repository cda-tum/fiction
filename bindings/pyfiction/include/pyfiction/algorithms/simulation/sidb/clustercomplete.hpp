//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_CLUSTERCOMPLETE_HPP
#define PYFICTION_CLUSTERCOMPLETE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/clustercomplete.hpp>
#include <fiction/layouts/coordinates.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void clustercomplete(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("clustercomplete", &fiction::clustercomplete<Lyt>, "lyt"_a,
          "params"_a = fiction::clustercomplete_params<fiction::offset::ucoord_t>{}, DOC(fiction_clustercomplete));
}

}  // namespace detail

inline void clustercomplete(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    namespace py = pybind11;
    using namespace pybind11::literals;

    /**
     * Report *Ground State Space* stats.
     */
    // TODO: add docu
    py::enum_<fiction::ground_state_space_reporting>(m, "ground_state_space_reporting")
        .value("ENABLED", fiction::ground_state_space_reporting::ENABLED)
        .value("DISABLED", fiction::ground_state_space_reporting::DISABLED);

    /**
     * ClusterComplete parameters.
     */
    py::class_<fiction::clustercomplete_params<fiction::offset::ucoord_t>>(m, "clustercomplete_params",
                                                                           DOC(fiction_clustercomplete_params))
        .def(py::init<>())
        .def_readwrite("simulation_parameters",
                       &fiction::clustercomplete_params<fiction::offset::ucoord_t>::simulation_parameters,
                       DOC(fiction_clustercomplete_params_simulation_parameters))
        .def_readwrite("local_external_potential",
                       &fiction::clustercomplete_params<fiction::offset::ucoord_t>::local_external_potential,
                       DOC(fiction_clustercomplete_params_local_external_potential))
        .def_readwrite("global_potential",
                       &fiction::clustercomplete_params<fiction::offset::ucoord_t>::global_potential,
                       DOC(fiction_clustercomplete_params_global_potential))
        .def_readwrite("validity_witness_partitioning_max_cluster_size_gss",
                       &fiction::clustercomplete_params<
                           fiction::offset::ucoord_t>::validity_witness_partitioning_max_cluster_size_gss,
                       DOC(fiction_clustercomplete_params_validity_witness_partitioning_max_cluster_size_gss))
        .def_readwrite("num_overlapping_witnesses_limit_gss",
                       &fiction::clustercomplete_params<fiction::offset::ucoord_t>::num_overlapping_witnesses_limit_gss,
                       DOC(fiction_clustercomplete_params_num_overlapping_witnesses_limit_gss))
        .def_readwrite("available_threads",
                       &fiction::clustercomplete_params<fiction::offset::ucoord_t>::available_threads,
                       DOC(fiction_clustercomplete_params_available_threads))
        .def_readwrite("report_gss_stats",
                       &fiction::clustercomplete_params<fiction::offset::ucoord_t>::report_gss_stats,
                       DOC(fiction_clustercomplete_params_report_gss_stats));

    detail::clustercomplete<py_sidb_100_lattice>(m);
    detail::clustercomplete<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif // PYFICTION_CLUSTERCOMPLETE_HPP

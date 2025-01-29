//
// Created by Jan Drewniok on 01.08.24.
//

#ifndef PYFICTION_DISPLACEMENT_ROBUSTNESS_DOMAIN_HPP
#define PYFICTION_DISPLACEMENT_ROBUSTNESS_DOMAIN_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/displacement_robustness_domain.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void determine_displacement_robustness_domain(pybind11::module& m, const std::string& lattice = "")
{
    namespace py = pybind11;

    py::class_<fiction::displacement_robustness_domain<Lyt>>(
        m, fmt::format("displacement_robustness_domain_{}", lattice).c_str(),
        DOC(fiction_displacement_robustness_domain))
        .def(py::init<>())
        .def_readwrite("influence_information", &fiction::displacement_robustness_domain<Lyt>::operational_values,
                       DOC(fiction_displacement_robustness_domain_operational_values))

        ;

    m.def(fmt::format("determine_displacement_robustness_domain_{}", lattice).c_str(),
          &fiction::determine_displacement_robustness_domain<Lyt, py_tt>, py::arg("layout"), py::arg("spec"),
          py::arg("params"), py::arg("stats") = nullptr, DOC(fiction_determine_displacement_robustness_domain));
}

}  // namespace detail

inline void determine_displacement_robustness_domain(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::dimer_displacement_policy>(
        m, "dimer_displacement_policy", DOC(fiction_displacement_robustness_domain_params_dimer_displacement_policy))
        .value("STAY_ON_ORIGINAL_DIMER",
               fiction::displacement_robustness_domain_params<
                   fiction::offset::ucoord_t>::dimer_displacement_policy::STAY_ON_ORIGINAL_DIMER,
               DOC(fiction_displacement_robustness_domain_params_dimer_displacement_policy_STAY_ON_ORIGINAL_DIMER))
        .value("ALLOW_OTHER_DIMER",
               fiction::displacement_robustness_domain_params<
                   fiction::offset::ucoord_t>::dimer_displacement_policy::ALLOW_OTHER_DIMER,
               DOC(fiction_displacement_robustness_domain_params_dimer_displacement_policy_ALLOW_OTHER_DIMER))

        ;

    py::enum_<fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::displacement_analysis_mode>(
        m, "displacement_analysis_mode", DOC(fiction_displacement_robustness_domain_params_analysis_mode))
        .value("EXHAUSTIVE",
               fiction::displacement_robustness_domain_params<
                   fiction::offset::ucoord_t>::displacement_analysis_mode::EXHAUSTIVE,
               DOC(fiction_displacement_robustness_domain_params_displacement_analysis_mode_EXHAUSTIVE))
        .value("RANDOM",
               fiction::displacement_robustness_domain_params<
                   fiction::offset::ucoord_t>::displacement_analysis_mode::RANDOM,
               DOC(fiction_displacement_robustness_domain_params_displacement_analysis_mode_RANDOM))

        ;

    py::class_<fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>>(
        m, "displacement_robustness_domain_params", DOC(fiction_displacement_robustness_domain_params))
        .def(py::init<>())
        .def_readwrite("analysis_mode",
                       &fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::analysis_mode,
                       DOC(fiction_displacement_robustness_domain_params_analysis_mode))
        .def_readwrite("percentage_of_analyzed_displaced_layouts",
                       &fiction::displacement_robustness_domain_params<
                           fiction::offset::ucoord_t>::percentage_of_analyzed_displaced_layouts,
                       DOC(fiction_displacement_robustness_domain_params_percentage_of_analyzed_displaced_layouts))
        .def_readwrite(
            "displacement_variations",
            &fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::displacement_variations,
            DOC(fiction_displacement_robustness_domain_params_displacement_variations))
        .def_readwrite("operational_params",
                       &fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::operational_params,
                       DOC(fiction_displacement_robustness_domain_params_operational_params))
        .def_readwrite("fixed_sidbs",
                       &fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::fixed_sidbs,
                       DOC(fiction_displacement_robustness_domain_params_fixed_sidbs))
        .def_readwrite("dimer_policy",
                       &fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::dimer_policy,
                       DOC(fiction_displacement_robustness_domain_params_dimer_policy))

        ;

    py::class_<fiction::displacement_robustness_domain_stats>(m, "displacement_robustness_domain_stats",
                                                              DOC(fiction_displacement_robustness_domain_stats))
        .def(py::init<>())
        .def_readwrite("time_total", &fiction::displacement_robustness_domain_stats::time_total,
                       DOC(fiction_displacement_robustness_domain_stats_duration))
        .def_readwrite("num_operational_sidb_displacements",
                       &fiction::displacement_robustness_domain_stats::num_operational_sidb_displacements,
                       DOC(fiction_displacement_robustness_domain_stats_num_operational_sidb_displacements))
        .def_readwrite("num_non_operational_sidb_displacements",
                       &fiction::displacement_robustness_domain_stats::num_non_operational_sidb_displacements,
                       DOC(fiction_displacement_robustness_domain_stats_num_non_operational_sidb_displacements))

        ;

    // NOTE: be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::determine_displacement_robustness_domain<py_sidb_100_lattice>(m, "100");
    detail::determine_displacement_robustness_domain<py_sidb_111_lattice>(m, "111");
}

}  // namespace pyfiction

#endif  // PYFICTION_DISPLACEMENT_ROBUSTNESS_DOMAIN_HPP

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

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void determine_displacement_robustness_domain(pybind11::module& m, const std::string& lattice = "")
{

    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::displacement_robustness_domain<Lyt>>(
        m, fmt::format("displacement_robustness_domain{}", lattice).c_str())
        .def(py::init<>())
        .def_readwrite("operational_values", &fiction::displacement_robustness_domain<Lyt>::operational_values);

    m.def(fmt::format("determine_displacement_robustness_domain{}", lattice).c_str(),
          &fiction::determine_displacement_robustness_domain<Lyt, py_tt>, "layout"_a, "spec"_a, "params"_a,
          "stats"_a = nullptr);
}

}  // namespace detail

inline void determine_displacement_robustness_domain(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::enum_<fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::displacement_analysis_mode>(
        m, "displacement_analysis_mode")
        .value("EXHAUSTIVE", fiction::displacement_robustness_domain_params<
                                 fiction::offset::ucoord_t>::displacement_analysis_mode::EXHAUSTIVE)
        .value("RANDOM", fiction::displacement_robustness_domain_params<
                             fiction::offset::ucoord_t>::displacement_analysis_mode::RANDOM);

    py::class_<fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>>(
        m, "displacement_robustness_domain_params")
        .def(py::init<>())
        .def_readwrite("analysis_mode",
                       &fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::analysis_mode)
        .def_readwrite("percentage_of_analyzed_displaced_layouts",
                       &fiction::displacement_robustness_domain_params<
                           fiction::offset::ucoord_t>::percentage_of_analyzed_displaced_layouts)
        .def_readwrite(
            "displacement_variations",
            &fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::displacement_variations)
        .def_readwrite("operational_params",
                       &fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::operational_params)
        .def_readwrite("fixed_sidbs",
                       &fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::fixed_sidbs)
        .def_readwrite("dimer_policy",
                       &fiction::displacement_robustness_domain_params<fiction::offset::ucoord_t>::dimer_policy);

    py::class_<fiction::displacement_robustness_domain_stats>(m, "displacement_robustness_domain_stats")
        .def(py::init<>())
        .def_readwrite("time_total", &fiction::displacement_robustness_domain_stats::time_total)
        .def_readwrite("num_operational_sidb_displacements",
                       &fiction::displacement_robustness_domain_stats::num_operational_sidb_displacements)
        .def_readwrite("num_non_operational_sidb_displacements",
                       &fiction::displacement_robustness_domain_stats::num_non_operational_sidb_displacements);

    // NOTE: be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::determine_displacement_robustness_domain<py_sidb_100_lattice>(m, "_100");
    detail::determine_displacement_robustness_domain<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction

#endif  // PYFICTION_DISPLACEMENT_ROBUSTNESS_DOMAIN_HPP

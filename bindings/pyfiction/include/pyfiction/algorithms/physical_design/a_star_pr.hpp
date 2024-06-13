//
// Created by simon on 13.06.24.
//

#ifndef PYFICTION_A_STAR_PR_HPP
#define PYFICTION_A_STAR_PR_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/a_star_pr.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <sstream>

namespace pyfiction
{

/**
 * A*-based physical design.
 */
inline void a_star_pr(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::a_star_pr_params>(m, "a_star_pr_params", DOC(fiction_a_star_pr_params)).def(py::init<>())

        ;

    py::class_<fiction::a_star_pr_stats>(m, "a_star_pr_stats", DOC(fiction_a_star_pr_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::a_star_pr_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })

        ;

    m.def("a_star_pr", &fiction::a_star_pr<py_cartesian_gate_layout, py_logic_network>, "network"_a,
          "parameters"_a = fiction::a_star_pr_params{}, "statistics"_a = nullptr, DOC(fiction_a_star_pr));
}

}  // namespace pyfiction

#endif  // PYFICTION_A_STAR_PR_HPP

//
// Created by marcel on 14.06.23.
//

#ifndef PYFICTION_TIME_TO_SOLUTION_HPP
#define PYFICTION_TIME_TO_SOLUTION_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/time_to_solution.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void time_to_solution(pybind11::module& m)
{
    namespace py = pybind11;

    m.def("time_to_solution", &fiction::time_to_solution<Lyt>, py::arg("lyt"), py::arg("quickim_params"),
          py::arg("tts_params") = fiction::time_to_solution_params{}, py::arg("ps") = nullptr,
          DOC(fiction_time_to_solution));
    m.def("time_to_solution_for_given_simulation_results", &fiction::time_to_solution_for_given_simulation_results<Lyt>,
          py::arg("results_exact"), py::arg("results_heuristic"), py::arg("confidence_level") = 0.997,
          py::arg("ps") = nullptr, DOC(fiction_time_to_solution_for_given_simulation_results));
}

}  // namespace detail

inline void time_to_solution(pybind11::module& m)
{
    namespace py = pybind11;

    /**
     * Parameters.
     */
    py::class_<fiction::time_to_solution_params>(m, "time_to_solution_params", DOC(fiction_time_to_solution_params))
        .def(py::init<>())
        .def_readwrite("engine", &fiction::time_to_solution_params::engine, DOC(fiction_time_to_solution_params_engine))
        .def_readwrite("repetitions", &fiction::time_to_solution_params::repetitions,
                       DOC(fiction_time_to_solution_params_repetitions))
        .def_readwrite("confidence_level", &fiction::time_to_solution_params::confidence_level,
                       DOC(fiction_time_to_solution_params_confidence_level));
    /**
     * Statistics.
     */
    py::class_<fiction::time_to_solution_stats>(m, "time_to_solution_stats", DOC(fiction_time_to_solution_stats))
        .def(py::init<>())
        .def("__repr__",
             [](const fiction::time_to_solution_stats& stats)
             {
                 std::stringstream stream{};
                 stats.report(stream);
                 return stream.str();
             })
        .def("report", &fiction::time_to_solution_stats::report, DOC(fiction_time_to_solution_stats_report))
        .def_readonly("time_to_solution", &fiction::time_to_solution_stats::time_to_solution,
                      DOC(fiction_time_to_solution_stats_time_to_solution))
        .def_readonly("acc", &fiction::time_to_solution_stats::acc, DOC(fiction_time_to_solution_stats_acc))
        .def_readonly("mean_single_runtime", &fiction::time_to_solution_stats::mean_single_runtime,
                      DOC(fiction_time_to_solution_stats_mean_single_runtime))
        .def_readonly("single_runtime_exact", &fiction::time_to_solution_stats::single_runtime_exact,
                      DOC(fiction_time_to_solution_stats_single_runtime_exact))
        .def_readonly("algorithm", &fiction::time_to_solution_stats::algorithm,
                      DOC(fiction_time_to_solution_stats_algorithm))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::time_to_solution<py_sidb_100_lattice<py_offset_coordinate>>(m);
    detail::time_to_solution<py_sidb_111_lattice<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_TIME_TO_SOLUTION_HPP

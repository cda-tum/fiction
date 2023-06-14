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
    using namespace pybind11::literals;

    m.def("sim_acc_tts", &fiction::sim_acc_tts<Lyt>, "lyt"_a, "quickim_params"_a, "ps"_a = nullptr,
          "repetitions"_a = 100, "confidence_level"_a = 0.997, DOC(fiction_sim_acc_tts));
}

}  // namespace detail

inline void time_to_solution(pybind11::module& m)
{
    namespace py = pybind11;

    /**
     * Time_To_Solution parameters.
     */
    py::class_<fiction::time_to_solution_stats>(m, "time_to_solution_stats", DOC(fiction_time_to_solution_stats))
        .def(py::init<>())
        .def_readwrite("time_to_solution", &fiction::time_to_solution_stats::time_to_solution,
                       DOC(fiction_time_to_solution_stats_time_to_solution))
        .def_readwrite("acc", &fiction::time_to_solution_stats::acc, DOC(fiction_time_to_solution_stats_acc))
        .def_readwrite("mean_single_runtime", &fiction::time_to_solution_stats::mean_single_runtime,
                       DOC(fiction_time_to_solution_stats_mean_single_runtime))
        .def_readwrite("single_runtime_exhaustive", &fiction::time_to_solution_stats::single_runtime_exhaustive,
                       DOC(fiction_time_to_solution_stats_single_runtime_exhaustive))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::time_to_solution<py_charge_distribution_surface>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_TIME_TO_SOLUTION_HPP

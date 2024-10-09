//
// Created by marcel on 18.09.23.
//

#ifndef PYFICTION_SIDB_SIMULATION_ENGINE_HPP
#define PYFICTION_SIDB_SIMULATION_ENGINE_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>

#include <pybind11/pybind11.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename EngineType>
void sidb_simulation_engine_name(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def(
        "sidb_simulation_engine_name", [](const EngineType& engine) -> std::string
        { return fiction::sidb_simulation_engine_name(engine); }, "engine"_a, DOC(fiction_sidb_simulation_engine_name));
}

}  // namespace detail

inline void sidb_simulation_engine(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::sidb_simulation_engine>(m, "sidb_simulation_engine", DOC(fiction_sidb_simulation_engine))
        .value("EXGS", fiction::sidb_simulation_engine::EXGS, DOC(fiction_sidb_simulation_engine_EXGS))
        .value("QUICKSIM", fiction::sidb_simulation_engine::QUICKSIM, DOC(fiction_sidb_simulation_engine_QUICKSIM))
        .value("QUICKEXACT", fiction::sidb_simulation_engine::QUICKEXACT,
               DOC(fiction_sidb_simulation_engine_QUICKEXACT))
        .value("CLUSTERCOMPLETE", fiction::sidb_simulation_engine::CLUSTERCOMPLETE,
               DOC(fiction_sidb_simulation_engine_CLUSTERCOMPLETE))

        ;

    py::enum_<fiction::exact_sidb_simulation_engine>(m, "exact_sidb_simulation_engine",
                                                     DOC(fiction_exact_sidb_simulation_engine))
        .value("EXGS", fiction::exact_sidb_simulation_engine::EXGS, DOC(fiction_exact_sidb_simulation_engine_EXGS))
        .value("QUICKEXACT", fiction::exact_sidb_simulation_engine::QUICKEXACT,
               DOC(fiction_exact_sidb_simulation_engine_QUICKEXACT))
    py::enum_<fiction::exact_sidb_simulation_engine>(m, "exhaustive_sidb_simulation_engine",
                                                          DOC(fiction_exhaustive_sidb_simulation_engine))
        .value("EXGS", fiction::exact_sidb_simulation_engine::EXGS,
               DOC(fiction_exhaustive_sidb_simulation_engine_EXGS))
        .value("QUICKEXACT", fiction::exact_sidb_simulation_engine::QUICKEXACT,
               DOC(fiction_exhaustive_sidb_simulation_engine_QUICKEXACT))
        .value("CLUSTERCOMPLETE", fiction::exact_sidb_simulation_engine::CLUSTERCOMPLETE,
               DOC(fiction_exhaustive_sidb_simulation_engine_CLUSTERCOMPLETE))

        ;

    py::enum_<fiction::heuristic_sidb_simulation_engine>(m, "heuristic_sidb_simulation_engine",
                                                         DOC(fiction_heuristic_sidb_simulation_engine))
        .value("QUICKSIM", fiction::heuristic_sidb_simulation_engine::QUICKSIM,
               DOC(fiction_heuristic_sidb_simulation_engine_QUICKSIM))

        ;

    detail::sidb_simulation_engine_name<fiction::sidb_simulation_engine>(m);
    detail::sidb_simulation_engine_name<fiction::exact_sidb_simulation_engine>(m);
    detail::sidb_simulation_engine_name<fiction::heuristic_sidb_simulation_engine>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_SIMULATION_ENGINE_HPP

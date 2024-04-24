//
// Created by marcel on 18.09.23.
//

#ifndef PYFICTION_SIDB_SIMULATION_ENGINE_HPP
#define PYFICTION_SIDB_SIMULATION_ENGINE_HPP

#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_engine.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

inline void sidb_simulation_engine(pybind11::module& m)
{
    namespace py = pybind11;

    py::enum_<fiction::sidb_simulation_engine>(m, "sidb_simulation_engine", DOC(fiction_sidb_simulation_engine))
        .value("EXGS", fiction::sidb_simulation_engine::EXGS, DOC(fiction_sidb_simulation_engine_EXGS))
        .value("QUICKSIM", fiction::sidb_simulation_engine::QUICKSIM, DOC(fiction_sidb_simulation_engine_QUICKSIM))
        .value("QUICKEXACT", fiction::sidb_simulation_engine::QUICKEXACT,
               DOC(fiction_sidb_simulation_engine_QUICKEXACT))

        ;

    py::enum_<fiction::exhaustive_sidb_simulation_engine>(m, "exhaustive_sidb_simulation_engine",
                                                          DOC(fiction_exhaustive_sidb_simulation_engine))
        .value("EXGS", fiction::exhaustive_sidb_simulation_engine::EXGS,
               DOC(fiction_exhaustive_sidb_simulation_engine_EXGS))
        .value("QUICKEXACT", fiction::exhaustive_sidb_simulation_engine::QUICKEXACT,
               DOC(fiction_exhaustive_sidb_simulation_engine_QUICKEXACT))

        ;
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_SIMULATION_ENGINE_HPP

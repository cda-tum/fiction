//
// Created by marcel on 22.05.23.
//

#ifndef PYFICTION_EXHAUSTIVE_GROUND_STATE_SIMULATION_HPP
#define PYFICTION_EXHAUSTIVE_GROUND_STATE_SIMULATION_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void exhaustive_ground_state_simulation(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def(
        "exhaustive_ground_state_simulation",
        [](const Lyt&                                 lyt,
           const fiction::sidb_simulation_parameters& params) -> fiction::sidb_simulation_result<py_cds_base>
        { return fiction::exhaustive_ground_state_simulation<py_cds_base>(lyt, params); },
        "lyt"_a, "params"_a = fiction::sidb_simulation_parameters{}, DOC(fiction_exhaustive_ground_state_simulation));
}

}  // namespace detail

inline void exhaustive_ground_state_simulation(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::exhaustive_ground_state_simulation<py_charge_distribution_surface>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_EXHAUSTIVE_GROUND_STATE_SIMULATION_HPP

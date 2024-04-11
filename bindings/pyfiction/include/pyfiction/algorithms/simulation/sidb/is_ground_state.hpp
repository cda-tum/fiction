//
// Created by marcel on 22.05.23.
//

#ifndef PYFICTION_IS_GROUND_STATE_HPP
#define PYFICTION_IS_GROUND_STATE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/is_ground_state.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void is_ground_state(pybind11::module& m, const std::string& lattice = "")
{
    using namespace pybind11::literals;

    m.def(fmt::format("is_ground_state{}", lattice).c_str(), &fiction::is_ground_state<Lyt>, "heuristic_results"_a,
          "exhaustive_results"_a, DOC(fiction_is_ground_state));
}

}  // namespace detail

inline void is_ground_state(pybind11::module& m)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::is_ground_state<py_sidb_100_lattice>(m, "_100");
    detail::is_ground_state<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction

#endif  // PYFICTION_IS_GROUND_STATE_HPP

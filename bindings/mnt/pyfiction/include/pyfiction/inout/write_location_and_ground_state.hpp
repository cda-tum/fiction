//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_WRITE_LOCATION_AND_GROUND_STATE_HPP
#define PYFICTION_WRITE_LOCATION_AND_GROUND_STATE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_location_and_ground_state.hpp>
#include <fiction/utils/layout_utils.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_location_and_ground_state(pybind11::module& m)
{
    namespace py = pybind11;

    m.def(
        "write_location_and_ground_state",
        [](const fiction::sidb_simulation_result<Lyt>& sim_result, const std::string_view& filename)
        { fiction::write_location_and_ground_state(sim_result, filename); }, py::arg("sim_result"), py::arg("filename"),
        DOC(fiction_write_location_and_ground_state));
}

}  // namespace detail

inline void write_location_and_ground_state(pybind11::module& m)
{
    detail::write_location_and_ground_state<py_sidb_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_LOCATION_AND_GROUND_STATE_HPP

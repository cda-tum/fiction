//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_WRITE_LOCATION_AND_GROUND_STATE_HPP
#define PYFICTION_WRITE_LOCATION_AND_GROUND_STATE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_location_and_ground_state.hpp>

#include <string_view>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_location_and_ground_state(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def(
        "write_location_and_ground_state",
        [](const fiction::sidb_simulation_result<Lyt>& lyt, const std::string_view& filename)
        { fiction::write_location_and_ground_state(lyt, filename); },
        "layout"_a, "filename"_a, DOC(fiction_write_location_and_ground_state));
}

}  // namespace detail

inline void write_location_and_ground_state(pybind11::module& m)
{
    detail::write_location_and_ground_state<py_cds_base>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_LOCATION_AND_GROUND_STATE_HPP

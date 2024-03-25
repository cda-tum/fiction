//
// Created by marcel on 21.11.23.
//

#ifndef PYFICTION_SIDB_NM_POSITION_HPP
#define PYFICTION_SIDB_NM_POSITION_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb_nm_position.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void sidb_nm_position(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def("sidb_nm_position", &fiction::sidb_nm_position<Lyt>, "sp"_a, "c"_a, DOC(fiction_sidb_nm_position));
}

}  // namespace detail

inline void sidb_nm_position(pybind11::module& m)
{
    detail::sidb_nm_position<py_charge_distribution_surface>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_NM_POSITION_HPP

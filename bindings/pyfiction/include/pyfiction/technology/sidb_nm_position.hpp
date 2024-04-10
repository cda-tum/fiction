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

    m.def("sidb_nm_position", &fiction::sidb_nm_position<Lyt>, "lyt"_a, "c"_a, DOC(fiction_sidb_nm_position));
}

}  // namespace detail

inline void sidb_nm_position(pybind11::module& m)
{
    detail::sidb_nm_position<py_charge_distribution_surface>(m);
    detail::sidb_nm_position<py_sidb_layout>(m);
    detail::sidb_nm_position<py_sidb_lattice<fiction::sidb_100_lattice>>(m);
    detail::sidb_nm_position<py_sidb_lattice<fiction::sidb_111_lattice>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_SIDB_NM_POSITION_HPP

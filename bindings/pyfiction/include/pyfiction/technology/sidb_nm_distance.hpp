//
// Created by Jan Drewniok on 11.04.24.
//

#ifndef FICTION_SIDB_NANOMETER_DISTANCE_HPP
#define FICTION_SIDB_NANOMETER_DISTANCE_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb_nm_distance.hpp>

#include <pybind11/pybind11.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void nanometer_distance(pybind11::module& m, const std::string& lattice = "")
{
    using namespace pybind11::literals;

    m.def(fmt::format("sidb_nm_distance{}", lattice).c_str(), &fiction::sidb_nm_distance<Lyt>, "lyt"_a, "source"_a,
          "target"_a, DOC(fiction_sidb_nm_distance));
}

}  // namespace detail

inline void sidb_nm_distance(pybind11::module& m)
{
    detail::nanometer_distance<py_sidb_100_lattice>(m, "_100");
    detail::nanometer_distance<py_sidb_111_lattice>(m, "_111");
}

}  // namespace pyfiction

#endif  // FICTION_SIDB_NANOMETER_DISTANCE_HPP

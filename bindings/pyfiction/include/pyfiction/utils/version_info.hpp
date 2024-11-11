//
// Created by Jan Drewniok on 11.11.24.
//

#ifndef PYFICTION_VERSION_INFO_HPP
#define PYFICTION_VERSION_INFO_HPP

#include <fiction/utils/version_info.hpp.in>

#include <pybind11/pybind11.h>

namespace pyfiction
{

using namespace pybind11::literals;

inline void version_info(pybind11::module& m)
{
    m.attr("FICTION_VERSION") = fiction::FICTION_VERSION;
    m.attr("FICTION_REPO")    = fiction::FICTION_REPO;
    m.attr("COMPILED_DATE")   = fiction::COMPILED_DATE;
    m.attr("COMPILED_TIME")   = fiction::COMPILED_TIME;

    m.doc() = "Python bindings for the fiction project version information.";
}

}  // namespace pyfiction

#endif  // PYFICTION_VERSION_INFO_HPP

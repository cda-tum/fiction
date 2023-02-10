//
// Created by marcel on 16.12.22.
//

#ifndef PYFICTION_READ_FQCA_LAYOUT_HPP
#define PYFICTION_READ_FQCA_LAYOUT_HPP

#include "pyfiction/types.hpp"

#include <fiction/io/read_fqca_layout.hpp>

#include <pybind11/pybind11.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void read_fqca_layout(pybind11::module& m)
{
    using namespace pybind11::literals;

    Lyt (*read_fqca_layout_function_pointer)(const std::string&, const std::string&) = &fiction::read_fqca_layout<Lyt>;

    m.def("read_fqca_layout", read_fqca_layout_function_pointer, "filename"_a, "layout_name"_a = "");
}

}  // namespace detail

inline void read_fqca_layout(pybind11::module& m)
{
    detail::read_fqca_layout<py_qca_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_READ_FQCA_LAYOUT_HPP

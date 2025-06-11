//
// Created by marcel on 16.12.22.
//

#ifndef PYFICTION_WRITE_QLL_LAYOUT_HPP
#define PYFICTION_WRITE_QLL_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_qll_layout.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_qll_layout(pybind11::module& m)
{
    namespace py = pybind11;

    void (*write_qll_layout_function_pointer)(const Lyt&, const std::string_view&) = &fiction::write_qll_layout<Lyt>;

    m.def("write_qll_layout", write_qll_layout_function_pointer, py::arg("layout"), py::arg("filename"),
          DOC(fiction_write_qll_layout));
}

}  // namespace detail

inline void write_qll_layout(pybind11::module& m)
{
    detail::write_qll_layout<py_qca_layout<py_offset_coordinate>>(m);
    detail::write_qll_layout<py_inml_layout<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_QLL_LAYOUT_HPP

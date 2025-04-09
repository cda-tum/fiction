//
// Created by marcel on 16.12.22.
//

#ifndef PYFICTION_READ_FQCA_LAYOUT_HPP
#define PYFICTION_READ_FQCA_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/read_fqca_layout.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void read_fqca_layout(pybind11::module& m)
{
    namespace py = pybind11;

    Lyt (*read_fqca_layout_function_pointer)(const std::string_view&, const std::string_view&) =
        &fiction::read_fqca_layout<Lyt>;

    m.def("read_fqca_layout", read_fqca_layout_function_pointer, py::arg("filename"), py::arg("layout_name") = "",
          DOC(fiction_read_fqca_layout));
}

}  // namespace detail

inline void read_fqca_layout(pybind11::module& m)
{
    namespace py = pybind11;

    py::register_exception<fiction::unsupported_character_exception>(m, "unsupported_character_exception");
    py::register_exception<fiction::undefined_cell_label_exception>(m, "undefined_cell_label_exception");
    py::register_exception<fiction::unrecognized_cell_definition_exception>(m,
                                                                            "unrecognized_cell_definition_exception");

    detail::read_fqca_layout<py_qca_layout<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_READ_FQCA_LAYOUT_HPP

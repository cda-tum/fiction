//
// Created by marcel on 08.07.22.
//

#ifndef PYFICTION_WRITE_SQD_LAYOUT_HPP
#define PYFICTION_WRITE_SQD_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_sqd_layout.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{
template <typename Lyt>
inline void write_sqd_layout(pybind11::module& m)
{
    namespace py = pybind11;

    void (*write_sqd_layout_function_pointer)(const Lyt&, const std::string_view&) = &fiction::write_sqd_layout<Lyt>;

    m.def("write_sqd_layout", write_sqd_layout_function_pointer, py::arg("layout"), py::arg("filename"),
          DOC(fiction_write_sqd_layout));
}
}  // namespace detail

inline void write_sqd_layout(pybind11::module& m)
{
    detail::write_sqd_layout<py_sidb_111_lattice<py_offset_coordinate>>(m);
    detail::write_sqd_layout<py_sidb_100_lattice<py_offset_coordinate>>(m);
    detail::write_sqd_layout<py_sidb_layout<py_offset_coordinate>>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_SQD_LAYOUT_HPP

//
// Created by marcel on 25.04.23.
//

#ifndef PYFICTION_READ_SQD_LAYOUT_HPP
#define PYFICTION_READ_SQD_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/read_sqd_layout.hpp>

#include <pybind11/pybind11.h>

#include <string>
#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void read_sqd_layout(pybind11::module& m, std::string lattice_orientation, std::string coordinate_type)
{
    namespace py = pybind11;
    using namespace py::literals;

    // Common function pointer for all cases
    Lyt (*read_sqd_layout_function_pointer)(const std::string_view&, const std::string_view&) =
        &fiction::read_sqd_layout<Lyt>;

    m.def(fmt::format("read_sqd_layout_{}{}", lattice_orientation, coordinate_type).c_str(),
          read_sqd_layout_function_pointer, "filename"_a, "name"_a = "", DOC(fiction_read_sqd_layout_3));
}

}  // namespace detail

inline void read_sqd_layout(pybind11::module& m)
{
    namespace py = pybind11;
    py::register_exception<fiction::sqd_parsing_error>(m, "sqd_parsing_error", PyExc_RuntimeError);

    detail::read_sqd_layout<py_sidb_100_lattice>(m, "100", "");
    detail::read_sqd_layout<py_sidb_111_lattice>(m, "111", "");
    detail::read_sqd_layout<py_sidb_100_lattice_cube>(m, "100", "_cube");
    detail::read_sqd_layout<py_sidb_111_lattice_cube>(m, "111", "_cube");
}

}  // namespace pyfiction

#endif  // PYFICTION_READ_SQD_LAYOUT_HPP

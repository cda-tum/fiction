//
// Created by marcel on 25.04.23.
//

#ifndef PYFICTION_READ_SQD_LAYOUT_HPP
#define PYFICTION_READ_SQD_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/read_sqd_layout.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void read_sqd_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace py::literals;

    py::register_exception<fiction::sqd_parsing_error>(m, "sqd_parsing_error", PyExc_RuntimeError);

    if constexpr (fiction::is_sidb_lattice_100_v<Lyt>)
    {
        Lyt (*read_sqd_layout_function_pointer)(const std::string_view&, const std::string_view&) =
            &fiction::read_sqd_layout<Lyt>;

        m.def("read_sqd_layout_100", read_sqd_layout_function_pointer, "filename"_a, "name"_a = "",
              DOC(fiction_read_sqd_layout_3));
    }
    else
    {
        Lyt (*read_sqd_layout_function_pointer)(const std::string_view&, const std::string_view&) =
            &fiction::read_sqd_layout<Lyt>;

        m.def("read_sqd_layout_111", read_sqd_layout_function_pointer, "filename"_a, "name"_a = "",
              DOC(fiction_read_sqd_layout_3));
    }
}

}  // namespace detail

inline void read_sqd_layout_100(pybind11::module& m)
{
    detail::read_sqd_layout<py_sidb_100_lattice>(m);
}

inline void read_sqd_layout_111(pybind11::module& m)
{
    detail::read_sqd_layout<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_READ_SQD_LAYOUT_HPP

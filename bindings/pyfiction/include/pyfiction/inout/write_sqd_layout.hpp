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

inline void write_sqd_layout(pybind11::module& m)
{
    using namespace pybind11::literals;

    void (*write_sqd_layout_function_pointer)(const py_sidb_layout&, const std::string_view&) =
        &fiction::write_sqd_layout<py_sidb_layout>;

    m.def("write_sqd_layout", write_sqd_layout_function_pointer, "layout"_a, "filename"_a,
          DOC(fiction_write_sqd_layout));
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_SQD_LAYOUT_HPP

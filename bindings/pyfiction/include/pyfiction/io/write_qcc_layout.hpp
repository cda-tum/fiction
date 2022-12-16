//
// Created by marcel on 16.12.22.
//

#ifndef PYFICTION_WRITE_QCC_LAYOUT_HPP
#define PYFICTION_WRITE_QCC_LAYOUT_HPP

#include "pybind11/pybind11.h"
#include "pyfiction/types.hpp"

#include <fiction/io/write_qcc_layout.hpp>

#include <string>

namespace pyfiction
{

inline void write_qcc_layout(pybind11::module& m)
{
    using namespace pybind11::literals;

    m.def(
        "write_qca_layout",
        [](const py_inml_layout& lyt, const std::string& filename) { fiction::write_qcc_layout(lyt, filename); },
        "layout"_a, "filename"_a);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_QCC_LAYOUT_HPP

//
// Created by marcel on 08.07.22.
//

#ifndef PYFICTION_WRITE_QCA_LAYOUT_HPP
#define PYFICTION_WRITE_QCA_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_qca_layout.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

inline void write_qca_layout(pybind11::module& m)
{
    namespace py = pybind11;
    using namespace pybind11::literals;

    py::class_<fiction::write_qca_layout_params>(m, "write_qca_layout_params", DOC(fiction_write_qca_layout_params))
        .def(py::init<>())
        .def_readwrite("create_inter_layer_via_cells", &fiction::write_qca_layout_params::create_inter_layer_via_cells,
                       DOC(fiction_write_qca_layout_params_create_inter_layer_via_cells))

        ;

    void (*write_qca_layout_function_pointer)(const py_qca_layout&, const std::string_view&,
                                              fiction::write_qca_layout_params) =
        &fiction::write_qca_layout<py_qca_layout>;

    m.def("write_qca_layout", write_qca_layout_function_pointer, "layout"_a, "filename"_a,
          "params"_a = fiction::write_qca_layout_params{}, DOC(fiction_write_qca_layout));
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_QCA_LAYOUT_HPP

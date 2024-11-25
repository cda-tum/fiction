//
// Created by marcel on 16.12.22.
//

#ifndef PYFICTION_WRITE_FQCA_LAYOUT_HPP
#define PYFICTION_WRITE_FQCA_LAYOUT_HPP

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_fqca_layout.hpp>

#include <pybind11/pybind11.h>

#include <string_view>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_fqca_layout(pybind11::module& m)
{
    namespace py = pybind11;

    void (*write_fqca_layout_function_pointer)(const Lyt&, const std::string_view&,
                                               const fiction::write_fqca_layout_params) =
        &fiction::write_fqca_layout<Lyt>;

    m.def("write_fqca_layout", write_fqca_layout_function_pointer, py::arg("layout"), py::arg("filename"),
          py::arg("params") = fiction::write_fqca_layout_params{}, DOC(fiction_write_fqca_layout));
}

}  // namespace detail

inline void write_fqca_layout(pybind11::module& m)
{
    namespace py = pybind11;

    py::register_exception<fiction::out_of_cell_names_exception>(m, "out_of_cell_names_exception", PyExc_IndexError);

    py::class_<fiction::write_fqca_layout_params>(m, "write_fqca_layout_params", DOC(fiction_write_fqca_layout_params))
        .def(py::init<>())
        .def_readwrite("create_inter_layer_via_cells", &fiction::write_fqca_layout_params::create_inter_layer_via_cells,
                       DOC(fiction_write_fqca_layout_params_create_inter_layer_via_cells))

        ;

    detail::write_fqca_layout<py_qca_layout>(m);
}

}  // namespace pyfiction

#endif  // PYFICTION_WRITE_FQCA_LAYOUT_HPP

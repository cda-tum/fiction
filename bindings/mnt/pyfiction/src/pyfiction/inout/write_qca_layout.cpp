//
// Created by marcel on 08.07.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_qca_layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

void write_qca_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::class_<fiction::write_qca_layout_params>(m, "write_qca_layout_params", DOC(fiction_write_qca_layout_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("create_inter_layer_via_cells", &fiction::write_qca_layout_params::create_inter_layer_via_cells,
                DOC(fiction_write_qca_layout_params_create_inter_layer_via_cells))

        ;

    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_qca_layout_function_pointer)(const py_qca_layout&, const std::string_view&,
                                                    fiction::write_qca_layout_params) =
        &fiction::write_qca_layout<py_qca_layout>;

    m.def("write_qca_layout", write_qca_layout_function_pointer, py::arg("layout"), py::arg("filename"),
          py::arg("params") = fiction::write_qca_layout_params{}, DOC(fiction_write_qca_layout));
}

}  // namespace pyfiction

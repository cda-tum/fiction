//
// Created by marcel on 16.12.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/read_fqca_layout.hpp>

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

namespace detail
{

template <typename Lyt>
void read_fqca_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // NOLINTNEXTLINE(misc-const-correctness)
    Lyt (*const read_fqca_layout_function_pointer)(const std::string_view&, const std::string_view&) =
        &fiction::read_fqca_layout<Lyt>;

    m.def("read_fqca_layout", read_fqca_layout_function_pointer, py::arg("filename"), py::arg("layout_name") = "",
          DOC(fiction_read_fqca_layout));
}

}  // namespace detail

void read_fqca_layout(nanobind::module_& m)
{
    namespace py = nanobind;

    py::exception<fiction::unsupported_character_exception>(m, "unsupported_character_exception");
    py::exception<fiction::undefined_cell_label_exception>(m, "undefined_cell_label_exception");
    py::exception<fiction::unrecognized_cell_definition_exception>(m, "unrecognized_cell_definition_exception");

    detail::read_fqca_layout<py_qca_layout>(m);
}

}  // namespace pyfiction

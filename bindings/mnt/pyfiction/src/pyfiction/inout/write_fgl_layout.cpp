//
// Created by simon on 09.10.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_fgl_layout.hpp>
#include <fiction/utils/name_utils.hpp>  // NOLINT(misc-include-cleaner): Required by write_fgl_layout.hpp.

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
void write_fgl_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_fgl_layout",
        [](const Lyt& lyt, const std::string_view& filename) { fiction::write_fgl_layout<Lyt>(lyt, filename); },
        py::arg("layout"), py::arg("filename"), DOC(fiction_write_fgl_layout_2));
}

}  // namespace detail

void write_fgl_layout(nanobind::module_& m)
{
    detail::write_fgl_layout<py_cartesian_gate_layout>(m);
    detail::write_fgl_layout<py_shifted_cartesian_gate_layout>(m);
    detail::write_fgl_layout<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

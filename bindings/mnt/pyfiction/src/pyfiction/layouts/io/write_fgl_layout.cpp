//
// Created by simon on 09.10.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/io/write_fgl_layout.hpp>
#include <fiction/networks/name_utils.hpp>  // NOLINT(misc-include-cleaner): Required by write_fgl_layout.hpp.

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>   // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_fgl_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_fgl_layout", [](const Lyt& lyt, const std::string_view& filename)
        { fiction::layouts::io::write_fgl_layout<Lyt>(lyt, filename); }, py::arg("layout"), py::arg("filename"),
        DOC(fiction_layouts_io_write_fgl_layout_2));
}

}  // namespace detail

void write_fgl_layout(nanobind::module_& m)
{
    detail::write_fgl_layout<py_cartesian_gate_layout>(m);
    detail::write_fgl_layout<py_shifted_cartesian_gate_layout>(m);
    detail::write_fgl_layout<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

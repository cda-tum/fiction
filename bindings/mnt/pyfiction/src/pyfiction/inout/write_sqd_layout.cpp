//
// Created by marcel on 08.07.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/write_sqd_layout.hpp>

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
void write_sqd_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_sqd_layout_function_pointer)(const Lyt&, const std::string_view&) =
        &fiction::write_sqd_layout<Lyt>;

    m.def("write_sqd_layout", write_sqd_layout_function_pointer, py::arg("layout"), py::arg("filename"),
          DOC(fiction_write_sqd_layout));
}
}  // namespace detail

void write_sqd_layout(nanobind::module_& m)
{
    detail::write_sqd_layout<py_sidb_111_lattice>(m);
    detail::write_sqd_layout<py_sidb_100_lattice>(m);
    detail::write_sqd_layout<py_sidb_layout>(m);
}

}  // namespace pyfiction

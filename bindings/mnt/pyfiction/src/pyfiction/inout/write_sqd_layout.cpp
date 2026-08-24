//
// Created by marcel on 08.07.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/io/write_sqd_layout.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{
template <typename Lyt>
void sidb::io::write_sqd_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    // NOLINTNEXTLINE(misc-const-correctness)
    void (*const write_sqd_layout_function_pointer)(const Lyt&, const std::string_view&) =
        &fiction::sidb::io::write_sqd_layout<Lyt>;

    m.def("write_sqd_layout", write_sqd_layout_function_pointer, py::arg("layout"), py::arg("filename"),
          DOC(fiction_write_sqd_layout));
}
}  // namespace detail

void sidb::io::write_sqd_layout(nanobind::module_& m)
{
    detail::write_sqd_layout<py_sidb_111_lattice>(m);
    detail::write_sqd_layout<py_sidb_100_lattice>(m);
    detail::write_sqd_layout<py_sidb_layout>(m);
}

}  // namespace pyfiction

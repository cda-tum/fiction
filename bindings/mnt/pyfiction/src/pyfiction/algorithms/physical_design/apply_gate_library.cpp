#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/technology/inml_topolinano_library.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

template <typename GateLibrary, typename GateLyt>
void apply_fcn_gate_library(nanobind::module_& m, const std::string& lib_name)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    using py_cartesian_technology_cell_layout = py_cartesian_cell_layout<fiction::technology<GateLibrary>>;

    m.def(fmt::format("apply_{}_library", lib_name).c_str(),
          &fiction::apply_gate_library<py_cartesian_technology_cell_layout, GateLibrary, GateLyt>, py::arg("layout"),
          DOC(fiction_apply_gate_library));
}

}  // namespace detail

void apply_gate_library(nanobind::module_& m)
{
    detail::apply_fcn_gate_library<fiction::qca_one_library, py_cartesian_gate_layout>(m, "qca_one");
    detail::apply_fcn_gate_library<fiction::inml_topolinano_library, py_shifted_cartesian_gate_layout>(m, "topolinano");
    detail::apply_fcn_gate_library<fiction::sidb_bestagon_library, py_hexagonal_gate_layout>(m, "bestagon");
}

}  // namespace pyfiction

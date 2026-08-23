#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/apply_gate_library.hpp>
#include <fiction/technology/inml_topolinano_library.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/technology/sim7_mol_library.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

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
          &fiction::physical_design::apply_gate_library<py_cartesian_technology_cell_layout, GateLibrary, GateLyt>,
          py::arg("layout"), DOC(fiction_apply_gate_library));
}

}  // namespace detail

void physical_design::apply_gate_library(nanobind::module_& m)
{
    detail::apply_fcn_gate_library<fiction::qca_one_library, py_cartesian_gate_layout>(m, "qca_one");
    detail::apply_fcn_gate_library<fiction::sim7_mol_library, py_cartesian_gate_layout>(m, "sim7_mol");
    detail::apply_fcn_gate_library<fiction::inml_topolinano_library, py_shifted_cartesian_gate_layout>(m, "topolinano");
    detail::apply_fcn_gate_library<fiction::sidb_bestagon_library, py_hexagonal_gate_layout>(m, "bestagon");
}

}  // namespace pyfiction

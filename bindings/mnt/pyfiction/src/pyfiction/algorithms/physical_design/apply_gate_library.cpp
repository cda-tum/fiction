#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/physical_design/apply_gate_library.hpp>
#include <fiction/technology/inml_topolinano_library.hpp>
#include <fiction/technology/qca_one_library.hpp>
#include <fiction/technology/sidb_bestagon_library.hpp>
#include <fiction/traits.hpp>

#include <fmt/format.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>

namespace pyfiction
{

namespace detail
{

template <typename GateLibrary, typename GateLyt>
void apply_fcn_gate_library(pybind11::module& m, const std::string& lib_name)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    using py_cartesian_technology_cell_layout = py_cartesian_cell_layout<fiction::technology<GateLibrary>>;

    m.def(fmt::format("apply_{}_library", lib_name).c_str(),
          &fiction::apply_gate_library<py_cartesian_technology_cell_layout, GateLibrary, GateLyt>, py::arg("layout"),
          DOC(fiction_apply_gate_library));
}

}  // namespace detail

void apply_gate_library(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    detail::apply_fcn_gate_library<fiction::qca_one_library, py_cartesian_gate_layout>(m, "qca_one");
    detail::apply_fcn_gate_library<fiction::inml_topolinano_library, py_shifted_cartesian_gate_layout>(m, "topolinano");
    detail::apply_fcn_gate_library<fiction::sidb_bestagon_library, py_hexagonal_gate_layout>(m, "bestagon");
}

}  // namespace pyfiction

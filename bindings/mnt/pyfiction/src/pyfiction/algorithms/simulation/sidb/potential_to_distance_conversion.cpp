#include "pyfiction/documentation.hpp"

#include <fiction/algorithms/simulation/sidb/potential_to_distance_conversion.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

void potential_to_distance_conversion(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    m.def("potential_to_distance_conversion", &fiction::potential_to_distance_conversion, py::arg("potential"),
          py::arg("params"), py::arg("precision"), DOC(fiction_potential_to_distance_conversion));
}

}  // namespace pyfiction

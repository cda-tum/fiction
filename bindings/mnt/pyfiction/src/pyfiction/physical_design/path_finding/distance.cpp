#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/path_finding/distance.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void distance_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("manhattan_distance", &fiction::physical_design::path_finding::manhattan_distance<Lyt>, py::arg("layout"),
          py::arg("source"), py::arg("target"), DOC(fiction_physical_design_path_finding_manhattan_distance));
    m.def("euclidean_distance", &fiction::physical_design::path_finding::euclidean_distance<Lyt>, py::arg("layout"),
          py::arg("source"), py::arg("target"), DOC(fiction_physical_design_path_finding_euclidean_distance));
    m.def("squared_euclidean_distance", &fiction::physical_design::path_finding::squared_euclidean_distance<Lyt>,
          py::arg("layout"), py::arg("source"), py::arg("target"),
          DOC(fiction_physical_design_path_finding_squared_euclidean_distance));
    m.def("twoddwave_distance", &fiction::physical_design::path_finding::twoddwave_distance<Lyt>, py::arg("layout"),
          py::arg("source"), py::arg("target"), DOC(fiction_physical_design_path_finding_twoddwave_distance));
    m.def("chebyshev_distance", &fiction::physical_design::path_finding::chebyshev_distance<Lyt>, py::arg("layout"),
          py::arg("source"), py::arg("target"), DOC(fiction_physical_design_path_finding_chebyshev_distance));
}

}  // namespace detail

void distance(nanobind::module_& m)
{
    detail::distance_impl<py_cartesian_layout>(m);
    detail::distance_impl<py_shifted_cartesian_layout>(m);
    detail::distance_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction

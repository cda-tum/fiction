#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/path_finding/distance.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void distance_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("manhattan_distance", &fiction::manhattan_distance<Lyt>, py::arg("layout"), py::arg("source"),
          py::arg("target"), DOC(fiction_manhattan_distance));
    m.def("euclidean_distance", &fiction::euclidean_distance<Lyt>, py::arg("layout"), py::arg("source"),
          py::arg("target"), DOC(fiction_euclidean_distance));
    m.def("squared_euclidean_distance", &fiction::squared_euclidean_distance<Lyt>, py::arg("layout"), py::arg("source"),
          py::arg("target"), DOC(fiction_squared_euclidean_distance));
    m.def("twoddwave_distance", &fiction::twoddwave_distance<Lyt>, py::arg("layout"), py::arg("source"),
          py::arg("target"), DOC(fiction_twoddwave_distance));
    m.def("chebyshev_distance", &fiction::chebyshev_distance<Lyt>, py::arg("layout"), py::arg("source"),
          py::arg("target"), DOC(fiction_chebyshev_distance));
}

}  // namespace detail

void distance(nanobind::module_& m)
{
    detail::distance_impl<py_cartesian_layout>(m);
    detail::distance_impl<py_shifted_cartesian_layout>(m);
    detail::distance_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction

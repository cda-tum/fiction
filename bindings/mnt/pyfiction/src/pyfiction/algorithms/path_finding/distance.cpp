#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/path_finding/distance.hpp>

#include <pybind11/pybind11.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void distance_impl(pybind11::module& m)
{
    namespace py = pybind11;

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

void distance(pybind11::module& m)
{
    detail::distance_impl<py_cartesian_layout>(m);
    detail::distance_impl<py_shifted_cartesian_layout>(m);
    detail::distance_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction

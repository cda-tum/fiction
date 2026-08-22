#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/detect_bdl_pairs.hpp>
#include <fiction/layouts/coordinates.hpp>
#include <fiction/technology/cell_technologies.hpp>

#include <fmt/format.h>

#include <optional>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_set.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void detect_bdl_pairs_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("detect_bdl_pairs", &fiction::detect_bdl_pairs<Lyt>, py::arg("lyt"), py::arg("type") = std::nullopt,
          py::arg("params") = fiction::detect_bdl_pairs_params{}, DOC(fiction_detect_bdl_pairs));
}

}  // namespace detail

void detect_bdl_pairs(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::bdl_pair<fiction::layouts::offset::ucoord_t>>(m, "bdl_pair", DOC(fiction_bdl_pair))
        .def(py::init<>(), DOC(fiction_bdl_pair_bdl_pair))
        .def(py::init<fiction::sidb_technology::cell_type, fiction::layouts::offset::ucoord_t,
                      fiction::layouts::offset::ucoord_t>(),
             py::arg("t"), py::arg("u"), py::arg("l"), DOC(fiction_bdl_pair_bdl_pair_2))
        .def_ro("type", &fiction::bdl_pair<fiction::layouts::offset::ucoord_t>::type, DOC(fiction_bdl_pair_type))
        .def_ro("upper", &fiction::bdl_pair<fiction::layouts::offset::ucoord_t>::upper, DOC(fiction_bdl_pair_upper))
        .def_ro("lower", &fiction::bdl_pair<fiction::layouts::offset::ucoord_t>::lower, DOC(fiction_bdl_pair_lower));

    py::class_<fiction::detect_bdl_pairs_params>(m, "detect_bdl_pairs_params", DOC(fiction_detect_bdl_pairs_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("minimum_distance", &fiction::detect_bdl_pairs_params::minimum_distance,
                DOC(fiction_detect_bdl_pairs_params_minimum_distance))
        .def_rw("maximum_distance", &fiction::detect_bdl_pairs_params::maximum_distance,
                DOC(fiction_detect_bdl_pairs_params_maximum_distance))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::detect_bdl_pairs_impl<py_sidb_100_lattice>(m);
    detail::detect_bdl_pairs_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

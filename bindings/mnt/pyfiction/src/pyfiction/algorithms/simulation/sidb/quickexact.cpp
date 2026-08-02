#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/quickexact.hpp>

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
void quickexact_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("quickexact", &fiction::quickexact<Lyt>, py::arg("lyt"), py::arg("params") = fiction::quickexact_params<>{},
          DOC(fiction_quickexact));
}

}  // namespace detail

void quickexact(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    py::enum_<fiction::quickexact_params<>::automatic_base_number_detection>(
        m, "automatic_base_number_detection", DOC(fiction_quickexact_params_automatic_base_number_detection))
        .value("ON", fiction::quickexact_params<>::automatic_base_number_detection::ON,
               DOC(fiction_quickexact_params_automatic_base_number_detection_ON))
        .value("OFF", fiction::quickexact_params<>::automatic_base_number_detection::OFF,
               DOC(fiction_quickexact_params_automatic_base_number_detection_OFF));

    /**
     * QuickExact parameters.
     */
    py::class_<fiction::quickexact_params<>>(m, "quickexact_params", DOC(fiction_quickexact_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("simulation_parameters", &fiction::quickexact_params<>::simulation_parameters,
                DOC(fiction_quickexact_params_simulation_parameters))
        .def_rw("base_number_detection", &fiction::quickexact_params<>::base_number_detection,
                DOC(fiction_quickexact_params_base_number_detection))
        .def_rw("local_external_potential", &fiction::quickexact_params<>::local_external_potential,
                DOC(fiction_quickexact_params_local_external_potential))
        .def_rw("global_potential", &fiction::quickexact_params<>::global_potential,
                DOC(fiction_quickexact_params_global_potential));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::quickexact_impl<py_sidb_100_lattice>(m);
    detail::quickexact_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

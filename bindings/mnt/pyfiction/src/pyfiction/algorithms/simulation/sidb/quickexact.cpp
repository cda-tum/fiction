#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/quickexact.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void quickexact_impl(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    m.def("quickexact", &fiction::quickexact<Lyt>, py::arg("lyt"), py::arg("params") = fiction::quickexact_params<>{},
          DOC(fiction_quickexact));
}

}  // namespace detail

void quickexact(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

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
        .def(py::init<>())
        .def_readwrite("simulation_parameters", &fiction::quickexact_params<>::simulation_parameters,
                       DOC(fiction_quickexact_params_simulation_parameters))
        .def_readwrite("base_number_detection", &fiction::quickexact_params<>::base_number_detection,
                       DOC(fiction_quickexact_params_base_number_detection))
        .def_readwrite("local_external_potential", &fiction::quickexact_params<>::local_external_potential,
                       DOC(fiction_quickexact_params_local_external_potential))
        .def_readwrite("global_potential", &fiction::quickexact_params<>::global_potential,
                       DOC(fiction_quickexact_params_global_potential));

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!
    detail::quickexact_impl<py_sidb_100_lattice>(m);
    detail::quickexact_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

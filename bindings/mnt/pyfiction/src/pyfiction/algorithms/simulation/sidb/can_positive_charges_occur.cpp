#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/can_positive_charges_occur.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void can_positive_charges_occur_impl(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    namespace py = pybind11;  // NOLINT(misc-unused-alias-decls)

    m.def("can_positive_charges_occur", &fiction::can_positive_charges_occur<Lyt>, py::arg("lyt"),
          py::arg("sim_params"), DOC(fiction_can_positive_charges_occur));
}

}  // namespace detail

void can_positive_charges_occur(pybind11::module& m)  // NOLINT(misc-use-internal-linkage)
{
    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::can_positive_charges_occur_impl<py_sidb_100_lattice>(m);
    detail::can_positive_charges_occur_impl<py_sidb_111_lattice>(m);
}

}  // namespace pyfiction

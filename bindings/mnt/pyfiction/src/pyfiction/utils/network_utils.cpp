//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/utils/network_utils.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>    // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Ntk>
void has_high_degree_fanin_nodes(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def("has_high_degree_fanin_nodes", &fiction::has_high_degree_fanin_nodes<Ntk>, py::arg("ntk"),
          py::arg("threshold") = 2, DOC(fiction_has_high_degree_fanin_nodes));
}

}  // namespace detail

void network_utils(nanobind::module_& m)
{
    namespace py = nanobind;

    // NOLINTNEXTLINE(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    py::exception<fiction::high_degree_fanin_exception>(
        m, "high_degree_fanin_exception",
        PyExc_ValueError);  // NOLINT(misc-include-cleaner): included through nanobind.h

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::has_high_degree_fanin_nodes<py_logic_network>(m);
}

}  // namespace pyfiction

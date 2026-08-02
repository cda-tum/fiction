//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/utils/network_utils.hpp>

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

    py::exception<fiction::high_degree_fanin_exception>(
        m, "high_degree_fanin_exception",
        PyExc_ValueError);  // NOLINT(misc-include-cleaner): included through nanobind.h

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::has_high_degree_fanin_nodes<py_logic_network>(m);
}

}  // namespace pyfiction

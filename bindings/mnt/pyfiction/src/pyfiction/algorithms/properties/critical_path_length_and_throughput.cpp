#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>

#include <cstdint>
#include <utility>

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
void critical_path_length_and_throughput_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "critical_path_length_and_throughput",
        [](const Lyt& lyt) -> std::pair<uint64_t, uint64_t>
        {
            const auto result = fiction::critical_path_length_and_throughput(lyt);

            return {result.critical_path_length, result.throughput};
        },
        py::arg("layout"), DOC(fiction_critical_path_length_and_throughput));
}

}  // namespace detail

void critical_path_length_and_throughput(nanobind::module_& m)
{
    detail::critical_path_length_and_throughput_impl<py_cartesian_gate_layout>(m);
    detail::critical_path_length_and_throughput_impl<py_shifted_cartesian_gate_layout>(m);
    detail::critical_path_length_and_throughput_impl<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/properties/critical_path_length_and_throughput.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <utility>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void critical_path_length_and_throughput_impl(pybind11::module& m)
{
    namespace py = pybind11;

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

void critical_path_length_and_throughput(pybind11::module& m)
{
    detail::critical_path_length_and_throughput_impl<py_cartesian_gate_layout>(m);
    detail::critical_path_length_and_throughput_impl<py_shifted_cartesian_gate_layout>(m);
    detail::critical_path_length_and_throughput_impl<py_hexagonal_gate_layout>(m);
}

}  // namespace pyfiction

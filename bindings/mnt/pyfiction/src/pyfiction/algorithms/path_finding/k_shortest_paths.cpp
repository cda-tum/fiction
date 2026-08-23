#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/physical_design/path_finding/k_shortest_paths.hpp>
#include <fiction/physical_design/utils/routing_utils.hpp>
#include <fiction/traits.hpp>

#include <cstdint>
#include <vector>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void yen_k_shortest_paths_impl(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "yen_k_shortest_paths",
        [](const Lyt& lyt, const fiction::coordinate<Lyt>& source, const fiction::coordinate<Lyt>& target,
           const uint32_t k, const fiction::physical_design::path_finding::yen_k_shortest_paths_params& params)
        {
            const auto k_paths = fiction::physical_design::path_finding::yen_k_shortest_paths<
                fiction::physical_design::utils::layout_coordinate_path<Lyt>, Lyt>(lyt, {source, target}, k, params);

            std::vector<std::vector<fiction::coordinate<Lyt>>> paths{};
            paths.reserve(k_paths.size());

            for (const auto& path : k_paths)
            {
                paths.emplace_back(path.cbegin(), path.cend());
            }

            return paths;
        },
        py::arg("layout"), py::arg("source"), py::arg("target"), py::arg("k"),
        py::arg("params") = fiction::physical_design::path_finding::yen_k_shortest_paths_params{},
        DOC(fiction_yen_k_shortest_paths));
}

}  // namespace detail

void physical_design::path_finding::yen_k_shortest_paths(nanobind::module_& m)
{
    namespace py = nanobind;

    py::class_<fiction::physical_design::path_finding::yen_k_shortest_paths_params>(
        m, "yen_k_shortest_paths_params", DOC(fiction_yen_k_shortest_paths_params))
        .def(py::init<>(), "Default constructor.")
        .def_rw("a_star_params", &fiction::physical_design::path_finding::yen_k_shortest_paths_params::astar_params,
                DOC(fiction_yen_k_shortest_paths_params_astar_params))

        ;

    // NOTE be careful with the order of the following calls! Python will resolve the first matching overload!

    detail::yen_k_shortest_paths_impl<py_cartesian_obstruction_layout>(m);
    detail::yen_k_shortest_paths_impl<py_cartesian_gate_layout>(m);
    detail::yen_k_shortest_paths_impl<py_cartesian_clocked_layout>(m);
    detail::yen_k_shortest_paths_impl<py_cartesian_layout>(m);
    detail::yen_k_shortest_paths_impl<py_shifted_cartesian_obstruction_layout>(m);
    detail::yen_k_shortest_paths_impl<py_shifted_cartesian_gate_layout>(m);
    detail::yen_k_shortest_paths_impl<py_shifted_cartesian_clocked_layout>(m);
    detail::yen_k_shortest_paths_impl<py_shifted_cartesian_layout>(m);
    detail::yen_k_shortest_paths_impl<py_hexagonal_obstruction_layout>(m);
    detail::yen_k_shortest_paths_impl<py_hexagonal_gate_layout>(m);
    detail::yen_k_shortest_paths_impl<py_hexagonal_clocked_layout>(m);
    detail::yen_k_shortest_paths_impl<py_hexagonal_layout>(m);
}

}  // namespace pyfiction

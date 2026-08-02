//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/io/write_location_and_ground_state.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string_view.h>
#include <nanobind/stl/tuple.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/unordered_set.h>
#include <nanobind/stl/vector.h>

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void write_location_and_ground_state(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_location_and_ground_state",
        [](const fiction::sidb_simulation_result<Lyt>& sim_result, const std::string_view& filename)
        { fiction::write_location_and_ground_state(sim_result, filename); }, py::arg("sim_result"), py::arg("filename"),
        DOC(fiction_write_location_and_ground_state));
}

}  // namespace detail

void write_location_and_ground_state(nanobind::module_& m)
{
    detail::write_location_and_ground_state<py_sidb_layout>(m);
}

}  // namespace pyfiction

//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/technology/sidb/io/write_location_and_ground_state.hpp>
#include <fiction/technology/sidb/simulation/result.hpp>

#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>           // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>            // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/unordered_map.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>         // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void sidb::io::write_location_and_ground_state(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_location_and_ground_state",
        [](const fiction::sidb::simulation::result<Lyt>& sim_result, const std::string_view& filename)
        { fiction::sidb::io::write_location_and_ground_state(sim_result, filename); }, py::arg("sim_result"),
        py::arg("filename"), DOC(fiction_write_location_and_ground_state));
}

}  // namespace detail

void sidb::io::write_location_and_ground_state(nanobind::module_& m)
{
    detail::write_location_and_ground_state<py_sidb_layout>(m);
}

}  // namespace pyfiction

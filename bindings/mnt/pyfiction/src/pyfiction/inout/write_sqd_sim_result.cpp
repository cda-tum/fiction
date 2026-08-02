//
// Created by marcel on 21.11.23.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp>
#include <fiction/io/write_sqd_sim_result.hpp>

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
void write_sqd_sim_result(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_sqd_sim_result", [](const fiction::sidb_simulation_result<Lyt>& lyt, const std::string_view& filename)
        { fiction::write_sqd_sim_result(lyt, filename); }, py::arg("layout"), py::arg("filename"),
        DOC(fiction_write_sqd_sim_result));
}

}  // namespace detail

void write_sqd_sim_result(nanobind::module_& m)
{
    detail::write_sqd_sim_result<py_sidb_layout>(m);
}

}  // namespace pyfiction

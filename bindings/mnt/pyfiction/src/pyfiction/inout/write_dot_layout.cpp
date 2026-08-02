//
// Created by marcel on 16.12.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/io/dot_drawers.hpp>
#include <fiction/traits.hpp>

#include <mockturtle/io/write_dot.hpp>

#include <string>
#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>
#include <nanobind/stl/function.h>
#include <nanobind/stl/map.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/set.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
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
void write_dot_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_dot_layout",
        [](const Lyt& lyt, const std::string_view& filename)
        {
            if constexpr (fiction::is_cartesian_layout_v<Lyt>)
            {
                fiction::write_dot_layout<Lyt, fiction::gate_layout_cartesian_drawer<Lyt>>(lyt, filename);
            }
            else if constexpr (fiction::is_hexagonal_layout_v<Lyt>)
            {
                fiction::write_dot_layout<Lyt, fiction::gate_layout_hexagonal_drawer<Lyt>>(lyt, filename);
            }
        },
        py::arg("layout"), py::arg("filename"), DOC(fiction_write_dot_layout));
}

template <typename Ntk>
void write_dot_network(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_dot_network", [](const Ntk& ntk, const std::string_view& filename)
        { mockturtle::write_dot(ntk, std::string{filename}, fiction::technology_dot_drawer<Ntk>{}); },
        py::arg("network"), py::arg("filename"));
}

}  // namespace detail

void write_dot_layout(nanobind::module_& m)
{
    detail::write_dot_layout<py_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_shifted_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_hexagonal_gate_layout>(m);

    detail::write_dot_network<py_logic_network>(m);
}

}  // namespace pyfiction

//
// Created by marcel on 16.12.22.
//

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/io/layout_drawers.hpp>
#include <fiction/networks/io/dot_drawers.hpp>
#include <fiction/traits.hpp>

#include <mockturtle/io/write_dot.hpp>

#include <string>
#include <string_view>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>     // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>          // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>   // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>       // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename Lyt>
void layouts::io::write_dot_layout(nanobind::module_& m)
{
    namespace py = nanobind;  // NOLINT(misc-unused-alias-decls)

    m.def(
        "write_dot_layout",
        [](const Lyt& lyt, const std::string_view& filename)
        {
            if constexpr (fiction::is_cartesian_layout_v<Lyt>)
            {
                fiction::layouts::io::write_dot_layout<Lyt, fiction::layouts::io::gate_layout_cartesian_drawer<Lyt>>(
                    lyt, filename);
            }
            else if constexpr (fiction::is_hexagonal_layout_v<Lyt>)
            {
                fiction::layouts::io::write_dot_layout<Lyt, fiction::layouts::io::gate_layout_hexagonal_drawer<Lyt>>(
                    lyt, filename);
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
        { mockturtle::write_dot(ntk, std::string{filename}, fiction::networks::io::technology_dot_drawer<Ntk>{}); },
        py::arg("network"), py::arg("filename"));
}

}  // namespace detail

void layouts::io::write_dot_layout(nanobind::module_& m)
{
    detail::write_dot_layout<py_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_shifted_cartesian_gate_layout>(m);
    detail::write_dot_layout<py_hexagonal_gate_layout>(m);

    detail::write_dot_network<py_logic_network>(m);
}

}  // namespace pyfiction

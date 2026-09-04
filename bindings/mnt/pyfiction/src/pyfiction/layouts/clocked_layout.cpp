/*
 * Copyright (c) 2018 - 2023 Marcel Walter
 * Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

/**
 * @file
 * @brief Python bindings for `fiction/layouts/clocked_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 * @author Simon Hofmann (simon1hofmann)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/layout_dimension.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/clocking_scheme.hpp>
#include <fiction/layouts/io/print_layout.hpp>  // NOLINT(misc-include-cleaner): Used in dependent template contexts below.
#include <fiction/traits.hpp>

#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include <nanobind/nanobind.h>
#include <nanobind/stl/array.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/function.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/optional.h>    // NOLINT(misc-include-cleaner)
#include <nanobind/stl/pair.h>        // NOLINT(misc-include-cleaner)
#include <nanobind/stl/set.h>         // NOLINT(misc-include-cleaner)
#include <nanobind/stl/shared_ptr.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>      // NOLINT(misc-include-cleaner)
#include <nanobind/stl/vector.h>      // NOLINT(misc-include-cleaner)

namespace pyfiction
{

namespace detail
{

template <typename LytBase, typename ClockedLyt>
void clocked_layout(nanobind::module_& m, const std::string& class_name)
{
    namespace py = nanobind;

    using coordinate = fiction::coordinate<ClockedLyt>;

    constexpr auto supports_explicit_minimum = requires(ClockedLyt& lyt, const coordinate& minimum,
                                                        const coordinate& maximum) { lyt.resize(minimum, maximum); };

    /**
     * Clocked Cartesian layout.
     */
    auto layout = py::class_<ClockedLyt, LytBase>(m, class_name.c_str(), DOC(fiction_is_clocked_layout));

    layout.def(py::init<>(), DOC(fiction_layouts_clocked_layout_clocked_layout));

    if constexpr (supports_explicit_minimum)
    {
        layout
            .def(
                "__init__",
                [](py::pointer_and_handle<ClockedLyt> self, const std::pair<coordinate, coordinate>& dimension)
                { construct_layout_from_bounds(self, dimension); }, py::arg("dimension"),
                "Constructs a clocked layout from inclusive minimum and maximum coordinates.")
            .def(
                "__init__",
                [](py::pointer_and_handle<ClockedLyt> self, const std::pair<coordinate, coordinate>& dimension,
                   const std::string& scheme_name)
                {
                    if (const auto scheme = fiction::layouts::clocking::get_scheme<ClockedLyt>(scheme_name);
                        scheme.has_value())
                    {
                        construct_layout_from_bounds(self, dimension, *scheme);
                        return;
                    }

                    throw std::runtime_error("Given name does not refer to a supported clocking scheme");
                },
                py::arg("dimension"), py::arg("clocking_scheme"),
                "Constructs a clocked layout from coordinate bounds and a clocking scheme.");
    }

    layout
        .def(
            "__init__", [](py::pointer_and_handle<ClockedLyt> self, const fiction::aspect_ratio<ClockedLyt>& dimension)
            { new (self.p) ClockedLyt{validate_layout_maximum(dimension)}; }, py::arg("dimension"),
            DOC(fiction_layouts_clocked_layout_clocked_layout))
        .def(
            "__init__",
            [](py::pointer_and_handle<ClockedLyt> self, const fiction::aspect_ratio<ClockedLyt>& dimension,
               const std::string& scheme_name)
            {
                if (const auto scheme = fiction::layouts::clocking::get_scheme<ClockedLyt>(scheme_name);
                    scheme.has_value())
                {
                    new (self.p) ClockedLyt{validate_layout_maximum(dimension), *scheme};
                    return;
                }

                throw std::runtime_error("Given name does not refer to a supported clocking scheme");
            },
            py::arg("dimension"), py::arg("clocking_scheme") = "2DDWave",
            DOC(fiction_layouts_clocked_layout_clocked_layout_2));

    layout
        .def("assign_clock_number", &ClockedLyt::assign_clock_number, py::arg("cz"), py::arg("cn"),
             DOC(fiction_layouts_clocked_layout_assign_clock_number))
        .def("get_clock_number", &ClockedLyt::get_clock_number, py::arg("cz"),
             DOC(fiction_layouts_clocked_layout_get_clock_number))
        .def("num_clocks", &ClockedLyt::num_clocks, DOC(fiction_layouts_clocked_layout_num_clocks))
        .def("is_regularly_clocked", &ClockedLyt::is_regularly_clocked,
             DOC(fiction_layouts_clocked_layout_is_regularly_clocked))

        .def("is_clocking_scheme", &ClockedLyt::is_clocking_scheme, py::arg("name"),
             DOC(fiction_layouts_clocked_layout_is_clocking_scheme))

        .def("is_incoming_clocked", &ClockedLyt::is_incoming_clocked, py::arg("cz1"), py::arg("cz2"),
             DOC(fiction_layouts_clocked_layout_is_incoming_clocked))
        .def("is_outgoing_clocked", &ClockedLyt::is_outgoing_clocked, py::arg("cz1"), py::arg("cz2"),
             DOC(fiction_layouts_clocked_layout_is_outgoing_clocked))

        .def("incoming_clocked_zones", &ClockedLyt::incoming_clocked_zones, py::arg("cz"),
             DOC(fiction_layouts_clocked_layout_incoming_clocked_zones))
        .def("outgoing_clocked_zones", &ClockedLyt::outgoing_clocked_zones, py::arg("cz"),
             DOC(fiction_layouts_clocked_layout_outgoing_clocked_zones))

        .def("in_degree", &ClockedLyt::in_degree, py::arg("cz"), DOC(fiction_layouts_clocked_layout_in_degree))
        .def("out_degree", &ClockedLyt::out_degree, py::arg("cz"), DOC(fiction_layouts_clocked_layout_out_degree))
        .def("degree", &ClockedLyt::degree, py::arg("cz"), DOC(fiction_layouts_clocked_layout_degree))

        .def(
            "__repr__",
            [](const ClockedLyt& lyt) -> std::string
            {
                std::stringstream stream{};
                fiction::layouts::io::print_layout(lyt, stream);
                return stream.str();
            },
            "Returns a string representation of the layout.")

        ;
}

}  // namespace detail

void clocked_layout(nanobind::module_& m)
{
    detail::clocked_layout<py_cartesian_layout, py_cartesian_clocked_layout>(m, "clocked_cartesian_layout");
    detail::clocked_layout<py_cartesian_layout_cube, py_cartesian_clocked_layout_cube>(m,
                                                                                       "clocked_cartesian_layout_cube");
    detail::clocked_layout<py_shifted_cartesian_layout, py_shifted_cartesian_clocked_layout>(
        m, "clocked_shifted_cartesian_layout");
    detail::clocked_layout<py_hexagonal_layout, py_hexagonal_clocked_layout>(m, "clocked_hexagonal_layout");
}

}  // namespace pyfiction

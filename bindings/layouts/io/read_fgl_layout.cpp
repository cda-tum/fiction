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
 * @brief Python bindings for `fiction/layouts/io/read_fgl_layout.hpp`.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#include "pyfiction/documentation.hpp"
#include "pyfiction/types.hpp"

#include <fiction/layouts/io/read_fgl_layout.hpp>

#include <string_view>
#include <type_traits>

#include <nanobind/nanobind.h>
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts filename and layout name arguments.

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

namespace detail
{
/**
 * @brief Bind the FGL overload that selects a registered layout class.
 * @tparam Lyt Layout representation.
 * @param m Layout I/O module.
 * @param signature Python signature including the selected return type.
 */
template <typename Lyt>
void register_read_fgl_layout(nb::module_& m, const char* signature)
{
    /** @brief Read the selected registered layout representation. */
    const auto read =
        [](const std::string_view& filename, const std::string_view& layout_name, const nb::handle layout_type)
    {
        // Keep the GIL for Python type dispatch, then release it for native parsing.
        if (!layout_type.is(nb::type<Lyt>()))
        {
            throw nb::next_overload();
        }
        const nb::gil_scoped_release release{};
        return fiction::layouts::io::read_fgl_layout<Lyt>(filename, layout_name);
    };
    if constexpr (std::is_same_v<Lyt, py_cartesian_gate_layout>)
    {
        m.def("read_fgl_layout", read, nb::arg("filename"), nb::arg("layout_name") = "", nb::kw_only(),
              nb::arg("layout_type") = nb::type<Lyt>(), nb::sig(signature), DOC(fiction_layouts_io_read_fgl_layout_3));
    }
    else
    {
        m.def("read_fgl_layout", read, nb::arg("filename"), nb::arg("layout_name") = "", nb::kw_only(),
              nb::arg("layout_type"), nb::sig(signature));
    }
}
}  // namespace detail

/** @brief Register FGL readers and their parsing exception. @param m Layout I/O module. */
void register_read_fgl_layout(nb::module_& m)
{
    // NOLINTBEGIN(bugprone-throw-keyword-missing,bugprone-unused-raii): registers the exception
    // translator with the module; it is not meant to be thrown here
    nb::exception<fiction::layouts::io::fgl_parsing_error>(
        m, "fgl_parsing_error",
        PyExc_RuntimeError);  // NOLINT(misc-include-cleaner): Included through nanobind.h
    // NOLINTEND(bugprone-throw-keyword-missing,bugprone-unused-raii)

    detail::register_read_fgl_layout<py_cartesian_gate_layout>(
        m, "def read_fgl_layout(filename: str, layout_name: str = \"\", *, layout_type: "
           "type[mnt.pyfiction.layouts.cartesian_gate_layout] = mnt.pyfiction.layouts.cartesian_gate_layout) -> "
           "mnt.pyfiction.layouts.cartesian_gate_layout");
    detail::register_read_fgl_layout<py_shifted_cartesian_gate_layout>(
        m, "def read_fgl_layout(filename: str, layout_name: str = \"\", *, layout_type: "
           "type[mnt.pyfiction.layouts.shifted_cartesian_gate_layout]) -> "
           "mnt.pyfiction.layouts.shifted_cartesian_gate_layout");
    detail::register_read_fgl_layout<py_hexagonal_gate_layout>(
        m, "def read_fgl_layout(filename: str, layout_name: str = \"\", *, layout_type: "
           "type[mnt.pyfiction.layouts.hexagonal_gate_layout]) -> mnt.pyfiction.layouts.hexagonal_gate_layout");
    m.def(
        "read_fgl_layout", [](const std::string_view&, const std::string_view&, const nb::handle) -> nb::object
        { throw nb::type_error("layout_type must be a registered gate layout class"); }, nb::arg("filename"),
        nb::arg("layout_name") = "", nb::kw_only(), nb::arg("layout_type"),
        nb::sig(
            "def read_fgl_layout(filename: str, layout_name: str = \"\", *, layout_type: "
            "type[mnt.pyfiction.layouts.cartesian_gate_layout | mnt.pyfiction.layouts.shifted_cartesian_gate_layout | "
            "mnt.pyfiction.layouts.hexagonal_gate_layout]) -> mnt.pyfiction.layouts.cartesian_gate_layout | "
            "mnt.pyfiction.layouts.shifted_cartesian_gate_layout | mnt.pyfiction.layouts.hexagonal_gate_layout"));
}

}  // namespace pyfiction

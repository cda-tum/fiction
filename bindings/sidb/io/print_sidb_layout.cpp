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
 * @brief Python binding for the textual SiDB layout printer.
 * @author Marcel Walter (marcelwa)
 */

#include <fiction/technology/sidb/charge_distribution.hpp>
#include <fiction/technology/sidb/io/print_sidb_layout.hpp>
#include <fiction/technology/sidb/layout.hpp>

#include <optional>
#include <sstream>

#include <nanobind/nanobind.h>
#include <nanobind/stl/optional.h>  // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string.h>    // NOLINT(misc-include-cleaner)

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds print sidb layout helpers.
 * @param m Python module.
 */
void register_print_sidb_layout(nb::module_& m)
{
    m.def(
        "print_sidb_layout",
        [](const fiction::sidb::layout& lyt, const std::optional<fiction::sidb::charge_distribution>& cd,
           const bool lat_color, const bool crop_layout, const bool draw_lattice)
        {
            std::ostringstream os{};
            fiction::sidb::io::print_sidb_layout(os, lyt, cd.has_value() ? &*cd : nullptr, lat_color, crop_layout,
                                                 draw_lattice);
            return os.str();
        },
        nb::call_guard<nb::gil_scoped_release>(), nb::arg("layout"), nb::arg("charge_distribution") = nb::none(),
        nb::arg("lat_color") = true, nb::arg("crop_layout") = false, nb::arg("draw_lattice") = true,
        R"doc(Returns a SiDB layout as a lattice picture in a string.

The picture contains one symbol per lattice site of the bounding box, row by row,
with a blank line between the dimer rows of an H-Si(100)-2x1 surface and an indented
second row per unit cell of an H-Si(111)-1x1 surface. Inputs, outputs, normal SiDBs,
defects, and empty sites differ by color and symbol. With a charge distribution,
a charged SiDB is a filled circle colored by its sign and a neutral one a hollow circle.

Args:
    layout: Layout to render.
    charge_distribution: Charge distribution over the layout's SiDBs, or None to render dot tags only.
    lat_color: Whether to color the picture.
    crop_layout: Whether to pad the bounding box by two columns and one unit cell instead of rendering it tight.
    draw_lattice: Whether to render empty lattice sites as dots.

Returns:
    The lattice picture.

Raises:
    ValueError: The charge distribution sites differ from the layout.
    IndexError: Padding exceeds the lattice-site coordinate range.
)doc");
}

}  // namespace pyfiction

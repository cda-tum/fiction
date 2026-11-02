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
 * @brief Python bindings for `fiction/technology/sidb/io/write_sqd_layout.hpp`.
 * @author Marcel Walter (marcelwa)
 */

#include <fiction/technology/sidb/io/write_sqd_layout.hpp>
#include <fiction/technology/sidb/layout.hpp>
#include <fiction/utils/progress.hpp>

#include <string>

#include <nanobind/nanobind.h>
#include <nanobind/stl/function.h>     // NOLINT(misc-include-cleaner): enables callback conversion
#include <nanobind/stl/string.h>       // NOLINT(misc-include-cleaner)
#include <nanobind/stl/string_view.h>  // NOLINT(misc-include-cleaner): converts callback task names

/**
 * @brief Namespace alias for nanobind.
 */
namespace nb = nanobind;

namespace pyfiction
{

/**
 * @brief Binds write sqd layout helpers.
 * @param m Python module.
 */
void register_write_sqd_layout(nb::module_& m)
{
    m.def(
        "write_sqd_layout",
        [](const fiction::sidb::layout& lyt, const std::string& filename,
           const fiction::utils::progress_callback& on_progress)
        {
            if (on_progress)
            {
                /** @brief Layout snapshot taken while the GIL is held. */
                const auto snapshot = lyt;  // NOLINT(performance-unnecessary-copy-initialization): Callback isolation.
                /** @brief Releases the GIL while the snapshot is written. */
                const nb::gil_scoped_release release{};
                fiction::sidb::io::write_sqd_layout(snapshot, filename, on_progress);
            }
            else
            {
                /** @brief Releases the GIL while the borrowed layout is written. */
                const nb::gil_scoped_release release{};
                fiction::sidb::io::write_sqd_layout(lyt, filename, on_progress);
            }
        },
        nb::arg("layout"), nb::arg("filename"), nb::arg("on_progress").none() = nb::none(),
        R"doc(Writes a SiDB layout as an SQD file for SiQAD.

The file records the layout's lattice, SiDB lattice coordinates and dot tags,
and surface defects.

With on_progress, the writer copies the layout before calling Python. Callback
mutations of the source layout do not change the exported snapshot. Without a
callback, the writer borrows the layout; other threads must not modify the layout
until this call returns.

Callback exceptions propagate from the initial report and progress updates.
The final report during reporter destruction ignores callback exceptions.

Args:
    layout: Layout to write.
    filename: File to write into.
    on_progress: Receives completed dot and defect records; None disables progress reporting.

Raises:
    ValueError: A lattice component is non-finite or a charged-defect material
        parameter is non-finite or negative; the destination remains unchanged.
    RuntimeError: Creating, writing, closing, or replacing the output file fails.
)doc");
}

}  // namespace pyfiction

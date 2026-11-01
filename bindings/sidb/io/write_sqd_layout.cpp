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

#include "pyfiction/documentation.hpp"

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
                const auto snapshot = lyt;
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
        (parameter_doc(DOC(fiction_sidb_io_write_sqd_layout_2), {{"lyt", "layout"}}) +
         "\n\nWith on_progress, the writer copies the layout before calling Python. Callback mutations do not change "
         "the exported snapshot. Without a callback, the writer borrows the layout.\n"
         "Callback exceptions propagate from the initial report and progress updates. The final report during reporter "
         "destruction ignores callback exceptions.")
            .c_str());
}

}  // namespace pyfiction

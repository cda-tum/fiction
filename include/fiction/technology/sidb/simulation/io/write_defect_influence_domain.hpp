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
 * @brief Writer that serializes a defect influence domain to CSV.
 * @author Jan Drewniok (Drewniok)
 * @author Marcel Walter (marcelwa)
 * @author OpenAI (Codex)
 */

#pragma once

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/simulation/defects/defect_influence.hpp"
#include "fiction/utils/atomic_write.hpp"
#include "fiction/utils/io/csv_writer.hpp"

#include <fstream>
#include <ostream>
#include <string>
#include <string_view>

namespace fiction::sidb::simulation::io
{

/**
 * Parameters for writing a defect influence domain to a CSV file.
 */
struct write_defect_influence_domain_params
{
    /**
     * The tag used to represent the influential value of a defect position.
     */
    std::string_view influential_tag = "influential";
    /**
     * The tag used to represent the non-influential value of a defect position.
     */
    std::string_view non_influential_tag = "non-influential";
};

/**
 * @brief Writes the defect influence at each evaluated lattice position as CSV.
 *
 * Each data row contains the column, the SiQAD row (`2y + z`), and the influence tag.
 * The header is `x,y,operational status`.
 *
 * @param defect_infdom The domain to write.
 * @param os The output stream.
 * @param params Tags used for influential and non-influential defect positions.
 */
inline void write_defect_influence_domain(const defects::defect_influence_domain& defect_infdom, std::ostream& os,
                                          const write_defect_influence_domain_params& params = {})
{
    utils::io::csv_writer writer{os};

    writer.write_line("x", "y", "operational status");

    defect_infdom.for_each(
        [&params, &writer](const auto& pos, const auto& op_val)
        {
            writer.write_line(pos.x, row_of(pos),
                              std::get<0>(op_val) == defects::defect_influence_status::INFLUENTIAL ?
                                  params.influential_tag :
                                  params.non_influential_tag);
        });
}
/**
 * @brief Writes a defect influence domain as CSV to a file; see the stream overload.
 *
 * @param defect_infdom The domain to write.
 * @param filename The file to write to.
 * @param params Tags used for influential and non-influential defect positions.
 * @throws std::ofstream::failure if the file cannot be opened.
 */
inline void write_defect_influence_domain(const defects::defect_influence_domain&     defect_infdom,
                                          const std::string_view&                     filename,
                                          const write_defect_influence_domain_params& params = {})
{
    fiction::detail::atomic_write(filename,
                                  [&](std::ostream& os) { write_defect_influence_domain(defect_infdom, os, params); });
}

}  // namespace fiction::sidb::simulation::io

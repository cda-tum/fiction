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
 */

#pragma once

#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/simulation/defects/defect_influence.hpp"
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
 * Writes a CSV representation of an defect influence domain to the specified output stream. The data are written
 * as rows, each corresponding to one set of simulation parameters and their corresponding influence status.
 *
 * The output CSV format is as follows:
 * X_DIMENSION, Y_DIMENSION, Influence STATUS
 * ... subsequent rows for each set of simulation parameters.
 *
 * @param defect_infdom The defect influence domain to be written. It contains a mapping from defect positions to their
 * influence status.
 * @param os The output stream where the CSV representation of the defect influence domain is written to.
 * @param params The parameters used for writing, including the influential and non-influential tags. Defaults to an
 * empty `write_defect_influence_domain_params` object, which provides standard tags.
 */
/**
 * Writes a defect influence domain as CSV: one line per evaluated defect position with its column, its row
 * (`2y + z` of the lattice site), and the influence tag.
 *
 * @param defect_infdom The domain to write.
 * @param os The output stream.
 * @param params Parameters.
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
 * Writes a defect influence domain as CSV to a file; see the stream overload.
 *
 * @param defect_infdom The domain to write.
 * @param filename The file to write to.
 * @param params Parameters.
 * @throws std::ofstream::failure if the file cannot be opened.
 */
inline void write_defect_influence_domain(const defects::defect_influence_domain&     defect_infdom,
                                          const std::string_view&                     filename,
                                          const write_defect_influence_domain_params& params = {})
{
    std::ofstream os{std::string{filename}, std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_defect_influence_domain(defect_infdom, os, params);

    os.close();
}

}  // namespace fiction::sidb::simulation::io

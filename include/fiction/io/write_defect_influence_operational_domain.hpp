//
// Created by Jan Drewniok on 05.03.2024.
//

#ifndef FICTION_WRITE_DEFECT_OPERATIONAL_DOMAIN_HPP
#define FICTION_WRITE_DEFECT_OPERATIONAL_DOMAIN_HPP

#include "fiction/algorithms/simulation/sidb/defect_influence_operational_domain.hpp"
#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/io/csv_writer.hpp"

#include <fstream>
#include <ostream>
#include <string>
#include <string_view>

namespace fiction
{

/**
 * Parameters for writing a defect influence operational domain to a CSV file.
 */
struct write_defect_operational_domain_params
{
    /**
     * The tag used to represent the operational value of a defect position.
     */
    std::string_view operational_tag = "operational";
    /**
     * The tag used to represent the non-operational value of a defect position.
     */
    std::string_view non_operational_tag = "non-operational";
};

/**
 * Writes a CSV representation of an operational domain to the specified output stream. The data are written
 * as rows, each corresponding to one set of simulation parameters and their corresponding operational status.
 *
 * The output CSV format is as follows:
 * X_DIMENSION, Y_DIMENSION, OPERATIONAL STATUS
 * ... subsequent rows for each set of simulation parameters.
 *
 * The operational status is a binary value represented by specified tags in `params` indicating whether the simulation
 * parameters are within the operational domain or not.
 *
 * @param defect_opdom The operational domain to be written. It contains a mapping from sets of simulation parameters
 * (represented as a pair of sweep parameters for the X and Y dimensions) to their operational status.
 * @param os The output stream where the CSV representation of the operational domain is written to.
 * @param params The parameters used for writing, including the operational and non-operational tags. Defaults to an
 * empty `write_defect_operational_domain_params` object, which provides standard tags.
 */
template <typename Lyt>
inline void write_defect_influence_operational_domain(const defect_operational_domain<Lyt>& defect_opdom,
                                                      std::ostream&                                   os,
                                                      const write_defect_operational_domain_params&   params = {})
{
    csv_writer writer{os};

    writer.write_line("x", "y", "operational status");

    for (const auto& [sim_param, op_val] : defect_opdom.operational_values)
    {
        writer.write_line(sim_param.x, sim_param.y,
                          op_val == operational_status::OPERATIONAL ? params.operational_tag :
                                                                      params.non_operational_tag);
    }
}
/**
 * Writes a CSV representation of an operational domain to the specified file. The data are written as rows, each
 * corresponding to one set of simulation parameters and their corresponding operational status.
 *
 * The output CSV format is as follows:
 * X_DIMENSION, Y_DIMENSION, OPERATIONAL STATUS
 * ... subsequent rows for each set of simulation parameters.
 *
 * The operational status is a binary value represented by specified tags in `params` indicating whether the simulation
 * parameters are within the operational domain or not.
 *
 * @param defect_opdom The operational domain to be written. It contains a mapping from sets of simulation parameters
 * (represented as a pair of sweep parameters for the X and Y dimensions) to their operational status.
 * @param filename The filename where the CSV representation of the operational domain is written to.
 * @param params The parameters used for writing, including the operational and non-operational tags. Defaults to an
 * empty `write_defect_operational_domain_params` object, which provides standard tags.
 */
template <typename Lyt>
inline void write_defect_influence_operational_domain(const defect_operational_domain<Lyt>& defect_opdom,
                                                      const std::string_view&                         filename,
                                                      const write_defect_operational_domain_params&   params = {})
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_defect_influence_operational_domain(defect_opdom, os, params);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_WRITE_DEFECT_OPERATIONAL_DOMAIN_HPP

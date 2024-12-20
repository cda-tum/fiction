//
// Created by Jan Drewniok on 05.03.2024.
//

#ifndef FICTION_WRITE_DEFECT_INFLUENCE_DOMAIN_HPP
#define FICTION_WRITE_DEFECT_INFLUENCE_DOMAIN_HPP

#include "fiction/algorithms/simulation/sidb/defect_influence.hpp"
#include "fiction/io/csv_writer.hpp"

#include <fstream>
#include <ostream>
#include <string_view>

namespace fiction
{

/**
 * Parameters for writing a defect influence domain to a CSV file.
 */
struct write_defect_operational_domain_params
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
 * Writes a CSV representation of an operational domain to the specified output stream. The data are written
 * as rows, each corresponding to one set of simulation parameters and their corresponding influence status.
 *
 * The output CSV format is as follows:
 * X_DIMENSION, Y_DIMENSION, Influence STATUS
 * ... subsequent rows for each set of simulation parameters.
 *
 * @param defect_infdom The defect influence domain to be written. It contains a mapping from sets of simulation
 * parameters (represented as a pair of sweep parameters for the X and Y dimensions) to their influence status.
 * @param os The output stream where the CSV representation of the defect influence domain is written to.
 * @param params The parameters used for writing, including the influential and non-influential tags. Defaults to an
 * empty `write_defect_operational_domain_params` object, which provides standard tags.
 */
template <typename Lyt>
inline void write_defect_operational_domain(const defect_influence_domain<Lyt>& defect_infdom, std::ostream& os,
                                            const write_defect_operational_domain_params& params = {})
{
    csv_writer writer{os};

    writer.write_line("x", "y", "operational status");

    for (const auto& [sim_param, op_val] : defect_infdom.influence_information)
    {
        writer.write_line(sim_param.x, sim_param.y,
                          op_val == defect_influence_status::EXISTENT ? params.influential_tag :
                                                                        params.non_influential_tag);
    }
}
/**
 * Writes a CSV representation of an operational domain to the specified file. The data are written
 * as rows, each corresponding to one set of simulation parameters and their corresponding influence status.
 *
 * The output CSV format is as follows:
 * X_DIMENSION, Y_DIMENSION, Influence STATUS
 * ... subsequent rows for each set of simulation parameters.
 *
 * @param defect_infdom The defect influence domain to be written. It contains a mapping from sets of simulation
 * parameters (represented as a pair of sweep parameters for the X and Y dimensions) to their influence status.
 * @param filename The filename where the CSV representation of the operational domain is written to.
 * @param params The parameters used for writing, including the influential and non-influential tags. Defaults to an
 * empty `write_defect_operational_domain_params` object, which provides standard tags.
 */
template <typename Lyt>
inline void write_defect_operational_domain(const defect_influence_domain<Lyt>&           defect_opdom,
                                            const std::string_view&                       filename,
                                            const write_defect_operational_domain_params& params = {})
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_defect_operational_domain(defect_opdom, os, params);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_WRITE_DEFECT_INFLUENCE_DOMAIN_HPP

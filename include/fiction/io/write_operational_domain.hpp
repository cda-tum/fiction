//
// Created by marcel on 02.08.23.
//

#ifndef FICTION_WRITE_OPERATIONAL_DOMAIN_HPP
#define FICTION_WRITE_OPERATIONAL_DOMAIN_HPP

#include "fiction/algorithms/simulation/sidb/is_operational.hpp"
#include "fiction/algorithms/simulation/sidb/operational_domain.hpp"
#include "fiction/io/csv_writer.hpp"

#include <fstream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

namespace fiction
{

/**
 * Parameters for writing an operational domain to a CSV file.
 */
struct write_operational_domain_params
{
    /**
     * The tag used to represent the operational value of a parameter set.
     */
    std::string_view operational_tag = "1";
    /**
     * The tag used to represent the non-operational value of a parameter set.
     */
    std::string_view non_operational_tag = "0";
    /**
     * Whether to write non-operational samples to the CSV file. If set to `false`, only operational samples are
     * written. This yields a significantly smaller CSV file. It is recommended to set this option for 3D plots because
     * the non-operational samples would shadow the operational samples anyway.
     */
    bool write_non_operational_samples = true;
};

namespace detail
{

/**
 * Converts a sweep parameter to a string representation. This is used to write the parameter name to the CSV file.
 *
 * @param param The sweep parameter to be converted.
 * @return The string representation of the sweep parameter.
 */
[[nodiscard]] static inline std::string
sweep_parameter_to_string(const operational_domain::sweep_parameter& param) noexcept
{
    switch (param)
    {
        case operational_domain::sweep_parameter::EPSILON_R:
        {
            return "epsilon_r";
        }
        case operational_domain::sweep_parameter::LAMBDA_TF:
        {
            return "lambda_tf";
        }
        case operational_domain::sweep_parameter::MU_MINUS:
        {
            return "mu_minus";
        }
    }

    return "";
}

}  // namespace detail

/**
 * Writes a CSV representation of an operational domain to the specified output stream. The data are written
 * as rows, each corresponding to one set of simulation parameters and their corresponding operational status.
 *
 * The output CSV format is e.g. as follows:
    \verbatim embed:rst
    .. code-block:: RST

     epsilon_r, lambda_tf, operational status
     0.0, 0.0, 0
     0.1, 0.0, 1
     ... subsequent rows for each set of simulation parameters
    \endverbatim
 *
 * The operational status is a binary value represented by specified tags in `params` indicating whether the simulation
 * parameters are within the operational domain or not.
 *
 * @param opdom The operational domain to be written. It contains a mapping from sets of simulation parameters
 * (represented as a pair of sweep parameters for the X and Y dimensions) to their operational status.
 * @param os The output stream where the CSV representation of the operational domain is written to.
 * @param params The parameters used for writing, including the operational and non-operational tags. Defaults to an
 * empty `write_operational_domain_params` object, which provides standard tags.
 */
inline void write_operational_domain(const operational_domain& opdom, std::ostream& os,
                                     const write_operational_domain_params& params = {})
{
    csv_writer writer{os};

    const auto num_dimensions = opdom.dimensions.size();

    if (num_dimensions == 0 || num_dimensions > 3)
    {
        throw std::invalid_argument("unsupported number of dimensions in the given operational domain");
    }

    if (num_dimensions == 1)
    {
        writer.write_line(detail::sweep_parameter_to_string(opdom.dimensions[0]), "operational status");
    }
    else if (num_dimensions == 2)
    {
        writer.write_line(detail::sweep_parameter_to_string(opdom.dimensions[0]),
                          detail::sweep_parameter_to_string(opdom.dimensions[1]), "operational status");
    }
    else  // num_dimensions == 3
    {
        writer.write_line(detail::sweep_parameter_to_string(opdom.dimensions[0]),
                          detail::sweep_parameter_to_string(opdom.dimensions[1]),
                          detail::sweep_parameter_to_string(opdom.dimensions[2]), "operational status");
    }

    for (const auto& [sim_param, op_val] : opdom.operational_values)
    {
        // skip non-operational samples if the respective flag is set
        if (!params.write_non_operational_samples && op_val == operational_status::NON_OPERATIONAL)
        {
            continue;
        }

        const auto tag =
            op_val == operational_status::OPERATIONAL ? params.operational_tag : params.non_operational_tag;

        if (num_dimensions == 1)
        {
            writer.write_line(sim_param.parameters[0], tag);
        }
        else if (num_dimensions == 2)
        {
            writer.write_line(sim_param.parameters[0], sim_param.parameters[1], tag);
        }
        else  // num_dimensions == 3
        {
            writer.write_line(sim_param.parameters[0], sim_param.parameters[1], sim_param.parameters[2], tag);
        }
    }
}
/**
 * Writes a CSV representation of an operational domain to the specified file. The data are written as rows, each
 * corresponding to one set of simulation parameters and their corresponding operational status.
 *
 * The output CSV format is e.g. as follows:
    \verbatim embed:rst
    .. code-block:: RST

     epsilon_r, lambda_tf, operational status
     0.0, 0.0, 0
     0.1, 0.0, 1
     ... subsequent rows for each set of simulation parameters
    \endverbatim
 *
 * The operational status is a binary value represented by specified tags in `params` indicating whether the simulation
 * parameters are within the operational domain or not.
 *
 * @param opdom The operational domain to be written. It contains a mapping from sets of simulation parameters
 * (represented as a pair of sweep parameters for the X and Y dimensions) to their operational status.
 * @param filename The filename where the CSV representation of the operational domain is written to.
 * @param params The parameters used for writing, including the operational and non-operational tags. Defaults to an
 * empty `write_operational_domain_params` object, which provides standard tags.
 */
inline void write_operational_domain(const operational_domain& opdom, const std::string_view& filename,
                                     const write_operational_domain_params& params = {})
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_operational_domain(opdom, os, params);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_WRITE_OPERATIONAL_DOMAIN_HPP

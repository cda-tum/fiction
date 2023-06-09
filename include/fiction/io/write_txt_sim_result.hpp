//
// Created by Jan Drewniok on 09.06.23.
//

#ifndef FICTION_WRITE_TXT_SIM_RESULT_HPP
#define FICTION_WRITE_TXT_SIM_RESULT_HPP

#include "fiction/algorithms/simulation/sidb/exhaustive_ground_state_simulation.hpp"
#include "fiction/algorithms/simulation/sidb/minimum_energy.hpp"
#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/io/read_sqd_layout.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/math_utils.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

namespace fiction
{

namespace detail
{

template <typename Lyt>
class write_txt_sim_result_impl
{
  public:
    write_txt_sim_result_impl(const sidb_simulation_result<Lyt>& result, std::ostream& s) :

            sim_result{result},
            os{s}

    {}

    void run()
    {
        auto min_energy =
            round_to_n_decimal_places(minimum_energy<sidb_cell_clk_lyt_siqad>(sim_result.charge_distributions), 6);

        std::vector<charge_distribution_surface<sidb_cell_clk_lyt_siqad>> ground_state_layouts{};
        for (const auto& valid_layout : sim_result.charge_distributions)
        {
            if (round_to_n_decimal_places(valid_layout.get_system_energy(), 6) == min_energy)
            {
                ground_state_layouts.emplace_back(charge_distribution_surface<sidb_cell_clk_lyt_siqad>{valid_layout});
            }
        }
        if (!ground_state_layouts.empty())
        {

            // Set the floating-point precision for the output file
            os << std::fixed << std::setprecision(3);

            // Write the column headers
            os << "x [nm];"
               << "y [nm];";

            for (uint64_t i = 0; i < ground_state_layouts.size(); i++)
            {
                os << "GS_" << std::to_string(i) << ";";
            }
            os << std::endl;

            auto       sidbs              = ground_state_layouts.front().get_all_sidb_cells();
            const auto physical_parameter = ground_state_layouts.front().get_phys_params();

            std::sort(sidbs.begin(), sidbs.end());
            for (const auto& sidb : sidbs)
            {
                auto pos = sidb_nm_position<sidb_cell_clk_lyt_siqad>(physical_parameter, sidb);
                os << pos.first << ";" << pos.second << ";";
                for (const auto& valid_layout : ground_state_layouts)
                {
                    os << std::to_string(charge_state_to_sign(valid_layout.get_charge_state(sidb))) << ";";
                }
                os << std::endl;
            }
        }
    };

  private:
    /**
     * Simulation results.
     */
    sidb_simulation_result<Lyt> sim_result;
    /**
     * Output stream used for writing the simulation result.
     */
    std::ostream& os;
};

}  // namespace detail

/**
 * Writes the coordinates of all SiDBs of a layout together with the charge distribution of the ground state(s) into a
 * file.
 *
 * This overload uses an output stream to write into.
 *
 * @tparam Lyt Cell-level SiDB layout type.
 * @param sim_result The simulation result to write.
 * @param os The output stream to write into.
 */
template <typename Lyt>
void write_txt_sim_result(const sidb_simulation_result<Lyt>& sim_result, std::ostream& os)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");

    detail::write_txt_sim_result_impl p{sim_result, os};

    p.run();
}
/**
 * Writes the coordinates of all SiDBs of a layout together with the charge distribution of the ground state(s) into a
 * file.
 *
 * This overload uses a file name to create and write into.
 *
 * @tparam sim_result Simulation result of physical simulation.
 * @param filename The file name to create and write into. Should preferably use the `.sqd` extension.
 */
template <typename Lyt>
void write_txt_sim_result(const sidb_simulation_result<Lyt>& sim_result, const std::string_view& filename)
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_txt_sim_result(sim_result, os);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_WRITE_TXT_SIM_RESULT_HPP

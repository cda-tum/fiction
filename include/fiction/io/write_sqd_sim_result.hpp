//
// Created by marcel on 04.04.23.
//

#ifndef FICTION_WRITE_SQD_SIM_RESULT_HPP
#define FICTION_WRITE_SQD_SIM_RESULT_HPP

#include "fiction/algorithms/simulation/sidb/sidb_simulation_result.hpp"
#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/charge_distribution_surface.hpp"
#include "fiction/technology/sidb_charge_state.hpp"
#include "fiction/technology/sidb_lattice_orientations.hpp"
#include "fiction/technology/sidb_nm_position.hpp"
#include "fiction/traits.hpp"
#include "utils/version_info.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <algorithm>
#include <any>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <functional>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace fiction
{

namespace detail
{

namespace siqad
{

inline constexpr const char* XML_HEADER    = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
inline constexpr const char* OPEN_SIM_OUT  = "<sim_out>\n";
inline constexpr const char* CLOSE_SIM_OUT = "</sim_out>\n";

inline constexpr const char* ENG_INFO_BLOCK = "    <eng_info>\n"
                                              "        <engine>{}</engine>\n"
                                              "        <version>{}</version>\n"
                                              "        <repo>{}</repo>\n"
                                              "        <return_code>{}</return_code>\n"
                                              "        <timestamp>{}</timestamp>\n"
                                              "        <time_elapsed_s>{}</time_elapsed_s>\n"
                                              "    </eng_info>\n";

inline constexpr const char* OPEN_SIM_PARAMS  = "    <sim_params>\n";
inline constexpr const char* CLOSE_SIM_PARAMS = "    </sim_params>\n";

inline constexpr const char* PHYS_SIM_PARAMS = "        <debye_length>{}</debye_length>\n"
                                               "        <eps_r>{}</eps_r>\n"
                                               "        <muzm>{}</muzm>\n";

inline constexpr const char* ADD_SIM_PARAM = "        <{}>{}</{}>\n";

inline constexpr const char* OPEN_PHYSLOC  = "    <physloc>\n";
inline constexpr const char* CLOSE_PHYSLOC = "    </physloc>\n";

inline constexpr const char* DBDOT = "        <dbdot x=\"{:.6f}\" y=\"{:.6f}\"/>\n";

inline constexpr const char* OPEN_ELEC_DIST  = "    <elec_dist>\n";
inline constexpr const char* CLOSE_ELEC_DIST = "    </elec_dist>\n";

inline constexpr const char* DIST_ENERGY =
    "        <dist energy=\"{:.6f}\" count=\"{}\" physically_valid=\"{}\" state_count=\"{}\">{}</dist>\n";

}  // namespace siqad

/**
 * Converts an `std::any` to a string if it contains an alpha-numerical standard data type.
 *
 * @param value The value to convert.
 * @return The string representation of the value.
 */
[[nodiscard]] inline std::string any_to_string(const std::any& value) noexcept
{
    static const std::unordered_map<std::type_index, std::function<std::string(const std::any&)>> converters{
        {typeid(int8_t), [](const std::any& v) { return std::to_string(std::any_cast<int8_t>(v)); }},
        {typeid(uint8_t), [](const std::any& v) { return std::to_string(std::any_cast<uint8_t>(v)); }},
        {typeid(int16_t), [](const std::any& v) { return std::to_string(std::any_cast<int16_t>(v)); }},
        {typeid(uint16_t), [](const std::any& v) { return std::to_string(std::any_cast<uint16_t>(v)); }},
        {typeid(int32_t), [](const std::any& v) { return std::to_string(std::any_cast<int32_t>(v)); }},
        {typeid(uint32_t), [](const std::any& v) { return std::to_string(std::any_cast<uint32_t>(v)); }},
        {typeid(int64_t), [](const std::any& v) { return std::to_string(std::any_cast<int64_t>(v)); }},
        {typeid(uint64_t), [](const std::any& v) { return std::to_string(std::any_cast<uint64_t>(v)); }},
        {typeid(float), [](const std::any& v) { return std::to_string(std::any_cast<float>(v)); }},
        {typeid(double), [](const std::any& v) { return std::to_string(std::any_cast<double>(v)); }},
        {typeid(long double), [](const std::any& v) { return std::to_string(std::any_cast<long double>(v)); }},
        {typeid(std::string), [](const std::any& v) { return std::any_cast<std::string>(v); }},
        {typeid(const char*), [](const std::any& v) { return std::string(std::any_cast<const char*>(v)); }},
        {typeid(char), [](const std::any& v) { return std::string(1, std::any_cast<char>(v)); }},
    };

    const auto& type = value.type();
    const auto& it   = converters.find(type);

    return it == converters.end() ? std::string() : it->second(value);
}

template <typename Lyt>
class write_sqd_sim_result_impl
{
  public:
    write_sqd_sim_result_impl(const sidb_simulation_result<Lyt>& result, std::ostream& s) :
            sim_result{result},
            os{s},
            ordered_cells{obtain_ordered_cells()}
    {}

    void run()
    {
        os << siqad::XML_HEADER << siqad::OPEN_SIM_OUT;

        write_engine_info();

        write_simulation_parameters();

        write_physical_locations();

        write_electron_distributions();

        os << siqad::CLOSE_SIM_OUT;
    }

  private:
    /**
     * The simulation result to write.
     */
    const sidb_simulation_result<Lyt>& sim_result;
    /**
     * The output stream to write to.
     */
    std::ostream& os;
    /**
     * A reproducible ordering of cells in the surfaces.
     */
    const std::vector<cell<Lyt>> ordered_cells;

    /**
     * Obtains all cells in the surfaces and orders them by their position to achieve a reproducible output.
     *
     * @return An ordering of cells.
     */
    std::vector<cell<Lyt>> obtain_ordered_cells() const noexcept
    {
        std::vector<cell<Lyt>> cells{};

        if (sim_result.charge_distributions.empty())
        {
            return cells;
        }

        // take the first distribution as the reference layout
        const auto& lyt = sim_result.charge_distributions.front();

        cells.reserve(lyt.num_cells());

        // obtain all cells in the surfaces and order them by their position to achieve a reproducible output
        lyt.foreach_cell([&cells](const cell<Lyt>& c) { cells.push_back(c); });

        // sort the cells by their position using their respective operator<
        std::sort(cells.begin(), cells.end());

        return cells;
    }

    /**
     * Writes the engine information to the output stream in XML format.
     */
    void write_engine_info()
    {
        os << fmt::format(siqad::ENG_INFO_BLOCK, sim_result.algorithm_name, FICTION_VERSION, FICTION_REPO, 0,
                          fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr))),
                          sim_result.simulation_runtime.count());
    }

    /**
     * Writes both the `physical_parameters` as well as the `additional_simulation_parameters` to the output stream in
     * XML format.
     */
    void write_simulation_parameters()
    {
        os << siqad::OPEN_SIM_PARAMS;

        // physical parameters
        os << fmt::format(siqad::PHYS_SIM_PARAMS, sim_result.physical_parameters.lambda_tf,
                          sim_result.physical_parameters.epsilon_r, sim_result.physical_parameters.mu_minus);

        // additional simulation parameters
        std::for_each(sim_result.additional_simulation_parameters.cbegin(),
                      sim_result.additional_simulation_parameters.cend(),
                      [this](const auto& p)
                      {
                          const auto& [name, value] = p;

                          if (value.has_value())
                          {
                              os << fmt::format(siqad::ADD_SIM_PARAM, name, any_to_string(value), name);
                          }
                      });

        os << siqad::CLOSE_SIM_PARAMS;
    }

    /**
     * Writes the physical locations of all SiDBs to the output stream in XML format.
     */
    void write_physical_locations()
    {
        os << siqad::OPEN_PHYSLOC;

        std::for_each(ordered_cells.cbegin(), ordered_cells.cend(),
                      [this](const auto& c)
                      {
                          const auto [nm_x, nm_y] = sidb_nm_position<Lyt>(c);
                          os << fmt::format(siqad::DBDOT, nm_x * 10,
                                            nm_y * 10);  // convert nm to Angstrom
                      });

        os << siqad::CLOSE_PHYSLOC;
    }

    /**
     * Writes all charge distributions to the output stream in XML format.
     */
    void write_electron_distributions()
    {
        os << siqad::OPEN_ELEC_DIST;

        // a vector of pointers to avoid copying the surfaces (use raw pointers at your own risk, kids!)
        std::vector<const charge_distribution_surface<Lyt>*> ordered_surface_pointers{};
        ordered_surface_pointers.reserve(sim_result.charge_distributions.size());

        // obtain pointers to all the surfaces
        std::for_each(sim_result.charge_distributions.cbegin(), sim_result.charge_distributions.cend(),
                      [&ordered_surface_pointers](const auto& surface)
                      { ordered_surface_pointers.push_back(&surface); });

        // sort the surface references by their system energy
        std::sort(ordered_surface_pointers.begin(), ordered_surface_pointers.end(),
                  [](const auto& a, const auto& b) { return a->get_system_energy() < b->get_system_energy(); });

        // write the distributions to the output stream
        std::for_each(
            ordered_surface_pointers.cbegin(), ordered_surface_pointers.cend(),
            [this](const auto& surface)
            {
                // obtain the charges in the same order as the cells
                std::vector<sidb_charge_state> ordered_charges{};
                ordered_charges.reserve(ordered_cells.size());

                std::for_each(ordered_cells.cbegin(), ordered_cells.cend(),
                              [&ordered_charges, &surface](const auto& c)
                              { ordered_charges.push_back(surface->get_charge_state(c)); });

                os << fmt::format(
                    siqad::DIST_ENERGY,
                    surface->get_system_energy(),            // system energy
                    1,                                       // occurrence count
                    surface->is_physically_valid() ? 1 : 0,  // physical validity
                    3,  // simulation state count (fixed to 3 since state count = 2 is not supported by SiQAD yet).
                    charge_configuration_to_string(ordered_charges)  // charge distribution as a string
                );
            });

        os << siqad::CLOSE_ELEC_DIST;
    }
};

}  // namespace detail

/**
 * Writes an SiDB simulation result to an XML file that is used by SiQAD (https://github.com/siqad/siqad), a physical
 * simulator for the SiDB technology platform.
 *
 * This overload uses an output stream to write into.
 *
 * @tparam Lyt Cell-level SiDB layout type.
 * @param sim_result The simulation result to write.
 * @param os The output stream to write into.
 */
template <typename Lyt>
void write_sqd_sim_result(const sidb_simulation_result<Lyt>& sim_result, std::ostream& os)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");

    detail::write_sqd_sim_result_impl p{sim_result, os};

    p.run();
}

/**
 * Writes an SiDB simulation result to an XML file that is used by SiQAD (https://github.com/siqad/siqad), a physical
 * simulator for the SiDB technology platform.
 *
 * This overload uses a file name to create and write into.
 *
 * @tparam Lyt Cell-level SiDB layout type.
 * @param sim_result The simulation result to write.
 * @param filename The file name to create and write into. Should preferably use the `.xml` extension.
 */
template <typename Lyt>
void write_sqd_sim_result(const sidb_simulation_result<Lyt>& sim_result, const std::string_view& filename)
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_sqd_sim_result(sim_result, os);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_WRITE_SQD_SIM_RESULT_HPP

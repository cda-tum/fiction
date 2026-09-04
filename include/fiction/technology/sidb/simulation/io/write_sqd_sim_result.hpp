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
 * @brief Writer for SiDB simulation results in SiQAD's XML format.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/technology/sidb/charge_distribution.hpp"
#include "fiction/technology/sidb/model/charge_state.hpp"
#include "fiction/technology/sidb/simulation/result.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/stl/stl_utils.hpp"
#include "fiction/utils/version_info.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>

#include <algorithm>
#include <any>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <functional>
#include <ostream>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace fiction::sidb::simulation::io
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
        // `long double` isn't portable across platforms, but this map must round-trip whatever type was actually
        // stored in the `std::any`, and callers are free to pass a `long double`.
        // NOLINTNEXTLINE(google-runtime-float)
        {typeid(long double), [](const std::any& v) { return std::to_string(std::any_cast<long double>(v)); }},
        {typeid(std::string), [](const std::any& v) { return std::any_cast<std::string>(v); }},
        {typeid(const char*), [](const std::any& v) { return std::string(std::any_cast<const char*>(v)); }},
        {typeid(char), [](const std::any& v) { return std::string(1, std::any_cast<char>(v)); }},
    };

    const auto& type = value.type();
    const auto& it   = converters.find(type);

    return it == converters.end() ? std::string() : it->second(value);
}

/**
 * Writes a `result` as a SiQAD simulation result file: engine info, physical parameters, the SiDB positions in
 * ångström, and every charge distribution sorted by energy.
 */
class sqd_sim_result_writer
{
  public:
    sqd_sim_result_writer(const sidb::simulation::result& src, std::ostream& s) : sim_result{src}, os{s} {}

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
    const sidb::simulation::result& sim_result;
    std::ostream&                   os;

    void write_engine_info()
    {
        const auto current_time = std::time(nullptr);
        os << fmt::format(siqad::ENG_INFO_BLOCK, sim_result.algorithm_name, FICTION_VERSION, FICTION_REPO, 0,
                          fmt::format("{:%Y-%m-%d %H:%M:%S}", fiction::utils::stl::safe_localtime(current_time)),
                          sim_result.simulation_runtime.count());
    }

    void write_simulation_parameters()
    {
        os << siqad::OPEN_SIM_PARAMS;

        os << fmt::format(siqad::PHYS_SIM_PARAMS, sim_result.sim_params.lambda_tf, sim_result.sim_params.epsilon_r,
                          sim_result.sim_params.mu_minus);

        for (const auto& [name, value] : sim_result.additional_simulation_parameters)
        {
            if (value.has_value())
            {
                os << fmt::format(siqad::ADD_SIM_PARAM, name, any_to_string(value), name);
            }
        }

        os << siqad::CLOSE_SIM_PARAMS;
    }

    void write_physical_locations()
    {
        os << siqad::OPEN_PHYSLOC;

        for (const auto& s : sim_result.lyt.sidbs())
        {
            const auto [nm_x, nm_y] = sim_result.lyt.get_lattice().nm_position(s);
            os << fmt::format(siqad::DBDOT, nm_x * 10, nm_y * 10);  // convert nm to Angstrom
        }

        os << siqad::CLOSE_PHYSLOC;
    }

    void write_electron_distributions()
    {
        os << siqad::OPEN_ELEC_DIST;

        std::vector<const charge_distribution*> ordered{};
        ordered.reserve(sim_result.charge_distributions.size());

        for (const auto& cd : sim_result.charge_distributions)
        {
            ordered.push_back(&cd);
        }

        std::ranges::sort(ordered, [](const auto* a, const auto* b) { return a->energy() < b->energy(); });

        for (const auto* cd : ordered)
        {
            os << fmt::format(
                siqad::DIST_ENERGY,
                cd->energy(),  // system energy
                1,             // occurrence count
                1,             // physical validity: a result only holds valid distributions
                3,  // simulation state count (fixed to 3 since state count = 2 is not supported by SiQAD yet).
                sidb::model::charge_configuration_to_string(cd->charge_states())  // charge distribution
            );
        }

        os << siqad::CLOSE_ELEC_DIST;
    }
};

}  // namespace detail

/**
 * Writes a simulation result as a SiQAD simulation result file to a stream.
 *
 * @param sim_result Result to write.
 * @param os Output stream to write into.
 */
inline void write_sqd_sim_result(const sidb::simulation::result& sim_result, std::ostream& os)
{
    detail::sqd_sim_result_writer p{sim_result, os};

    p.run();
}
/**
 * Writes a simulation result as a SiQAD simulation result file.
 *
 * @param sim_result Result to write.
 * @param filename File to write into.
 * @throws std::ofstream::failure if the file cannot be opened.
 */
inline void write_sqd_sim_result(const sidb::simulation::result& sim_result, const std::string_view& filename)
{
    std::ofstream os{std::string{filename}, std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_sqd_sim_result(sim_result, os);
    os.close();
}

}  // namespace fiction::sidb::simulation::io

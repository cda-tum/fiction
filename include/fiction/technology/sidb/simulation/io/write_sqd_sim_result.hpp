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
#include "fiction/utils/atomic_write.hpp"
#include "fiction/utils/io/xml_character.hpp"
#include "fiction/utils/stl/stl_utils.hpp"
#include "fiction/utils/version_info.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <tinyxml2.h>

#include <algorithm>
#include <any>
#include <compare>
#include <cstdint>
#include <ctime>
#include <functional>
#include <ostream>
#include <stdexcept>
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
 * @throws std::invalid_argument If the value contains a null C string.
 */
[[nodiscard]] inline std::string any_to_string(const std::any& value)
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
        {typeid(const char*),
         [](const std::any& v)
         {
             const auto* text = std::any_cast<const char*>(v);
             if (text == nullptr)
             {
                 throw std::invalid_argument("SiQAD metadata requires a non-null C string");
             }
             return std::string{text};
         }},
        {typeid(char), [](const std::any& v) { return std::string(1, std::any_cast<char>(v)); }},
    };

    const auto& type = value.type();
    const auto& it   = converters.find(type);

    return it == converters.end() ? std::string() : it->second(value);
}

/**
 * @brief Whether a character can start an XML 1.0 name.
 * @param character Decoded Unicode character.
 * @return Whether the character matches NameStartChar.
 * @see https://www.w3.org/TR/xml/#NT-NameStartChar
 */
[[nodiscard]] inline bool xml_name_start(const uint32_t character) noexcept
{
    return character == ':' || character == '_' || (character >= 'A' && character <= 'Z') ||
           (character >= 'a' && character <= 'z') || (character >= 0xc0 && character <= 0xd6) ||
           (character >= 0xd8 && character <= 0xf6) || (character >= 0xf8 && character <= 0x2ff) ||
           (character >= 0x370 && character <= 0x37d) || (character >= 0x37f && character <= 0x1fff) ||
           (character >= 0x200c && character <= 0x200d) || (character >= 0x2070 && character <= 0x218f) ||
           (character >= 0x2c00 && character <= 0x2fef) || (character >= 0x3001 && character <= 0xd7ff) ||
           (character >= 0xf900 && character <= 0xfdcf) || (character >= 0xfdf0 && character <= 0xfffd) ||
           (character >= 0x10000 && character <= 0xeffff);
}

/**
 * @brief Validate an XML 1.0 name encoded as UTF-8.
 * @param name Parameter element name.
 * @throws std::invalid_argument If the name, UTF-8 encoding, or XML characters are invalid.
 * @see https://www.w3.org/TR/xml/#NT-Name
 */
inline void validate_xml_name(std::string_view name)
{
    if (name.empty() || !xml_name_start(utils::io::detail::next_xml_character(name)))
    {
        throw std::invalid_argument("SiQAD parameter names require XML name characters");
    }
    while (!name.empty())
    {
        const auto character = utils::io::detail::next_xml_character(name);
        if (!xml_name_start(character) && character != '-' && character != '.' &&
            (character < '0' || character > '9') && character != 0xb7 && (character < 0x300 || character > 0x36f) &&
            (character < 0x203f || character > 0x2040))
        {
            throw std::invalid_argument("SiQAD parameter names require XML name characters");
        }
    }
}

/**
 * @brief Escape XML element text and preserve carriage returns when parsed.
 * @param value Text to escape.
 * @return Escaped XML text.
 * @throws std::invalid_argument If the text contains invalid UTF-8 or an illegal XML 1.0 character.
 */
[[nodiscard]] inline std::string xml_text(const std::string& value)
{
    std::string_view remaining{value};
    while (!remaining.empty())
    {
        static_cast<void>(utils::io::detail::next_xml_character(remaining));
    }
    tinyxml2::XMLPrinter printer{};
    printer.PushText(value.c_str());
    std::string text{};
    text.reserve(value.size());
    for (const auto character : std::string_view{printer.CStr()})
    {
        if (character == '\r')
        {
            text.append("&#13;");
        }
        else
        {
            text.push_back(character);
        }
    }
    return text;
}

/**
 * Writes a `result` as a SiQAD simulation result file: engine info, physical parameters, the SiDB positions in
 * ångström, and every charge distribution sorted by energy.
 */
class sqd_sim_result_writer
{
  public:
    /**
     * Creates a SiQAD simulation-result writer.
     *
     * @param src Simulation result.
     * @param s Output stream.
     */
    sqd_sim_result_writer(const sidb::simulation::result& src, std::ostream& s) : sim_result{src}, os{s} {}

    /**
     * Writes the complete SiQAD simulation-result document.
     *
     * @throws std::invalid_argument If a distribution has different sites from the result layout, a parameter name
     * contains invalid XML name characters, or metadata contains invalid UTF-8, an illegal XML character, or a null
     * C string.
     */
    void run()
    {
        for (const auto& cd : sim_result.charge_distributions)
        {
            if (cd.sites() != sim_result.lyt.sidbs())
            {
                throw std::invalid_argument("Charge distribution sites do not match the result layout");
            }
        }
        const auto engine_info = create_engine_info();
        const auto parameters  = create_simulation_parameters();
        os << siqad::XML_HEADER << siqad::OPEN_SIM_OUT << engine_info << parameters;

        write_physical_locations();
        write_electron_distributions();

        os << siqad::CLOSE_SIM_OUT;
    }

  private:
    /**
     * Simulation result to write.
     */
    const sidb::simulation::result& sim_result;
    /**
     * Output stream.
     */
    std::ostream& os;

    /**
     * Creates the engine name, version, and runtime XML fragment.
     * @return Engine metadata with escaped text.
     */
    [[nodiscard]] std::string create_engine_info() const
    {
        const auto current_time = std::time(nullptr);
        return fmt::format(siqad::ENG_INFO_BLOCK, xml_text(sim_result.algorithm_name), FICTION_VERSION, FICTION_REPO, 0,
                           fmt::format("{:%Y-%m-%d %H:%M:%S}", fiction::utils::stl::safe_localtime(current_time)),
                           sim_result.simulation_runtime.count());
    }

    /**
     * Creates the physical and additional simulation parameter XML fragment.
     * @return Simulation parameters with valid element names and escaped text.
     */
    [[nodiscard]] std::string create_simulation_parameters() const
    {
        std::string parameters{siqad::OPEN_SIM_PARAMS};

        parameters.append(fmt::format(siqad::PHYS_SIM_PARAMS, sim_result.sim_params.lambda_tf,
                                      sim_result.sim_params.epsilon_r, sim_result.sim_params.mu_minus));

        for (const auto& [name, value] : sim_result.additional_simulation_parameters)
        {
            if (value.has_value())
            {
                validate_xml_name(name);
                parameters.append(fmt::format(siqad::ADD_SIM_PARAM, name, xml_text(any_to_string(value)), name));
            }
        }

        parameters.append(siqad::CLOSE_SIM_PARAMS);
        return parameters;
    }

    /**
     * Writes the SiDB positions in ångström.
     */
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

    /**
     * Writes the charge distributions in energy order.
     */
    void write_electron_distributions()
    {
        os << siqad::OPEN_ELEC_DIST;

        std::vector<const charge_distribution*> ordered{};
        ordered.reserve(sim_result.charge_distributions.size());

        for (const auto& cd : sim_result.charge_distributions)
        {
            ordered.push_back(&cd);
        }

        // Floating-point < does not order NaN energies.
        std::ranges::sort(ordered,
                          [](const auto* a, const auto* b) { return std::weak_order(a->energy(), b->energy()) < 0; });

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
 * Writes a simulation result as a SiQAD simulation result file to a stream. Metadata text is XML-escaped, and
 * additional parameter names use XML 1.0 name characters. Metadata uses UTF-8. Invalid input leaves the stream
 * unchanged. Charge distributions follow `std::weak_order` on their energies. Equivalent energies have no
 * specified relative order.
 *
 * @param sim_result Result to write.
 * @param os Output stream to write into.
 * @throws std::invalid_argument If a distribution has different sites from the result layout, a parameter name
 * contains invalid XML name characters, or metadata contains invalid UTF-8, an illegal XML character, or a null
 * C string.
 */
inline void write_sqd_sim_result(const sidb::simulation::result& sim_result, std::ostream& os)
{
    detail::sqd_sim_result_writer p{sim_result, os};

    p.run();
}
/**
 * Writes a simulation result as a SiQAD simulation result file. Invalid input preserves an existing file.
 * Charge distributions follow `std::weak_order` on their energies. Equivalent energies have no specified relative
 * order.
 *
 * @param sim_result Result to write.
 * @param filename File to write into.
 * @throws std::ios_base::failure If creating, writing, closing, or replacing the output file fails.
 * @throws std::invalid_argument If a distribution has different sites from the result layout, a parameter name
 * contains invalid XML name characters, or metadata contains invalid UTF-8, an illegal XML character, or a null
 * C string.
 */
inline void write_sqd_sim_result(const sidb::simulation::result& sim_result, const std::string_view& filename)
{
    fiction::detail::atomic_write(filename, [&](std::ostream& os) { write_sqd_sim_result(sim_result, os); });
}

}  // namespace fiction::sidb::simulation::io

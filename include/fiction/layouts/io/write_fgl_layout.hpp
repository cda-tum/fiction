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
 * @brief Writer for gate-level layouts in the FGL file format.
 * @author Simon Hofmann (simon1hofmann)
 * @author Marcel Walter (marcelwa)
 */

#pragma once

#include "fiction/layouts/arrangement.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/atomic_write.hpp"
#include "fiction/utils/progress.hpp"
#include "fiction/utils/stl/stl_utils.hpp"
#include "fiction/utils/version_info.hpp"
#include "fiction/verification/design_rule_violations.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <kitty/print.hpp>
#include <tinyxml2.h>

#include <cstddef>
#include <cstdint>
#include <ctime>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace fiction::layouts::io
{

namespace detail
{

namespace fgl
{

/**
 * @brief Escape user-provided text for an XML element.
 * @param value Layout or port name.
 * @return XML text preserving the original label when parsed.
 */
inline std::string xml_text(const std::string& value)
{
    tinyxml2::XMLPrinter printer{};
    printer.PushText(value.c_str());
    return printer.CStr();
}

/** @brief FGL XML fragment. */
inline constexpr const char* FGL_HEADER = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* OPEN_FGL = "<fgl version=\"2\">\n";
/** @brief FGL XML fragment. */
inline constexpr const char* CLOSE_FGL = "</fgl>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* FICTION_METADATA = "  <fiction>\n"
                                                "    <fiction_version>{}</fiction_version>\n"
                                                "    <available_at>{}</available_at>\n"
                                                "    <date>{}</date>\n"
                                                "  </fiction>\n";

/** @brief FGL XML fragment. */
inline constexpr const char* OPEN_LAYOUT_METADATA = "  <layout>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* CLOSE_LAYOUT_METADATA = "  </layout>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* LAYOUT_METADATA = "    <name>{}</name>\n"
                                               "    <topology>{}</topology>\n"
                                               "    <size>\n"
                                               "      <x>{}</x>\n"
                                               "      <y>{}</y>\n"
                                               "      <z>{}</z>\n"
                                               "    </size>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* OPEN_CLOCKING = "    <clocking>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* CLOSE_CLOCKING = "    </clocking>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* CLOCKING_SCHEME_NAME = "      <name>{}</name>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* OPEN_CLOCK_ZONES = "      <zones>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* CLOSE_CLOCK_ZONES = "      </zones>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* CLOCK_ZONE = "        <zone>\n"
                                          "          <x>{}</x>\n"
                                          "          <y>{}</y>\n"
                                          "          <clock>{}</clock>\n"
                                          "        </zone>\n";

/** @brief FGL XML fragment. */
inline constexpr const char* OPEN_GATES = "  <gates>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* CLOSE_GATES = "  </gates>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* OPEN_GATE = "    <gate>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* CLOSE_GATE = "    </gate>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* GATE = "      <id>{}</id>\n"
                                    "      <type>{}</type>\n"
                                    "      <name>{}</name>\n"
                                    "      <loc>\n"
                                    "        <x>{}</x>\n"
                                    "        <y>{}</y>\n"
                                    "        <z>{}</z>\n"
                                    "      </loc>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* OPEN_INCOMING = "      <incoming>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* CLOSE_INCOMING = "      </incoming>\n";
/** @brief FGL XML fragment. */
inline constexpr const char* SIGNAL = "        <signal>\n"
                                      "          <x>{}</x>\n"
                                      "          <y>{}</y>\n"
                                      "          <z>{}</z>\n"
                                      "        </signal>\n";

}  // namespace fgl

/** @brief Serialize a finished layout. @tparam Lyt Gate-level layout type. */
template <typename Lyt>
class write_fgl_layout_impl
{
  public:
    /**
     * @brief Creates a writer with optional serialization progress.
     * @param src Layout to write.
     * @param s Output stream.
     * @param callback Receives completed serialization work.
     */
    write_fgl_layout_impl(const Lyt& src, std::ostream& s, utils::progress_callback callback = {}) :
            lyt{src},
            os{s},
            on_progress{std::move(callback)}
    {}

    /** @brief Validate the finished layout and serialize FGL version 2. */
    void run()
    {
        validate();

        // metadata
        os << fgl::FGL_HEADER << fgl::OPEN_FGL;
        const auto current_time = std::time(nullptr);
        const auto time_str = fmt::format("{:%Y-%m-%d %H:%M:%S}", fiction::utils::stl::safe_localtime(current_time));
        os << fmt::format(fgl::FICTION_METADATA, FICTION_VERSION, FICTION_REPO, time_str);

        os << fgl::OPEN_LAYOUT_METADATA;
        const std::string layout_name{lyt.get_layout_name()};

        // check if topology matches Lyt
        std::string topology{};
        if constexpr (is_cartesian_layout_v<Lyt>)
        {
            topology = "cartesian";
        }
        else if constexpr (is_shifted_cartesian_layout_v<Lyt>)
        {
            topology = fmt::format("{}_cartesian", layouts::to_string(lyt.get_arrangement()));
        }
        else if constexpr (is_hexagonal_layout_v<Lyt>)
        {
            topology = fmt::format("{}_hex", layouts::to_string(lyt.get_arrangement()));
        }

        os << fmt::format(fgl::LAYOUT_METADATA, fgl::xml_text(layout_name), topology, lyt.width(), lyt.height(),
                          lyt.layers());

        os << fgl::OPEN_CLOCKING;
        const auto& clocking_scheme = lyt.get_clocking_scheme();
        os << fmt::format(fgl::CLOCKING_SCHEME_NAME, clocking_name());

        os << fgl::OPEN_CLOCK_ZONES;
        utils::progress_reporter clocks{on_progress, "writing clock overrides"};
        clocking_scheme.foreach_override(
            [this, &clocks](const auto x, const auto y, const auto number)
            {
                os << fmt::format(fgl::CLOCK_ZONE, x, y, number);
                clocks.advance();
            });
        os << fgl::CLOSE_CLOCK_ZONES;

        if (lyt.num_se() != 0)
        {
            os << "      <synchronization_elements>\n";
            utils::progress_reporter synchronization{on_progress, "writing synchronization elements", lyt.num_se()};
            lyt.foreach_synchronization_element(
                [this, &synchronization](const auto& coordinate, const auto delay)
                {
                    os << fmt::format("        <element><x>{}</x><y>{}</y><z>{}</z><delay>{}</delay></element>\n",
                                      coordinate.x, coordinate.y, coordinate.z, delay);
                    synchronization.advance();
                });
            os << "      </synchronization_elements>\n";
        }

        os << fgl::CLOSE_CLOCKING;
        os << "    <inputs>\n";
        lyt.foreach_pi([this](const auto id) { os << fmt::format("      <id>{}</id>\n", id.index); });
        os << "    </inputs>\n    <outputs>\n";
        lyt.foreach_po([this](const auto id) { os << fmt::format("      <id>{}</id>\n", id.index); });
        os << "    </outputs>\n";
        os << fgl::CLOSE_LAYOUT_METADATA;
        os << fgl::OPEN_GATES;
        utils::progress_reporter progress{on_progress, "writing gates", lyt.size()};
        lyt.foreach_node(
            [this, &progress](const auto id)
            {
                const auto coordinate = lyt.get_tile(id);
                const auto type       = lyt.is_pi(id)  ? std::string{"PI"} :
                                        lyt.is_po(id)  ? std::string{"PO"} :
                                        lyt.is_buf(id) ? std::string{"BUF"} :
                                                         kitty::to_hex(lyt.node_function(id));
                os << fgl::OPEN_GATE;
                os << fmt::format(fgl::GATE, id.index, type, fgl::xml_text(lyt.get_name(id)), coordinate.x,
                                  coordinate.y, coordinate.z);
                os << fmt::format("      <arity>{}</arity>\n", lyt.input_count(id));
                os << fgl::OPEN_INCOMING;
                for (uint32_t input = 0; input < lyt.input_count(id); ++input)
                {
                    const auto source = *lyt.source({id, input});
                    os << fmt::format(
                        "        <signal><source>{}</source><index>{}</index><input>{}</input></signal>\n",
                        source.object.index, source.index, input);
                }
                os << fgl::CLOSE_INCOMING << fgl::CLOSE_GATE;
                progress.advance();
            });

        os << fgl::CLOSE_GATES;
        os << fgl::CLOSE_FGL;
    }

  private:
    /** @brief Return the FGL scheme name, including a three-phase suffix where needed. @return Scheme name. */
    [[nodiscard]] std::string clocking_name() const
    {
        const auto scheme = lyt.get_clocking_scheme();
        return scheme.name() + (scheme.num_clocks() == 3u && scheme.name() != layouts::clocking::BANCS_NAME ? "3" : "");
    }

    /**
     * @brief Reject missing inputs, cycles, and physical rule violations before stream mutation.
     * @throws std::invalid_argument If the layout is incomplete, cyclic, physically invalid, or uses an unsupported
     * scheme.
     */
    void validate() const
    {
        std::unordered_map<typename Lyt::object_id, uint32_t> remaining{};
        std::vector<typename Lyt::object_id>                  ready{};
        lyt.foreach_node(
            [&](const auto id)
            {
                remaining.emplace(id, lyt.input_count(id));
                for (uint32_t input = 0; input < lyt.input_count(id); ++input)
                {
                    if (!lyt.source({id, input}))
                    {
                        throw std::invalid_argument("FGL requires every declared input to be connected");
                    }
                }
                if (lyt.input_count(id) == 0)
                    ready.push_back(id);
            });
        for (std::size_t i = 0; i < ready.size(); ++i)
        {
            lyt.foreach_sink(lyt.output(ready[i]),
                             [&](const auto port)
                             {
                                 if (--remaining.at(port.object) == 0)
                                     ready.push_back(port.object);
                             });
        }
        if (ready.size() != remaining.size())
        {
            throw std::invalid_argument("FGL requires an acyclic layout");
        }
        std::optional<layouts::arrangement> arrangement{};
        if constexpr (is_hexagonal_layout_v<Lyt>)
            arrangement = lyt.get_arrangement();
        auto scheme = layouts::clocking::get_scheme(clocking_name(), arrangement);
        if (!scheme)
            throw std::invalid_argument("FGL requires a supported named clocking scheme");
        const auto source_scheme = lyt.get_clocking_scheme();
        source_scheme.foreach_override([&](const auto x, const auto y, const auto number)
                                       { scheme->override_clock_number(x, y, number); });
        if (*scheme != source_scheme)
            throw std::invalid_argument("FGL requires a supported named clocking base scheme");
        source_scheme.foreach_override(
            [this](const auto x, const auto y, const auto)
            {
                if (x < 0 || y < 0 || static_cast<uint64_t>(x) >= lyt.width() ||
                    static_cast<uint64_t>(y) >= lyt.height() || lyt.layers() == 0)
                    throw std::invalid_argument("FGL requires clock overrides inside the extent");
            });
        lyt.foreach_synchronization_element(
            [this](const auto& coordinate, const auto)
            {
                if (!lyt.contains_coordinate(coordinate))
                    throw std::invalid_argument("FGL requires synchronization elements inside the extent");
            });
        verification::gate_level_drv_params params{};
        params.missing_connections = false;
        params.has_io              = false;
        std::ostringstream report{};
        params.out = &report;
        verification::gate_level_drv_stats stats{};
        verification::gate_level_drvs(lyt, params, &stats);
        if (stats.drvs != 0)
        {
            throw std::invalid_argument("FGL requires a physically valid layout");
        }
    }

    /**
     * The layout to be written.
     */
    const Lyt& lyt;
    /**
     * The output stream to which the gate-level layout is written.
     */
    std::ostream& os;
    /** @brief Receives serialization progress. */
    utils::progress_callback on_progress;
};

}  // namespace detail

/**
 * Writes a finished layout in FGL version 2 with extent counts and declared interface order.
 *
 * Version 2 stores width, height, and layer counts, explicit PI/PO order, and indexed source references.
 * The file includes every placed object, including complete dangling cones. Clock overrides and synchronization
 * elements remain sparse. The format supports the standard named clocking schemes and their overrides.
 * Validation finishes before the output stream changes.
 *
 * This overload uses an output stream to write into.
 *
 * @tparam Lyt Layout.
 * @param lyt The layout to be written.
 * @param on_progress Receives completed serialization work.
 * @param os The output stream to write into.
 * @throws std::invalid_argument If the layout is incomplete, cyclic, physically invalid, or uses an unsupported scheme.
 */
template <typename Lyt>
void write_fgl_layout(const Lyt& lyt, std::ostream& os, utils::progress_callback on_progress = {})
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    detail::write_fgl_layout_impl p{lyt, os, std::move(on_progress)};

    p.run();
}
/**
 * Writes a finished layout in FGL version 2 with extent counts and declared interface order.
 *
 * This overload uses a file name to create and write into.
 *
 * @tparam Lyt Layout.
 * @param lyt The layout to be written.
 * @param on_progress Receives completed serialization work.
 * @param filename The file name to create and write into. Should preferably use the .fgl extension.
 * @throws std::invalid_argument If the layout is incomplete, cyclic, physically invalid, or uses an unsupported scheme.
 */
template <typename Lyt>
void write_fgl_layout(const Lyt& lyt, const std::string_view& filename, utils::progress_callback on_progress = {})
{
    fiction::detail::atomic_write(filename, [&](std::ostream& os) { write_fgl_layout(lyt, os, on_progress); });
}

}  // namespace fiction::layouts::io

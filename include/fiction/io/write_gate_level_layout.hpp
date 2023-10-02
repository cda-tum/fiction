//
// Created by simon on 25.09.23.
//

#ifndef FICTION_WRITE_GATE_LEVEL_LAYOUT_HPP
#define FICTION_WRITE_GATE_LEVEL_LAYOUT_HPP

#include "fiction/layouts/clocked_layout.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"
#include "utils/version_info.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <kitty/print.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <chrono>
#include <ctime>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fiction
{

namespace detail
{

namespace fcn
{

inline constexpr const char* FCN_HEADER       = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
inline constexpr const char* OPEN_FCN         = "<fcn>\n";
inline constexpr const char* CLOSE_FCN        = "</fcn>\n";
inline constexpr const char* FICTION_METADATA = "  <fiction>\n"
                                                "    <fiction_version>{}</fiction_version>\n"
                                                "    <available_at>{}</available_at>\n"
                                                "    <date>{}</date>\n"
                                                "  </fiction>\n";

inline constexpr const char* OPEN_LAYOUT_METADATA  = "  <layout>\n";
inline constexpr const char* CLOSE_LAYOUT_METADATA = "  </layout>\n";
inline constexpr const char* LAYOUT_METADATA       = "    <name>{}</name>\n"
                                                     "    <topology>{}</topology>\n"
                                                     "    <size>\n"
                                                     "      <x>{}</x>\n"
                                                     "      <y>{}</y>\n"
                                                     "      <z>{}</z>\n"
                                                     "    </size>\n";
inline constexpr const char* OPEN_CLOCKING         = "    <clocking>\n";
inline constexpr const char* CLOSE_CLOCKING        = "    </clocking>\n";
inline constexpr const char* CLOCKING_SCHEME_NAME  = "      <name>{}</name>\n";
inline constexpr const char* OPEN_CLOCK_ZONES      = "      <zones>\n";
inline constexpr const char* CLOSE_CLOCK_ZONES     = "      </zones>\n";
inline constexpr const char* CLOCK_ZONE            = "        <zone>\n"
                                                     "          <x>{}</x>\n"
                                                     "          <y>{}</y>\n"
                                                     "          <clock>{}</clock>\n"
                                                     "        </zone>\n";

inline constexpr const char* OPEN_GATES     = "  <gates>\n";
inline constexpr const char* CLOSE_GATES    = "  </gates>\n";
inline constexpr const char* OPEN_GATE      = "    <gate>\n";
inline constexpr const char* CLOSE_GATE     = "    </gate>\n";
inline constexpr const char* GATE           = "      <id>{}</id>\n"
                                              "      <type>{}</type>\n"
                                              "      <name>{}</name>\n"
                                              "      <loc>\n"
                                              "        <x>{}</x>\n"
                                              "        <y>{}</y>\n"
                                              "        <z>{}</z>\n"
                                              "      </loc>\n";
inline constexpr const char* OPEN_INCOMING  = "      <incoming>\n";
inline constexpr const char* CLOSE_INCOMING = "      </incoming>\n";
inline constexpr const char* SIGNAL         = "        <signal>\n"
                                              "          <x>{}</x>\n"
                                              "          <y>{}</y>\n"
                                              "          <z>{}</z>\n"
                                              "        </signal>\n";

}  // namespace fcn

template <typename Lyt>
class write_gate_level_layout_impl
{
  public:
    write_gate_level_layout_impl(const Lyt& src, std::ostream& s) : lyt{src}, os{s} {}

    void run()
    {
        std::stringstream header{};
        std::stringstream layout_metadata{};

        // metadata
        header << fcn::FCN_HEADER << fcn::OPEN_FCN;
        const auto time_str = fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr)));
        header << fmt::format(fcn::FICTION_METADATA, FICTION_VERSION, FICTION_REPO, time_str);
        os << header.str();

        os << fcn::OPEN_LAYOUT_METADATA;
        std::string layout_name = get_name(lyt);
        layout_metadata << fmt::format(fcn::LAYOUT_METADATA, layout_name, "Cartesian", lyt.x(), lyt.y(), lyt.z());
        os << layout_metadata.str();

        os << fcn::OPEN_CLOCKING;
        const auto clocking_scheme = lyt.get_clocking_scheme();
        os << fmt::format(fcn::CLOCKING_SCHEME_NAME, clocking_scheme.name);

        if (clocking_scheme.name == "OPEN")
        {
            os << fcn::OPEN_CLOCK_ZONES;
            for (uint64_t x = 0; x <= lyt.x(); ++x)
            {
                for (uint64_t y = 0; y <= lyt.y(); ++y)
                {
                    int clock = clocking_scheme({x, y});
                    os << fmt::format(fcn::CLOCK_ZONE, x, y, clock);
                }
            }
            os << fcn::CLOSE_CLOCK_ZONES;
        }
        os << fcn::CLOSE_CLOCKING;
        os << fcn::CLOSE_LAYOUT_METADATA;

        os << fcn::OPEN_GATES;

        // create topological ordering
        mockturtle::topo_view layout_topo{lyt};
        uint32_t              id = 0;

        // inputs
        layout_topo.foreach_pi(
            [&id, this](const auto& gate)
            {
                const auto coord = lyt.get_tile(gate);
                os << fcn::OPEN_GATE;
                os << fmt::format(fcn::GATE, id, "PI", lyt.get_name(gate), coord.x, coord.y, coord.z);
                os << fcn::CLOSE_GATE;
                id++;
            });

        // gates
        layout_topo.foreach_gate(
            [&id, this](const auto& gate)
            {
                os << fcn::OPEN_GATE;
                const auto coord = lyt.get_tile(gate);
                if (const auto signals = lyt.incoming_data_flow(coord); signals.size() == 1)
                {
                    const auto signal = signals[0];

                    if (lyt.is_po(gate))
                    {
                        os << fmt::format(fcn::GATE, id, "PO", lyt.get_name(gate), coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_wire(gate))
                    {
                        os << fmt::format(fcn::GATE, id, "BUF", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_inv(gate))
                    {
                        os << fmt::format(fcn::GATE, id, "INV", "", coord.x, coord.y, coord.z);
                    }

                    os << fcn::OPEN_INCOMING;
                    os << fmt::format(fcn::SIGNAL, signal.x, signal.y, signal.z);
                    os << fcn::CLOSE_INCOMING;
                }
                else if (signals.size() == 2)
                {
                    const auto signal_a = signals[0];
                    const auto signal_b = signals[1];

                    if (lyt.is_and(gate))
                    {
                        os << fmt::format(fcn::GATE, id, "AND", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_nand(gate))
                    {
                        os << fmt::format(fcn::GATE, id, "NAND", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_or(gate))
                    {
                        os << fmt::format(fcn::GATE, id, "OR", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_nor(gate))
                    {
                        os << fmt::format(fcn::GATE, id, "NOR", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_xor(gate))
                    {
                        os << fmt::format(fcn::GATE, id, "XOR", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_xnor(gate))
                    {
                        os << fmt::format(fcn::GATE, id, "XNOR", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_function(gate))
                    {
                        const auto node_fun = lyt.node_function(gate);

                        os << fmt::format(fcn::GATE, id, kitty::to_hex(node_fun), "", coord.x, coord.y, coord.z);
                    }
                    os << fcn::OPEN_INCOMING;
                    os << fmt::format(fcn::SIGNAL, signal_a.x, signal_a.y, signal_a.z);
                    os << fmt::format(fcn::SIGNAL, signal_b.x, signal_b.y, signal_b.z);
                    os << fcn::CLOSE_INCOMING;
                }
                else if (signals.size() == 3)
                {
                    const auto signal_a = signals[0];
                    const auto signal_b = signals[1];
                    const auto signal_c = signals[2];

                    if (lyt.is_maj(gate))
                    {
                        os << fmt::format(fcn::GATE, id, "MAJ", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_function(gate))
                    {
                        const auto node_fun = lyt.node_function(gate);

                        os << fmt::format(fcn::GATE, id, kitty::to_hex(node_fun), "", coord.x, coord.y, coord.z);
                    }
                    os << fcn::OPEN_INCOMING;
                    os << fmt::format(fcn::SIGNAL, signal_a.x, signal_a.y, signal_a.z);
                    os << fmt::format(fcn::SIGNAL, signal_b.x, signal_b.y, signal_b.z);
                    os << fmt::format(fcn::SIGNAL, signal_c.x, signal_c.y, signal_c.z);
                    os << fcn::CLOSE_INCOMING;
                }
                else if (lyt.is_function(gate))
                {
                    const auto node_fun = lyt.node_function(gate);

                    os << fmt::format(fcn::GATE, id, kitty::to_hex(node_fun), "", coord.x, coord.y, coord.z);

                    os << fcn::OPEN_INCOMING;
                    for (uint i = 0; i <= signals.size(); i++)
                    {
                        os << fmt::format(fcn::SIGNAL, signals[i].x, signals[i].y, signals[i].z);
                    }
                    os << fcn::CLOSE_INCOMING;
                }
                os << fcn::CLOSE_GATE;
                id++;
            });

        os << fcn::CLOSE_GATES;
        os << fcn::CLOSE_FCN;
    }

  private:
    /**
     * The layout to be written.
     */
    Lyt lyt;
    /**
     * The output stream to which the gate-level layout is written.
     */
    std::ostream& os;
};

}  // namespace detail

/**
 * Writes a gate-level layout to a fcn file that is used by Fiction (https://github.com/cda-tum/fiction).
 *
 * This overload uses an output stream to write into.
 *
 * @tparam Lyt Layout.
 * @param lyt The layout to be written.
 * @param os The output stream to write into.
 */
template <typename Lyt>
void write_gate_level_layout(const Lyt& lyt, std::ostream& os)
{
    static_assert(is_coordinate_layout_v<Lyt>, "Lyt is not a coordinate layout");
    static_assert(is_tile_based_layout_v<Lyt>, "Lyt is not a tile-based layout");
    static_assert(is_clocked_layout_v<Lyt>, "Lyt is not a clocked layout");
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    detail::write_gate_level_layout_impl p{lyt, os};

    p.run();
}
/**
 * Writes a gate-level layout to a fcn file that is used by Fiction (https://github.com/cda-tum/fiction).
 *
 * This overload uses a file name to create and write into.
 *
 * @tparam Lyt Layout.
 * @param lyt The layout to be written.
 * @param filename The file name to create and write into. Should preferably use the `.fcn` extension.
 */
template <typename Lyt>
void write_gate_level_layout(const Lyt& lyt, const std::string_view& filename)
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_gate_level_layout(lyt, os);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_WRITE_GATE_LEVEL_LAYOUT_HPP

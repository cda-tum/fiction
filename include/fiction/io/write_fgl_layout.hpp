//
// Created by simon on 25.09.23.
//

#ifndef FICTION_WRITE_FGL_LAYOUT_HPP
#define FICTION_WRITE_FGL_LAYOUT_HPP

#include "fiction/layouts/clocked_layout.hpp"
#include "fiction/layouts/clocking_scheme.hpp"
#include "fiction/traits.hpp"
#include "utils/version_info.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <kitty/print.hpp>
#include <mockturtle/views/topo_view.hpp>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>

namespace fiction
{

namespace detail
{

namespace fcn
{

inline constexpr const char* FGL_HEADER       = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
inline constexpr const char* OPEN_FGL         = "<fgl>\n";
inline constexpr const char* CLOSE_FGL        = "</fgl>\n";
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
class write_fgl_layout_impl
{
  public:
    write_fgl_layout_impl(const Lyt& src, std::ostream& s) : lyt{src}, os{s} {}

    void run()
    {
        // metadata
        os << fcn::FGL_HEADER << fcn::OPEN_FGL;
        const auto time_str = fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr)));
        os << fmt::format(fcn::FICTION_METADATA, FICTION_VERSION, FICTION_REPO, time_str);

        os << fcn::OPEN_LAYOUT_METADATA;
        std::string layout_name = get_name(lyt);

        // check if topology matches Lyt
        std::string topology = "";
        if constexpr (is_cartesian_layout_v<Lyt>)
        {
            topology = "cartesian";
        }
        else if constexpr (is_shifted_cartesian_layout_v<Lyt>)
        {
            if constexpr (has_odd_row_cartesian_arrangement_v<Lyt>)
            {
                topology = "odd_row_cartesian";
            }
            else if constexpr (has_even_row_cartesian_arrangement_v<Lyt>)
            {
                topology = "even_row_cartesian";
            }
            else if constexpr (has_odd_column_cartesian_arrangement_v<Lyt>)
            {
                topology = "odd_column_cartesian";
            }
            else if constexpr (has_even_column_cartesian_arrangement_v<Lyt>)
            {
                topology = "even_column_cartesian";
            }
        }
        else if constexpr (is_hexagonal_layout_v<Lyt>)
        {
            if constexpr (has_odd_row_hex_arrangement_v<Lyt>)
            {
                topology = "odd_row_hex";
            }
            else if constexpr (has_even_row_hex_arrangement_v<Lyt>)
            {
                topology = "even_row_hex";
            }
            else if constexpr (has_odd_column_hex_arrangement_v<Lyt>)
            {
                topology = "odd_column_hex";
            }
            else if constexpr (has_even_column_hex_arrangement_v<Lyt>)
            {
                topology = "even_column_hex";
            }
        }

        os << fmt::format(fcn::LAYOUT_METADATA, layout_name, topology, lyt.x(), lyt.y(), lyt.z());

        os << fcn::OPEN_CLOCKING;
        const auto clocking_scheme = lyt.get_clocking_scheme();
        os << fmt::format(fcn::CLOCKING_SCHEME_NAME, clocking_scheme.name);

        // if clocking scheme is irregular, overwrite clock zones
        if (!clocking_scheme.is_regular())
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
        uint32_t              gate_id = 0;

        // inputs
        layout_topo.foreach_pi(
            [&gate_id, this](const auto& gate)
            {
                const auto coord = lyt.get_tile(gate);
                os << fcn::OPEN_GATE;
                os << fmt::format(fcn::GATE, gate_id, "PI", lyt.get_name(gate), coord.x, coord.y, coord.z);
                os << fcn::CLOSE_GATE;
                gate_id++;
            });

        // gates
        layout_topo.foreach_gate(
            [&gate_id, this](const auto& gate)
            {
                os << fcn::OPEN_GATE;
                const auto coord = lyt.get_tile(gate);
                if (const auto signals = lyt.incoming_data_flow(coord); signals.size() == 1)
                {
                    const auto incoming_signal = signals[0];

                    if (lyt.is_po(gate))
                    {
                        os << fmt::format(fcn::GATE, gate_id, "PO", lyt.get_name(gate), coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_wire(gate))
                    {
                        os << fmt::format(fcn::GATE, gate_id, "BUF", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_inv(gate))
                    {
                        os << fmt::format(fcn::GATE, gate_id, "INV", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_function(gate))
                    {
                        const auto node_fun = lyt.node_function(gate);

                        os << fmt::format(fcn::GATE, gate_id, kitty::to_hex(node_fun), "", coord.x, coord.y, coord.z);
                    }

                    os << fcn::OPEN_INCOMING;
                    os << fmt::format(fcn::SIGNAL, incoming_signal.x, incoming_signal.y, incoming_signal.z);
                    os << fcn::CLOSE_INCOMING;
                }
                else if (signals.size() == 2)
                {
                    const auto incoming_signal_a = signals[0];
                    const auto incoming_signal_b = signals[1];

                    if (lyt.is_and(gate))
                    {
                        os << fmt::format(fcn::GATE, gate_id, "AND", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_nand(gate))
                    {
                        os << fmt::format(fcn::GATE, gate_id, "NAND", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_or(gate))
                    {
                        os << fmt::format(fcn::GATE, gate_id, "OR", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_nor(gate))
                    {
                        os << fmt::format(fcn::GATE, gate_id, "NOR", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_xor(gate))
                    {
                        os << fmt::format(fcn::GATE, gate_id, "XOR", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_xnor(gate))
                    {
                        os << fmt::format(fcn::GATE, gate_id, "XNOR", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_function(gate))
                    {
                        const auto node_fun = lyt.node_function(gate);

                        os << fmt::format(fcn::GATE, gate_id, kitty::to_hex(node_fun), "", coord.x, coord.y, coord.z);
                    }
                    os << fcn::OPEN_INCOMING;
                    os << fmt::format(fcn::SIGNAL, incoming_signal_a.x, incoming_signal_a.y, incoming_signal_a.z);
                    os << fmt::format(fcn::SIGNAL, incoming_signal_b.x, incoming_signal_b.y, incoming_signal_b.z);
                    os << fcn::CLOSE_INCOMING;
                }
                else if (signals.size() == 3)
                {
                    const auto incoming_signal_a = signals[0];
                    const auto incoming_signal_b = signals[1];
                    const auto incoming_signal_c = signals[2];

                    if (lyt.is_maj(gate))
                    {
                        os << fmt::format(fcn::GATE, gate_id, "MAJ", "", coord.x, coord.y, coord.z);
                    }
                    else if (lyt.is_function(gate))
                    {
                        const auto node_fun = lyt.node_function(gate);

                        os << fmt::format(fcn::GATE, gate_id, kitty::to_hex(node_fun), "", coord.x, coord.y, coord.z);
                    }
                    os << fcn::OPEN_INCOMING;
                    os << fmt::format(fcn::SIGNAL, incoming_signal_a.x, incoming_signal_a.y, incoming_signal_a.z);
                    os << fmt::format(fcn::SIGNAL, incoming_signal_b.x, incoming_signal_b.y, incoming_signal_b.z);
                    os << fmt::format(fcn::SIGNAL, incoming_signal_c.x, incoming_signal_c.y, incoming_signal_c.z);
                    os << fcn::CLOSE_INCOMING;
                }
                else if (lyt.is_function(gate))
                {
                    const auto node_fun = lyt.node_function(gate);

                    os << fmt::format(fcn::GATE, gate_id, kitty::to_hex(node_fun), "", coord.x, coord.y, coord.z);

                    os << fcn::OPEN_INCOMING;
                    for (std::size_t i = 0; i < signals.size(); i++)
                    {
                        os << fmt::format(fcn::SIGNAL, signals[i].x, signals[i].y, signals[i].z);
                    }
                    os << fcn::CLOSE_INCOMING;
                }
                os << fcn::CLOSE_GATE;
                gate_id++;
            });

        os << fcn::CLOSE_GATES;
        os << fcn::CLOSE_FGL;
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
 * Writes an FGL layout to a file.
 *
 * This overload uses an output stream to write into.
 *
 * @tparam Lyt Layout.
 * @param lyt The layout to be written.
 * @param os The output stream to write into.
 */
template <typename Lyt>
void write_fgl_layout(const Lyt& lyt, std::ostream& os)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");

    detail::write_fgl_layout_impl p{lyt, os};

    p.run();
}
/**
 * Writes an FGL layout to a file.
 *
 * This overload uses a file name to create and write into.
 *
 * @tparam Lyt Layout.
 * @param lyt The layout to be written.
 * @param filename The file name to create and write into. Should preferably use the .fgl extension.
 */
template <typename Lyt>
void write_fgl_layout(const Lyt& lyt, const std::string_view& filename)
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_fgl_layout(lyt, os);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_WRITE_FGL_LAYOUT_HPP

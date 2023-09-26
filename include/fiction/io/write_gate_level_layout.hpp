//
// Created by simon on 25.09.23.
//

#ifndef FICTION_WRITE_GATE_LEVEL_LAYOUT_HPP
#define FICTION_WRITE_GATE_LEVEL_LAYOUT_HPP

#include "fiction/traits.hpp"
#include "utils/version_info.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>

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
inline constexpr const char* FICTION_METADATA = "    <fiction>\n"
                                                "        <fiction_version>{}</fiction_version>\n"
                                                "        <available_at>{}</available_at>\n"
                                                "        <date>{}</date>\n"
                                                "    </fiction>\n";

inline constexpr const char* LAYOUT_METADATA = "    <layout>\n"
                                               "        <name>{}</name>\n"
                                               "        <topology>{}</topology>\n"
                                               "        <clocking>{}</clocking>\n"
                                               "        <size>\n"
                                               "          <x>{}</x>\n"
                                               "          <y>{}</y>\n"
                                               "          <z>{}</z>\n"
                                               "        </size>\n"
                                               "    </layout>\n";

}  // namespace fcn

template <typename Lyt>
class write_gate_level_layout_impl
{
  public:
    write_gate_level_layout_impl(const Lyt& src, std::ostream& s) : lyt{src}, os{s} {}

    void run()
    {
        std::stringstream header{}, layout_metadata{};

        header << fcn::FCN_HEADER << fcn::OPEN_FCN;

        const auto time_str = fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr)));

        header << fmt::format(fcn::FICTION_METADATA, FICTION_VERSION, FICTION_REPO, time_str);

        const auto clocking_scheme = lyt.get_clocking_scheme().name();

        layout_metadata << fmt::format(fcn::LAYOUT_METADATA, get_name(lyt), "Cartesian", clocking_scheme, lyt.x(), lyt.y(), lyt.z());

        os << header.str();

        os << layout_metadata.str();

        os << fcn::CLOSE_FCN;
    }

  private:
    Lyt lyt;

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

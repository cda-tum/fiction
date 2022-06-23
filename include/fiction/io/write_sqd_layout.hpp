//
// Created by marcel on 14.07.21.
//

#ifndef FICTION_WRITE_SQD_LAYOUT_HPP
#define FICTION_WRITE_SQD_LAYOUT_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/traits.hpp"
#include "utils/version_info.hpp"

#include <fmt/format.h>

#include <chrono>
#include <ctime>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace fiction
{

namespace detail
{

namespace siqad
{

static constexpr const char* XML_HEADER    = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
static constexpr const char* OPEN_SIQAD    = "<siqad>\n";
static constexpr const char* CLOSE_SIQAD   = "</siqad>\n";
static constexpr const char* PROGRAM_BLOCK = "    <program>\n"
                                             "        <file_purpose>{}</file_purpose>\n"
                                             "        <created_by>{}</created_by>\n"
                                             "        <available_at>{}</available_at>\n"
                                             "        <date>{}</date>\n"
                                             "    </program>\n";

static constexpr const char* GUI_BLOCK = "    <gui>\n"
                                         "        <zoom>{}</zoom>\n"
                                         "        <displayed_region x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\"/>\n"
                                         "        <scroll x=\"{}\" y=\"{}\"/>\n"
                                         "    </gui>\n";

static constexpr const char* LAYERS_BLOCK     = "    <layers>\n{}"
                                                "    </layers>\n";
static constexpr const char* LATTICE_LAYER    = "        <layer_prop>\n"
                                                "            <name>Lattice</name>\n"
                                                "            <type>Lattice</type>\n"
                                                "            <role>Design</role>\n"
                                                "            <zoffset>0</zoffset>\n"
                                                "            <zheight>0</zheight>\n"
                                                "            <visible>1</visible>\n"
                                                "            <active>0</active>\n"
                                                "            <lat_vec>\n"
                                                "                <a1 x=\"3.84\" y=\"0\"/>\n"
                                                "                <a2 x=\"0\" y=\"7.68\"/>\n"
                                                "                <N>2</N>\n"
                                                "                <b1 x=\"0\" y=\"0\"/>\n"
                                                "                <b2 x=\"0\" y=\"2.25\"/>\n"
                                                "            </lat_vec>\n"
                                                "        </layer_prop>\n";
static constexpr const char* SCREENSHOT_LAYER = "        <layer_prop>\n"
                                                "            <name>Screenshot Overlay</name>\n"
                                                "            <type>Misc</type>\n"
                                                "            <role>Overlay</role>\n"
                                                "            <zoffset>0</zoffset>\n"
                                                "            <zheight>0</zheight>\n"
                                                "            <visible>0</visible>\n"
                                                "            <active>0</active>\n"
                                                "        </layer_prop>\n";
static constexpr const char* SURFACE_LAYER    = "        <layer_prop>\n"
                                                "            <name>Surface</name>\n"
                                                "            <type>DB</type>\n"
                                                "            <role>Design</role>\n"
                                                "            <zoffset>0</zoffset>\n"
                                                "            <zheight>0</zheight>\n"
                                                "            <visible>1</visible>\n"
                                                "            <active>0</active>\n"
                                                "        </layer_prop>\n";
static constexpr const char* ELECTRODE_LAYER  = "        <layer_prop>\n"
                                                "            <name>Metal</name>\n"
                                                "            <type>Electrode</type>\n"
                                                "            <role>Design</role>\n"
                                                "            <zoffset>1000</zoffset>\n"
                                                "            <zheight>100</zheight>\n"
                                                "            <visible>1</visible>\n"
                                                "            <active>0</active>\n"
                                                "        </layer_prop>\n";
static constexpr const char* DEFECT_LAYER     = "        <layer_prop>\n"
                                                "            <name>Defects</name>\n"
                                                "            <type>Defects</type>\n"
                                                "            <zoffset>0</zoffset>\n"
                                                "            <zheight>0</zheight>\n"
                                                "            <visible>1</visible>\n"
                                                "            <active>0</active>\n"
                                                "        </layer_prop>\n";

static constexpr const char* OPEN_DESIGN  = "    <design>\n"
                                            "        <layer type=\"Lattice\"/>\n"
                                            "        <layer type=\"Misc\"/>\n"
                                            "        <layer type=\"DB\">\n";
static constexpr const char* CLOSE_DESIGN = "    </design>\n";

static constexpr const char* LATTICE_COORDINATE = R"(<latcoord n ="{}" m="{}" l="{}"/>)";

static constexpr const char* DBDOT_BLOCK = "            <dbdot>\n"
                                           "                <layer_id>2</layer_id>\n"
                                           "                {}\n"
                                           "                <color>{}</color>\n"
                                           "            </dbdot>\n";

static constexpr const char* DEFECT_BLOCK = "            <defect>\n"
                                            "                <layer_id>5</layer_id>\n"
                                            "                <incl_coords>\n"
                                            "                    {}\n"
                                            "                </incl_coords>\n"
                                            "                <coulomb charge=\"{}\" eps_r=\"{}\" lambda_tf=\"{}\"/>\n"
                                            "            </defect>\n";

// color format is Alpha RBG
static constexpr const char* NORMAL_COLOR = "#ffc8c8c8";
static constexpr const char* INPUT_COLOR  = "#ff008dc8";
static constexpr const char* OUTPUT_COLOR = "#ffe28686";
static constexpr const char* CONST_COLOR  = "#ff000000";

}  // namespace siqad

template <typename Lyt>
class write_sqd_layout_impl
{
  public:
    write_sqd_layout_impl(const Lyt& src, std::ofstream& s) : lyt{src}, file{s} {}

    void run()
    {
        std::stringstream header{}, gui{}, design{};

        header << siqad::XML_HEADER << siqad::OPEN_SIQAD;

        const auto now      = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto       time_str = std::string{std::ctime(&now)};
        time_str.pop_back();  // remove trailing new line

        header << fmt::format(siqad::PROGRAM_BLOCK, "layout simulation", FICTION_VERSION, FICTION_REPO, time_str);

        std::vector<const char*> active_layers{siqad::LATTICE_LAYER, siqad::SCREENSHOT_LAYER, siqad::SURFACE_LAYER,
                                               siqad::ELECTRODE_LAYER};

        // add the defect layer if Lyt implements the defect interface
        if constexpr (has_get_sidb_defect_v<Lyt>)
        {
            active_layers.push_back(siqad::DEFECT_LAYER);
        }

        design << fmt::format(siqad::LAYERS_BLOCK, fmt::join(active_layers, "")) << siqad::OPEN_DESIGN;

        generate_db_blocks(design);
        generate_defect_blocks(design);

        design << siqad::CLOSE_DESIGN;

        if (!file.is_open())
        {
            throw std::ofstream::failure("could not open file");
        }

        file << header.str();
        file << gui.str();
        file << design.str();

        file << siqad::CLOSE_SIQAD;
    }

  private:
    Lyt lyt;

    std::ofstream& file;

    void generate_db_blocks(std::stringstream& design) noexcept
    {
        lyt.foreach_cell(
            [this, &design](const auto& c)
            {
                // generate SiDB cells
                if constexpr (std::is_same_v<technology<Lyt>, sidb_technology>)
                {
                    design << fmt::format(siqad::DBDOT_BLOCK,
                                          fmt::format(siqad::LATTICE_COORDINATE, c.x, c.y / 2, c.y % 2),
                                          siqad::NORMAL_COLOR);
                }
                // generate QCA cell blocks
                else if constexpr (std::is_same_v<technology<Lyt>, qca_technology>)
                {
                    const auto type = this->lyt.get_cell_type(c);

                    const auto color = qca_technology::is_input_cell(type)    ? siqad::INPUT_COLOR :
                                       qca_technology::is_output_cell(type)   ? siqad::OUTPUT_COLOR :
                                       qca_technology::is_constant_cell(type) ? siqad::CONST_COLOR :
                                                                                siqad::NORMAL_COLOR;

                    if (!qca_technology::is_const_1_cell(type))
                    {
                        // top left
                        design << fmt::format(siqad::DBDOT_BLOCK,
                                              fmt::format(siqad::LATTICE_COORDINATE, c.x * 14, c.y * 7, 0), color);
                        // bottom right
                        design << fmt::format(siqad::DBDOT_BLOCK,
                                              fmt::format(siqad::LATTICE_COORDINATE, (c.x * 14) + 6, (c.y * 7) + 3, 0),
                                              color);
                    }
                    if (!qca_technology::is_const_0_cell(type))
                    {
                        // top right
                        design << fmt::format(siqad::DBDOT_BLOCK,
                                              fmt::format(siqad::LATTICE_COORDINATE, (c.x * 14) + 6, c.y * 7, 0),
                                              color);
                        // bottom left
                        design << fmt::format(siqad::DBDOT_BLOCK,
                                              fmt::format(siqad::LATTICE_COORDINATE, c.x * 14, (c.y * 7) + 3, 0),
                                              color);
                    }
                }
            });
    }

    void generate_defect_blocks(std::stringstream& design) noexcept
    {
        if constexpr (has_foreach_sidb_defect_v<Lyt>)
        {
            lyt.foreach_sidb_defect(
                [this, &design](const auto& cd)
                {
                    const auto& cell   = cd.first;
                    const auto& defect = cd.second;

                    design << fmt::format(siqad::DEFECT_BLOCK,
                                          fmt::format(siqad::LATTICE_COORDINATE, cell.x, cell.y / 2, cell.y % 2),
                                          defect.charge, defect.epsilon_r, defect.lambda_tf);
                });
        }
    }
};

}  // namespace detail

/**
 * Writes a cell-level SiDB or QCA layout to a sqd file that is used by SiQAD (https://github.com/siqad/siqad),
 * a physical simulator for the SiDB technology platform.
 *
 * This overload uses an output stream to write into.
 *
 * @tparam Lyt Cell-level SiDB or QCA layout type.
 * @param lyt The layout to be written.
 * @param os The output stream to write into.
 */
template <typename Lyt>
void write_sqd_layout(const Lyt& lyt, std::ofstream& os)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(std::is_same_v<technology<Lyt>, qca_technology> || std::is_same_v<technology<Lyt>, sidb_technology>,
                  "Lyt must be a QCA or SiDB layout");

    detail::write_sqd_layout_impl p{lyt, os};

    p.run();
}
/**
 * Writes a cell-level SiDB or QCA layout to a sqd file that is used by SiQAD (https://github.com/siqad/siqad),
 * a physical simulator for the SiDB technology platform.
 *
 * This overload uses file name to create and write into..
 *
 * @tparam Lyt Cell-level SiDB or QCA layout type.
 * @param lyt The layout to be written.
 * @param filename The file name to create and write into. Should preferably use the ".sqd" extension.
 */
template <typename Lyt>
void write_sqd_layout(const Lyt& lyt, const std::string& filename)
{
    std::ofstream os{filename.c_str(), std::ofstream::out};
    write_sqd_layout(lyt, os);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_WRITE_SQD_LAYOUT_HPP

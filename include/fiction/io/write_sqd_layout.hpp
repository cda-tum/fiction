//
// Created by marcel on 14.07.21.
//

#ifndef FICTION_WRITE_SQD_LAYOUT_HPP
#define FICTION_WRITE_SQD_LAYOUT_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"
#include "utils/version_info.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <cassert>
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

namespace siqad
{

inline constexpr const char* SQD_HEADER    = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
inline constexpr const char* OPEN_SIQAD    = "<siqad>\n";
inline constexpr const char* CLOSE_SIQAD   = "</siqad>\n";
inline constexpr const char* PROGRAM_BLOCK = "    <program>\n"
                                             "        <file_purpose>{}</file_purpose>\n"
                                             "        <created_by>{}</created_by>\n"
                                             "        <available_at>{}</available_at>\n"
                                             "        <date>{}</date>\n"
                                             "    </program>\n";

inline constexpr const char* GUI_BLOCK = "    <gui>\n"
                                         "        <zoom>{}</zoom>\n"
                                         "        <displayed_region x1=\"{}\" y1=\"{}\" x2=\"{}\" y2=\"{}\"/>\n"
                                         "        <scroll x=\"{}\" y=\"{}\"/>\n"
                                         "    </gui>\n";

inline constexpr const char* LAYERS_BLOCK                    = "    <layers>\n{}"
                                                               "    </layers>\n";
inline constexpr const char* LATTICE_LAYER_DEFINITION_SI_100 = "        <layer_prop>\n"
                                                               "            <name>Lattice</name>\n"
                                                               "            <type>Lattice</type>\n"
                                                               "            <role>Design</role>\n"
                                                               "            <zoffset>0</zoffset>\n"
                                                               "            <zheight>0</zheight>\n"
                                                               "            <visible>1</visible>\n"
                                                               "            <active>0</active>\n"
                                                               "            <lat_vec>\n"
                                                               "                <name>Si(100) 2x1</name>\n"
                                                               "                <a1 x=\"3.84\" y=\"0\"/>\n"
                                                               "                <a2 x=\"0\" y=\"7.68\"/>\n"
                                                               "                <N>2</N>\n"
                                                               "                <b1 x=\"0\" y=\"0\"/>\n"
                                                               "                <b2 x=\"0\" y=\"2.25\"/>\n"
                                                               "            </lat_vec>\n"
                                                               "        </layer_prop>\n";
inline constexpr const char* LATTICE_LAYER_DEFINITION_SI_111 = "        <layer_prop>\n"
                                                               "            <name>Lattice</name>\n"
                                                               "            <type>Lattice</type>\n"
                                                               "            <role>Design</role>\n"
                                                               "            <zoffset>0</zoffset>\n"
                                                               "            <zheight>0</zheight>\n"
                                                               "            <visible>1</visible>\n"
                                                               "            <active>0</active>\n"
                                                               "            <lat_vec>\n"
                                                               "                 <name>Si(111) 1x1</name>\n"
                                                               "                 <a1 x=\"6.65\" y=\"0\"/>\n"
                                                               "                 <a2 x=\"0\" y=\"3.84\"/>\n"
                                                               "                 <N>2</N>\n"
                                                               "                 <b1 x=\"0\" y=\"0\"/>\n"
                                                               "                 <b2 x=\"3.3255\" y=\"1.92\"/>\n"
                                                               "            </lat_vec>\n"
                                                               "        </layer_prop>\n";
inline constexpr const char* SCREENSHOT_LAYER_DEFINITION     = "        <layer_prop>\n"
                                                               "            <name>Screenshot Overlay</name>\n"
                                                               "            <type>Misc</type>\n"
                                                               "            <role>Overlay</role>\n"
                                                               "            <zoffset>0</zoffset>\n"
                                                               "            <zheight>0</zheight>\n"
                                                               "            <visible>0</visible>\n"
                                                               "            <active>0</active>\n"
                                                               "        </layer_prop>\n";
inline constexpr const char* SURFACE_LAYER_DEFINITION        = "        <layer_prop>\n"
                                                               "            <name>Surface</name>\n"
                                                               "            <type>DB</type>\n"
                                                               "            <role>Design</role>\n"
                                                               "            <zoffset>0</zoffset>\n"
                                                               "            <zheight>0</zheight>\n"
                                                               "            <visible>1</visible>\n"
                                                               "            <active>0</active>\n"
                                                               "        </layer_prop>\n";
inline constexpr const char* ELECTRODE_LAYER_DEFINITION      = "        <layer_prop>\n"
                                                               "            <name>Metal</name>\n"
                                                               "            <type>Electrode</type>\n"
                                                               "            <role>Design</role>\n"
                                                               "            <zoffset>1000</zoffset>\n"
                                                               "            <zheight>100</zheight>\n"
                                                               "            <visible>1</visible>\n"
                                                               "            <active>0</active>\n"
                                                               "        </layer_prop>\n";
inline constexpr const char* DEFECT_LAYER_DEFINITION         = "        <layer_prop>\n"
                                                               "            <name>Defects</name>\n"
                                                               "            <type>Defects</type>\n"
                                                               "            <zoffset>0</zoffset>\n"
                                                               "            <zheight>0</zheight>\n"
                                                               "            <visible>1</visible>\n"
                                                               "            <active>0</active>\n"
                                                               "        </layer_prop>\n";

inline constexpr const char* OPEN_DESIGN         = "    <design>\n";
inline constexpr const char* LATTICE_LAYER       = "        <layer type=\"Lattice\"/>\n";
inline constexpr const char* MISC_LAYER          = "        <layer type=\"Misc\"/>\n";
inline constexpr const char* OPEN_DB_LAYER       = "        <layer type=\"DB\">\n";
inline constexpr const char* CLOSE_DB_LAYER      = "        </layer>\n";
inline constexpr const char* ELECTRODE_LAYER     = "        <layer type=\"Electrode\"/>\n";
inline constexpr const char* OPEN_DEFECTS_LAYER  = "        <layer type=\"Defects\">\n";
inline constexpr const char* CLOSE_DEFECTS_LAYER = "        </layer>\n";
inline constexpr const char* CLOSE_DESIGN        = "    </design>\n";

inline constexpr const char* LATTICE_COORDINATE = R"(<latcoord n="{}" m="{}" l="{}"/>)";
inline constexpr const char* DOT_TYPE           = R"(<type>{}</type>)";

inline constexpr const char* DBDOT_BLOCK = "            <dbdot>\n"
                                           "                <layer_id>2</layer_id>\n"
                                           "                {}\n"  // lattice coordinates
                                           "                {}\n"  // dot type
                                           "                <color>{}</color>\n"
                                           "            </dbdot>\n";

inline constexpr const char* DEFECT_BLOCK = "            <defect>\n"
                                            "                <layer_id>5</layer_id>\n"
                                            "                <incl_coords>\n"
                                            "                    {}\n"
                                            "                </incl_coords>\n{}"
                                            "                <property_map>\n"
                                            "                    <type_label>\n"
                                            "                        <val>{}</val>\n"
                                            "                    </type_label>\n"
                                            "                </property_map>\n"
                                            "            </defect>\n";

inline constexpr const char* COULOMB = "                <coulomb charge=\"{}\" eps_r=\"{}\" lambda_tf=\"{}\"/>\n";

// color format is Alpha RBG
inline constexpr const char* NORMAL_COLOR = "#ffc8c8c8";
inline constexpr const char* INPUT_COLOR  = "#ff008dc8";
inline constexpr const char* OUTPUT_COLOR = "#ffe28686";
inline constexpr const char* CONST_COLOR  = "#ff000000";

// maps defect types to their respective string representation
static const std::unordered_map<sidb_defect_type, const char*> defect_type_to_name{
    {{sidb_defect_type::NONE, "H-Si"},
     {sidb_defect_type::DB, "DB"},
     {sidb_defect_type::SI_VACANCY, "Vacancy"},
     {sidb_defect_type::SINGLE_DIHYDRIDE, "Single_Dihydride"},
     {sidb_defect_type::DIHYDRIDE_PAIR, "Dihydride"},
     {sidb_defect_type::ONE_BY_ONE, "1By1"},
     {sidb_defect_type::THREE_BY_ONE, "3By1"},
     {sidb_defect_type::SILOXANE, "Siloxane"},
     {sidb_defect_type::RAISED_SI, "Raised_Silicon"},
     {sidb_defect_type::MISSING_DIMER, "Missing_Dimer"},
     {sidb_defect_type::ETCH_PIT, "Etch_Pit"},
     {sidb_defect_type::STEP_EDGE, "Step_Edge"},
     {sidb_defect_type::GUNK, "Gunk"},
     {sidb_defect_type::UNKNOWN, "Unknown"}}};

}  // namespace siqad

template <typename Lyt>
class write_sqd_layout_impl
{
  public:
    write_sqd_layout_impl(const Lyt& src, std::ostream& s) : lyt{src}, os{s} {}

    void run()
    {
        std::stringstream header{}, gui{}, design{};

        header << siqad::SQD_HEADER << siqad::OPEN_SIQAD;

        const auto time_str = fmt::format("{:%Y-%m-%d %H:%M:%S}", fmt::localtime(std::time(nullptr)));

        header << fmt::format(siqad::PROGRAM_BLOCK, "layout simulation", FICTION_VERSION, FICTION_REPO, time_str);

        std::vector<const char*> active_layers{};

        if constexpr (is_sidb_lattice_111_v<Lyt>)
        {
            active_layers = {siqad::LATTICE_LAYER_DEFINITION_SI_111, siqad::SCREENSHOT_LAYER_DEFINITION,
                             siqad::SURFACE_LAYER_DEFINITION, siqad::ELECTRODE_LAYER_DEFINITION};
        }
        else
        {
            active_layers = {siqad::LATTICE_LAYER_DEFINITION_SI_100, siqad::SCREENSHOT_LAYER_DEFINITION,
                             siqad::SURFACE_LAYER_DEFINITION, siqad::ELECTRODE_LAYER_DEFINITION};
        }

        // add the defect layer if Lyt implements the defect interface
        if constexpr (has_get_sidb_defect_v<Lyt>)
        {
            active_layers.push_back(siqad::DEFECT_LAYER_DEFINITION);
        }

        design << fmt::format(siqad::LAYERS_BLOCK, fmt::join(active_layers, "")) << siqad::OPEN_DESIGN
               << siqad::LATTICE_LAYER << siqad::MISC_LAYER;

        design << siqad::OPEN_DB_LAYER;
        generate_db_blocks(design);
        design << siqad::CLOSE_DB_LAYER;

        if constexpr (has_get_sidb_defect_v<Lyt>)
        {
            design << siqad::OPEN_DEFECTS_LAYER;
            generate_defect_blocks(design);
            design << siqad::CLOSE_DEFECTS_LAYER;
        }

        design << siqad::ELECTRODE_LAYER;

        design << siqad::CLOSE_DESIGN;

        os << header.str();
        os << gui.str();
        os << design.str();

        os << siqad::CLOSE_SIQAD;
    }

  private:
    Lyt lyt;

    std::ostream& os;

    void generate_db_blocks(std::stringstream& design) noexcept
    {
        lyt.foreach_cell(
            [this, &design](const auto& c)
            {
                // generate SiDB cells
                if constexpr (has_sidb_technology_v<Lyt>)
                {
                    const auto type = this->lyt.get_cell_type(c);

                    std::string type_str;

                    switch (type)
                    {
                        case (sidb_technology::cell_type::NORMAL):
                        {
                            type_str = "";
                            break;
                        }
                        case (sidb_technology::cell_type::INPUT):
                        {
                            type_str = fmt::format(siqad::DOT_TYPE, "input");
                            break;
                        }
                        case (sidb_technology::cell_type::OUTPUT):
                        {
                            type_str = fmt::format(siqad::DOT_TYPE, "output");
                            break;
                        }
                        case (sidb_technology::cell_type::LOGIC):
                        {
                            type_str = fmt::format(siqad::DOT_TYPE, "logic");
                            break;
                        }
                            // LCOV_EXCL_START
                        case (sidb_technology::cell_type::EMPTY):
                        {
                            // this case can never happen; it exists to comfort the compilers
                            break;
                        }
                            // LCOV_EXCL_STOP
                    }

                    if constexpr (has_siqad_coord_v<Lyt>)
                    {
                        design << fmt::format(siqad::DBDOT_BLOCK, fmt::format(siqad::LATTICE_COORDINATE, c.x, c.y, c.z),
                                              type_str, siqad::NORMAL_COLOR);
                    }
                    else
                    {
                        const auto siqad_coord = fiction::siqad::to_siqad_coord(c);

                        design << fmt::format(
                            siqad::DBDOT_BLOCK,
                            fmt::format(siqad::LATTICE_COORDINATE, siqad_coord.x, siqad_coord.y, siqad_coord.z),
                            type_str, siqad::NORMAL_COLOR);
                    }
                }
                // generate QCA cell blocks
                else if constexpr (has_qca_technology_v<Lyt>)
                {
                    const auto type = this->lyt.get_cell_type(c);

                    const auto* color = siqad::NORMAL_COLOR;

                    switch (type)
                    {
                        case (qca_technology::cell_type::INPUT):
                        {
                            color = siqad::INPUT_COLOR;
                            break;
                        }
                        case (qca_technology::cell_type::OUTPUT):
                        {
                            color = siqad::OUTPUT_COLOR;
                            break;
                        }
                        case (qca_technology::cell_type::CONST_0):
                        case (qca_technology::cell_type::CONST_1):
                        {
                            color = siqad::CONST_COLOR;
                            break;
                        }
                    }

                    if (!qca_technology::is_const_1_cell(type))
                    {
                        // top left
                        design << fmt::format(siqad::DBDOT_BLOCK,
                                              fmt::format(siqad::LATTICE_COORDINATE, c.x * 14, c.y * 7, 0), "", color);
                        // bottom right
                        design << fmt::format(siqad::DBDOT_BLOCK,
                                              fmt::format(siqad::LATTICE_COORDINATE, (c.x * 14) + 6, (c.y * 7) + 3, 0),
                                              "", color);
                    }
                    if (!qca_technology::is_const_0_cell(type))
                    {
                        // top right
                        design << fmt::format(siqad::DBDOT_BLOCK,
                                              fmt::format(siqad::LATTICE_COORDINATE, (c.x * 14) + 6, c.y * 7, 0), "",
                                              color);
                        // bottom left
                        design << fmt::format(siqad::DBDOT_BLOCK,
                                              fmt::format(siqad::LATTICE_COORDINATE, c.x * 14, (c.y * 7) + 3, 0), "",
                                              color);
                    }
                }
            });
    }

    [[nodiscard]] static const char* get_defect_type_name(const sidb_defect_type& type) noexcept
    {
        const auto it = siqad::defect_type_to_name.find(type);
        return it == siqad::defect_type_to_name.cend() ? "Unknown" : it->second;
    }

    void generate_defect_blocks(std::stringstream& design) noexcept
    {
        if constexpr (has_foreach_sidb_defect_v<Lyt>)
        {
            lyt.foreach_sidb_defect(
                [&design](const auto& cd)
                {
                    const auto& defect = cd.second;

                    // layout is not based on SiQAD coordinates, coordinate transformation is performed
                    if constexpr (has_siqad_coord_v<Lyt>)
                    {
                        const auto& cell = cd.first;

                        design << fmt::format(
                            siqad::DEFECT_BLOCK, fmt::format(siqad::LATTICE_COORDINATE, cell.x, cell.y, cell.z),
                            is_charged_defect_type(defect) ?
                                fmt::format(siqad::COULOMB, defect.charge, defect.epsilon_r, defect.lambda_tf) :
                                "",
                            get_defect_type_name(defect.type));
                    }
                    else
                    {
                        const auto cell = fiction::siqad::to_siqad_coord(cd.first);

                        design << fmt::format(
                            siqad::DEFECT_BLOCK, fmt::format(siqad::LATTICE_COORDINATE, cell.x, cell.y, cell.z),
                            is_charged_defect_type(defect) ?
                                fmt::format(siqad::COULOMB, defect.charge, defect.epsilon_r, defect.lambda_tf) :
                                "",
                            get_defect_type_name(defect.type));
                    }
                });
        }
    }
};

}  // namespace detail

/**
 * Writes a cell-level SiDB or QCA layout to an sqd file that is used by SiQAD (https://github.com/siqad/siqad),
 * a physical simulator for the SiDB technology platform.
 *
 * If the provided cell-level layout type can represent SiDB defects, they will be written to the file as well.
 *
 * This overload uses an output stream to write into.
 *
 * @tparam Lyt Cell-level SiDB or QCA layout type.
 * @param lyt The layout to be written.
 * @param os The output stream to write into.
 */
template <typename Lyt>
void write_sqd_layout(const Lyt& lyt, std::ostream& os)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_qca_technology_v<Lyt> || has_sidb_technology_v<Lyt>, "Lyt must be a QCA or SiDB layout");

    detail::write_sqd_layout_impl p{lyt, os};

    p.run();
}
/**
 * Writes a cell-level SiDB or QCA layout to an sqd file that is used by SiQAD (https://github.com/siqad/siqad),
 * a physical simulator for the SiDB technology platform.
 *
 * If the provided cell-level layout type can represent SiDB defects, they will be written to the file as well.
 *
 * This overload uses a file name to create and write into.
 *
 * @tparam Lyt Cell-level SiDB or QCA layout type.
 * @param lyt The layout to be written.
 * @param filename The file name to create and write into. Should preferably use the `.sqd` extension.
 */
template <typename Lyt>
void write_sqd_layout(const Lyt& lyt, const std::string_view& filename)
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_sqd_layout(lyt, os);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_WRITE_SQD_LAYOUT_HPP

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
 * @brief Writer for SiDB and QCA layouts in the SQD format used by SiQAD.
 * @author Marcel Walter (marcelwa)
 * @author Jan Drewniok (Drewniok)
 */

#pragma once

#include "fiction/technology/qca/technology.hpp"
#include "fiction/technology/sidb/lattice.hpp"
#include "fiction/technology/sidb/layout.hpp"
#include "fiction/technology/sidb/model/defect.hpp"
#include "fiction/technology/sidb/technology.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/stl/stl_utils.hpp"
#include "fiction/utils/version_info.hpp"

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <cassert>
#include <ctime>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fiction::sidb::io
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
inline constexpr const char* LATTICE_LAYER_DEFINITION        = "        <layer_prop>\n"
                                                               "            <name>Lattice</name>\n"
                                                               "            <type>Lattice</type>\n"
                                                               "            <role>Design</role>\n"
                                                               "            <zoffset>0</zoffset>\n"
                                                               "            <zheight>0</zheight>\n"
                                                               "            <visible>1</visible>\n"
                                                               "            <active>0</active>\n"
                                                               "            <lat_vec>\n"
                                                               "                <name>{}</name>\n"
                                                               "                <a1 x=\"{}\" y=\"{}\"/>\n"
                                                               "                <a2 x=\"{}\" y=\"{}\"/>\n"
                                                               "                <N>2</N>\n"
                                                               "                <b1 x=\"{}\" y=\"{}\"/>\n"
                                                               "                <b2 x=\"{}\" y=\"{}\"/>\n"
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
inline const std::unordered_map<sidb::model::defect_type, const char*> defect_type_to_name{
    {{sidb::model::defect_type::NONE, "H-Si"},
     {sidb::model::defect_type::DB, "DB"},
     {sidb::model::defect_type::SI_VACANCY, "Vacancy"},
     {sidb::model::defect_type::SINGLE_DIHYDRIDE, "Single_Dihydride"},
     {sidb::model::defect_type::DIHYDRIDE_PAIR, "Dihydride"},
     {sidb::model::defect_type::ONE_BY_ONE, "1By1"},
     {sidb::model::defect_type::THREE_BY_ONE, "3By1"},
     {sidb::model::defect_type::SILOXANE, "Siloxane"},
     {sidb::model::defect_type::RAISED_SI, "Raised_Silicon"},
     {sidb::model::defect_type::MISSING_DIMER, "Missing_Dimer"},
     {sidb::model::defect_type::ETCH_PIT, "Etch_Pit"},
     {sidb::model::defect_type::STEP_EDGE, "Step_Edge"},
     {sidb::model::defect_type::GUNK, "Gunk"},
     {sidb::model::defect_type::UNKNOWN, "Unknown"}}};

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

        const auto current_time = std::time(nullptr);
        const auto time_str = fmt::format("{:%Y-%m-%d %H:%M:%S}", fiction::utils::stl::safe_localtime(current_time));

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
                        case (sidb::sidb_technology::cell_type::NORMAL):
                        {
                            type_str = "";
                            break;
                        }
                        case (sidb::sidb_technology::cell_type::INPUT):
                        {
                            type_str = fmt::format(siqad::DOT_TYPE, "input");
                            break;
                        }
                        case (sidb::sidb_technology::cell_type::OUTPUT):
                        {
                            type_str = fmt::format(siqad::DOT_TYPE, "output");
                            break;
                        }
                        case (sidb::sidb_technology::cell_type::LOGIC):
                        {
                            type_str = fmt::format(siqad::DOT_TYPE, "logic");
                            break;
                        }
                            // LCOV_EXCL_START
                        case (sidb::sidb_technology::cell_type::EMPTY):
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
                        const auto siqad_coord = fiction::layouts::coords::to_siqad(c);

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
                        case (qca::qca_technology::cell_type::INPUT):
                        {
                            color = siqad::INPUT_COLOR;
                            break;
                        }
                        case (qca::qca_technology::cell_type::OUTPUT):
                        {
                            color = siqad::OUTPUT_COLOR;
                            break;
                        }
                        case (qca::qca_technology::cell_type::CONST_0):
                        case (qca::qca_technology::cell_type::CONST_1):
                        {
                            color = siqad::CONST_COLOR;
                            break;
                        }
                        case (qca::qca_technology::cell_type::EMPTY):
                        case (qca::qca_technology::cell_type::NORMAL):
                        {
                            // both keep the `NORMAL_COLOR` the variable was initialized with
                            break;
                        }
                    }

                    if (!qca::qca_technology::is_const_1_cell(type))
                    {
                        // top left
                        design << fmt::format(siqad::DBDOT_BLOCK,
                                              fmt::format(siqad::LATTICE_COORDINATE, c.x * 14, c.y * 7, 0), "", color);
                        // bottom right
                        design << fmt::format(siqad::DBDOT_BLOCK,
                                              fmt::format(siqad::LATTICE_COORDINATE, (c.x * 14) + 6, (c.y * 7) + 3, 0),
                                              "", color);
                    }
                    if (!qca::qca_technology::is_const_0_cell(type))
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

    [[nodiscard]] static const char* get_defect_type_name(const sidb::model::defect_type& type) noexcept
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
                            sidb::model::is_charged_defect_type(defect) ?
                                fmt::format(siqad::COULOMB, defect.charge, defect.epsilon_r, defect.lambda_tf) :
                                "",
                            get_defect_type_name(defect.type));
                    }
                    else
                    {
                        const auto cell = fiction::layouts::coords::to_siqad(cd.first);

                        design << fmt::format(
                            siqad::DEFECT_BLOCK, fmt::format(siqad::LATTICE_COORDINATE, cell.x, cell.y, cell.z),
                            sidb::model::is_charged_defect_type(defect) ?
                                fmt::format(siqad::COULOMB, defect.charge, defect.epsilon_r, defect.lambda_tf) :
                                "",
                            get_defect_type_name(defect.type));
                    }
                });
        }
    }
};

/**
 * Writes an `sidb::layout` as an SQD file: the layout's lattice as the lattice layer, every SiDB as a `<dbdot>` at
 * its `(n, m, l)` lattice coordinate, and every surface defect as a `<defect>`.
 */
class sqd_writer
{
  public:
    sqd_writer(const layout& src, std::ostream& s) : lyt{src}, os{s} {}

    void run()
    {
        std::stringstream header{}, design{};

        header << siqad::SQD_HEADER << siqad::OPEN_SIQAD;

        const auto current_time = std::time(nullptr);
        const auto time_str = fmt::format("{:%Y-%m-%d %H:%M:%S}", fiction::utils::stl::safe_localtime(current_time));

        header << fmt::format(siqad::PROGRAM_BLOCK, "layout simulation", FICTION_VERSION, FICTION_REPO, time_str);

        const auto lattice_layer =
            fmt::format(siqad::LATTICE_LAYER_DEFINITION, lyt.get_lattice().name, lyt.get_lattice().a1.first,
                        lyt.get_lattice().a1.second, lyt.get_lattice().a2.first, lyt.get_lattice().a2.second,
                        lyt.get_lattice().basis[0].first, lyt.get_lattice().basis[0].second,
                        lyt.get_lattice().basis[1].first, lyt.get_lattice().basis[1].second);

        std::vector<const char*> active_layers{lattice_layer.c_str(), siqad::SCREENSHOT_LAYER_DEFINITION,
                                               siqad::SURFACE_LAYER_DEFINITION, siqad::ELECTRODE_LAYER_DEFINITION};

        if (lyt.num_defects() > 0)
        {
            active_layers.push_back(siqad::DEFECT_LAYER_DEFINITION);
        }

        design << fmt::format(siqad::LAYERS_BLOCK, fmt::join(active_layers, "")) << siqad::OPEN_DESIGN
               << siqad::LATTICE_LAYER << siqad::MISC_LAYER;

        design << siqad::OPEN_DB_LAYER;
        lyt.foreach_cell([this, &design](const auto& s) { write_db_dot(design, s); });
        design << siqad::CLOSE_DB_LAYER;

        if (lyt.num_defects() > 0)
        {
            design << siqad::OPEN_DEFECTS_LAYER;
            lyt.foreach_defect([&design](const auto& sd) { write_defect(design, sd.first, sd.second); });
            design << siqad::CLOSE_DEFECTS_LAYER;
        }

        design << siqad::ELECTRODE_LAYER << siqad::CLOSE_DESIGN;

        os << header.str() << design.str() << siqad::CLOSE_SIQAD;
    }

  private:
    const layout& lyt;
    std::ostream& os;

    void write_db_dot(std::stringstream& design, const lattice_site& s) const
    {
        std::string type_str{};

        switch (lyt.get_cell_type(s))
        {
            case sidb_technology::cell_type::INPUT:
            {
                type_str = fmt::format(siqad::DOT_TYPE, "input");
                break;
            }
            case sidb_technology::cell_type::OUTPUT:
            {
                type_str = fmt::format(siqad::DOT_TYPE, "output");
                break;
            }
            case sidb_technology::cell_type::LOGIC:
            {
                type_str = fmt::format(siqad::DOT_TYPE, "logic");
                break;
            }
            case sidb_technology::cell_type::NORMAL:
            case sidb_technology::cell_type::EMPTY:
            {
                break;
            }
        }

        design << fmt::format(siqad::DBDOT_BLOCK, fmt::format(siqad::LATTICE_COORDINATE, s.x, s.y, s.z), type_str,
                              siqad::NORMAL_COLOR);
    }
    static void write_defect(std::stringstream& design, const lattice_site& s, const sidb::model::defect& d)
    {
        const auto it = siqad::defect_type_to_name.find(d.type);

        design << fmt::format(siqad::DEFECT_BLOCK, fmt::format(siqad::LATTICE_COORDINATE, s.x, s.y, s.z),
                              sidb::model::is_charged_defect_type(d) ?
                                  fmt::format(siqad::COULOMB, d.charge, d.epsilon_r, d.lambda_tf) :
                                  "",
                              it == siqad::defect_type_to_name.cend() ? "Unknown" : it->second);
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
    std::ofstream os{std::string{filename}, std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_sqd_layout(lyt, os);
    os.close();
}

/**
 * Writes an `sidb::layout` as an SQD file to a stream. The layout's lattice becomes the file's lattice layer, SiDBs
 * are written at their `(n, m, l)` lattice coordinates, and surface defects, if any, in a defect layer.
 *
 * @param lyt Layout to write.
 * @param os Output stream to write into.
 */
inline void write_sqd_layout(const layout& lyt, std::ostream& os)
{
    detail::sqd_writer p{lyt, os};

    p.run();
}
/**
 * Writes an `sidb::layout` as an SQD file. See the stream overload for the file's content.
 *
 * @param lyt Layout to write.
 * @param filename File to write into.
 * @throws std::ofstream::failure if the file cannot be opened.
 */
inline void write_sqd_layout(const layout& lyt, const std::string_view& filename)
{
    std::ofstream os{std::string{filename}, std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_sqd_layout(lyt, os);
    os.close();
}

}  // namespace fiction::sidb::io

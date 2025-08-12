//
// Created by marcel on 23.06.22.
//

#ifndef FICTION_READ_SQD_LAYOUT_HPP
#define FICTION_READ_SQD_LAYOUT_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/technology/sidb_lattice_orientations.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/name_utils.hpp"

#include <tinyxml2.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <exception>
#include <fstream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fiction
{

/**
 * Exception thrown when an error occurs during parsing of an SQD file.
 */
class sqd_parsing_error : public std::runtime_error
{
  public:
    explicit sqd_parsing_error(const std::string_view& msg) noexcept : std::runtime_error(msg.data()) {}
};

namespace detail
{

template <typename Lyt>
class read_sqd_layout_impl
{
  public:
    read_sqd_layout_impl(std::istream& s, const std::string_view& name) : lyt{}, is{s}
    {
        set_name(lyt, name);
    }

    read_sqd_layout_impl(Lyt& tgt, std::istream& s) : lyt{tgt}, is{s} {}

    Lyt run()
    {
        // tinyXML2 does not support std::istream, so we have to read the whole file into a string first
        std::stringstream buffer{};
        buffer << is.rdbuf();
        const std::string sqd_content{buffer.str()};

        tinyxml2::XMLDocument xml_document{};
        xml_document.Parse(sqd_content.c_str());

        if (xml_document.ErrorID() != 0)
        {
            throw sqd_parsing_error("Error parsing SQD file: " + std::string(xml_document.ErrorName()));
        }

        auto* const siqad_root = xml_document.FirstChildElement("siqad");

        if (siqad_root == nullptr)
        {
            throw sqd_parsing_error("Error parsing SQD file: no root element 'siqad'");
        }

        const auto* const layers_element = siqad_root->FirstChildElement("layers");

        if (layers_element == nullptr)
        {
            throw sqd_parsing_error("Error parsing SQD file: no element 'layers'");
        }

        const auto* const layer_prop_element = layers_element->FirstChildElement("layer_prop");

        if (layer_prop_element == nullptr)
        {
            throw sqd_parsing_error("Error parsing SQD file: no element 'layer_prop'");
        }

        const auto* const lat_vec_element = layer_prop_element->FirstChildElement("lat_vec");

        if (lat_vec_element == nullptr)
        {
            throw sqd_parsing_error("Error parsing SQD file: no element 'lat_vec'");
        }

        std::string lattice_orientation = "Si(100) 2x1";

        if (const auto* const name = lat_vec_element->FirstChildElement("name"); name != nullptr)
        {
            if (const auto* const text = name->GetText(); text != nullptr)
            {
                lattice_orientation = text;
            }
        }

        parse_lat_type(lattice_orientation);

        const auto* const design_element = siqad_root->FirstChildElement("design");

        if (design_element == nullptr)
        {
            throw sqd_parsing_error("Error parsing SQD file: no element 'design'");
        }

        for (const auto* layer = design_element->FirstChildElement("layer"); layer != nullptr;
             layer             = layer->NextSiblingElement("layer"))
        {
            const auto* const layer_type = layer->Attribute("type");

            if (layer_type == nullptr)
            {
                throw sqd_parsing_error("Error parsing SQD file: no attribute 'type' in element 'layer'");
            }

            if (std::string{layer_type} == "DB")
            {
                for (const auto* db_dot = layer->FirstChildElement("dbdot"); db_dot != nullptr;
                     db_dot             = db_dot->NextSiblingElement("dbdot"))
                {
                    parse_db_dot(db_dot);
                }
            }

            else if (std::string{layer_type} == "Defects")
            {
                for (const auto* defect = layer->FirstChildElement("defect"); defect != nullptr;
                     defect             = defect->NextSiblingElement("defect"))
                {
                    parse_defect(defect);
                }
            }
        }

        // resize the layout to fit all cells
        lyt.resize(max_cell_pos);

        return lyt;
    }

  private:
    /**
     * The layout to which the parsed cells are added.
     */
    Lyt lyt;
    /**
     * The input stream from which the SQD file is read.
     */
    std::istream& is;
    /**
     * The maximum position of a cell in the layout.
     */
    cell<Lyt> max_cell_pos{};

    /**
     * Updates the bounding box given by the maximum position of a cell in the layout
     *
     * @param cell The cell to challenge the stored maximum position of a cell in the layout against.
     */
    void update_bounding_box(const cell<Lyt>& cell)
    {
        if (cell.x > max_cell_pos.x)
        {
            max_cell_pos.x = cell.x;
        }
        if (cell.y > max_cell_pos.y)
        {
            max_cell_pos.y = cell.y;
        }
        if (cell.z > max_cell_pos.z)
        {
            max_cell_pos.z = cell.z;
        }
    }
    /**
     * Converts a dimer position to a cell position. Additionally updates the maximum cell position parsed so far.
     *
     * @param n The x-coordinate of the dimer.
     * @param m The y-coordinate of the dimer.
     * @param l 0 for the upper dot, 1 for the lower dot.
     * @return The cell position converted from the dimer position.
     */
    cell<Lyt> dimer_to_cell(const int64_t n, const int64_t m, const int64_t l)
    {
        if (n < 0 || m < 0)
        {
            throw sqd_parsing_error("Error parsing SQD file: dimer has negative coordinates");
        }
        if (l < 0 || l > 1)
        {
            throw sqd_parsing_error("Error parsing SQD file: dimer has invalid dot index");
        }

        const cell<Lyt> cell{n, m * 2 + l};

        update_bounding_box(cell);

        return cell;
    }
    /**
     * Parses a <latcoord> element from the SQD file and returns its specified cell position.
     *
     * @param latcoord The <latcoord> element.
     * @return The cell position specified by the <latcoord> element.
     */
    void parse_lat_type(const std::string& name)
    {
        if (name == "Si(111) 1x1")
        {
            if (!has_given_lattice_orientation_v<Lyt, sidb_111_lattice>)
            {
                throw sqd_parsing_error("Error parsing SQD file: mismatch in lattice orientations");
            }
        }
        else if (name == "Si(100) 2x1")
        {
            if (!has_given_lattice_orientation_v<Lyt, sidb_100_lattice>)
            {
                throw sqd_parsing_error("Error parsing SQD file: mismatch in lattice orientations");
            }
        }
        else
        {
            throw sqd_parsing_error("Error parsing SQD file: unknown lattice orientation");
        }
    }
    /**
     * Parses a <latcoord> element from the SQD file and returns its specified cell position.
     *
     * @param latcoord The <latcoord> element.
     * @return The cell position specified by the <latcoord> element.
     */
    cell<Lyt> parse_latcoord(const tinyxml2::XMLElement* latcoord)
    {
        const auto n = latcoord->Attribute("n"), m = latcoord->Attribute("m"), l = latcoord->Attribute("l");

        if (n == nullptr || m == nullptr || l == nullptr)
        {
            throw sqd_parsing_error("Error parsing SQD file: no attribute 'n', 'm' or 'l' in element 'latcoord'");
        }

        // special case for SiQAD coordinates
        if constexpr (has_siqad_coord_v<Lyt>)
        {
            cell<Lyt> cell{std::stoll(n), std::stoll(m), std::stoll(l)};
            update_bounding_box(cell);
            return cell;
        }

        // Cartesian coordinates
        return dimer_to_cell(std::stoll(n), std::stoll(m), std::stoll(l));
    }
    /**
     * Parses the <type> attribute of a <dbdot> element from the SQD file and returns the corresponding cell type.
     *
     * @param db_dot The <dbdot> element.
     * @return The cell type specified by the <dbdot> element. If non is specified, the cell type is assumed to be
     * normal.
     */
    sidb_technology::cell_type parse_dot_type(const tinyxml2::XMLElement* dot_type)
    {
        // if no dot type is given, assume normal dot
        if (dot_type == nullptr)
        {
            return sidb_technology::cell_type::NORMAL;
        }

        const auto* const type = dot_type->GetText();

        if (type == nullptr)
        {
            throw sqd_parsing_error("Error parsing SQD file: no text in element 'type'");
        }

        if (std::string{type} == "input")
        {
            return sidb_technology::cell_type::INPUT;
        }
        if (std::string{type} == "output")
        {
            return sidb_technology::cell_type::OUTPUT;
        }
        if (std::string{type} == "normal")
        {
            return sidb_technology::cell_type::NORMAL;
        }
        if (std::string{type} == "logic")
        {
            return sidb_technology::cell_type::LOGIC;
        }

        throw sqd_parsing_error("Error parsing SQD file: invalid dot type");
    }
    /**
     * Parses a <dbdot> element from the SQD file and adds the respective dot to the layout.
     *
     * @param db_dot The <dbdot> element.
     */
    void parse_db_dot(const tinyxml2::XMLElement* db_dot)
    {
        const auto* const latcoord = db_dot->FirstChildElement("latcoord");

        if (latcoord == nullptr)
        {
            throw sqd_parsing_error("Error parsing SQD file: no element 'latcoord' in element 'dbdot'");
        }

        const auto* const dot_type = db_dot->FirstChildElement("type");

        lyt.assign_cell_type(parse_latcoord(latcoord), parse_dot_type(dot_type));
    }
    /**
     * Parses a <val> attribute of a <type_label> element of a <property_map> element from the SQD file and converts it
     * to the respective SiDB defect type.
     *
     * @param label The <type_label> element's <val> attribute.
     * @return The SiDB defect type corresponding to the given label.
     */
    [[nodiscard]] static sidb_defect_type parse_defect_label(const char* label) noexcept
    {
        // maps defect names to their respective types
        static const std::unordered_map<std::string, sidb_defect_type> defect_name_to_type{
            {{"h-si", sidb_defect_type::NONE},
             {"db", sidb_defect_type::DB},
             {"vacancy", sidb_defect_type::SI_VACANCY},
             {"single_dihydride", sidb_defect_type::SINGLE_DIHYDRIDE},
             {"dihydride", sidb_defect_type::DIHYDRIDE_PAIR},
             {"1by1", sidb_defect_type::ONE_BY_ONE},
             {"3by1", sidb_defect_type::THREE_BY_ONE},
             {"siloxane", sidb_defect_type::SILOXANE},
             {"raised_silicon", sidb_defect_type::RAISED_SI},
             {"missing_dimer", sidb_defect_type::MISSING_DIMER},
             {"etch_pit", sidb_defect_type::ETCH_PIT},
             {"step_edge", sidb_defect_type::STEP_EDGE},
             {"gunk", sidb_defect_type::GUNK},
             {"unknown", sidb_defect_type::UNKNOWN}}};

        std::string name{label};
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);

        const auto it = defect_name_to_type.find(name);
        return it == defect_name_to_type.cend() ? sidb_defect_type::UNKNOWN : it->second;
    }
    /**
     * Parses a <defect> element from the SQD file and adds the respective defect to the layout if it implements the
     * has_assign_sidb_defect function..
     *
     * @param label The <defect> element.
     */
    void parse_defect(const tinyxml2::XMLElement* defect)
    {
        if constexpr (has_assign_sidb_defect_v<Lyt>)
        {
            std::vector<cell<Lyt>> incl_cells{};
            sidb_defect_type       defect_type{sidb_defect_type::UNKNOWN};
            int64_t                charge{0};
            double                 eps_r{0.0};
            double                 lambda_tf{0.0};

            if (const auto* const incl_coords = defect->FirstChildElement("incl_coords"); incl_coords != nullptr)
            {
                for (const auto* latcoord = incl_coords->FirstChildElement("latcoord"); latcoord != nullptr;
                     latcoord             = latcoord->NextSiblingElement("latcoord"))
                {
                    incl_cells.push_back(parse_latcoord(latcoord));
                }
                if (incl_cells.empty())
                {
                    throw sqd_parsing_error("Error parsing SQD file: no element 'latcoord' in element 'incl_coords'");
                }
            }
            if (const auto* const property_map = defect->FirstChildElement("property_map"); property_map != nullptr)
            {
                if (const auto* const type_label = property_map->FirstChildElement("type_label"); type_label != nullptr)
                {
                    if (const auto* const val = type_label->FirstChildElement("val"); val != nullptr)
                    {
                        defect_type = parse_defect_label(val->GetText());
                    }
                }
            }
            if (const auto* const coulomb = defect->FirstChildElement("coulomb"); coulomb == nullptr)
            {
                // no coulomb data available; use default values
            }
            else
            {
                const auto charge_string = coulomb->Attribute("charge"), eps_r_string = coulomb->Attribute("eps_r"),
                           lambda_tf_string = coulomb->Attribute("lambda_tf");

                if (charge_string == nullptr || eps_r_string == nullptr || lambda_tf_string == nullptr)
                {
                    throw sqd_parsing_error(
                        "Error parsing SQD file: no attribute 'charge', 'eps_r', or 'lambda_tf' in element 'coulomb'");
                }

                charge    = std::stoll(charge_string);
                eps_r     = std::stod(eps_r_string);
                lambda_tf = std::stod(lambda_tf_string);
            }

            std::for_each(incl_cells.begin(), incl_cells.end(),
                          [this, &defect_type, &charge, &eps_r, &lambda_tf](const auto& cell)
                          { lyt.assign_sidb_defect(cell, sidb_defect{defect_type, charge, eps_r, lambda_tf}); });
        }
    }
};

}  // namespace detail

/**
 * Reads a cell-level SiDB layout from an sqd file provided as an input stream. The format is used by SiQAD
 * (https://github.com/siqad/siqad).
 *
 * If the provided cell-level layout type can represent SiDB defects, they will be parsed from the sqd file as well.
 *
 * May throw an `sqd_parsing_exception` if the sqd file is malformed.
 *
 * @tparam Lyt The layout type to be created from an input. Must be an SiDB lattice cell-level SiDB layout.
 * @param is The input stream to read from.
 * @param name The name to give to the generated layout.
 * @return The cell-level SiDB layout read from the sqd file.
 */
template <typename Lyt>
Lyt read_sqd_layout(std::istream& is, const std::string_view& name = "")
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");
    static_assert(is_sidb_lattice_v<Lyt>, "Lyt must be a lattice layout");

    detail::read_sqd_layout_impl<Lyt> p{is, name};

    const auto lyt = p.run();

    return lyt;
}
/**
 * Reads a cell-level SiDB layout from an sqd file provided as an input stream. The format is used by SiQAD
 * (https://github.com/siqad/siqad).
 *
 * If The provided cell-level layout type can represent SiDB defects, they will be parsed from the sqd file as well.
 *
 * May throw an `sqd_parsing_exception` if the sqd file is malformed.
 *
 * This is an in-place version of read_sqd_layout that utilizes the given layout as a target to write to.
 *
 * @tparam Lyt The layout type to be created from an input. Must be an SiDB lattice cell-level SiDB layout.
 * @param lyt The layout to write to.
 * @param is The input stream to read from.
 */
template <typename Lyt>
void read_sqd_layout(Lyt& lyt, std::istream& is)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");
    static_assert(is_sidb_lattice_v<Lyt>, "Lyt must be a lattice layout");

    detail::read_sqd_layout_impl<Lyt> p{lyt, is};

    lyt = p.run();
}
/**
 * Reads a cell-level SiDB layout from an sqd file provided as a file name. The format is used by SiQAD
 * (https://github.com/siqad/siqad).
 *
 * If The provided cell-level layout type can represent SiDB defects, they will be parsed from the sqd file as well.
 *
 * May throw an `sqd_parsing_exception` if the sqd file is malformed.
 *
 * @tparam Lyt The layout type to be created from an input. Must be an SiDB lattice cell-level SiDB layout.
 * @param filename The file name to open and read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
Lyt read_sqd_layout(const std::string_view& filename, const std::string_view& name = "")
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");
    static_assert(is_sidb_lattice_v<Lyt>, "Lyt must be a lattice layout");

    std::ifstream is{filename.data(), std::ifstream::in};

    if (!is.is_open())
    {
        throw std::ifstream::failure("could not open file");
    }

    const auto lyt = read_sqd_layout<Lyt>(is, name);
    is.close();

    return lyt;
}
/**
 * Reads a cell-level SiDB layout from an sqd file provided as a file name. The format is used by SiQAD
 * (https://github.com/siqad/siqad).
 *
 * If The provided cell-level layout type can represent SiDB defects, they will be parsed from the sqd file as well.
 *
 * May throw an `sqd_parsing_exception` if the sqd file is malformed.
 *
 * This is an in-place version of `read_sqd_layout` that utilizes the given layout as a target to write to.
 *
 * @tparam Lyt The layout type to be created from an input. Must be an SiDB lattice cell-level SiDB layout.
 * @param lyt The layout to write to.
 * @param filename The file name to open and read from.
 */
template <typename Lyt>
void read_sqd_layout(Lyt& lyt, const std::string_view& filename)
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_sidb_technology_v<Lyt>, "Lyt must be an SiDB layout");
    static_assert(is_sidb_lattice_v<Lyt>, "Lyt must be a lattice layout");

    std::ifstream is{filename.data(), std::ifstream::in};

    if (!is.is_open())
    {
        throw std::ifstream::failure("could not open file");
    }

    read_sqd_layout<Lyt>(lyt, is);
    is.close();
}

}  // namespace fiction

#endif  // FICTION_READ_SQD_LAYOUT_HPP

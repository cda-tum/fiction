//
// Created by marcel on 23.06.22.
//

#ifndef FICTION_READ_SQD_LAYOUT_HPP
#define FICTION_READ_SQD_LAYOUT_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/technology/sidb_defects.hpp"
#include "fiction/traits.hpp"
#include "fiction/utils/name_utils.hpp"

#include <tinyxml2.h>

#include <algorithm>
#include <exception>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <type_traits>

namespace fiction
{

class sqd_parsing_exception : public std::runtime_error
{
  public:
    explicit sqd_parsing_exception(const std::string& msg) noexcept : std::runtime_error(msg) {}
};

namespace detail
{

template <typename Lyt>
class read_sqd_layout_impl
{
  public:
    explicit read_sqd_layout_impl(std::istream& s, const std::string& name) : lyt{}, is{s}
    {
        set_name(lyt, name);
    }

    Lyt run()
    {
        std::string sqd_content(std::istreambuf_iterator<char>(is), {});

        tinyxml2::XMLDocument xml_document{};
        xml_document.Parse(sqd_content.c_str());

        if (xml_document.ErrorID() != 0)
        {
            throw sqd_parsing_exception("Error parsing SQD file: " + std::string(xml_document.ErrorName()));
        }

        if (const auto siqad_root = xml_document.FirstChildElement("siqad"); siqad_root == nullptr)
        {
            throw sqd_parsing_exception("Error parsing SQD file: no root element 'siqad'");
        }
        else
        {
            if (const auto design_element = siqad_root->FirstChildElement("design"); design_element == nullptr)
            {
                throw sqd_parsing_exception("Error parsing SQD file: no element 'design'");
            }
            else
            {
                for (auto layer = design_element->FirstChildElement("layer"); layer != nullptr;
                     layer      = layer->NextSiblingElement("layer"))
                {
                    if (const auto layer_type = layer->Attribute("type"); layer_type == nullptr)
                    {
                        throw sqd_parsing_exception("Error parsing SQD file: no attribute 'type' in element 'layer'");
                    }
                    else if (layer_type == std::string{"DB"})
                    {
                        for (auto db_dot = layer->FirstChildElement("dbdot"); db_dot != nullptr;
                             db_dot      = db_dot->NextSiblingElement("dbdot"))
                        {
                            parse_db_dot(db_dot);
                        }
                    }
                    else if (layer_type == std::string{"Defects"})
                    {
                        for (auto defect = layer->FirstChildElement("defect"); defect != nullptr;
                             defect      = defect->NextSiblingElement("defect"))
                        {
                            parse_defect(defect);
                        }
                    }
                }
            }
        }

        // resize the layout to fit all cells
        lyt.resize(max_cell_pos);

        return lyt;
    }

  private:
    Lyt lyt;

    std::istream& is;
    /**
     * The maximum position of a cell in the layout.
     */
    cell<Lyt> max_cell_pos{};

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
        if (n < 0 || m < 0 || l < 0)
        {
            throw sqd_parsing_exception("Error parsing SQD file: dimer has negative coordinates");
        }

        const cell<Lyt> cell{n, m * 2 + l};

        // store latest cell position via bounding box
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

        return cell;
    }
    /**
     * Parses a <latcoord> element from the SQD file and returns its specified cell position.
     *
     * @param latcoord The <latcoord> element.
     * @return The cell position specified by the <latcoord> element.
     */
    cell<Lyt> parse_latcoord(const tinyxml2::XMLElement* latcoord)
    {
        if (const auto n = latcoord->Attribute("n"), m = latcoord->Attribute("m"), l = latcoord->Attribute("l");
            n == nullptr || m == nullptr || l == nullptr)
        {
            throw sqd_parsing_exception("Error parsing SQD file: no attribute 'n', 'm' or 'l' in element 'latcoord'");
        }
        else
        {
            return dimer_to_cell(std::stoll(n), std::stoll(m), std::stoll(l));
        }
    }
    /**
     * Parses a <dbdot> element from the SQD file and adds the respective dot to the layout.
     *
     * @param db_dot The <dbdot> element.
     */
    void parse_db_dot(const tinyxml2::XMLElement* db_dot)
    {
        if (const auto latcoord = db_dot->FirstChildElement("latcoord"); latcoord == nullptr)
        {
            throw sqd_parsing_exception("Error parsing SQD file: no element 'latcoord' in element 'dbdot'");
        }
        else
        {
            lyt.assign_cell_type(parse_latcoord(latcoord), sidb_technology::cell_type::NORMAL);
        }
    }
    /**
     * Parses a <val> attribute of a <type_label> element of a <property_map> element from the SQD file and converts it
     * to the respective SiDB defect type.
     *
     * @param label The <type_label> element's <val> attribute.
     * @return The SiDB defect type corresponding to the given label.
     */
    sidb_defect_type parse_defect_label(const char* label)
    {
        if (label == std::string{"DB"})
        {
            return sidb_defect_type::DB;
        }
        // NOTE more defect types can be added here
        else
        {
            return sidb_defect_type::UNKNOWN;
        }
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

            if (const auto incl_coords = defect->FirstChildElement("incl_coords"); incl_coords != nullptr)
            {
                for (auto latcoord = incl_coords->FirstChildElement("latcoord"); latcoord != nullptr;
                     latcoord      = latcoord->NextSiblingElement("latcoord"))
                {
                    incl_cells.push_back(parse_latcoord(latcoord));
                }
                if (incl_cells.empty())
                {
                    throw sqd_parsing_exception(
                        "Error parsing SQD file: no element 'latcoord' in element 'incl_coords'");
                }
            }
            if (const auto property_map = defect->FirstChildElement("property_map"); property_map != nullptr)
            {
                if (const auto type_label = property_map->FirstChildElement("type_label"); type_label != nullptr)
                {
                    if (const auto val = type_label->Attribute("val"); val != nullptr)
                    {
                        defect_type = parse_defect_label(val);
                    }
                }
            }
            if (const auto coulomb = defect->FirstChildElement("coulomb"); coulomb == nullptr)
            {
                throw sqd_parsing_exception("Error parsing SQD file: no element 'coulomb' in element 'defect'");
            }
            else
            {
                if (const auto charge = coulomb->Attribute("charge"), eps_r = coulomb->Attribute("eps_r"),
                    lambda_tf = coulomb->Attribute("lambda_tf");
                    charge == nullptr || eps_r == nullptr || lambda_tf == nullptr)
                {
                    throw sqd_parsing_exception(
                        "Error parsing SQD file: no attribute 'charge', 'eps_r', or 'lambda_tf' in element 'coulomb'");
                }
                else
                {
                    const auto coulomb_charge_val = std::stod(charge);
                    const auto eps_r_val          = std::stod(eps_r);
                    const auto lambda_tf_val      = std::stod(lambda_tf);

                    std::for_each(
                        incl_cells.begin(), incl_cells.end(),
                        [this, &defect_type, &coulomb_charge_val, &eps_r_val, &lambda_tf_val](const auto& cell) {
                            lyt.assign_sidb_defect(
                                cell, sidb_defect{defect_type, coulomb_charge_val, eps_r_val, lambda_tf_val});
                        });
                }
            }
        }
    }
};

}  // namespace detail

/**
 * Reads a cell-level SiDB layout from an sqd file provided as an input stream. The format is used by SiQAD
 * (https://github.com/siqad/siqad).
 *
 * May throw an sqd_parsing_exception if the sqd file is malformed.
 *
 * @tparam Lyt The layout type to be created from an input. Must be a cell-level SiDB layout.
 * @param is The input stream to read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
Lyt read_sqd_layout(std::istream& is, const std::string& name = "")
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(std::is_same_v<technology<Lyt>, sidb_technology>, "Lyt must be an SiDB layout");

    detail::read_sqd_layout_impl<Lyt> p{is, name};

    const auto lyt = p.run();

    return lyt;
}
/**
 * Reads a cell-level SiDB layout from an sqd file provided as a file name. The format is used by SiQAD
 * (https://github.com/siqad/siqad).
 *
 * May throw an sqd_parsing_exception if the sqd file is malformed.
 *
 * @tparam Lyt The layout type to be created from an input. Must be a cell-level SiDB layout.
 * @param filename The file name to open and read from.
 * @param name The name to give to the generated layout.
 */
template <typename Lyt>
Lyt read_sqd_layout(const std::string& filename, const std::string& name = "")
{
    std::ifstream is{filename.c_str(), std::ifstream::in};

    if (!is.is_open())
        throw std::ifstream::failure("could not open file");

    const auto lyt = read_sqd_layout<Lyt>(is, name);
    is.close();

    return lyt;
}

}  // namespace fiction

#endif  // FICTION_READ_SQD_LAYOUT_HPP

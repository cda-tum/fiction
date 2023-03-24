//
// Created by marcel on 05.10.18.
//

#ifndef FICTION_WRITE_QCA_LAYOUT_HPP
#define FICTION_WRITE_QCA_LAYOUT_HPP

#include "fiction/technology/cell_technologies.hpp"
#include "fiction/traits.hpp"
#include "utils/version_info.hpp"

#include <fmt/format.h>

#include <fstream>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace fiction
{

/**
 * Parameters for writing QCADesigner layouts.
 */
struct write_qca_layout_params
{
    /**
     * Create via cells in between each layer.
     */
    bool create_inter_layer_via_cells = true;
};

namespace detail
{

namespace qcad
{

inline constexpr const char* VERSION_2_HEADER = "[VERSION]\n"
                                                "qcadesigner_version=2.000000\n"
                                                "[#VERSION]\n"
                                                "[LAYOUT]\n"
                                                "{}"
                                                "generated_by={}\n"
                                                "repo={}\n"
                                                "[#LAYOUT]\n"
                                                "[TYPE:DESIGN]\n"
                                                "[TYPE:QCADLayer]\n"
                                                "type=3\n"
                                                "status=1\n"
                                                "pszDescription=Drawing Layer\n"
                                                "[#TYPE:QCADLayer]\n"
                                                "[TYPE:QCADLayer]\n"
                                                "type=0\n"
                                                "status=1\n"
                                                "pszDescription=Substrate\n"
                                                "[TYPE:QCADSubstrate]\n"
                                                "[TYPE:QCADStretchyObject]\n"
                                                "[TYPE:QCADDesignObject]\n"
                                                "x=3000.000000\n"
                                                "y=1500.000000\n"
                                                "bSelected=FALSE\n"
                                                "clr.red=65535\n"
                                                "clr.green=65535\n"
                                                "clr.blue=65535\n"
                                                "bounding_box.xWorld=0.000000\n"
                                                "bounding_box.yWorld=0.000000\n"
                                                "bounding_box.cxWorld=6000.000000\n"
                                                "bounding_box.cyWorld=3000.000000\n"
                                                "[#TYPE:QCADDesignObject]\n"
                                                "[#TYPE:QCADStretchyObject]\n"
                                                "grid_spacing=20.000000\n"
                                                "[#TYPE:QCADSubstrate]\n"
                                                "[#TYPE:QCADLayer]\n";

inline constexpr const char* CLOSE_DESIGN               = "[#TYPE:DESIGN]\n";
inline constexpr const char* OPEN_QCAD_LAYER            = "[TYPE:QCADLayer]\n";
inline constexpr const char* CLOSE_QCAD_LAYER           = "[#TYPE:QCADLayer]\n";
inline constexpr const char* OPEN_QCAD_CELL             = "[TYPE:QCADCell]\n";
inline constexpr const char* CLOSE_QCAD_CELL            = "[#TYPE:QCADCell]\n";
inline constexpr const char* OPEN_QCAD_LABEL            = "[TYPE:QCADLabel]\n";
inline constexpr const char* CLOSE_QCAD_LABEL           = "[#TYPE:QCADLabel]\n";
inline constexpr const char* OPEN_QCAD_DESIGN_OBJECT    = "[TYPE:QCADDesignObject]\n";
inline constexpr const char* CLOSE_QCAD_DESIGN_OBJECT   = "[#TYPE:QCADDesignObject]\n";
inline constexpr const char* OPEN_QCAD_STRETCHY_OBJECT  = "[TYPE:QCADStretchyObject]\n";
inline constexpr const char* CLOSE_QCAD_STRETCHY_OBJECT = "[#TYPE:QCADStretchyObject]\n";
inline constexpr const char* OPEN_CELL_DOT              = "[TYPE:CELL_DOT]\n";
inline constexpr const char* CLOSE_CELL_DOT             = "[#TYPE:CELL_DOT]\n";

inline constexpr const char* TYPE            = "type=";
inline constexpr const char* STATUS          = "status=";
inline constexpr const char* PSZ_DESCRIPTION = "pszDescription=";
inline constexpr const char* PSZ             = "psz=";
inline constexpr const char* B_SELECTED      = "bSelected=";

inline constexpr const char* SELECTED_TRUE  = "TRUE";
inline constexpr const char* SELECTED_FALSE = "FALSE";

inline constexpr const char* COLOR = "clr.red={}\nclr.green={}\nclr.blue={}\n";

inline constexpr const char* BOUNDING_BOX_X  = "bounding_box.xWorld=";
inline constexpr const char* BOUNDING_BOX_Y  = "bounding_box.yWorld=";
inline constexpr const char* BOUNDING_BOX_CX = "bounding_box.cxWorld=";
inline constexpr const char* BOUNDING_BOX_CY = "bounding_box.cyWorld=";

inline constexpr const char* CELL_OPTIONS_CX           = "cell_options.cxCell=";
inline constexpr const char* CELL_OPTIONS_CY           = "cell_options.cyCell=";
inline constexpr const char* CELL_OPTIONS_DOT_DIAMETER = "cell_options.dot_diameter=";
inline constexpr const char* CELL_OPTIONS_CLOCK        = "cell_options.clock=";
inline constexpr const char* CELL_OPTIONS_MODE         = "cell_options.mode=";
inline constexpr const char* CELL_FUNCTION             = "cell_function=";
inline constexpr const char* NUMBER_OF_DOTS_4          = "number_of_dots=4\n";

inline constexpr const char* CELL_MODE_NORMAL     = "QCAD_CELL_MODE_NORMAL";
inline constexpr const char* CELL_MODE_VERTICAL   = "QCAD_CELL_MODE_VERTICAL";
inline constexpr const char* CELL_MODE_CROSSOVER  = "QCAD_CELL_MODE_CROSSOVER";
inline constexpr const char* CELL_MODE_ROTATED    = "QCAD_CELL_MODE_ROTATED";
inline constexpr const char* CELL_FUNCTION_NORMAL = "QCAD_CELL_NORMAL";
inline constexpr const char* CELL_FUNCTION_FIXED  = "QCAD_CELL_FIXED";
inline constexpr const char* CELL_FUNCTION_INPUT  = "QCAD_CELL_INPUT";
inline constexpr const char* CELL_FUNCTION_OUTPUT = "QCAD_CELL_OUTPUT";

inline constexpr const char* X_POS     = "x=";
inline constexpr const char* Y_POS     = "y=";
inline constexpr const char* DIAMETER  = "diameter=";
inline constexpr const char* CHARGE    = "charge=";
inline constexpr const char* SPIN      = "spin=";
inline constexpr const char* POTENTIAL = "potential=";

inline constexpr const char* CHARGE_0      = "0.000000e+00";
inline constexpr const char* CHARGE_1      = "1.602176e-19";
inline constexpr const char* CHARGE_8      = "8.010882e-20";
inline constexpr const char* NEGATIVE_SPIN = "-319472243083548083355648.000000";

inline constexpr const int   COLOR_MAX       = 65535;
inline constexpr const int   COLOR_HALF      = 32768;
inline constexpr const int   COLOR_MIN       = 0;
inline constexpr const int   CELL_DISTANCE   = 20;
inline constexpr const int   X_Y_OFFSET      = 100;
inline constexpr const float CELL_SIZE       = 18.0f;
inline constexpr const float DOT_SIZE        = 5.0f;
inline constexpr const float LABEL_Y_OFFSET  = 21.5f;
inline constexpr const float BB_X_OFFSET     = 9.0f;
inline constexpr const float BB_Y_OFFSET     = 33.0f;
inline constexpr const float BB_CX_OFFSET    = 4.0f;
inline constexpr const float BB_CY_OFFSET    = 23.0f;
inline constexpr const float CHARACTER_WIDTH = 10.0f;

struct cell_pos
{
    float x, y;
};

struct color
{
    unsigned red, green, blue;
};

}  // namespace qcad

template <typename Lyt>
class write_qca_layout_impl
{
  public:
    write_qca_layout_impl(const Lyt& src, std::ostream& s, const write_qca_layout_params p) : lyt{src}, os{s}, ps{p} {}

    void run()
    {

        write_header();

        write_cell_layers();

        // close design block
        os << qcad::CLOSE_DESIGN << std::flush;
    }

  private:
    Lyt lyt;

    std::ostream& os;

    const write_qca_layout_params ps;

    uint32_t via_counter{1ul};

    // via cells
    std::vector<cell<Lyt>> via_layer_cells{};

    void write_header()
    {
        const auto layout_name = lyt.get_layout_name();
        os << fmt::format(qcad::VERSION_2_HEADER, (layout_name.empty() ? "" : fmt::format("name={}\n", layout_name)),
                          FICTION_VERSION, FICTION_REPO);
    }

    void write_cell_layers()
    {
        // for each layer
        for (decltype(lyt.z()) z = 0; z <= lyt.z(); ++z)
        {
            write_via_cells();

            // open design layer
            os << qcad::OPEN_QCAD_LAYER;

            os << qcad::TYPE << "1\n";
            os << qcad::STATUS << "0\n";
            os << qcad::PSZ_DESCRIPTION << ((z == 0) ? "Ground Layer" : ("Crossing Layer " + std::to_string(z)))
               << '\n';

            // for each row
            for (decltype(lyt.y()) y = 0; y <= lyt.y(); ++y)
            {
                // for each cell
                for (decltype(lyt.x()) x = 0; x <= lyt.x(); ++x)
                {
                    const cell<Lyt> c{x, y, z};

                    // skip empty cells
                    if (!lyt.is_empty_cell(c))
                    {
                        write_cell(c, ps.create_inter_layer_via_cells);
                    }
                }
            }

            // close design layer
            os << qcad::CLOSE_QCAD_LAYER;
        }
    }

    qcad::color write_cell_colors(const cell<Lyt>& c)
    {
        const auto cell_type = lyt.get_cell_type(c);

        // colors for 4 clocks are supported exclusively
        qcad::color color{};
        if (qca_technology::is_input_cell(cell_type))
        {
            color.red   = qcad::COLOR_MIN;
            color.green = qcad::COLOR_MIN;
            color.blue  = qcad::COLOR_MAX;
        }
        else if (qca_technology::is_output_cell(cell_type))
        {
            color.red   = qcad::COLOR_MAX;
            color.green = qcad::COLOR_MAX;
            color.blue  = qcad::COLOR_MIN;
        }
        else if (qca_technology::is_constant_cell(cell_type))
        {
            color.red   = qcad::COLOR_MAX;
            color.green = qcad::COLOR_HALF;
            color.blue  = qcad::COLOR_MIN;
        }
        else
        {
            switch (lyt.get_clock_number(c))
            {
                case 0:
                {
                    color.red   = qcad::COLOR_MIN;
                    color.green = qcad::COLOR_MAX;
                    color.blue  = qcad::COLOR_MIN;
                    break;
                }
                case 1:
                {
                    color.red   = qcad::COLOR_MAX;
                    color.green = qcad::COLOR_MIN;
                    color.blue  = qcad::COLOR_MAX;
                    break;
                }
                case 2:
                {
                    color.red   = qcad::COLOR_MIN;
                    color.green = qcad::COLOR_MAX;
                    color.blue  = qcad::COLOR_MAX;
                    break;
                }
                case 3:
                {
                    color.red   = qcad::COLOR_MAX;
                    color.green = qcad::COLOR_MAX;
                    color.blue  = qcad::COLOR_MAX;
                    break;
                }
                default: break;
            }
        }
        os << fmt::format(qcad::COLOR, color.red, color.green, color.blue);

        return color;
    }

    void write_cell_mode(const cell<Lyt>& c, bool save_via_cells)
    {
        // handle cell mode
        os << qcad::CELL_OPTIONS_MODE;
        if (const auto mode = lyt.get_cell_mode(c); qca_technology::is_vertical_cell_mode(mode))
        {
            os << qcad::CELL_MODE_VERTICAL;

            if (save_via_cells)
            {
                via_layer_cells.push_back(c);  // save via cell for inter-layer
            }
        }
        else if (lyt.is_crossing_layer(c))
        {
            os << qcad::CELL_MODE_CROSSOVER;
        }
        else if (qca_technology::is_rotated_cell_mode(mode))
        {
            os << qcad::CELL_MODE_ROTATED;
        }
        else
        {
            os << qcad::CELL_MODE_NORMAL;
        }
    }

    void write_cell_function(const cell<Lyt>& c)
    {
        const auto cell_type = lyt.get_cell_type(c);

        // handle cell function
        os << qcad::CELL_FUNCTION;

        if (qca_technology::is_normal_cell(cell_type))
        {
            os << qcad::CELL_FUNCTION_NORMAL;
        }
        else if (qca_technology::is_constant_cell(cell_type))
        {
            os << qcad::CELL_FUNCTION_FIXED;
        }
        else if (qca_technology::is_input_cell(cell_type))
        {
            os << qcad::CELL_FUNCTION_INPUT;
        }
        else if (qca_technology::is_output_cell(cell_type))
        {
            os << qcad::CELL_FUNCTION_OUTPUT;
        }
    }

    void write_quantum_dots(const cell<Lyt>& c, const qcad::cell_pos pos)
    {
        const auto cell_type = lyt.get_cell_type(c);

        os << '\n' << qcad::NUMBER_OF_DOTS_4;

        // create quantum dots
        for (int i = 1; i > -2; i -= 2)
        {
            for (int j2 = 1; j2 > -2; j2 -= 2)
            {
                int j = i == 1 ? -j2 : j2;

                // open dot
                os << qcad::OPEN_CELL_DOT;

                os << qcad::X_POS << std::to_string(pos.x + (qcad::CELL_SIZE / 4.0f) * static_cast<float>(i)) << '\n';
                os << qcad::Y_POS << std::to_string(pos.y + (qcad::CELL_SIZE / 4.0f) * static_cast<float>(j)) << '\n';
                os << qcad::DIAMETER << qcad::DOT_SIZE << '\n';

                // determine charge
                os << qcad::CHARGE;
                if (!qca_technology::is_constant_cell(cell_type))
                {
                    os << qcad::CHARGE_8;
                }
                else if ((qca_technology::is_const_0_cell(cell_type) && std::abs(i + j) == 2) ||
                         (qca_technology::is_const_1_cell(cell_type) && std::abs(i + j) == 0))
                {
                    os << qcad::CHARGE_1;
                }
                else if ((qca_technology::is_const_0_cell(cell_type) && std::abs(i + j) == 0) ||
                         (qca_technology::is_const_1_cell(cell_type) && std::abs(i + j) == 2))
                {
                    os << qcad::CHARGE_0;
                }
                os << '\n';

                // determine spin
                os << qcad::SPIN;
                if (qca_technology::is_input_cell(cell_type) || qca_technology::is_output_cell(cell_type))
                {
                    os << qcad::NEGATIVE_SPIN;
                }
                else
                {
                    os << std::to_string(0.0f);
                }
                os << '\n';

                os << qcad::POTENTIAL << std::to_string(0.0f) << '\n';

                // close dot
                os << qcad::CLOSE_CELL_DOT;
            }
        }
    }

    void write_cell_name(const cell<Lyt>& c, const qcad::cell_pos pos, const qcad::color color)
    {
        const auto cell_type = lyt.get_cell_type(c);

        // override cell_name if cell is constant; if cell has a name
        if (auto cell_name = qca_technology::is_const_0_cell(cell_type) ? "-1.00" :
                             qca_technology::is_const_1_cell(cell_type) ? "1.00" :
                                                                          lyt.get_cell_name(c);
            !cell_name.empty())
        {
            // open label
            os << qcad::OPEN_QCAD_LABEL;
            os << qcad::OPEN_QCAD_STRETCHY_OBJECT;
            os << qcad::OPEN_QCAD_DESIGN_OBJECT;

            os << qcad::X_POS << std::to_string(pos.x) << '\n';
            os << qcad::Y_POS << std::to_string(pos.y - qcad::LABEL_Y_OFFSET) << '\n';
            os << qcad::B_SELECTED << qcad::SELECTED_FALSE << '\n';
            os << fmt::format(qcad::COLOR, color.red, color.green, color.blue);
            os << qcad::BOUNDING_BOX_X << std::to_string(pos.x - qcad::BB_X_OFFSET) << '\n';
            os << qcad::BOUNDING_BOX_Y << std::to_string(pos.y - qcad::BB_Y_OFFSET) << '\n';
            os << qcad::BOUNDING_BOX_CX
               << std::to_string(static_cast<float>(cell_name.size()) * qcad::CHARACTER_WIDTH + qcad::BB_CX_OFFSET)
               << '\n';
            os << qcad::BOUNDING_BOX_CY << qcad::BB_CY_OFFSET << '\n';

            os << qcad::CLOSE_QCAD_DESIGN_OBJECT;
            os << qcad::CLOSE_QCAD_STRETCHY_OBJECT;

            os << qcad::PSZ << cell_name << '\n';

            // close label
            os << qcad::CLOSE_QCAD_LABEL;
        }
    };

    void write_cell(const cell<Lyt>& c, const bool save_via_cells)
    {
        // open cell
        os << qcad::OPEN_QCAD_CELL;
        // open design object
        os << qcad::OPEN_QCAD_DESIGN_OBJECT;

        // calculate cell position
        const qcad::cell_pos pos{
            static_cast<float>(c.x * static_cast<decltype(c.x)>(qcad::CELL_DISTANCE) + qcad::X_Y_OFFSET),
            static_cast<float>(c.y * static_cast<decltype(c.y)>(qcad::CELL_DISTANCE) + qcad::X_Y_OFFSET)};

        // write cell position
        os << qcad::X_POS << std::to_string(pos.x) << '\n';
        os << qcad::Y_POS << std::to_string(pos.y) << '\n';
        os << qcad::B_SELECTED << qcad::SELECTED_FALSE << '\n';

        // write cell colors
        const auto color = write_cell_colors(c);

        // write cell bounding box
        os << qcad::BOUNDING_BOX_X << std::to_string(pos.x - qcad::CELL_SIZE / 2.0f) << '\n';
        os << qcad::BOUNDING_BOX_Y << std::to_string(pos.y - qcad::CELL_SIZE / 2.0f) << '\n';
        os << qcad::BOUNDING_BOX_CX << qcad::CELL_SIZE << '\n';
        os << qcad::BOUNDING_BOX_CY << qcad::CELL_SIZE << '\n';

        // close design object
        os << qcad::CLOSE_QCAD_DESIGN_OBJECT;

        // write cell options
        os << qcad::CELL_OPTIONS_CX << qcad::CELL_SIZE << '\n';
        os << qcad::CELL_OPTIONS_CY << qcad::CELL_SIZE << '\n';
        os << qcad::CELL_OPTIONS_DOT_DIAMETER << qcad::DOT_SIZE << '\n';
        os << qcad::CELL_OPTIONS_CLOCK << std::to_string(lyt.get_clock_number(c)) << '\n';

        // write cell mode
        write_cell_mode(c, save_via_cells);

        os << '\n';

        write_cell_function(c);

        write_quantum_dots(c, pos);

        write_cell_name(c, pos, color);

        // close cell
        os << qcad::CLOSE_QCAD_CELL;
    }

    void write_via_cells()
    {
        if (via_layer_cells.empty())
        {
            return;
        }

        // open via layer
        os << qcad::OPEN_QCAD_LAYER;

        os << qcad::TYPE << "1\n";
        os << qcad::STATUS << "0\n";
        os << qcad::PSZ_DESCRIPTION << "Via Layer " << std::to_string(via_counter++) << '\n';

        for (const auto& v : via_layer_cells)
        {
            write_cell(v, false);
        }

        // close design layer
        os << qcad::CLOSE_QCAD_LAYER;

        via_layer_cells.clear();
    }
};

}  // namespace detail

/**
 * Writes a cell-level QCA layout to a qca file that is used by QCADesigner (https://waluslab.ece.ubc.ca/qcadesigner/),
 * a physical simulator for the QCA technology platform.
 *
 * This overload uses an output stream to write into.
 *
 * @tparam Lyt Cell-level QCA layout type.
 * @param lyt The layout to be written.
 * @param os The output stream to write into.
 * @param ps Parameters.
 */
template <typename Lyt>
void write_qca_layout(const Lyt& lyt, std::ostream& os, write_qca_layout_params ps = {})
{
    static_assert(is_cell_level_layout_v<Lyt>, "Lyt is not a cell-level layout");
    static_assert(has_qca_technology_v<Lyt>, "Lyt must be a QCA layout");

    detail::write_qca_layout_impl p{lyt, os, ps};

    p.run();
}
/**
 * Writes a cell-level QCA layout to a qca file that is used by QCADesigner (https://waluslab.ece.ubc.ca/qcadesigner/),
 * a physical simulator for the QCA technology platform.
 *
 * This overload uses a file name to create and write into.
 *
 * @tparam Lyt Cell-level QCA layout type.
 * @param lyt The layout to be written.
 * @param filename The file name to create and write into. Should preferably use the `.qca` extension.
 * @param ps Parameters.
 */
template <typename Lyt>
void write_qca_layout(const Lyt& lyt, const std::string_view& filename, write_qca_layout_params ps = {})
{
    std::ofstream os{filename.data(), std::ofstream::out};

    if (!os.is_open())
    {
        throw std::ofstream::failure("could not open file");
    }

    write_qca_layout(lyt, os, ps);
    os.close();
}

}  // namespace fiction

#endif  // FICTION_WRITE_QCA_LAYOUT_HPP

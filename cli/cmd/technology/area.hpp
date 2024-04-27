//
// Created by marcel on 25.11.19.
//

#ifndef FICTION_CMD_AREA_HPP
#define FICTION_CMD_AREA_HPP

#include <fiction/technology/area.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>

#include <type_traits>
#include <variant>

namespace alice
{
/**
 * Prints the area usage of the current cell layout in nm². Uses standard values or given ones.
 */
class area_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit area_command(const environment::ptr& e) :
            command(e, "Prints the area usage (in nm²) of the current cell layout in store. Dimensions for each "
                       "cell use default values from QCADesigner (QCA), NMLSim (iNML), or SiQAD (SiDB) if no value is "
                       "provided.")
    {
        add_option("--width,-x", width, "Cell width in nm");
        add_option("--height,-y", height, "Cell height in nm");
        add_option("--hspace", hspace, "Horizontal cell spacing in nm");
        add_option("--vspace", vspace, "Vertical cell spacing in nm");
    }

  protected:
    /**
     * Function to perform the energy call. Prints estimated energy consumption for QCA-ONE library.
     */
    void execute() override
    {
        // reset area
        st = {};

        auto& s = store<fiction::cell_layout_t>();

        // error case: empty cell layout store
        if (s.empty())
        {
            env->out() << "[w] no cell layout in store" << std::endl;
            return;
        }

        auto lyt = s.current();

        const auto calculate_area = [this](auto&& lyt_ptr)
        {
            using tech = typename std::decay_t<decltype(lyt_ptr)>::element_type::technology;

            if (!is_set("width"))
            {
                width = tech::CELL_WIDTH;
            }
            if (!is_set("height"))
            {
                height = tech::CELL_HEIGHT;
            }
            if (!is_set("hspace"))
            {
                hspace = tech::CELL_HSPACE;
            }
            if (!is_set("vspace"))
            {
                vspace = tech::CELL_VSPACE;
            }

            fiction::area_params<tech, double> ps{width, height, hspace, vspace};

            fiction::area(*lyt_ptr, ps, &st);
        };

        std::visit(calculate_area, lyt);

        st.report(env->out());
    }

  private:
    /**
     * Layout area in nm².
     */
    fiction::area_stats<double> st{};
    /**
     * Width and height of each cell.
     */
    double width{0.0}, height{0.0};
    /**
     * Horizontal and vertical spacing between cells.
     */
    double hspace{0.0}, vspace{0.0};
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the area usage.
     */
    nlohmann::json log() const override
    {
        return {{"area (nm²)", st.area}};
    }
};

ALICE_ADD_COMMAND(area, "Technology")

}  // namespace alice

#endif  // FICTION_CMD_AREA_HPP

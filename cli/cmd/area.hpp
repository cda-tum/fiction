//
// Created by marcel on 25.11.19.
//

#ifndef FICTION_AREA_HPP
#define FICTION_AREA_HPP

#include <fiction/types.hpp>

#include <alice/alice.hpp>

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
        area = 0ul;

        auto& s = store<fiction::cell_layout_t>();

        // error case: empty cell layout store
        if (s.empty())
        {
            env->out() << "[w] no cell layout in store" << std::endl;
            return;
        }

        auto lyt = s.current();

        const auto calculate_area = [this](auto&& lyt)
        {
            using Tech = typename std::decay_t<decltype(lyt)>::element_type::technology;

            if (!is_set("width"))
            {
                width = Tech::cell_width;
            }
            if (!is_set("height"))
            {
                height = Tech::cell_height;
            }
            if (!is_set("hspace"))
            {
                hspace = Tech::cell_hspace;
            }
            if (!is_set("vspace"))
            {
                vspace = Tech::cell_vspace;
            }

            area = (static_cast<double>(lyt->x() + 1) * width + static_cast<double>(lyt->x()) * hspace) *
                   (static_cast<double>(lyt->y() + 1) * height + static_cast<double>(lyt->y()) * vspace);
        };

        std::visit(calculate_area, lyt);

        env->out() << fmt::format("[i] {} nm²", area) << std::endl;
    }

  private:
    /**
     * Layout area in nm².
     */
    double area{0.0};
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
        return {{"area (nm²)", area}};
    }
};

ALICE_ADD_COMMAND(area, "Technology")

}  // namespace alice

#endif  // FICTION_AREA_HP

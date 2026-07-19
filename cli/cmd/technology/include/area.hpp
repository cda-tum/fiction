//
// Created by marcel on 25.11.19.
//

#ifndef FICTION_CMD_AREA_HPP
#define FICTION_CMD_AREA_HPP

#include <fiction/technology/area.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

namespace alice
{

/**
 * Prints the area usage of the current cell layout in nm². Uses standard values or given ones.
 */
class area_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit area_command(const environment::ptr& e);

  protected:
    /**
     * Computes and reports layout area. Prints width, height, and area for the current cell layout.
     */
    void execute() override;

  private:
    /**
     * Layout area in nm².
     */
    fiction::area_stats st{};
    /**
     * Width of each cell.
     */
    double width{0.0};
    /**
     * Height of each cell.
     */
    double height{0.0};
    /**
     * Horizontal spacing between cells.
     */
    double hspace{0.0};
    /**
     * Vertical spacing between cells.
     */
    double vspace{0.0};
    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the area usage.
     */
    [[nodiscard]] nlohmann::json log() const override;
};

}  // namespace alice

#endif  // FICTION_CMD_AREA_HPP

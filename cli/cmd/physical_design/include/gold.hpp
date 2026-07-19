//
// Created by simon on 12.06.2024.
//

#ifndef FICTION_CMD_GOLD_HPP
#define FICTION_CMD_GOLD_HPP

#include <fiction/algorithms/physical_design/graph_oriented_layout_design.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <cstdint>

namespace alice
{

/**
 * Executes a physical design approach utilizing A* path finding in a search space graph.
 *
 * See include/fiction/algorithms/physical_design/graph_oriented_layout_design.hpp for more details.
 */
class gold_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit gold_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the physical design call. Generates a placed and routed FCN gate layout.
     */
    void execute() override;

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing information about the physical design process.
     */
    nlohmann::json log() const override;

  private:
    /**
     * Random seed used for random fanout substitution and random topological ordering in maximum-effort mode.
     */
    uint32_t seed{0u};
    /**
     * Parameters.
     */
    fiction::graph_oriented_layout_design_params ps{};
    /**
     * Statistics.
     */
    fiction::graph_oriented_layout_design_stats st{};

    template <typename Lyt>
    void graph_oriented_layout_design();
};

}  // namespace alice

#endif  // FICTION_CMD_GOLD_HPP

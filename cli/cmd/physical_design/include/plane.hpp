//
// Created by benjamin on 21.11.25.
//

#ifndef FICTION_CMD_PLANE_HPP
#define FICTION_CMD_PLANE_HPP

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>
#include <fiction/algorithms/physical_design/planar_layout_from_network_embedding.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <cstdint>

namespace alice
{
/**
 * Executes a physical design approach using a planar embedding of a logic network.
 *
 * See include/fiction/algorithms/physical_design/planar_layout_from_network_embedding.hpp for more details.
 */
class plane_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit plane_command(const environment::ptr& e);

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
     * Parameters for fanout substitution.
     */
    fiction::fanout_substitution_params fan_ps{};
    /**
     * Parameters for network path balancing.
     */
    fiction::network_balancing_params bal_ps{};
    /**
     * Parameters for planarization via node duplication.
     */
    fiction::node_duplication_planarization_params dup_ps{};
    /**
     * Parameters for planar layout generation from the network embedding.
     */
    fiction::planar_layout_from_network_embedding_params ps{};
    /**
     * Statistics collected during planar layout generation.
     */
    fiction::planar_layout_from_network_embedding_stats st{};
    /**
     * Number of clock phases used in the layout.
     */
    uint8_t num_clock_phases = 4u;
    /**
     * Determines whether primary output order is kept (0) or randomized (1).
     */
    uint32_t po_order{0u};
};

}  // namespace alice

#endif  // FICTION_CMD_PLANE_HPP

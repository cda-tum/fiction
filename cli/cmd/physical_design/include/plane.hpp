//
// Created by benjamin on 21.11.25.
//

#ifndef FICTION_CMD_PLANE_HPP
#define FICTION_CMD_PLANE_HPP

#include <fiction/algorithms/network_transformation/fanout_substitution.hpp>
#include <fiction/algorithms/network_transformation/network_balancing.hpp>
#include <fiction/algorithms/network_transformation/node_duplication_planarization.hpp>
#include <fiction/algorithms/physical_design/orthogonal.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <cstdint>

namespace alice
{

class plane_command final : public command
{
  public:
    explicit plane_command(const environment::ptr& e);

  protected:
    void execute() override;

    // Optional: enable JSON logging like ortho
    // nlohmann::json log() const override;

  private:
    fiction::fanout_substitution_params            fan_ps{};
    fiction::network_balancing_params              bal_ps{};
    fiction::node_duplication_planarization_params dup_ps{};
    fiction::orthogonal_physical_design_stats      orth_stats{};

    uint32_t seed{0u};
    uint32_t po_order{0u};
};

}  // namespace alice

#endif  // FICTION_CMD_PLANE_HPP

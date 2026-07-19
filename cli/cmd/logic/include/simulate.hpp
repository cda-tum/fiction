//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_CMD_SIMULATE_HPP
#define FICTION_CMD_SIMULATE_HPP

#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace alice
{

/**
 * Simulates network or gate-level layout into truth tables.
 *
 * Parts of this code are originally from: https://github.com/msoeken/cirkit/blob/cirkit3/cli/algorithms/simulate.hpp.
 */
class simulate_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit simulate_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the simulate call. Computes truth tables.
     */
    void execute() override;

    /**
     * Logs the resulting information in a log file.
     *
     * @return JSON object containing details about the truth tables.
     */
    nlohmann::json log() const override;

  private:
    /**
     * Stores truth tables.
     */
    std::vector<fiction::tt> tables;
    /**
     * Stores PO names in order.
     */
    std::vector<std::string> po_names;

    template <typename NtkOrLytVariant>
    void perform_simulation(const NtkOrLytVariant& network_or_layout_variant);
};

}  // namespace alice

#endif  // FICTION_CMD_SIMULATE_HPP

//
// Created by marcel on 08.11.19.
//

#ifndef FICTION_CMD_RANDOM_HPP
#define FICTION_CMD_RANDOM_HPP

#include <fiction/algorithms/network_transformation/network_conversion.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <mockturtle/generators/random_network.hpp>

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

namespace alice
{

/**
 * Generates a random logic network.
 */
class random_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit random_command(const environment::ptr& e);

  protected:
    /**
     * Function to perform the synth call. Generates a random logic network.
     */
    void execute() override;

  private:
    /**
     * Parameters.
     */
    mockturtle::random_network_generator_params_size ps{static_cast<uint64_t>(
        std::chrono::system_clock::now().time_since_epoch().count())};  // default random seed is current time
    /**
     * The actual network generator. Builds a logic_network from the generated random logic.
     *
     * @tparam Ntk Network type to generate.
     */
    template <typename Ntk, typename Generator>
    void generate(Generator gen) const
    {
        store<fiction::logic_network_t>().extend() =
            std::make_shared<Ntk>(fiction::convert_network<Ntk>(gen.generate()), std::to_string(ps.seed));
    }
    /**
     * Reset all flags, necessary for some reason... alice bug?
     */
    void reset_flags();
};

}  // namespace alice

#endif  // FICTION_CMD_RANDOM_HPP

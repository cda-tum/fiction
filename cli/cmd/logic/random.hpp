//
// Created by marcel on 08.11.19.
//

#ifndef FICTION_CMD_RANDOM_HPP
#define FICTION_CMD_RANDOM_HPP

#include <fiction/algorithms/network_transformation/network_conversion.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <mockturtle/generators/random_network.hpp>
#include <mockturtle/traits.hpp>

#include <memory>
#include <random>
#include <string>

namespace alice
{
/**
 * Generates a random logic network.
 */
class random_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit random_command(const environment::ptr& e) :
            command(e,
                    "Generates a random logic network. The random seed will be used as its name for reproducibility.")
    {
        add_flag("--aig,-a", "Create random AIG network");
        add_flag("--xag,-x", "Create random XAG network");
        add_flag("--mig,-m", "Create random MIG network");
        add_flag("--tec,-t", "Create random technology network");
        add_option("--rnd_num_inp,-n", ps.num_pis, "Number of primary inputs", true);
        add_option("--rnd_num_gates,-g", ps.num_gates, "Number of gates (excluding inverters and fan-outs)", true);
        add_option("--rnd_seed,-s", ps.seed, "Random seed");
    }

  protected:
    /**
     * Function to perform the synth call. Generates a random logic network.
     */
    void execute() override
    {
        if (!is_set("aig") && !is_set("xag") && !is_set("mig") && !is_set("tec"))
        {
            env->out() << "[e] at least one network type must be specified" << std::endl;
        }
        else
        {
            if (is_set("aig"))
            {
                generate<fiction::aig_nt>(mockturtle::random_aig_generator(ps));
            }
            if (is_set("xag"))
            {
                generate<fiction::xag_nt>(mockturtle::random_xag_generator(ps));
            }
            if (is_set("mig"))
            {
                generate<fiction::mig_nt>(mockturtle::random_mig_generator(ps));
            }
            if (is_set("tec"))
            {
                generate<fiction::tec_nt>(mockturtle::mixed_random_mig_generator(ps));
            }
        }

        reset_flags();
    }

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
    void reset_flags()
    {
        ps      = {};
        ps.seed = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
    }
};

ALICE_ADD_COMMAND(random, "Logic")

}  // namespace alice

#endif  // FICTION_CMD_RANDOM_HPP

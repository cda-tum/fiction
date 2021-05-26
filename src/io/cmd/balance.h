//
// Created by marcel on 24.10.19.
//

#ifndef FICTION_BALANCE_H
#define FICTION_BALANCE_H

#include "network_balancing.hpp"
#include "types.hpp"

#include <alice/alice.hpp>

#include <variant>

namespace alice
{
/**
 * Creates a hierarchy of the current logic network in store and performs a path balancing.
 */
class balance_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param env alice::environment that specifies stores etc.
     */
    explicit balance_command(const environment::ptr& env) :
            command(env,
                    "Subdivides edges of the current logic network in store so that all paths leading to "
                    "any vertex have the same length. Therefore, balance wire vertices are inserted. This does not "
                    "respect possible crossings in the network. Also, if a mapping from balance vertices to the "
                    "original subdivided edges is needed, one should call the hierarchy within an algorithm instead "
                    "of using this command because that information is lost after destruction of the hierarchy "
                    "object.")
    {
        add_flag("--unify_outputs,-u", ps.unify_outputs,
                 "Additionally, push all primary outputs down to the same level");
    }

  protected:
    /**
     * Function to perform the balance call. Inserts wire vertices to equalize paths.
     */
    void execute() override
    {
        auto& s = store<fiction::logic_network_t>();

        // error case: empty logic network store
        if (s.empty())
        {
            env->out() << "[w] no logic network in store" << std::endl;

            ps = {};
            return;
        }

        const auto balance = [this](auto&& net)
        { return std::make_shared<fiction::top_nt>(fiction::network_balancing<fiction::top_nt>(*net, ps)); };

        s.extend() = std::visit(balance, s.current());

        ps = {};
    }

  private:
    fiction::network_balancing_params ps{};
};

ALICE_ADD_COMMAND(balance, "Logic")

}  // namespace alice

#endif  // FICTION_BALANCE_H

//
// Created by marcel on 05.03.20.
//

#include "equivalence_checker.h"


equivalence_checker::equivalence_checker(fcn_gate_layout_ptr fgl)
        :
        miter{mockturtle::miter<logic_network::mig_nt>(fgl->extract(), fgl->network->mig)},
        layout{std::move(fgl)}
{}

equivalence_checker::equivalence_checker(fcn_gate_layout_ptr fgl1, fcn_gate_layout_ptr fgl2)
        :
        miter{mockturtle::miter<logic_network::mig_nt>(fgl1->extract(), fgl2->extract())},
        layout{std::move(fgl1)}
{}

equivalence_checker::equiv_result equivalence_checker::operator()()
{
    equiv_result result{};
    mockturtle::equivalence_checking_stats stats{};

    if (miter)
    {
        if (auto eq = mockturtle::equivalence_checking(*miter, {}, &stats); eq)
        {
            if (*eq)
            {
                // logically equivalent
                mockturtle::call_with_stopwatch(stats.time_total, [this, &result]
                {
                    const auto[cp, tp] = layout->critical_path_length_and_throughput();

                    if (tp == 1ul)
                    {
                        result.eq = equiv_result::eq_type::STRONG;
                    }
                    else
                    {
                        result.eq = equiv_result::eq_type::WEAK;
                    }

                    result.delay = tp;
                });
            }
            else
            {
                // not equivalent
                result.eq = equiv_result::eq_type::NONE;
                result.counter_example = stats.counter_example;
            }
        }
        else
        {
            std::cout << "[e] resource limit exceeded" << std::endl;
        }
    }
    else
    {
        std::cout << "[e] gate layout and specification have different numbers of primary inputs and/or outputs" << std::endl;
    }

    result.runtime = stats.time_total;

    return result;
}

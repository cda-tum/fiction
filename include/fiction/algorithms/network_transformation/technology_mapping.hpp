//
// Created by marcel on 06.09.23.
//

#ifndef FICTION_TECHNOLOGY_MAPPING_HPP
#define FICTION_TECHNOLOGY_MAPPING_HPP

#include "fiction/algorithms/network_transformation/network_conversion.hpp"
#include "fiction/technology/technology_mapping_library.hpp"
#include "fiction/types.hpp"
#include "fiction/utils/name_utils.hpp"

#include <lorina/genlib.hpp>
#include <mockturtle/algorithms/mapper.hpp>
#include <mockturtle/io/genlib_reader.hpp>
#include <mockturtle/utils/tech_library.hpp>

#include <cassert>
#include <sstream>
#include <vector>

namespace fiction
{

struct technology_mapping_params
{
    /**
     * mockturtle's mapper parameters.
     */
    mockturtle::map_params mapper_params{};

    /**
     * Enforce the application of at least one constant input to three-input gates.
     */
    bool decay{false};

    // 1-input functions

    bool inv{false};

    // 2-input functions

    /**
     * 2-input AND gate.
     */
    bool and2{false};
    /**
     * 2-input NAND gate.
     */
    bool nand2{false};
    /**
     * 2-input OR gate.
     */
    bool or2{false};
    /**
     * 2-input NOR gate.
     */
    bool nor2{false};
    /**
     * 2-input XOR gate.
     */
    bool xor2{false};
    /**
     * 2-input XNOR gate.
     */
    bool xnor2{false};
    /**
     * 2-input less-than gate.
     */
    bool lt2{false};
    /**
     * 2-input greater-than gate.
     */
    bool gt2{false};
    /**
     * 2-input less-or-equal gate.
     */
    bool le2{false};
    /**
     * 2-input greater-or-equal gate.
     */
    bool ge2{false};

    // 3-input functions

    /**
     * 3-input AND gate.
     */
    bool and3{false};
    /**
     * 3-input XOR-AND gate.
     */
    bool xor_and{false};
    /**
     * 3-input OR-AND gate.
     */
    bool or_and{false};
    /**
     * 3-input ONEHOT gate.
     */
    bool onehot{false};
    /**
     * 3-input MAJ gate.
     */
    bool maj3{false};
    /**
     * 3-input GAMBLE gate.
     */
    bool gamble{false};
    /**
     * 3-input DOT gate.
     */
    bool dot{false};
    /**
     * 3-input MUX gate.
     */
    bool mux{false};
    /**
     * 3-input AND-XOR gate.
     */
    bool and_xor{false};
};
/**
 * Auxiliary function to create technology mapping parameters for AND, OR, and NOT gates.
 *
 * @return Technology mapping parameters.
 */
[[nodiscard]] inline technology_mapping_params and_or_not() noexcept
{
    technology_mapping_params params{};

    params.inv  = true;
    params.and2 = true;
    params.or2  = true;

    return params;
}
/**
 * Auxiliary function to create technology mapping parameters for AND, OR, NOT, and MAJ gates.
 *
 * @return Technology mapping parameters.
 */
[[nodiscard]] inline technology_mapping_params and_or_not_maj() noexcept
{
    technology_mapping_params params{};

    params.inv  = true;
    params.and2 = true;
    params.or2  = true;
    params.maj3 = true;

    return params;
}
/**
 * Auxiliary function to create technology mapping parameters for AND, OR, NAND, NOR, XOR, XNOR, and NOT gates.
 *
 * @return Technology mapping parameters.
 */
[[nodiscard]] inline technology_mapping_params all_standard_2_input_functions() noexcept
{
    technology_mapping_params params{};

    params.inv = true;

    params.and2  = true;
    params.nand2 = true;
    params.or2   = true;
    params.nor2  = true;
    params.xor2  = true;
    params.xnor2 = true;

    return params;
}
/**
 * Auxiliary function to create technology mapping parameters for AND, OR, NAND, NOR, XOR, XNOR, LE, GE, LT, GT,
 * and NOT gates.
 *
 * @return Technology mapping parameters.
 */
[[nodiscard]] inline technology_mapping_params all_2_input_functions() noexcept
{
    technology_mapping_params params{};

    params.inv = true;

    params.and2  = true;
    params.nand2 = true;
    params.or2   = true;
    params.nor2  = true;
    params.xor2  = true;
    params.xnor2 = true;
    params.lt2   = true;
    params.gt2   = true;
    params.le2   = true;
    params.ge2   = true;

    return params;
}
/**
 * Auxiliary function to create technology mapping parameters for AND3, XOR_AND, OR_AND, ONEHOT, MAJ3, GAMBLE, DOT, MUX,
 * and AND_XOR gates.
 *
 * @return Technology mapping parameters.
 */
[[nodiscard]] inline technology_mapping_params all_standard_3_input_functions() noexcept
{
    technology_mapping_params params{};

    params.inv = true;

    params.and3    = true;
    params.xor_and = true;
    params.or_and  = true;
    params.onehot  = true;
    params.maj3    = true;
    params.gamble  = true;
    params.dot     = true;
    params.mux     = true;
    params.and_xor = true;

    return params;
}
/**
 * Auxiliary function to create technology mapping parameters for all supported standard functions.
 *
 * @return Technology mapping parameters.
 */
[[nodiscard]] inline technology_mapping_params all_supported_standard_functions() noexcept
{
    technology_mapping_params params{};

    params.inv = true;

    params.and2  = true;
    params.nand2 = true;
    params.or2   = true;
    params.nor2  = true;
    params.xor2  = true;
    params.xnor2 = true;

    params.and3    = true;
    params.xor_and = true;
    params.or_and  = true;
    params.onehot  = true;
    params.maj3    = true;
    params.gamble  = true;
    params.dot     = true;
    params.mux     = true;
    params.and_xor = true;

    return params;
}
/**
 * Auxiliary function to create technology mapping parameters for all supported functions.
 *
 * @return Technology mapping parameters.
 */
[[nodiscard]] inline technology_mapping_params all_supported_functions() noexcept
{
    technology_mapping_params params{};

    params.inv = true;

    params.and2  = true;
    params.nand2 = true;
    params.or2   = true;
    params.nor2  = true;
    params.xor2  = true;
    params.xnor2 = true;
    params.lt2   = true;
    params.gt2   = true;
    params.le2   = true;
    params.ge2   = true;

    params.and3    = true;
    params.xor_and = true;
    params.or_and  = true;
    params.onehot  = true;
    params.maj3    = true;
    params.gamble  = true;
    params.dot     = true;
    params.mux     = true;
    params.and_xor = true;

    return params;
}
/**
 * Statistics for technology mapping.
 */
struct technology_mapping_stats
{
    /**
     * Statistics for mockturtle's mapper.
     */
    mockturtle::map_stats mapper_stats{};
    /**
     * Report statistics.
     */
    void report() const
    {
        mapper_stats.report();
    }
};

namespace detail
{

template <typename Ntk>
class technology_mapping_impl
{
  public:
    technology_mapping_impl(const Ntk& src, const technology_mapping_params& ps, technology_mapping_stats& st) :
            ntk{src},
            params{ps},
            stats{st}
    {}

    tec_nt run()
    {
        const auto gate_library = set_up_gates();

        tec_nt mapped_ntk{};

        if (params.maj3 || params.dot || params.and3 || params.xor_and || params.or_and || params.onehot ||
            params.gamble || params.mux || params.and_xor)
        {
            mapped_ntk = perform_mapping<3>(gate_library);
        }
        else if (params.and2 || params.nand2 || params.or2 || params.nor2 || params.xor2 || params.xnor2)
        {
            mapped_ntk = perform_mapping<2>(gate_library);
        }
        else if (params.inv)
        {
            mapped_ntk = perform_mapping<1>(gate_library);
        }

        return mapped_ntk;
    }

  private:
    /**
     * Input network to be mapped.
     */
    Ntk ntk;
    /**
     * Technology mapping parameters.
     */
    technology_mapping_params params;
    /**
     * Technology mapping statistics.
     */
    technology_mapping_stats& stats;

    /**
     * Create a mockturtle gate library from the given parameters.
     *
     * @return A vector of mockturtle gates.
     */
    [[nodiscard]] std::vector<mockturtle::gate> set_up_gates() const
    {
        // gather library description
        std::stringstream library_stream{};

        library_stream << fiction::GATE_ZERO << fiction::GATE_ONE << fiction::GATE_BUF;

        // 1-input functions
        if (params.inv)
        {
            library_stream << fiction::GATE_INV;
        }
        // 2-input functions
        if (params.and2)
        {
            library_stream << fiction::GATE_AND2;
        }
        if (params.nand2)
        {
            library_stream << fiction::GATE_NAND2;
        }
        if (params.or2)
        {
            library_stream << fiction::GATE_OR2;
        }
        if (params.nor2)
        {
            library_stream << fiction::GATE_NOR2;
        }
        if (params.xor2)
        {
            library_stream << fiction::GATE_XOR2;
        }
        if (params.xnor2)
        {
            library_stream << fiction::GATE_XNOR2;
        }
        if (params.lt2)
        {
            library_stream << fiction::GATE_LT2;
        }
        if (params.gt2)
        {
            library_stream << fiction::GATE_GT2;
        }
        if (params.le2)
        {
            library_stream << fiction::GATE_LE2;
        }
        if (params.ge2)
        {
            library_stream << fiction::GATE_GE2;
        }
        // 3-input functions
        if (params.maj3)
        {
            if (!params.decay)
            {
                library_stream << fiction::GATE_MAJ3;
            }

            library_stream << fiction::DECAY_MAJ3;
        }
        if (params.dot)
        {
            if (!params.decay)
            {
                library_stream << fiction::GATE_DOT;
            }

            library_stream << fiction::DECAY_DOT;
        }
        if (params.and3)
        {
            if (!params.decay)
            {
                library_stream << fiction::GATE_AND3;
            }

            library_stream << fiction::DECAY_AND3;
        }
        if (params.xor_and)
        {
            if (!params.decay)
            {
                library_stream << fiction::GATE_XOR_AND;
            }

            library_stream << fiction::DECAY_XOR_AND;
        }
        if (params.or_and)
        {
            if (!params.decay)
            {
                library_stream << fiction::GATE_OR_AND;
            }

            library_stream << fiction::DECAY_OR_AND;
        }
        if (params.onehot)
        {
            if (!params.decay)
            {
                library_stream << fiction::GATE_ONEHOT;
            }

            library_stream << fiction::DECAY_ONEHOT;
        }
        if (params.gamble)
        {
            if (!params.decay)
            {
                library_stream << fiction::GATE_GAMBLE;
            }

            library_stream << fiction::DECAY_GAMBLE;
        }
        if (params.mux)
        {
            if (!params.decay)
            {
                library_stream << fiction::GATE_MUX;
            }

            library_stream << fiction::DECAY_MUX;
        }
        if (params.and_xor)
        {
            if (!params.decay)
            {
                library_stream << fiction::GATE_AND_XOR;
            }

            library_stream << fiction::DECAY_AND_XOR;
        }

        // generate technology library
        std::vector<mockturtle::gate> gates{};

        [[maybe_unused]] const auto result = lorina::read_genlib(library_stream, mockturtle::genlib_reader{gates});
        assert(result == lorina::return_code::success && "Could not parse technology library.");

        return gates;
    }
    /**
     * Perform technology mapping with the given number of inputs.
     *
     * @tparam NumInp Maximum input number of the gates in the technology library.
     * @param gates Technology library.
     * @return Mapped network.
     */
    template <unsigned NumInp>
    [[nodiscard]] tec_nt perform_mapping(const std::vector<mockturtle::gate>& gates) const noexcept
    {
        mockturtle::tech_library<NumInp> lib{gates};

        const auto mapped_ntk = mockturtle::map(ntk, lib, params.mapper_params, &stats.mapper_stats);

        tec_nt converted_ntk{};

        if (!stats.mapper_stats.mapping_error)
        {
            // convert network
            converted_ntk = convert_network<tec_nt>(mapped_ntk);
            restore_names(ntk, converted_ntk);
        }

        return converted_ntk;
    }
};

}  // namespace detail

/**
 * Performs technology mapping on the given network. Technology mapping is the process of replacing the gates in a
 * network with gates from a given technology library. This function utilizes `mockturtle::map` to perform the
 * technology mapping. This function is a wrapper around that interface to provide a more convenient usage.
 *
 * @tparam Ntk Input logic network type.
 * @param ntk Input logic network.
 * @param params Technology mapping parameters.
 * @param pst Technology mapping statistics.
 * @return Mapped network exclusively using gates from the provided library.
 */
template <typename Ntk>
[[nodiscard]] tec_nt technology_mapping(const Ntk& ntk, const technology_mapping_params& params = {},
                                        technology_mapping_stats* pst = nullptr)
{
    static_assert(mockturtle::is_network_type_v<Ntk>, "Ntk is not a network type");

    technology_mapping_stats        st{};
    detail::technology_mapping_impl p{ntk, params, st};

    const auto result = p.run();

    if (pst)
    {
        *pst = st;
    }

    return result;
}

}  // namespace fiction

#endif  // FICTION_TECHNOLOGY_MAPPING_HPP

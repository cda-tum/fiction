//
// Created by marcel on 13.07.21.
//

#ifndef FICTION_ENERGY_MODEL_HPP
#define FICTION_ENERGY_MODEL_HPP

#include "fiction/traits.hpp"

#include <mockturtle/traits.hpp>

#include <cstdint>
#include <ostream>

#if (PROGRESS_BARS)
#include <mockturtle/utils/progress_bar.hpp>
#endif

namespace fiction
{

struct energy_dissipation_stats
{
    /**
     * Energy dissipation information in meV for slow (25 GHz) and fast (100 GHz) clocking.
     */
    double slow{0.0}, fast{0.0};

    uint32_t unknown{0};

    void report(std::ostream& out = std::cout) const
    {
        out << fmt::format("[i] slow  (25 GHz): {:.2f} meV,\n[i] fast (100 GHz): {:.2f} meV\n", slow, fast);

        if (unknown > 0)
        {
            out << fmt::format(
                "[w] {} gates could not be identified, thus, the actual energy dissipation might be higher\n", unknown);
        }
    }
};

namespace detail
{

/**
 * Collection of energy values in meV for different QCA-ONE structures. The values are taken from \"An Energy-aware
 * Model for the Logic Synthesis of Quantum-Dot Cellular Automata\" by Frank Sill Torres, Robert Wille, Philipp Niemann,
 * and Rolf Drechsler in TCAD 2018.
 */
namespace qca_energy
{
/**
 * Energy dissipation (slow) of a normal wire.
 */
inline constexpr double WIRE_SLOW = 0.09;
/**
 * Energy dissipation (fast) of a normal wire.
 */
inline constexpr double WIRE_FAST = 0.82;
/**
 * Energy dissipation (slow) of a fan-out.
 */
inline constexpr double FANOUT_SLOW = 0.12;
/**
 * Energy dissipation (fast) of a fan-out.
 */
inline constexpr double FANOUT_FAST = 1.15;
/**
 * Energy dissipation (slow) of a straight inverter.
 */
inline constexpr double INVERTER_STRAIGHT_SLOW = 0.13;
/**
 * Energy dissipation (fast) of a straight inverter.
 */
inline constexpr double INVERTER_STRAIGHT_FAST = 1.19;
/**
 * Energy dissipation (slow) of a bent inverter.
 */
inline constexpr double INVERTER_BENT_SLOW = 0.10;
/**
 * Energy dissipation (fast) of a bent inverter.
 */
inline constexpr double INVERTER_BENT_FAST = 0.84;
/**
 * Energy dissipation (slow) of a crossing.
 */
inline constexpr double CROSSING_SLOW = 0.28;
/**
 * Energy dissipation (fast) of a crossing.
 */
inline constexpr double CROSSING_FAST = 2.57;
/**
 * Energy dissipation (slow) of an AND gate.
 */
inline constexpr double AND_SLOW = 0.47;
/**
 * Energy dissipation (fast) of an AND gate.
 */
inline constexpr double AND_FAST = 1.39;
/**
 * Energy dissipation (slow) of an OR gate.
 */
inline constexpr double OR_SLOW = 0.47;
/**
 * Energy dissipation (fast) of an AND gate.
 */
inline constexpr double OR_FAST = 1.39;
/**
 * Energy dissipation (slow) of a classic majority gate.
 */
inline constexpr double MAJORITY_SLOW = 0.65;
/**
 * Energy dissipation (fast) of a classic majority gate.
 */
inline constexpr double MAJORITY_FAST = 1.68;

}  // namespace qca_energy

template <typename Lyt>
class qca_energy_dissipation_impl
{
  public:
    qca_energy_dissipation_impl(const Lyt& src, energy_dissipation_stats& st) : lyt{src}, pst{st} {}

    void run()
    {
#if (PROGRESS_BARS)
        // initialize a progress bar
        mockturtle::progress_bar bar{static_cast<uint32_t>(lyt.size()), "[i] calculating energy dissipation: |{0}|"};
#endif
        lyt.foreach_node(
            [&, this](const auto& n, [[maybe_unused]] auto i)
            {
#if (PROGRESS_BARS)
                // update progress
                bar(i);
#endif
                if (!lyt.is_constant(n))
                {
                    if constexpr (fiction::has_is_fanout_v<Lyt>)
                    {
                        if (lyt.is_fanout(n))
                        {
                            pst.slow += qca_energy::FANOUT_SLOW;
                            pst.fast += qca_energy::FANOUT_FAST;
                            return true;
                        }
                    }
                    if constexpr (fiction::has_is_buf_v<Lyt>)
                    {
                        if (lyt.is_buf(n))
                        {
                            if (const auto t = lyt.get_tile(n); lyt.is_crossing_layer(t))
                            {
                                // skip crossing wires and include them in the ground check instead
                            }
                            // node has a crossing wire (crossing structure)
                            else if (auto at = lyt.above(t); t != at && lyt.is_buf(lyt.get_node(at)))
                            {
                                pst.slow += qca_energy::CROSSING_SLOW;
                                pst.fast += qca_energy::CROSSING_FAST;
                            }
                            // node is a regular wire
                            else
                            {
                                pst.slow += qca_energy::WIRE_SLOW;
                                pst.fast += qca_energy::WIRE_FAST;
                            }

                            return true;
                        }
                    }
                    if constexpr (fiction::has_is_inv_v<Lyt>)
                    {
                        if (lyt.is_inv(n))
                        {
                            // straight inverter
                            if (const auto t = lyt.get_tile(n); lyt.has_opposite_incoming_and_outgoing_signals(t))
                            {
                                pst.slow += qca_energy::INVERTER_STRAIGHT_SLOW;
                                pst.fast += qca_energy::INVERTER_STRAIGHT_FAST;
                            }
                            // bent inverter
                            else
                            {
                                pst.slow += qca_energy::INVERTER_BENT_SLOW;
                                pst.fast += qca_energy::INVERTER_BENT_FAST;
                            }

                            return true;
                        }
                    }
                    if constexpr (mockturtle::has_is_and_v<Lyt>)
                    {
                        if (lyt.is_and(n))
                        {
                            pst.slow += qca_energy::AND_SLOW;
                            pst.fast += qca_energy::AND_FAST;
                            return true;
                        }
                    }
                    if constexpr (mockturtle::has_is_or_v<Lyt>)
                    {
                        if (lyt.is_or(n))
                        {
                            pst.slow += qca_energy::OR_SLOW;
                            pst.fast += qca_energy::OR_FAST;
                            return true;
                        }
                    }
                    if constexpr (mockturtle::has_is_maj_v<Lyt>)
                    {
                        if (lyt.is_maj(n))
                        {
                            pst.slow += qca_energy::MAJORITY_SLOW;
                            pst.fast += qca_energy::MAJORITY_FAST;
                            return true;
                        }
                    }

                    ++pst.unknown;
                }

                return true;
            });
    }

  private:
    Lyt lyt;

    energy_dissipation_stats& pst;
};

}  // namespace detail

/**
 * Estimates the energy dissipation of a gate-level layout if it were to be converted to a QCA cell-level layout via the
 * QCA ONE gate library. This estimation was proposed in \"An Energy-aware Model for the Logic Synthesis of Quantum-Dot
 * Cellular Automata\" by Frank Sill Torres, Robert Wille, Philipp Niemann, and Rolf Drechsler in TCAD 2018.
 *
 * As done in that publication, energy dissipation values are being given in meV for slow (25 GHz) and fast (100 GHz)
 * clocking.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt The gate-level layout whose energy dissipation is to be calculated.
 * @param pst Statistics.
 */
template <typename Lyt>
void qca_energy_dissipation(const Lyt& lyt, energy_dissipation_stats* pst = nullptr)
{
    static_assert(fiction::is_gate_level_layout_v<Lyt>, "Lyt is not a gate-level layout");
    static_assert(mockturtle::has_foreach_gate_v<Lyt>, "Lyt does not implement the foreach_gate function");
    static_assert(mockturtle::has_is_constant_v<Lyt>, "Lyt does not implement the is_constant function");

    energy_dissipation_stats            st{};
    detail::qca_energy_dissipation_impl p{lyt, st};

    p.run();

    if (pst)
    {
        *pst = st;
    }
}

}  // namespace fiction

#endif  // FICTION_ENERGY_MODEL_HPP

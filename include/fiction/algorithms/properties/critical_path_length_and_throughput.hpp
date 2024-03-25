//
// Created by marcel on 05.11.21.
//

#ifndef FICTION_CRITICAL_PATH_LENGTH_AND_THROUGHPUT_HPP
#define FICTION_CRITICAL_PATH_LENGTH_AND_THROUGHPUT_HPP

#include "fiction/traits.hpp"

#include <mockturtle/traits.hpp>
#include <phmap.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <vector>

namespace fiction
{

/**
 * Critical path length and throughput storage struct.
 */
struct cp_and_tp
{
    /**
     * Length of the critical path in tiles.
     */
    uint64_t critical_path_length{0ull};
    /**
     * Throughput of the layout in clock cycles as \f$\frac{1}{x}\f$ where only \f$x\f$ is stored.
     */
    uint64_t throughput{0ull};
};

namespace detail
{

template <typename Lyt>
class critical_path_length_and_throughput_impl
{
  public:
    explicit critical_path_length_and_throughput_impl(const Lyt& src) : lyt{src} {}

    cp_and_tp run()
    {
        lyt.foreach_po(
            [this](const auto& po)
            {
                result.critical_path_length =
                    std::max(signal_delay(static_cast<tile<Lyt>>(po)).length, result.critical_path_length);
            });

        const auto max_diff =
            std::max_element(delay_cache.cbegin(), delay_cache.cend(),
                             [](const auto& i1, const auto& i2) { return i1.second.diff < i2.second.diff; });

        if (max_diff != delay_cache.cend())
        {
            result.throughput = max_diff->second.diff;
        }

        // give throughput in cycles, not in phases
        result.throughput /= lyt.num_clocks();

        // convert cycle difference to throughput, i.e., x where throughput == 1/x
        result.throughput++;

        return result;
    }

  private:
    /**
     * Gate-level layout.
     */
    Lyt lyt;
    /**
     * Result storage.
     */
    cp_and_tp result;

    struct path_info
    {
        path_info() = default;
        path_info(const uint64_t len, const uint64_t dly, const uint64_t dff) : length(len), delay(dly), diff(dff){};

        uint64_t length{0ull}, delay{0ull}, diff{0ull};
    };

    phmap::flat_hash_map<tile<Lyt>, path_info> delay_cache{};

    path_info signal_delay(const tile<Lyt> t) noexcept
    {
        if (lyt.is_empty_tile(t))
        {
            return {};
        }

        const auto idf = lyt.incoming_data_flow(t);
        if (idf.empty())
        {
            return {1, lyt.get_clock_number(t), 0};
        }
        if (const auto it = delay_cache.find(t); it != delay_cache.end())  // cache hit
        {
            return it->second;
        }

        // cache miss
        // fetch information about all incoming paths
        std::vector<path_info> infos{};

        std::transform(idf.cbegin(), idf.cend(), std::back_inserter(infos),
                       [this](const auto& in_tile) { return signal_delay(in_tile); });

        path_info dominant_path{};

        if (lyt.is_pi_tile(t))  // primary input to the circuit
        {
            infos.emplace_back(
                1ull, static_cast<uint64_t>((lyt.get_clock_number(t) + (lyt.num_clocks() - 1)) % lyt.num_clocks()),
                0ull);
        }

        if (infos.size() == 1)  // size cannot be 0
        {
            dominant_path = infos.front();
        }
        else  // fetch the highest delay and difference
        {
            // sort by path length
            std::sort(infos.begin(), infos.end(), [](const auto& i1, const auto& i2) { return i1.length < i2.length; });

            dominant_path.length = infos.back().length;
            dominant_path.delay  = infos.back().delay;
            dominant_path.diff =
                static_cast<uint64_t>(std::abs(static_cast<int64_t>(infos.back().delay - infos.front().delay)));
        }

        // incorporate self
        ++dominant_path.length;
        ++dominant_path.delay;

        // cache value for gates only
        if (!lyt.is_wire_tile(t))
        {
            delay_cache[t] = dominant_path;
        }

        return dominant_path;
    }
};

}  // namespace detail

/**
 * Computes the critical path length (CP) length and the throughput (TP) of a gate-level layout.
 *
 * The critical path length is defined as the longest path from any PI to any PO in tiles.
 *
 * The throughput is defined as \f$\frac{1}{x}\f$ where \f$x\f$ is the highest path length difference between any
 * sets of paths that lead to the same gate. This function provides only the denominator \f$x\f$, as the numerator is
 * always \f$1\f$. Furthermore, \f$x\f$ is given in clock cycles rather than clock phases because it is assumed that
 * a path length difference smaller than `lyt.num_clocks()` does not lead to any delay. Contrary, for any throughput
 * value \f$\frac{1}{x}\f$ with \f$x > 1\f$, the layout computes its represented Boolean function only every \f$x\f$
 * full clock cycles after the first inputs have been propagated through the design. Thereby, all PIs need to be
 * held constant for \f$x\f$ clock phases to ensure proper computation.
 *
 * For more information on the concept of throughput and delay see \"Synchronization of Clocked Field-Coupled Circuits\"
 * by F. Sill Torres, M. Walter, R. Wille, D. Große, and R. Drechsler in IEEE NANO 2018; or \"Design Automation for
 * Field-coupled Nanotechnologies\" by M. Walter, R. Wille, F. Sill Torres, and R. Drechsler published by Springer
 * Nature in 2022.
 *
 * The complexity of this function is \f$\mathcal{O}(|T|)\f$ where \f$T\f$ is the set of all occupied tiles in `lyt`.
 *
 * @tparam Lyt Gate-level layout type.
 * @param lyt The gate-level layout whose CP and TP are desired.
 * @return A struct containing the CP and TP.
 */
template <typename Lyt>
cp_and_tp critical_path_length_and_throughput(const Lyt& lyt)
{
    static_assert(is_gate_level_layout_v<Lyt>, "Lyt is not a gate layout type");

    detail::critical_path_length_and_throughput_impl p{lyt};

    return p.run();
}

}  // namespace fiction

#endif  // FICTION_CRITICAL_PATH_LENGTH_AND_THROUGHPUT_HPP

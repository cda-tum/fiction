//
// Created by marcel on 16.11.21.
//

#ifndef FICTION_FICTION_EXPERIMENTS_HPP
#define FICTION_FICTION_EXPERIMENTS_HPP

#include <fmt/format.h>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <experiments.hpp>  // mockturtle's experiments setup

namespace fiction_experiments
{

// NOLINTBEGIN(readability-identifier-naming)
// clang-format off

/* Trindade et al. 2016 benchmarks */
inline constexpr uint64_t mux21            = 0b0000000000000000000000000000000000000000000000000000000000000001;
inline constexpr uint64_t xor2_t           = 0b0000000000000000000000000000000000000000000000000000000000000010;
inline constexpr uint64_t xnor2            = 0b0000000000000000000000000000000000000000000000000000000000000100;
inline constexpr uint64_t par_gen          = 0b0000000000000000000000000000000000000000000000000000000000001000;
inline constexpr uint64_t half_adder       = 0b0000000000000000000000000000000000000000000000000000000000010000;
inline constexpr uint64_t full_adder       = 0b0000000000000000000000000000000000000000000000000000000000100000;
inline constexpr uint64_t par_check        = 0b0000000000000000000000000000000000000000000000000000000001000000;
inline constexpr uint64_t trindade16       = 0b0000000000000000000000000000000000000000000000000000000001111111;

/* Fontes et al. 2018 benchmarks */
inline constexpr uint64_t xor2_f           = 0b0000000000000000000000000000000000000000000000000000000010000000;
inline constexpr uint64_t one_bit_add_aoig = 0b0000000000000000000000000000000000000000000000000000000100000000;
inline constexpr uint64_t t                = 0b0000000000000000000000000000000000000000000000000000001000000000;
inline constexpr uint64_t t_5              = 0b0000000000000000000000000000000000000000000000000000010000000000;
inline constexpr uint64_t c17_f            = 0b0000000000000000000000000000000000000000000000000000100000000000;
inline constexpr uint64_t b1_r2            = 0b0000000000000000000000000000000000000000000000000001000000000000;
inline constexpr uint64_t majority         = 0b0000000000000000000000000000000000000000000000000010000000000000;
inline constexpr uint64_t majority_5_r1    = 0b0000000000000000000000000000000000000000000000000100000000000000;
inline constexpr uint64_t newtag           = 0b0000000000000000000000000000000000000000000000001000000000000000;
inline constexpr uint64_t clpl             = 0b0000000000000000000000000000000000000000000000010000000000000000;
inline constexpr uint64_t xor5_r1          = 0b0000000000000000000000000000000000000000000000100000000000000000;
inline constexpr uint64_t one_bit_add_maj  = 0b0000000000000000000000000000000000000000000001000000000000000000;
inline constexpr uint64_t cm82a_5          = 0b0000000000000000000000000000000000000000000010000000000000000000;
inline constexpr uint64_t two_bit_add_maj  = 0b0000000000000000000000000000000000000000000100000000000000000000;
inline constexpr uint64_t xor5_maj         = 0b0000000000000000000000000000000000000000001000000000000000000000;
inline constexpr uint64_t parity           = 0b0000000000000000000000000000000000000000010000000000000000000000;
inline constexpr uint64_t fontes18         = 0b0000000000000000000000000000000000000000011111111111111110000000;

/* ISCAS benchmarks */
inline constexpr uint64_t c17              = 0b0000000000000000000000000000000000000000100000000000000000000000;
inline constexpr uint64_t c432             = 0b0000000000000000000000000000000000000001000000000000000000000000;
inline constexpr uint64_t c499             = 0b0000000000000000000000000000000000000010000000000000000000000000;
inline constexpr uint64_t c880             = 0b0000000000000000000000000000000000000100000000000000000000000000;
inline constexpr uint64_t c1355            = 0b0000000000000000000000000000000000001000000000000000000000000000;
inline constexpr uint64_t c1908            = 0b0000000000000000000000000000000000010000000000000000000000000000;
inline constexpr uint64_t c2670            = 0b0000000000000000000000000000000000100000000000000000000000000000;
inline constexpr uint64_t c3540            = 0b0000000000000000000000000000000001000000000000000000000000000000;
inline constexpr uint64_t c5315            = 0b0000000000000000000000000000000010000000000000000000000000000000;
inline constexpr uint64_t c6288            = 0b0000000000000000000000000000000100000000000000000000000000000000;
inline constexpr uint64_t c7552            = 0b0000000000000000000000000000001000000000000000000000000000000000;
inline constexpr uint64_t iscas85          = 0b0000000000000000000000000000001111111111100000000000000000000000;

/* EPFL benchmarks */
inline constexpr uint64_t adder            = 0b0000000000000000000000000000010000000000000000000000000000000000;
inline constexpr uint64_t bar              = 0b0000000000000000000000000000100000000000000000000000000000000000;
inline constexpr uint64_t max              = 0b0000000000000000000000000001000000000000000000000000000000000000;
inline constexpr uint64_t sin              = 0b0000000000000000000000000010000000000000000000000000000000000000;
inline constexpr uint64_t cavlc            = 0b0000000000000000000000000100000000000000000000000000000000000000;
inline constexpr uint64_t ctrl             = 0b0000000000000000000000001000000000000000000000000000000000000000;
inline constexpr uint64_t dec              = 0b0000000000000000000000010000000000000000000000000000000000000000;
inline constexpr uint64_t i2c              = 0b0000000000000000000000100000000000000000000000000000000000000000;
inline constexpr uint64_t int2float        = 0b0000000000000000000001000000000000000000000000000000000000000000;
inline constexpr uint64_t priority         = 0b0000000000000000000010000000000000000000000000000000000000000000;
inline constexpr uint64_t router           = 0b0000000000000000000100000000000000000000000000000000000000000000;
inline constexpr uint64_t arbiter          = 0b0000000000000000001000000000000000000000000000000000000000000000;
inline constexpr uint64_t voter            = 0b0000000000000000010000000000000000000000000000000000000000000000;
inline constexpr uint64_t square           = 0b0000000000000000100000000000000000000000000000000000000000000000;
inline constexpr uint64_t log2             = 0b0000000000000001000000000000000000000000000000000000000000000000;
inline constexpr uint64_t sqrt             = 0b0000000000000010000000000000000000000000000000000000000000000000;
inline constexpr uint64_t multiplier       = 0b0000000000000100000000000000000000000000000000000000000000000000;
inline constexpr uint64_t epfl             = 0b0000000000000111111111111111110000000000000000000000000000000000;

inline constexpr uint64_t all              = 0b000000000000011111111111111111111111111111111111111111111111111;

/* extra functions */


static constexpr const std::array benchmarks {
    "trindade16/mux21", "trindade16/xor2", "trindade16/xnor2", "trindade16/par_gen",
    "trindade16/HA", "trindade16/FA", "trindade16/par_check",

    "fontes18/xor", "fontes18/1bitAdderAOIG", "fontes18/t", "fontes18/t_5", "fontes18/c17", "fontes18/b1_r2",
    "fontes18/majority", "fontes18/majority_5_r1", "fontes18/newtag", "fontes18/clpl", "fontes18/xor5_r1",
    "fontes18/1bitAdderMaj", "fontes18/cm82a_5", "fontes18/2bitAdderMaj", "fontes18/xor5Maj", "fontes18/parity",

    "ISCAS85/c17", "ISCAS85/c432", "ISCAS85/c499", "ISCAS85/c880", "ISCAS85/c1355", "ISCAS85/c1908", "ISCAS85/c2670",
    "ISCAS85/c3540", "ISCAS85/c5315", "ISCAS85/c6288", "ISCAS85/c7552",

    "EPFL/adder", "EPFL/bar", "EPFL/max", "EPFL/sin", "EPFL/cavlc", "EPFL/ctrl", "EPFL/dec", "EPFL/i2c",
    "EPFL/int2float", "EPFL/priority", "EPFL/router", "EPFL/arbiter", "EPFL/voter", "EPFL/square", "EPFL/log2",
    "EPFL/sqrt", "EPFL/multiplier",
    };

// clang-format on
// NOLINTEND(readability-identifier-naming)

inline std::vector<std::string> trindade16_benchmarks(uint64_t selection = trindade16)
{
    std::vector<std::string> result{};
    for (uint64_t i = 0u; i < 7u; ++i)
    {
        if (((selection >> i) & 1u) != 0u)
        {
            result.emplace_back(benchmarks.at(i));
        }
    }

    return result;
}

inline std::vector<std::string> fontes18_benchmarks(uint64_t selection = fontes18)
{
    std::vector<std::string> result{};
    for (uint64_t i = 7u; i < 23u; ++i)
    {
        if (((selection >> i) & 1u) != 0u)
        {
            result.emplace_back(benchmarks.at(i));
        }
    }

    return result;
}

inline std::vector<std::string> iscas85_benchmarks(uint64_t selection = iscas85)
{
    std::vector<std::string> result{};
    for (uint64_t i = 23u; i < 34u; ++i)
    {
        if (((selection >> i) & 1u) != 0u)
        {
            result.emplace_back(benchmarks.at(i));
        }
    }

    return result;
}

inline std::vector<std::string> epfl_benchmarks(uint64_t selection = epfl)
{
    std::vector<std::string> result{};
    for (uint64_t i = 34u; i < 51u; ++i)
    {
        if (((selection >> i) & 1u) != 0u)
        {
            result.emplace_back(benchmarks.at(i));
        }
    }

    return result;
}

inline std::vector<std::string> all_benchmarks(uint64_t selection = all)
{
    std::vector<std::string> result{};
    for (uint64_t i = 0u; i < 51u; ++i)
    {
        if (((selection >> i) & 1u) != 0u)
        {
            result.emplace_back(benchmarks.at(i));
        }
    }

    return result;
}

std::string benchmark_path(const std::string& benchmark_name, const std::string& benchmark_folder = "../benchmarks")
{
    return fmt::format("{}{}/{}.v", EXPERIMENTS_PATH, benchmark_folder, benchmark_name);
}

}  // namespace fiction_experiments

#endif  // FICTION_FICTION_EXPERIMENTS_HPP

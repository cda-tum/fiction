//
// Created by marcel on 16.11.21.
//

#ifndef FICTION_FICTION_EXPERIMENTS_HPP
#define FICTION_FICTION_EXPERIMENTS_HPP

#include <fmt/format.h>

#include <cstdint>
#include <string>
#include <vector>

#include <experiments.hpp>  // mockturtle's experiments setup

namespace fiction_experiments
{

// clang-format off

/* Trindade et al. 2016 benchmarks */
static constexpr uint64_t mux21            = 0b0000000000000000000000000000000000000000000000000000000000000001;
static constexpr uint64_t xor2_t           = 0b0000000000000000000000000000000000000000000000000000000000000010;
static constexpr uint64_t xnor2            = 0b0000000000000000000000000000000000000000000000000000000000000100;
static constexpr uint64_t par_gen          = 0b0000000000000000000000000000000000000000000000000000000000001000;
static constexpr uint64_t half_adder       = 0b0000000000000000000000000000000000000000000000000000000000010000;
static constexpr uint64_t full_adder       = 0b0000000000000000000000000000000000000000000000000000000000100000;
static constexpr uint64_t par_check        = 0b0000000000000000000000000000000000000000000000000000000001000000;
static constexpr uint64_t trindade16       = 0b0000000000000000000000000000000000000000000000000000000001111111;

/* Fontes et al. 2018 benchmarks */
static constexpr uint64_t xor2_f           = 0b0000000000000000000000000000000000000000000000000000000010000000;
static constexpr uint64_t one_bit_add_aoig = 0b0000000000000000000000000000000000000000000000000000000100000000;
static constexpr uint64_t t                = 0b0000000000000000000000000000000000000000000000000000001000000000;
static constexpr uint64_t t_5              = 0b0000000000000000000000000000000000000000000000000000010000000000;
static constexpr uint64_t c17              = 0b0000000000000000000000000000000000000000000000000000100000000000;
static constexpr uint64_t b1_r2            = 0b0000000000000000000000000000000000000000000000000001000000000000;
static constexpr uint64_t majority         = 0b0000000000000000000000000000000000000000000000000010000000000000;
static constexpr uint64_t majority_5_r1    = 0b0000000000000000000000000000000000000000000000000100000000000000;
static constexpr uint64_t newtag           = 0b0000000000000000000000000000000000000000000000001000000000000000;
static constexpr uint64_t clpl             = 0b0000000000000000000000000000000000000000000000010000000000000000;
static constexpr uint64_t xor5_r1          = 0b0000000000000000000000000000000000000000000000100000000000000000;
static constexpr uint64_t one_bit_add_maj  = 0b0000000000000000000000000000000000000000000001000000000000000000;
static constexpr uint64_t cm82a_5          = 0b0000000000000000000000000000000000000000000010000000000000000000;
static constexpr uint64_t two_bit_add_maj  = 0b0000000000000000000000000000000000000000000100000000000000000000;
static constexpr uint64_t xor5_maj         = 0b0000000000000000000000000000000000000000001000000000000000000000;
static constexpr uint64_t parity           = 0b0000000000000000000000000000000000000000010000000000000000000000;
static constexpr uint64_t fontes18         = 0b0000000000000000000000000000000000000000011111111111111110000000;

static constexpr uint64_t all              = 0b0000000000000000000000000000000000000000011111111111111111111111;

static const char* benchmarks[] = {
    "trindade16/mux21", "trindade16/xor2", "trindade16/xnor2", "trindade16/par_gen",
    "trindade16/HA", "trindade16/FA", "trindade16/par_check",

    "fontes18/xor", "fontes18/1bitAdderAOIG", "fontes18/t", "fontes18/t_5", "fontes18/c17", "fontes18/b1_r2",
    "fontes18/majority", "fontes18/majority_5_r1", "fontes18/newtag", "fontes18/clpl", "fontes18/xor5_r1",
    "fontes18/1bitAdderMaj", "fontes18/cm82a_5", "fontes18/2bitAdderMaj", "fontes18/xor5Maj", "fontes18/parity"};

// clang-format on

std::vector<std::string> trindade16_benchmarks(uint64_t selection = trindade16)
{
    std::vector<std::string> result{};
    for (uint32_t i = 0u; i < 7u; ++i)
    {
        if ((selection >> i) & 1)
        {
            result.emplace_back(benchmarks[i]);
        }
    }

    return result;
}

std::vector<std::string> fontes18_benchmarks(uint64_t selection = fontes18)
{
    std::vector<std::string> result{};
    for (uint32_t i = 7u; i < 23u; ++i)
    {
        if ((selection >> i) & 1)
        {
            result.emplace_back(benchmarks[i]);
        }
    }

    return result;
}

std::vector<std::string> all_benchmarks(uint64_t selection = all)
{
    std::vector<std::string> result{};
    for (uint32_t i = 0u; i < 23u; ++i)
    {
        if ((selection >> i) & 1)
        {
            result.emplace_back(benchmarks[i]);
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

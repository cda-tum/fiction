//
// Created by marcel on 15.01.23.
//

#ifndef FICTION_EXECUTION_UTILS_HPP
#define FICTION_EXECUTION_UTILS_HPP

#include <version>  // defines the feature-test macros the guard below reads

// if the library supports parallel algorithms and execution policies
//
// Clang pins __GNUC__ at 4 for GNU compatibility and reports its own version in __clang_major__
// instead, so it has to be exempt from the GCC version term rather than fail it.
#if (defined(__cpp_lib_parallel_algorithm) || defined(__cpp_lib_execution)) && \
    (!defined(__GNUC__) || defined(__clang__) || __GNUC__ > 9)  // GCC version >= 10

#include <execution>  // include execution policies only if the C++ library supports them

// define the execution policies as macros

/**
 * Sequential execution policy for STL algorithms.
 *
 * @note This macro automatically detects whether the C++ library supports execution policies and whether the compiler
 * is able to compile them. If not, the macro defaults to nothing.
 */
#define FICTION_EXECUTION_POLICY_SEQ std::execution::seq,
/**
 * Parallel execution policy for STL algorithms.
 *
 * @note This macro automatically detects whether the C++ library supports execution policies and whether the compiler
 * is able to compile them. If not, the macro defaults to nothing.
 */
#define FICTION_EXECUTION_POLICY_PAR std::execution::par,
/**
 * Parallel unsequenced execution policy for STL algorithms.
 *
 * @note This macro automatically detects whether the C++ library supports execution policies and whether the compiler
 * is able to compile them. If not, the macro defaults to nothing.
 */
#define FICTION_EXECUTION_POLICY_PAR_UNSEQ std::execution::par_unseq,

#else

/**
 * Sequential execution policy for STL algorithms.
 *
 * @note This macro automatically detects whether the C++ library supports execution policies and whether the compiler
 * is able to compile them. If not, the macro defaults to nothing.
 */
#define FICTION_EXECUTION_POLICY_SEQ
/**
 * Parallel execution policy for STL algorithms.
 *
 * @note This macro automatically detects whether the C++ library supports execution policies and whether the compiler
 * is able to compile them. If not, the macro defaults to nothing.
 */
#define FICTION_EXECUTION_POLICY_PAR
/**
 * Parallel unsequenced execution policy for STL algorithms.
 *
 * @note This macro automatically detects whether the C++ library supports execution policies and whether the compiler
 * is able to compile them. If not, the macro defaults to nothing.
 */
#define FICTION_EXECUTION_POLICY_PAR_UNSEQ

#endif

#endif  // FICTION_EXECUTION_UTILS_HPP

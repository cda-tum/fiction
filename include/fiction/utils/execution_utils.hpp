//
// Created by marcel on 15.01.23.
//

#ifndef FICTION_EXECUTION_UTILS_HPP
#define FICTION_EXECUTION_UTILS_HPP

// if the library supports parallel algorithms and execution policies
#if (__cpp_lib_parallel_algorithm || __cpp_lib_execution) && (!__GNUC__ || __GNUC__ > 9)  // GCC Version >= 9

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
 * @note This macro automatically detetcs whether the C++ library supports execution policies and whether the compiler
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

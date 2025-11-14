//
// Created by marcel on 05.04.19.
//

#ifndef FICTION_VERSION_INFO_HPP_IN
#define FICTION_VERSION_INFO_HPP_IN

namespace fiction
{

/**
 * Version string.
 */
inline constexpr const char* FICTION_VERSION = "fiction v0.6.12";
/**
 * Repository string.
 */
inline constexpr const char* FICTION_REPO = "https://github.com/cda-tum/fiction";
/**
 * Compiled at the stored date.
 */
inline constexpr const char* COMPILED_DATE = __DATE__;
/**
 * Compiled at the stored time.
 */
inline constexpr const char* COMPILED_TIME = __TIME__;

}  // namespace fiction

#endif  // FICTION_VERSION_INFO_HPP_IN

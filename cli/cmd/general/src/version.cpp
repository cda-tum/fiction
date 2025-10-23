//
// Created by marcel on 24.10.19.
//

#include "cmd/general/include/version.hpp"

#include "utils/version_info.hpp"

#include <alice/alice.hpp>
#include <fmt/format.h>

namespace alice
{

version_command::version_command(const environment::ptr& e) :
        command(e, "Outputs the version string as well as build time and date.")
{}

void version_command::execute()
{
    env->out() << fmt::format("[i] {} - compiled on {} at {}\n", fiction::FICTION_VERSION, fiction::COMPILED_DATE,
                              fiction::COMPILED_TIME)
               << fmt::format("[i] {}\n", fiction::FICTION_REPO);
}

}  // namespace alice

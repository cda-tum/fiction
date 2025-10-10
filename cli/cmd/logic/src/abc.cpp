//
// Created by marcel on 04.12.24.
//

#include "cmd/logic/include/abc.hpp"

#if (FICTION_ABC)

#include <fiction/io/network_reader.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <mockturtle/io/write_aiger.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>

namespace alice
{

abc_command::abc_command(const environment::ptr& e) :
        command(e,
                "ABC callback for logic synthesis and optimization. The command `abc -c \"<command>\"` will launch ABC "
                "and execute the given command string. Internally, it writes the current AIG or XAG to a temporary "
                "AIGER file, have ABC parse this file as an AIG using `read <filename>; strash`, and then execute the "
                "given command string. Finally, ABC will write the result back to the temporary file using "
                "`write_aiger -s <filename>`, which will be read back into the fiction CLI as a new AIG.")
{
    add_option("--command,-c", abc_command_str, "Command to pass to ABC")->required();
    add_flag("--dont_read,-r", "Do not read the network into ABC, only execute the command");
    add_flag("--dont_strash,-s", "Do not strash the network before executing the command");
    add_flag("--dont_write,-w", "Do not write the network back to the CLI after executing the command");
}

void abc_command::execute()
{
    if constexpr (ABC == nullptr)
    {
        env->out() << "[e] `ABC_EXECUTABLE` environment variable is not set. Cannot launch ABC." << std::endl;
        return;
    }

    const auto& s = store<fiction::logic_network_t>();

    // error case: empty logic network store
    if (s.empty())
    {
        env->out() << "[w] no logic network in store" << std::endl;
        return;
    }

    constexpr auto is_tech_ntk = []([[maybe_unused]] auto&& ntk_ptr) -> bool
    { return std::is_same_v<typename std::decay_t<decltype(ntk_ptr)>::element_type, fiction::tec_nt>; };

    if (std::visit(is_tech_ntk, s.current()))
    {
        env->out() << "[w] ABC callback does not support technology networks. Use AIGs or XAGs instead." << std::endl;
        return;
    }

    constexpr auto is_maj_ntk = []([[maybe_unused]] auto&& ntk_ptr) -> bool
    { return std::is_same_v<typename std::decay_t<decltype(ntk_ptr)>::element_type, fiction::mig_nt>; };

    if (std::visit(is_maj_ntk, s.current()))
    {
        env->out() << "[w] ABC callback does not support MIGs. Use AIGs or XAGs instead." << std::endl;
        return;
    }

    try
    {
        const auto fiction_network_path = write_current_network_to_temp_file();
        const auto abc_network_path     = abc_output_temp_file();

        const auto read_cmd =
            fmt::format("{}", is_set("dont_read") ? "" : fmt::format("read {}; ", fiction_network_path.string()));
        const auto strash_cmd = is_set("dont_strash") ? "" : "strash; ";
        const auto write_cmd =
            is_set("dont_write") ? "" : fmt::format("; write_aiger -s {}", abc_network_path.string());

        // call the ABC binary
        const auto abc_call =
            fmt::format("{0} -q \"{1}{2}{3}{4}\"", ABC, read_cmd, strash_cmd, abc_command_str, write_cmd);

        if (const auto ret = std::system(abc_call.c_str()); ret != 0)
        {
            env->out() << "[e] Failed to execute ABC command." << std::endl;
            return;
        }

        fiction::network_reader<fiction::aig_ptr> reader{abc_network_path.string(), env->out()};

        store<fiction::logic_network_t>().extend() = reader.get_networks().front();
    }
    catch (const std::exception& e)
    {
        env->out() << fmt::format("[e] {}", e.what()) << std::endl;
    }
}

std::filesystem::path abc_command::get_temp_directory()
{
    // Check common environment variables for the temporary directory
    if (auto* const tempdir = std::getenv("TMPDIR"); tempdir != nullptr)
    {
        return {tempdir};  // Linux + macOS
    }
    if (auto* const temp = std::getenv("TEMP"); temp != nullptr)
    {
        return {temp};  // Windows
    }
    if (auto* const tmp = std::getenv("TMP"); tmp != nullptr)
    {
        return {tmp};  // Windows fallback
    }

    // fallback: standard platform-specific defaults
#ifdef _WIN32
    return {"C:\\Windows\\Temp"};
#else
    return {"/tmp"};
#endif
}

std::filesystem::path abc_command::get_temp_fiction_directory()
{
    // get the temporary directory
    const auto temp_dir = get_temp_directory();

    // define the "fiction" directory path
    const auto fiction_dir = temp_dir / "fiction";

    // create the directory if it doesn't exist
    if (!std::filesystem::exists(fiction_dir))
    {
        if (!std::filesystem::create_directory(fiction_dir))
        {
            throw std::runtime_error("Failed to create directory: " + fiction_dir.string());
        }
    }

    return fiction_dir;
}

std::filesystem::path abc_command::write_current_network_to_temp_file() const
{
    const auto write = [](auto&& ntk_ptr) -> std::filesystem::path
    {
        const auto temp_file = get_temp_fiction_directory() / fmt::format("{}.aig", ntk_ptr->get_network_name());

        mockturtle::write_aiger(*ntk_ptr, temp_file.string());

        return temp_file;
    };

    return std::visit(write, store<fiction::logic_network_t>().current());
}

std::filesystem::path abc_command::abc_output_temp_file() const
{
    const auto get_name = [](auto&& ntk_ptr) -> std::string { return ntk_ptr->get_network_name(); };

    return get_temp_fiction_directory() /
           fmt::format("{}.aig", std::visit(get_name, store<fiction::logic_network_t>().current()));
}

}  // namespace alice

#endif  // FICTION_ABC

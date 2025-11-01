//
// Created by marcel on 04.12.24.
//

#if (FICTION_ABC)

#include "cmd/logic/include/abc.hpp"

#include "stores.hpp"  // NOLINT(misc-include-cleaner)

#include <fiction/io/network_reader.hpp>
#include <fiction/types.hpp>

#include <alice/alice.hpp>
#include <fmt/format.h>
#include <mockturtle/io/write_aiger.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

// Platform-specific headers for safe process execution
#ifdef _WIN32
#include <windows.h>
#else
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
extern char** environ;
#endif

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
        env->out() << "[e] `ABC_EXECUTABLE` environment variable is not set. Cannot launch ABC.\n";
        return;
    }

    const auto& s = store<fiction::logic_network_t>();

    // error case: empty logic network store
    if (s.empty())
    {
        env->out() << "[w] no logic network in store\n";
        return;
    }

    constexpr auto is_tech_ntk = []([[maybe_unused]] auto&& ntk_ptr) -> bool
    { return std::is_same_v<typename std::decay_t<decltype(ntk_ptr)>::element_type, fiction::tec_nt>; };

    if (std::visit(is_tech_ntk, s.current()))
    {
        env->out() << "[w] ABC callback does not support technology networks. Use AIGs or XAGs instead.\n";
        return;
    }

    constexpr auto is_maj_ntk = []([[maybe_unused]] auto&& ntk_ptr) -> bool
    { return std::is_same_v<typename std::decay_t<decltype(ntk_ptr)>::element_type, fiction::mig_nt>; };

    if (std::visit(is_maj_ntk, s.current()))
    {
        env->out() << "[w] ABC callback does not support MIGs. Use AIGs or XAGs instead.\n";
        return;
    }

    try
    {
        const auto fiction_network_path = write_current_network_to_temp_file();
        const auto abc_network_path     = abc_output_temp_file();

        // Build the ABC command string without shell metacharacters
        std::string abc_cmd_sequence;

        if (!is_set("dont_read"))
        {
            abc_cmd_sequence += fmt::format("read {}; ", fiction_network_path.string());
        }

        if (!is_set("dont_strash"))
        {
            abc_cmd_sequence += "strash; ";
        }

        abc_cmd_sequence += abc_command_str;

        if (!is_set("dont_write"))
        {
            abc_cmd_sequence += fmt::format("; write_aiger -s {}", abc_network_path.string());
        }

        // Safely execute ABC without invoking a shell
        const std::vector<std::string> abc_args = {"-q", abc_cmd_sequence};

        if (const auto ret = safe_execute_abc(ABC, abc_args); ret != 0)
        {
            env->out() << "[e] Failed to execute ABC command.\n";
            return;
        }

        fiction::network_reader<fiction::aig_ptr> reader{abc_network_path.string(), env->out()};

        store<fiction::logic_network_t>().extend() = reader.get_networks().front();
    }
    catch (const std::exception& e)
    {
        env->out() << fmt::format("[e] {}\n", e.what());
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

int abc_command::safe_execute_abc(const std::string& abc_path, const std::vector<std::string>& abc_args)
{
#ifdef _WIN32
    // Windows implementation using CreateProcess
    std::string command_line = abc_path;
    for (const auto& arg : abc_args)
    {
        command_line += " \"";
        // Escape quotes in the argument
        for (const char c : arg)
        {
            if (c == '"')
            {
                command_line += "\\\"";
            }
            else if (c == '\\')
            {
                command_line += "\\\\";
            }
            else
            {
                command_line += c;
            }
        }
        command_line += "\"";
    }

    STARTUPINFOA        si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    // CreateProcessA requires a non-const char*, so we need to copy the string
    std::vector<char> cmd_line(command_line.begin(), command_line.end());
    cmd_line.push_back('\0');

    if (!CreateProcessA(nullptr,          // Application name (use command line)
                        cmd_line.data(),  // Command line
                        nullptr,          // Process security attributes
                        nullptr,          // Thread security attributes
                        FALSE,            // Inherit handles
                        0,                // Creation flags
                        nullptr,          // Environment
                        nullptr,          // Current directory
                        &si,              // Startup info
                        &pi))             // Process information
    {
        return -1;  // Failed to create process
    }

    // Wait for the process to complete
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    // Clean up handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return static_cast<int>(exit_code);
#else
    // POSIX implementation using posix_spawn
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>(abc_path.c_str()));

    // Store string copies to ensure pointers remain valid
    std::vector<std::string> arg_storage;
    arg_storage.reserve(abc_args.size());

    for (const auto& arg : abc_args)
    {
        arg_storage.push_back(arg);
        argv.push_back(const_cast<char*>(arg_storage.back().c_str()));
    }
    argv.push_back(nullptr);

    pid_t     pid          = 0;
    const int spawn_result = posix_spawn(&pid, abc_path.c_str(), nullptr, nullptr, argv.data(), environ);

    if (spawn_result != 0)
    {
        return -1;  // Failed to spawn process
    }

    // Wait for the child process to complete
    int status = 0;
    if (waitpid(pid, &status, 0) == -1)
    {
        return -1;  // Failed to wait for process
    }

    // Return the exit code
    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }

    return -1;  // Process terminated abnormally
#endif
}

}  // namespace alice

#endif  // FICTION_ABC

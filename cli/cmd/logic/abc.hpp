//
// Created by marcel on 04.12.24.
//

#ifndef FICTION_CMD_ABC_HPP
#define FICTION_CMD_ABC_HPP

#if (FICTION_ABC)

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
/**
 * Callback command for ABC. Calls the ABC binary with a given command string and the current logic network in store.
 *
 * Internally, ABC performs a `read <filename>; strash; <command>; write_aiger <filename>` operation.
 */
class abc_command : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit abc_command(const environment::ptr& e) :
            command(
                e,
                "ABC callback for logic synthesis and optimization. The command `abc -c \"<command>\"` will launch ABC "
                "and execute the given command string. Internally, it writes the current AIG or XAG to a temporary "
                "AIGER file, have ABC parse this file as an AIG using `read <filename>; strash`, and then execute the "
                "given command string. Finally, ABC will write the result back to the temporary file using "
                "`write_aiger -s <filename>`, which will be read back into the fiction CLI as a new AIG.")
    {
        add_option("--command,-c", abc_command_str, "Command to pass to ABC")->required();
    }

  protected:
    /**
     * Function to perform the synth call. Generates a logic network from a truth table.
     */
    void execute() override
    {
        if constexpr (ABC == nullptr)
        {
            env->out() << "[e] ABC_EXECUTABLE environment variable is not set. Cannot launch ABC." << std::endl;
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
            env->out() << "[w] ABC callback does not support technology networks. Use AIGs or XAGs instead."
                       << std::endl;
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

            // call the ABC binary
            const auto abc_call =
                fmt::format("{} -q \"read {}; strash; {}; write_aiger -s {}\"", ABC, fiction_network_path.string(),
                            abc_command_str, abc_network_path.string());

            if (const auto ret = std::system(abc_call.c_str()); ret != 0)
            {
                env->out() << "[e] Failed to execute ABC command." << std::endl;
                return;
            }

            fiction::network_reader<fiction::aig_ptr> reader{abc_network_path.string(), env->out()};

            store<fiction::logic_network_t>().extend() = reader.get_networks().front();
        }
        catch (const std::runtime_error& e)
        {
            env->out() << fmt::format("[e] {}", e.what()) << std::endl;
        }
    }

  private:
    /**
     * Path to the ABC executable.
     */
    static constexpr auto ABC = ABC_EXECUTABLE;
    /**
     * Command string to pass to ABC.
     */
    std::string abc_command_str{};

    /**
     * Get the temporary directory depending on the platform.
     *
     * @return Path to the temporary directory.
     */
    static std::filesystem::path get_temp_directory()
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
    /**
     * Returns a path to "<temp>/fiction" where <temp> is the system's temporary directory. The "fiction" folder is
     * created if it doesn't exist.
     *
     * @return Path to the "<temp>/fiction" directory.
     */
    static std::filesystem::path get_temp_fiction_directory()
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
    /**
     * Writes the current logic network in store to a temporary file in AIGER format and returns the path to the file.
     *
     * @return Path to a temporary AIGER file where the current network is stored.
     */
    std::filesystem::path write_current_network_to_temp_file() const
    {
        const auto write = [](auto&& ntk_ptr) -> std::filesystem::path
        {
            const auto temp_file = get_temp_fiction_directory() / fmt::format("{}.aig", ntk_ptr->get_network_name());

            mockturtle::write_aiger(*ntk_ptr, temp_file.string());

            return temp_file;
        };

        return std::visit(write, store<fiction::logic_network_t>().current());
    }
    /**
     * Returns the path to the temporary file where ABC is supposed to write its output.
     *
     * @return Path to the temporary file where ABC is supposed to write its output.
     */
    std::filesystem::path abc_output_temp_file() const
    {
        const auto get_name = [](auto&& ntk_ptr) -> std::string { return ntk_ptr->get_network_name(); };

        return get_temp_fiction_directory() /
               fmt::format("{}.aig", std::visit(get_name, store<fiction::logic_network_t>().current()));
    }
};

ALICE_ADD_COMMAND(abc, "Logic")

}  // namespace alice

#endif  // FICTION_ABC

#endif  // FICTION_CMD_ABC_HPP

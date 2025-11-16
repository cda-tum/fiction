//
// Created by marcel on 04.12.24.
//

#ifndef FICTION_CMD_ABC_HPP
#define FICTION_CMD_ABC_HPP

#if (FICTION_ABC)

#include <alice/alice.hpp>

#include <filesystem>
#include <string>
#include <vector>

namespace alice
{

/**
 * Callback command for ABC. Calls the ABC binary with a given command string and the current logic network in store.
 *
 * Internally, ABC performs a `read <filename>; strash; <command>; write_aiger <filename>` operation.
 */
class abc_command final : public command
{
  public:
    /**
     * Standard constructor. Adds descriptive information, options, and flags.
     *
     * @param e alice::environment that specifies stores etc.
     */
    explicit abc_command(const environment::ptr& e);

  protected:
    /**
     * Executes the ABC callback: runs ABC on the current network and writes results back to the store.
     */
    void execute() override;

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
    static std::filesystem::path get_temp_directory();
    /**
     * Returns a path to "<temp>/fiction" where <temp> is the system's temporary directory. The "fiction" folder is
     * created if it doesn't exist.
     *
     * @return Path to the "<temp>/fiction" directory.
     */
    static std::filesystem::path get_temp_fiction_directory();
    /**
     * Writes the current logic network in store to a temporary file in AIGER format and returns the path to the file.
     *
     * @return Path to a temporary AIGER file where the current network is stored.
     */
    std::filesystem::path write_current_network_to_temp_file() const;
    /**
     * Returns the path to the temporary file where ABC is supposed to write its output.
     *
     * @return Path to the temporary file where ABC is supposed to write its output.
     */
    std::filesystem::path abc_output_temp_file() const;
    /**
     * Safely execute ABC without invoking a shell to prevent command injection.
     *
     * @param abc_path Path to the ABC executable.
     * @param abc_args Arguments to pass to ABC.
     * @return Exit code of the ABC process.
     */
    static int safe_execute_abc(const std::string& abc_path, const std::vector<std::string>& abc_args);
};

}  // namespace alice

#endif  // FICTION_ABC

#endif  // FICTION_CMD_ABC_HPP

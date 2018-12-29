//
// Created by marcel on 16.07.18.
//

#ifndef FICTION_VERILOG_PARSER_H
#define FICTION_VERILOG_PARSER_H

#include "logic_network.h"
#include <lorina/verilog.hpp>

/**
 * Parser class for verilog netlist files.
 */
class verilog_parser : public lorina::verilog_reader
{
public:
    /**
     * Standard constructor. Gets a network pointer to which the file description should be assigned.
     *
     * @param ln Pointer to an (empty) logic network, to which the a verilog description should be assigned.
     */
    explicit verilog_parser(logic_network_ptr ln) : ln(ln)
    {
        signals["0"] = ln->get_constant(false);
        signals["1"] = ln->get_constant(true);
    }
    /**
     * Standard destructor. Finishes the construction process by cleaning up the POs and substituting multiple input
     * connections by fan-outs as well as XOR vertices by AND/OR/NOT representations.
     */
    ~verilog_parser()
    {
        for (const auto& o : outputs)
            ln->create_po(signals.at(o), o);

        ln->substitute();
    }
    /**
     * Callable hook for lorina::verilog_reader. Creates input vertices.
     *
     * @param names PI names.
     */
    void on_inputs(const std::vector<std::string>& names) const override
    {
        for (const auto& n : names)
            signals[n] = ln->create_pi(n);
    }
    /**
     * Callable hook for lorina::verilog_reader. Creates output vertices.
     *
     * @param names PO names.
     */
    void on_outputs(const std::vector<std::string>& names) const override
    {
        for (const auto& n : names)
            outputs.emplace_back(n);
    }
    /**
     * Callable hook for lorina::verilog_reader. Creates buffer vertices.
     *
     * @param names Buffer (register) names.
     */
    void on_regs(const std::vector<std::string>& names) const override
    {
        for (const auto& n : names)
            signals[n] = ln->create_buf();
    }
    /**
     * Callable hook for lorina::verilog_reader. Creates a value assignment.
     *
     * @param lhs Name on left hand side.
     * @param rhs Name and signal on right hand side.
     */
    void on_assign(const std::string& lhs, const std::pair<std::string, bool>& rhs) const override
    {
        auto r = signals[rhs.first];
        signals[lhs] = rhs.second ? ln->create_not(r) : r;
    }
    /**
     * Callable hook for lorina::verilog_reader to parse conjunctions. Creates an AND vertex if at most one signal is
     * inverted. Creates a cheaper NOR vertex otherwise.
     *
     * @param lhs Name on left hand side.
     * @param op1 Named signal 1.
     * @param op2 Named signal 2.
     */
    void on_and(const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2) const override
    {
        auto a = signals[op1.first];
        auto b = signals[op2.first];

        if (op1.second && op2.second)
            signals[lhs] = ln->create_not(ln->create_or(a, b));
        else
            signals[lhs] = ln->create_and(op1.second ? ln->create_not(a) : a, op2.second ? ln->create_not(b) : b);
    }
    /**
     * Callable hook for lorina::verilog_reader to parse disjunctions. Creates an OR vertex if at most one signal is
     * inverted. Creates a cheaper NAND vertex otherwise.
     *
     * @param lhs Name on left hand side.
     * @param op1 Named signal 1.
     * @param op2 Named signal 2.
     */
    void on_or(const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2) const override
    {
        auto a = signals[op1.first];
        auto b = signals[op2.first];

        if (op1.second && op2.second)
            signals[lhs] = ln->create_not(ln->create_and(a, b));
        else
            signals[lhs] = ln->create_or(op1.second ? ln->create_not(a) : a, op2.second ? ln->create_not(b) : b);
    }
    /**
     * Callable hook for lorina::verilog_reader. Creates an XOR vertex.
     *
     * @param lhs Name on left hand side.
     * @param op1 Named signal 1.
     * @param op2 Named signal 2.
     */
    void on_xor(const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2) const override
    {
        auto a = signals[op1.first];
        auto b = signals[op2.first];
        signals[lhs] = ln->create_xor(op1.second ? ln->create_not(a) : a, op2.second ? ln->create_not(b) : b);
    }
    /**
     * Callable hook for lorina::verilog_reader. Creates a MAJ vertex.
     *
     * @param lhs Name on left hand side.
     * @param op1 Named signal 1.
     * @param op2 Named signal 2.
     * @param op3 Named signal 3.
     */
    void on_maj3(const std::string& lhs, const std::pair<std::string, bool>& op1, const std::pair<std::string, bool>& op2, const std::pair<std::string, bool>& op3) const override
    {
        auto a = signals[op1.first];
        auto b = signals[op2.first];
        auto c = signals[op3.first];
        signals[lhs] = ln->create_maj(op1.second ? ln->create_not(a) : a, op2.second ? ln->create_not(b) : b, op3.second ? ln->create_not(c) : c);
    }
    /**
     * Callable hook for lorina::verilog_reader. Creates a clock assignment.
     *
     * @param edges Rising or falling edge clock names.
     * @param lhs Buffer (register) name.
     * @param rhs Signal name.
     */
    void on_clock_assign(const std::vector<std::pair<std::string, bool>>& edges, const std::string& lhs, const std::pair<std::string, bool>& rhs) const override
    {
        (void)edges; // fix compiler warning
        auto reg = signals[lhs];
        auto sig = signals[rhs.first];
        ln->create_edge(rhs.second ? ln->create_not(sig) : sig, reg);
    }

private:
    /**
     * Pointer to logic network to assign the verilog file description.
     */
    logic_network_ptr ln;
    /**
     * Maps names to actual signals.
     */
    mutable std::unordered_map<std::string, logic_network::vertex> signals;
    /**
     * List of output names.
     */
    mutable std::vector<std::string> outputs;

};


#endif //FICTION_VERILOG_PARSER_H

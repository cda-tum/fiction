//
// Created by marcel on 07.03.18.
//

#ifndef FICTION_ENERGY_MODEL_H
#define FICTION_ENERGY_MODEL_H

/**
 * Collection of energy values in meV to determine different QCA-ONE structures.
 * Numbers are based on 'An Energy-aware Model for the Logic Synthesis of Quantum-Dot Cellular Automata'
 * by Frank Sill Torres, Robert Wille, Philipp Niemann, and Rolf Drechsler in TCAD 2018.
 */
namespace energy
{
    /**
     * Energy consumption (slow) of a normal wire.
     */
    constexpr double WIRE_SLOW = 0.09;
    /**
     * Energy consumption (fast) of a normal wire.
     */
    constexpr double WIRE_FAST = 0.82;
    /**
     * Energy consumption (slow) of a fan-out.
     */
    constexpr double FANOUT_SLOW = 0.12;
    /**
     * Energy consumption (fast) of a fan-out.
     */
    constexpr double FANOUT_FAST = 1.15;
    /**
     * Energy consumption (slow) of a straight inverter.
     */
    constexpr double INVERTER_STRAIGHT_SLOW = 0.13;
    /**
     * Energy consumption (fast) of a straight inverter.
     */
    constexpr double INVERTER_STRAIGHT_FAST = 1.19;
    /**
     * Energy consumption (slow) of a bent inverter.
     */
    constexpr double INVERTER_BENT_SLOW = 0.10;
    /**
     * Energy consumption (fast) of a bent inverter.
     */
    constexpr double INVERTER_BENT_FAST = 0.84;
    /**
     * Energy consumption (slow) of a crossing.
     */
    constexpr double CROSSING_SLOW = 0.28;
    /**
     * Energy consumption (fast) of a crossing.
     */
    constexpr double CROSSING_FAST = 2.57;
    /**
     * Energy consumption (slow) of an AND gate.
     */
    constexpr double AND_SLOW = 0.47;
    /**
     * Energy consumption (fast) of an AND gate.
     */
    constexpr double AND_FAST = 1.39;
    /**
     * Energy consumption (slow) of an OR gate.
     */
    constexpr double OR_SLOW = 0.47;
    /**
     * Energy consumption (fast) of an AND gate.
     */
    constexpr double OR_FAST = 1.39;
    /**
     * Energy consumption (slow) of a classic majority gate.
     */
    constexpr double MAJORITY_SLOW = 0.65;
    /**
     * Energy consumption (fast) of a classic majority gate.
     */
    constexpr double MAJORITY_FAST = 1.68;
    /**
     * Energy information slow (25 GHz) and fast (100 GHz).
     */
    using info = std::pair<double, double>;
}

#endif //FICTION_ENERGY_MODEL_H

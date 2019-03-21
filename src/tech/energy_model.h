//
// Created by marcel on 07.03.18.
//

#ifndef FICTION_ENERGY_MODEL_H
#define FICTION_ENERGY_MODEL_H

/**
 * Collection of energy values in meV to determine different QCA structures on a grid.
 * Numbers are based on 'An Energy-aware Model for the Logic Synthesis of Quantum-Dot Cellular Automata'
 * by Frank Sill Torres, Robert Wille, Philipp Niemann, and Rolf Drechsler in TCAD 2018.
 */
namespace energy
{
    /**
     * Energy consumption (slow) of a normal wire.
     */
    constexpr float WIRE_SLOW = 0.09f;
    /**
     * Energy consumption (fast) of a normal wire.
     */
    constexpr float WIRE_FAST = 0.82f;
    /**
     * Energy consumption (slow) of a fan-out.
     */
    constexpr float FANOUT_SLOW = 0.12f;
    /**
     * Energy consumption (fast) of a fan-out.
     */
    constexpr float FANOUT_FAST = 1.15f;
    /**
     * Energy consumption (slow) of a straight inverter.
     */
    constexpr float INVERTER_STRAIGHT_SLOW = 0.13f;
    /**
     * Energy consumption (fast) of a straight inverter.
     */
    constexpr float INVERTER_STRAIGHT_FAST = 1.19f;
    /**
     * Energy consumption (slow) of a bent inverter.
     */
    constexpr float INVERTER_BENT_SLOW = 0.10f;
    /**
     * Energy consumption (fast) of a bent inverter.
     */
    constexpr float INVERTER_BENT_FAST = 0.84f;
    /**
     * Energy consumption (slow) of a crossing.
     */
    constexpr float CROSSING_SLOW = 0.28f;
    /**
     * Energy consumption (fast) of a crossing.
     */
    constexpr float CROSSING_FAST = 2.57f;
    /**
     * Energy consumption (slow) of an AND gate.
     */
    constexpr float AND_SLOW = 0.47f;
    /**
     * Energy consumption (fast) of an AND gate.
     */
    constexpr float AND_FAST = 1.39f;
    /**
     * Energy consumption (slow) of an OR gate.
     */
    constexpr float OR_SLOW = 0.47f;
    /**
     * Energy consumption (fast) of an AND gate.
     */
    constexpr float OR_FAST = 1.39f;
    /**
     * Energy consumption (slow) of a classic majority gate.
     */
    constexpr float MAJORITY_SLOW = 0.65f;
    /**
     * Energy consumption (fast) of a classic majority gate.
     */
    constexpr float MAJORITY_FAST = 1.68f;
}

#endif //FICTION_ENERGY_MODEL_H

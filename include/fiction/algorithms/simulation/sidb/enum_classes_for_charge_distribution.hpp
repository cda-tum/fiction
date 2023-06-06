//
// Created by Jan Drewniok on 06.06.23.
//

#ifndef FICTION_ENUM_CLASSES_FOR_CHARGE_DISTRIBUTION_HPP
#define FICTION_ENUM_CLASSES_FOR_CHARGE_DISTRIBUTION_HPP

enum class dependent_cell_mode
{
    FIXED,
    VARIABLE
};

enum class energy_calculation
{
    KEEP,
    UPDATE
};

enum class history
{
    CONSIDER,
    NEGLECT
};

#endif  // FICTION_ENUM_CLASSES_FOR_CHARGE_DISTRIBUTION_HPP

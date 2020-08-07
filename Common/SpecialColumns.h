#pragma once

/**
 * @brief Enum used to tag special columns in plots.
 */
enum class ColumnTag : unsigned char
{
    DATE = 0,  // Date of event.
    VALUE      // Value (price, price per unit, number of units etc.).
};

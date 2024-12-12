#pragma once

/**
 * @brief Enum used to tag columns in plots.
 */
enum class ColumnTag : unsigned char
{
    DATE = 0,  // Date of event.
    VALUE = 3  // Value (price, price per unit, number of units etc.).
};

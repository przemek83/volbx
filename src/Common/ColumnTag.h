#pragma once

/// @brief Enum used to tag columns in plots.
enum class ColumnTag : unsigned char
{
    /// Date of event.
    DATE = 0,

    /// Value (price, price per unit, number of units etc.).
    VALUE = 3
};

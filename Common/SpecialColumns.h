#ifndef SPECIALCOLUMNS_H
#define SPECIALCOLUMNS_H

/**
 * @brief Enum used to tag special columns in plots.
 */
enum class SpecialColumn : unsigned char
{
    TRANSACTION_DATE = 0,  // Date of event.
    NUMBER_OF_UNITS,       // Number of units.
    PRICE,                 // Price
    PRICE_PER_UNIT         // Price per unit.
};

#endif  // SPECIALCOLUMNS_H

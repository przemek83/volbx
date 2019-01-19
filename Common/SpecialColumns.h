#ifndef SPECIALCOLUMNS_H
#define SPECIALCOLUMNS_H

/**
 * @brief Enum used to taf special columns in plots.
 */
enum SpecialColumn
{
    //Add new at end.
    SPECIAL_COLUMN_TRANSACTION_DATE = 0, //Date of event.
    SPECIAL_COLUMN_NUMBER_OF_UNITS,      //Number of units.
    SPECIAL_COLUMN_PRICE,                //Price
    SPECIAL_COLUMN_PRICE_PER_UNIT        //Price per unit.
};

#endif // SPECIALCOLUMNS_H

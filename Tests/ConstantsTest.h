#ifndef CONSTANTSTEST_H
#define CONSTANTSTEST_H

#include <QObject>

/**
 * @brief Unit test for Constants class.
 */
class ConstantsTest: public QObject
{
    Q_OBJECT
private slots:
    void testDatesConversion();
};

#endif // CONSTANTSTEST_H

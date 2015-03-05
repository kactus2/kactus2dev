//-----------------------------------------------------------------------------
// File: tst_ValueFormatter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.03.2015
//
// Description:
// Unit test for class ValueFormatter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <editors/ComponentEditor/common/ValueFormatter.h>

class tst_ValueFormatter : public QObject
{
    Q_OBJECT

public:
    tst_ValueFormatter();

private slots:
    void testFormatValue();
    void testFormatValue_data();
};

//-----------------------------------------------------------------------------
// Function: tst_ValueFormatter::tst_ValueFormatter()
//-----------------------------------------------------------------------------
tst_ValueFormatter::tst_ValueFormatter()
{
}

//-----------------------------------------------------------------------------
// Function: tst_ValueFormatter::testFormatValue()
//-----------------------------------------------------------------------------
void tst_ValueFormatter::testFormatValue()
{
    QFETCH(QString, value);
    QFETCH(int, formatBase);
    QFETCH(QString, expectedValue);

    ValueFormatter formatter;
    QCOMPARE(formatter.format(value, formatBase), expectedValue);
}

//-----------------------------------------------------------------------------
// Function: tst_ValueFormatter::testFormatValue_data()
//-----------------------------------------------------------------------------
void tst_ValueFormatter::testFormatValue_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<int>("formatBase");
    QTest::addColumn<QString>("expectedValue");

    QTest::newRow("Base of two in value 10 is formatted to binary value 1010") << "10" << 2 << "'b1010";
    QTest::newRow("Base of two in array {10,16} is formatted to binary array {1010,10000}") << "{10,16}" << 2 <<
        "{'b1010,'b10000}";

    QTest::newRow("Base of eight in value 10 is formatted to octal value 12") << "10" << 8 << "'o12";
    QTest::newRow("Base of eight in array {10,16} is formatted to octal array {12,20}") << "{10,16}" << 8 <<
        "{'o12,'o20}";

    QTest::newRow("Base of ten in value 10 is formatted to decimal value 10") << "10" << 10 << "10";
    QTest::newRow("Base of ten in array {10,16} is formatted to decimal array {10,16}") << "{10,16}" << 10 <<
        "{10,16}";

    QTest::newRow("Base of 16 in value 10 is formatted to hexadecimal value a") << "10" << 16 << "'ha";
    QTest::newRow("Base of 16 in array {10,16} is formatted to hexadecimal array {a,10}") << "{10,16}" << 16 <<
        "{'ha,'h10}";

    QTest::newRow("Base of ten in array '{10,16} is formatted to decimal array {10,16}") << "'{10,16}" << 10 <<
        "{10,16}";
}

QTEST_APPLESS_MAIN(tst_ValueFormatter)

#include "tst_ValueFormatter.moc"

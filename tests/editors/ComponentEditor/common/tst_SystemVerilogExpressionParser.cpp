//-----------------------------------------------------------------------------
// File: tst_SystemVerilogExpressionParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2014
//
// Description:
// Unit test for class SystemVerilogExpressionParser.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

class tst_SystemVerilogExpressionParser : public QObject
{
    Q_OBJECT

public:
    tst_SystemVerilogExpressionParser();

private slots:
    void testParseConstant();
    void testParseConstant_data();
};

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::tst_SystemVerilogExpressionParser()
//-----------------------------------------------------------------------------
tst_SystemVerilogExpressionParser::tst_SystemVerilogExpressionParser()
{
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseConstant()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseConstant()
{
    QFETCH(QString, constant);
    QFETCH(QString, expectedValue);

    SystemVerilogExpressionParser parser;
    QCOMPARE(parser.parseConstant(constant), expectedValue);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseConstant_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseConstant_data()
{
    QTest::addColumn<QString>("constant");
    QTest::addColumn<QString>("expectedValue");

    QTest::newRow("Empty expression should evaluate to zero") << "" << "0";

    //! Decimal numbers.
    QTest::newRow("Decimal number 0 should evaluate to 0") << "0" << "0";
    QTest::newRow("Decimal number 1 should evaluate to 1") << "1" << "1";
    QTest::newRow("Decimal number 7 should evaluate to 7") << "7" << "7";

    QTest::newRow("Decimal number 'd2 should evaluate to 2") << "'d2" << "2";
    QTest::newRow("Decimal number 'D8 should evaluate to 8") << "'D8" << "8";

    QTest::newRow("Decimal number 'sd2 should evaluate to 2") << "'sd2" << "2";
    QTest::newRow("Decimal number 'sD2 should evaluate to 2") << "'sD2" << "2";

    QTest::newRow("Decimal number 'Sd3 should evaluate to 3") << "'Sd3" << "3";
    QTest::newRow("Decimal number 'SD3 should evaluate to 3") << "'SD3" << "3";
    
    QTest::newRow("Decimal number 1'd3 with size should evaluate to 3") << "1'd3" << "3";

    QTest::newRow("Decimal number with underscore should evaluate without underscore") << "10_000" << "10000";
    QTest::newRow("Decimal number with multiple underscores should evaluate without underscores") 
        << "1_000_000" << "1000000";
    QTest::newRow("Decimal number with base and underscores should evaluate") << "'sd10_000" << "10000";

    //! Hexadecimal numbers.
    QTest::newRow("Hexadecimal number without base should evaluate to zero") << "ff" << "0";
    QTest::newRow("Hexadecimal number 'h1 should evaluate to 1") << "'h1" << "1";
    QTest::newRow("Hexadecimal number 'hA should evaluate to 10") << "'hA" << "10";
    QTest::newRow("Hexadecimal number 'Hf should evaluate to 15") << "'Hf" << "15";

    QTest::newRow("Hexadecimal number 'shf should evaluate to 15") << "'shf" << "15";
    QTest::newRow("Hexadecimal number 'sH2 should evaluate to 2") << "'sH2" << "2";
    
    QTest::newRow("Hexadecimal number 'Shb should evaluate to 11") << "'Shb" << "11";
    QTest::newRow("Hexadecimal number 'SH5 should evaluate to 5") << "'SH5" << "5";

    QTest::newRow("Hexadecimal number 8'h02 with size should evaluate to 2") << "8'd02" << "2";

    QTest::newRow("Hexadecimal number with underscore should evaluate to decimal without underscore") 
        << "'h1_F" << "31";
    QTest::newRow("Hexadecimal number with multiple underscores should evaluate to decimal without underscores") 
        << "'h0_F_F" << "255";

    //! Binary numbers.
    QTest::newRow("Binary number 'b1 should evaluate to 1") << "'b1" << "1";
    QTest::newRow("Binary number 'b10 should evaluate to 2") << "'b10" << "2";
    QTest::newRow("Binary number 'b111 should evaluate to 7") << "'b111" << "7";

    QTest::newRow("Binary number 'sb11 should evaluate to 3") << "'sb11" << "3";
    QTest::newRow("Binary number 'sB10 should evaluate to 2") << "'sB10" << "2";

    QTest::newRow("Binary number 'Sb110 should evaluate to 6") << "'Sb110" << "6";
    QTest::newRow("Binary number 'SB100 should evaluate to 4") << "'sB100" << "4";

    QTest::newRow("Binary number 4'b0111 with size should evaluate to 7") << "4'b0111" << "7";

    QTest::newRow("Binary number with underscore should evaluate to decimal without underscore") 
        << "'b1_1" << "3";
    QTest::newRow("Binary number with multiple underscores should evaluate to decimal without underscores") 
        << "'b1_1_1_1" << "15";

    //! Octal numbers.
    QTest::newRow("Octal number 'o1 should evaluate to 1") << "'o1" << "1";
    QTest::newRow("Octal number 'o7 should evaluate to 7") << "'o7" << "7";
    QTest::newRow("Octal number 'o10 should evaluate to 8") << "'o10" << "8";
    QTest::newRow("Octal number 'O12 should evaluate to 10") << "'O12" << "10";

    QTest::newRow("Octal number 'so4 should evaluate to 4") << "'so4" << "4";
    QTest::newRow("Octal number 'sO17 should evaluate to 15") << "'sO17" << "15";

    QTest::newRow("Octal number 'So20 should evaluate to 16") << "'So20" << "16";
    QTest::newRow("Octal number 'SO2 should evaluate to 2") << "'SO2" << "2";

    QTest::newRow("Octal number 3'o011 with size should evaluate to 9") << "3'o011" << "9";

    QTest::newRow("Octal number with underscore should evaluate to decimal without underscore") 
        << "'o1_0" << "8";
    QTest::newRow("Octal number with multiple underscores should evaluate to decimal without underscores") 
        << "'o1_0_0" << "64";
}

QTEST_APPLESS_MAIN(tst_SystemVerilogExpressionParser)

#include "tst_SystemVerilogExpressionParser.moc"

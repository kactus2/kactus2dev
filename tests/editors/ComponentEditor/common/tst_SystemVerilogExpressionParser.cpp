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

    void testParseAddition();
    void testParseAddition_data();
    void testParseSubtraction();
    void testParseSubtraction_data();

    void testParseMultiply();
    void testParseMultiply_data();
    void testParseDivision();
    void testParseDivision_data();

    void testParsePower();
    void testParsePower_data();

    void testClog2Function();
    void testClog2Function_data();

    void testParseMultipleOperations();
    void testParseMultipleOperations_data();

    void testParseExpressionWithParathesis();
    void testParseExpressionWithParathesis_data();

    void testGetBaseForExpression();
    void testGetBaseForExpression_data();

    void testIsPlainValue();
    void testIsPlainValue_data();

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
    QCOMPARE(parser.parseExpression(constant), expectedValue);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseConstant_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseConstant_data()
{
    QTest::addColumn<QString>("constant");
    QTest::addColumn<QString>("expectedValue");

    QTest::newRow("Empty expression should evaluate to unknown") << "" << "x";

    //! Decimal numbers.
    QTest::newRow("Decimal number 0 should evaluate to 0") << "0" << "0";
    QTest::newRow("Decimal number 1 should evaluate to 1") << "1" << "1";
    QTest::newRow("Decimal number 7 should evaluate to 7") << "7" << "7";

    QTest::newRow("Positive decimal number") << "+1" << "1";
    QTest::newRow("Negative decimal number") << "-1" << "-1";

    QTest::newRow("Decimal number 'd2 should evaluate to 2") << "'d2" << "2";
    QTest::newRow("Decimal number 'D8 should evaluate to 8") << "'D8" << "8";

    QTest::newRow("Decimal number 'sd2 should evaluate to 2") << "'sd2" << "2";
    QTest::newRow("Decimal number 'sD2 should evaluate to 2") << "'sD2" << "2";

    QTest::newRow("Decimal number 'Sd3 should evaluate to 3") << "'Sd3" << "3";
    QTest::newRow("Decimal number 'SD3 should evaluate to 3") << "'SD3" << "3";
    
    QTest::newRow("Decimal number 1'd3 with size should evaluate to 3") << "1'd3" << "3";

    QTest::newRow("Decimal number '2 should evaluate to 2") << "'2" << "2";

    QTest::newRow("Decimal number with underscore should evaluate without underscore") << "10_000" << "10000";
    QTest::newRow("Decimal number with multiple underscores should evaluate without underscores") 
        << "1_000_000" << "1000000";
    QTest::newRow("Decimal number with base and underscores should evaluate") << "'sd10_000" << "10000";

    //! Fixed-point numbers.
    QTest::newRow("Fixed-point number 0.0 should evaluate to 0.0") << "0.0" << "0.0";
    QTest::newRow("Fixed-point number 0.5 should evaluate to 0.5") << "0.5" << "0.5";
    QTest::newRow("Fixed-point number 0.25 should evaluate to 0.25") << "0.25" << "0.25";
    QTest::newRow("Fixed-point number 1.0 should evaluate to 1.0") << "1.0" << "1.0";
    QTest::newRow("Negative fixed-point number") << "-1.0" << "-1.0";

    //! Hexadecimal numbers.
    QTest::newRow("Hexadecimal number without base should evaluate to unknown") << "ff" << "x";
    QTest::newRow("Hexadecimal number 'h1 should evaluate to 1") << "'h1" << "1";
    QTest::newRow("Hexadecimal number 'hA should evaluate to 10") << "'hA" << "10";
    QTest::newRow("Hexadecimal number 'Hf should evaluate to 15") << "'Hf" << "15";

    QTest::newRow("Hexadecimal number 'shf should evaluate to 15") << "'shf" << "15";
    QTest::newRow("Hexadecimal number 'sH2 should evaluate to 2") << "'sH2" << "2";
    
    QTest::newRow("Hexadecimal number 'Shb should evaluate to 11") << "'Shb" << "11";
    QTest::newRow("Hexadecimal number 'SH5 should evaluate to 5") << "'SH5" << "5";

    QTest::newRow("Hexadecimal number 8'h02 with size should evaluate to 2") << "8'd02" << "2";
    QTest::newRow("Hexadecimal number 32'h00000001 with size should evaluate to 1") << "32'h00000001" << "1";

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

    QTest::newRow("Empty string literal") << "\"\"" << "\"\"";
    QTest::newRow("Non-empty string literal") << "\"text\"" << "\"text\"";
    QTest::newRow("Non-empty string literal with whitespace") << "\" text \"" << "\" text \"";
    QTest::newRow("Whitespace before and after string is ignored") << "  \"text\" " << "\"text\"";

    QTest::newRow("String missing a double quote is unknown") << "\"text" << "x";
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseAddition()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseAddition()
{
    QFETCH(QString, additionExpression);
    QFETCH(QString, expectedSum);

    SystemVerilogExpressionParser parser;
    QCOMPARE(parser.parseExpression(additionExpression), expectedSum);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseAddition_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseAddition_data()
{
    QTest::addColumn<QString>("additionExpression");
    QTest::addColumn<QString>("expectedSum");

    QTest::newRow("No addition on constant equals constant") << "1" << "1";
    QTest::newRow("Addition without second operand is unknown") << "1+" << "x";
    QTest::newRow("Addition without second operand and whitespaces is unknown") << " 1 + " << "x";

    //! Decimal numbers.
    QTest::newRow("Constant plus zero equals constant") << "1+0" << "1";
    QTest::newRow("One plus one equals two") << "1+1" << "2";
    QTest::newRow("One plus two equals three") << "1+2" << "3";
    QTest::newRow("Two plus one equals three") << "2+1" << "3";
    QTest::newRow("Two plus two with whitespaces equals four") << "2 + 2" << "4";
    QTest::newRow("Multiple whitespaces") << " 1  +     1 " << "2";
    QTest::newRow("Sum of decimal values with sizes and signs") << "8'sd8 + 8'sd9" << "17";

    //! Fixed-point numbers.
    QTest::newRow("One plus one equals exactly two") << "1.0+1.0" << "2";
    QTest::newRow("One plus decimal two equals three") << "1.0+2" << "3";
    QTest::newRow("Decimal one plus real two equals three") << "1+2.0" << "3";
    QTest::newRow("Half plus one quarter equals 0.75") << "0.5 + 0.25" << "0.75";
    QTest::newRow("Four quarters equals 1") << "0.25 + 0.25 + 0.25 + 0.25" << "1";

    //! Hexadecimal numbers.
    QTest::newRow("Sum of hexadecimal 'h10 + 'h0 equals 16") << "'h10 + 'h0" << "16";
    QTest::newRow("Sum of hexadecimal 'h10 + 'h10 equals 32") << "'h10 + 'h10" << "32";
    QTest::newRow("Sum of hexadecimal values with sizes and signs") << "8'sh10 + 8'sh02" << "18";
    QTest::newRow("Constant hexadecimal with long size") << "32'h00000001" << "1";

    //! Binary numbers.
    QTest::newRow("Sum of binary 'b100 + 'b010 equals 6") << "'b100 + 'b010" << "6";
    QTest::newRow("Sum of binary 'b0001 + 'b0111 equals 8") << "'b0001 + 'b0111" << "8";
    QTest::newRow("Sum of binary values with sizes and signs") << "4'sb0010 + 4'sb0001" << "3";

    //! Octal numbers.
    QTest::newRow("Sum of octal 'o1 + 'o1 equals 2") << "'o1 + 'o1" << "2";
    QTest::newRow("Sum of octal 'o2 + 'o7 equals 9") << "'o2 + 'o7" << "9";
    QTest::newRow("Sum of octal values with sizes and signs") << "3'so3 + 3'so4" << "7";

    QTest::newRow("Sum of decimal and octal 10 + 'o10 equals 18") << "10 + 'o10" << "18";
    QTest::newRow("Sum of hexadecimal and binary 'h10 + 'b10 equals 18") << "'h10 + 'b10" << "18";

    QTest::newRow("Sum of multiple values of same base") << "'h10 + 'h10 + 'h01" << "33";
    QTest::newRow("Sum of multiple values of different bases") << "'hA + 'b1010 + 'o12 + 10" << "40";

    QTest::newRow("Sum of ascii characters and a number is unknown") << "text + 2" << "x";

}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseSubtraction()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseSubtraction()
{
    QFETCH(QString, minusExpression);
    QFETCH(QString, expectedResult);

    SystemVerilogExpressionParser parser;
    QCOMPARE(parser.parseExpression(minusExpression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseSubtraction_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseSubtraction_data()
{
    QTest::addColumn<QString>("minusExpression");
    QTest::addColumn<QString>("expectedResult");

    //! Decimal numbers.
    QTest::newRow("Constant minus zero equals constant") << "1-0" << "1";
    QTest::newRow("Constant minus constant equals zero") << "1-1" << "0";
    QTest::newRow("Four minus two equals two") << "4-2" << "2";
    QTest::newRow("three minus one equals two") << "3-1" << "2";
    QTest::newRow("Negative result") << "1-2" << "-1";
    QTest::newRow("Negative operands") << "-1-2" << "-3";
    QTest::newRow("Two minus one with whitespaces equals one") << "2 - 1" << "1";
    QTest::newRow("Multiple whitespaces") << " 2  -     1 " << "1";
    QTest::newRow("Subtraction of decimal values with sizes and signs") << "8'sd9 - 8'sd2" << "7";

    //! Fixed-point numbers.
    QTest::newRow("4 - 2.25 two equals 1.75") << "4.0 - 2.25" << "1.75";
    QTest::newRow("Negative fixed-point result") << "0.5 - 2.0" << "-1.5";

    //! Hexadecimal numbers.
    QTest::newRow("Subtraction of hexadecimal 'h10 - 'h1 equals 15") << "'h10 - 'h1" << "15";
    QTest::newRow("Subtraction of hexadecimal 'h0E - 'h4 equals 10") << "'h0E - 'h4" << "10";
    QTest::newRow("Subtraction of hexadecimal values with sizes and signs") << "8'sh0B - 8'sh04" << "7";

    //! Binary numbers.
    QTest::newRow("Subtraction of binary 'b100 - 'b010 equals 2") << "'b100 - 'b010" << "2";
    QTest::newRow("Subtraction of binary 'b0111 - 'b011 equals 4") << "'b0111 - 'b011" << "4";
    QTest::newRow("Subtraction of binary values with sizes and signs") << "4'sb0010 - 4'sb0001" << "1";

    //! Octal numbers.
    QTest::newRow("Subtraction of octal 'o3 - 'o1 equals 2") << "'o3 - 'o1" << "2";
    QTest::newRow("Subtraction of octal 'o7 - 'o2 equals 5") << "'o7 - 'o2" << "5";
    QTest::newRow("Subtraction of octal values with sizes and signs") << "3'so6 - 3'so3" << "3";

    QTest::newRow("Subtraction of multiple values of same base") << "'h12 - 'h8 - 'h01" << "9";
    QTest::newRow("Subtraction of multiple values of different bases") << "'h20 - 'b1010 - 'o12 - 2" << "10";
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseMultiply()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseMultiply()
{
    QFETCH(QString, multiplyExpression);
    QFETCH(QString, expectedResult);

    SystemVerilogExpressionParser parser;
    QCOMPARE(parser.parseExpression(multiplyExpression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseMultiply_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseMultiply_data()
{
    QTest::addColumn<QString>("multiplyExpression");
    QTest::addColumn<QString>("expectedResult");

    QTest::newRow("Zero times zero equals zero") << "0*0" << "0";
    QTest::newRow("Zero times constant equals zero") << "0*1" << "0";
    QTest::newRow("Constant times zero equals zero") << "1*0" << "0";

    //! Decimal numbers.
    QTest::newRow("One times constant equals constant") << "1*2" << "2";
    QTest::newRow("Two times two equals four") << "2*2" << "4";
    QTest::newRow("Two times three with whitespaces equals six") << "2 * 3" << "6";
    QTest::newRow("Multiple whitespaces") << " 4  *     4 " << "16";
    QTest::newRow("Multiply of decimal values with sizes and signs") << "8'sd9 * 8'sd2" << "18";

    QTest::newRow("Negative constant times negative constant gives positive value") << "-2*-2" << "4";
    QTest::newRow("Negative constant times positive constant gives negative value") << "-2*2" << "-4";
    QTest::newRow("Positive constant times negative constant gives negative value") << "2*-2" << "-4";

    //! Fixed-point numbers.
    QTest::newRow("Two times one half equals one") << "2*0.5" << "1";
    QTest::newRow("Multiple fraction digits") << "0.25*0.25" << "0.0625";
    QTest::newRow("Zero times anything is zero") << "0.0*42" << "0";

    //! Hexadecimal numbers.
    QTest::newRow("Multiply of hexadecimal 'h10 * 'h2 equals 32") << "'h10 * 'h2" << "32";
    QTest::newRow("Multiply of hexadecimal 'hA * 'h3 equals 30") << "'hA * 'h3" << "30";
    QTest::newRow("Multiply of hexadecimal values with sizes and signs") << "8'sh0A * 8'sh05" << "50";
    
    //! Binary numbers.
    QTest::newRow("Multiply of binary 'b100 * 'b010 equals 8") << "'b100 * 'b010" << "8";
    QTest::newRow("Multiply of binary 'b0111 * 'b001 equals 7") << "'b0111 * 'b001" << "7";
    QTest::newRow("Multiply of binary values with sizes and signs") << "4'sb0010 * 4'sb0011" << "6";
    
    //! Octal numbers.
    QTest::newRow("Multiply of octal 'o3 * 'o7 equals 21") << "'o3 * 'o7" << "21";
    QTest::newRow("Multiply of octal 'o7 * 'o10 equals 56") << "'o7 * 'o10" << "56";
    QTest::newRow("Multiply of octal values with sizes and signs") << "6'so6 * 6'so3" << "18";

    QTest::newRow("Multiply of multiple values of same base") << "'h2 * 'h8 * 'h01" << "16";
    QTest::newRow("Multiply of multiple values of different bases") << "'h04 * 'b0100 * 'o2 * 2" << "64";
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseDivision()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseDivision()
{
    QFETCH(QString, divisionExpression);
    QFETCH(QString, expectedResult);

    SystemVerilogExpressionParser parser;
    QCOMPARE(parser.parseExpression(divisionExpression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseDivision_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseDivision_data()
{
    QTest::addColumn<QString>("divisionExpression");
    QTest::addColumn<QString>("expectedResult");

    QTest::newRow("Constant divided by same constant equals one") << "2/2" << "1";
    QTest::newRow("Constant divided by one equals the same constant") << "2/1" << "2";
    QTest::newRow("Constant divided by zero equals x") << "1/0" << "x";

    //! Decimal numbers.
    QTest::newRow("Division result for integer truncates towards zero") << "3/2" << "1";

    QTest::newRow("Four divided by two equals two") << "4/2" << "2";
    QTest::newRow("Ten divided by two equals five") << "10/2" << "5";
    QTest::newRow("Division with whitespaces") << "7 / 3" << "2";
    QTest::newRow("Division with multiple whitespaces") << " 4   /  2 " << "2";
    QTest::newRow("Division of decimal values with sizes and signs") << "8'sd9 / 8'sd3" << "3";

    QTest::newRow("Negative constant divided by negative constant gives positive value") << "-2/-2" << "1";
    QTest::newRow("Negative constant divided by positive constant gives negative value") << "-2/2" << "-1";
    QTest::newRow("Positive constant divided by negative constant gives negative value") << "2/-2" << "-1";

    //! Fixed-point numbers.
    QTest::newRow("Division result for integer divisor truncates towards zero") << "3/2.0" << "1";
    QTest::newRow("Division result for real does not truncate towards zero") << "3.0/2" << "1.5";
    QTest::newRow("Number of fractional digits is five by default") << "10.0/3" << "3.33333";
    QTest::newRow("Division by zero is unknown") << "3.0/0.0" << "x";

    //! Hexadecimal numbers.
    QTest::newRow("Division of hexadecimal 'h10 / 'h2 equals 8") << "'h10 / 'h2" << "8";
    QTest::newRow("Division of hexadecimal 'hA / 'h3 equals 3") << "'hA / 'h3" << "3";
    QTest::newRow("Division of hexadecimal values with sizes and signs") << "8'sh0A / 8'sh05" << "2";

    //! Binary numbers.
    QTest::newRow("Division of binary 'b100 / 'b010 equals 2") << "'b100 / 'b010" << "2";
    QTest::newRow("Division of binary 'b0111 / 'b010 equals 3") << "'b0111 / 'b010" << "3";
    QTest::newRow("Division of binary values with sizes and signs") << "4'sb1000 / 4'sb0010" << "4";

    //! Octal numbers.
    QTest::newRow("Division of octal 'o10 / 'o2 equals 4") << "'o10 /'o2" << "4";
    QTest::newRow("Division of octal 'o20 / 'o4 equals 4") << "'o20 / 'o4" << "4";
    QTest::newRow("Division of octal values with sizes and signs") << "6'so6 / 6'so3" << "2";

    QTest::newRow("Divide multiple values of same base") << "'h20 / 'h02 / 'h08" << "2";
    QTest::newRow("Divide multiple values of different bases") << "'h40 / 'b0010 / 'o2 / 2" << "8";
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParsePower()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParsePower()
{
    QFETCH(QString, powerExpression);
    QFETCH(QString, expectedResult);

    SystemVerilogExpressionParser parser;
    QCOMPARE(parser.parseExpression(powerExpression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParsePower_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParsePower_data()
{
    QTest::addColumn<QString>("powerExpression");
    QTest::addColumn<QString>("expectedResult");

    //! Decimal numbers.
    QTest::newRow("One to the power of zero equals one") << "1**0" << "1";
    QTest::newRow("Zero to the power of zero equals one") << "0**0" << "1";
    QTest::newRow("Anything to the power of zero equals one") << "42**0" << "1";

    QTest::newRow("Zero to the power of a negative value is unknown") << "0**-2" << "x";

    QTest::newRow("One to the power of anything equals one") << "1**42" << "1";

    QTest::newRow("Negative one to the power of an even value equals one") << "-1**2" << "1";
    QTest::newRow("Negative one to the power of an odd value equals negative one") << "-1**1" << "-1";

    QTest::newRow("Negative one to the power of an even negative value equals one") << "-1**-2" << "1";
    QTest::newRow("Negative one to the power of an odd negative value equals negative one") << "-1**-1" << "-1";
    
    QTest::newRow("Integer truncates to zero") << "2 ** -1" << "0";

    QTest::newRow("Two to the power of two equals four") << "2**2" << "4";
    QTest::newRow("Negative Two to the power of two equals four") << "-2**2" << "4";
    QTest::newRow("Two to the power of three equals eight") << "2**3" << "8";
    QTest::newRow("Negative two to the power of three equals negative eight") << "-2**3" << "-8";
    QTest::newRow("Two to the power of five equals 32") << "2**5" << "32";
    QTest::newRow("Three to the power of three equals 27") << "3**3" << "27";
    QTest::newRow("Power operation with whitespaces") << "4 ** 3" << "64";

    QTest::newRow("Multiple power operations") << "2**2**2" << "16";
    QTest::newRow("Power of multiple different bases") << "'h02 ** 'b0010 ** 'o2 ** 2" << "256";

    //! Fixed-point numbers.
    QTest::newRow("Real gives real reciprocal") << "2.0 ** -1" << "0.5";
    QTest::newRow("2.5 to the power of two") << "2.5 ** 2" << "6.25";
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testClog2Function()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testClog2Function()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    SystemVerilogExpressionParser parser;
    QCOMPARE(parser.parseExpression(expression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testClog2Function_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testClog2Function_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");

    QTest::newRow("$clog(0) equals 0") << "$clog2(0)" << "0";
    QTest::newRow("$clog(1) equals 1") << "$clog2(1)" << "1";
    QTest::newRow("$clog(2) equals 1") << "$clog2(2)" << "1";
    QTest::newRow("$clog(3) equals 2") << "$clog2(3)" << "2";
    QTest::newRow("$clog(127) equals 7") << "$clog2(127)" << "7";
    QTest::newRow("$clog(128) equals 7") << "$clog2(128)" << "7";
    QTest::newRow("$clog(129) equals 8") << "$clog2(129)" << "8";
    QTest::newRow("$clog(2048) equals 11") << "$clog2(2048)" << "11";

    QTest::newRow("$clog() for negative value is undefined") << "$clog2(-1)" << "x";

    QTest::newRow("$clog('h0F) for hexadecimal value equals 4") << "$clog2('h0F)" << "4";
    QTest::newRow("$clog('hFF) for hexadecimal value equals 8") << "$clog2('hFF)" << "8";

    QTest::newRow("Multiple clog2-functions") << "$clog2(2) + $clog2(4) + $clog2(4)" << "5";

    QTest::newRow("Simple expression as argument") << "$clog2(2 + 2)" << "2";
    QTest::newRow("Expression as argument") << "$clog2(2**12)" << "12";
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseMultipleOperations()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseMultipleOperations()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    SystemVerilogExpressionParser parser;
    QCOMPARE(parser.parseExpression(expression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseMultipleOperations_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseMultipleOperations_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");

    QTest::newRow("Additions followed by a subtraction") << "1 + 1 + 1 - 2" << "1";
    QTest::newRow("Addition and subtraction mixed") << "1 + 2 - 3 + 4 - 5" << "-1";
    QTest::newRow("Addition and subtraction for different bases") << "'h0F - 'b11 - 'o2 + 1" << "11";

    QTest::newRow("Multiply precedes addition") << "1 + 2*3" << "7";
    QTest::newRow("Multiply precedes subtraction") << "8 - 2*3" << "2";
    QTest::newRow("Multiply, addition and subtraction mixed") << "7 + 2*4*1 - 2*1 - 5*2" << "3";

    QTest::newRow("Division precedes addition") << "1 + 4/2" << "3";
    QTest::newRow("Division precedes subtraction") << "8 - 6/2" << "5";
    QTest::newRow("Division, addition and subtraction mixed") << "8 + 8/2 - 4/2 - 3/2" << "9";

    QTest::newRow("Division and multiplication evaluated from left to right") << "6/2*4*3/1" << "36";
    QTest::newRow("Division, multiplication, addition and subtraction mixed") << "8*2 - 4/2 + 4 - 3*2*2" << "6";

    QTest::newRow("Power precedes addition") << "1 + 2**3" << "9";
    QTest::newRow("Power precedes multiplication") << "4*2**3" << "32";
    QTest::newRow("Power precedes division") << "16/2**3" << "2";

    QTest::newRow("clog2() precedes power operation") << "2**$clog2(4) + 1" << "5";

    QTest::newRow("Value of clog2() preceds other operations") << "(2 + 2)*3*$clog2(4*2*2) + 2" << "50";
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseExpressionWithParathesis()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseExpressionWithParathesis()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);

    SystemVerilogExpressionParser parser;
    QCOMPARE(parser.parseExpression(expression), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseExpressionWithParathesis_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseExpressionWithParathesis_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");

    QTest::newRow("Empty parenthesis") << "()" << "x" ;
    QTest::newRow("Single constant in parenthesis") << "(1)" << "1" ;
    QTest::newRow("Single operation in parenthesis") << "(1+1)" << "2" ;
    QTest::newRow("Single operation and whitespace in parenthesis") << " ( 1 + 1 ) " << "2" ;

    QTest::newRow("Addition in parenthesis precedes multiplication") << "(1 + 1)*4" << "8";
    QTest::newRow("Subtraction in parenthesis precedes multiplication") << "(4 - 1)*3" << "9";
    QTest::newRow("Multiplication in parenthesis precedes division") << "9/(3*3)" << "1";

    QTest::newRow("Nested parentheses") << "2*(3*(1 + 1) - 5)" << "2";
    QTest::newRow("Double parentheses") << "((1 + 3)*4)/2" << "8";
    QTest::newRow("Double parentheses with whitespace") << "( (1 + 3)*4 ) /2" << "8";
    QTest::newRow("Triple parentheses") << "(((1 + 3))*4)/2" << "8";    
    QTest::newRow("Deeply nested parentheses") << "((((2))))" << "2";
    QTest::newRow("Deeply nested parentheses with whitespace") << " ( ((  (2 ) ))  ) " << "2";

    QTest::newRow("Parallel parentheses") << "(1 + 1)*(1 + 1)" << "4";
    QTest::newRow("Parallel and nested parentheses") << "(2 * (1 + 1))*(1 + 1)" << "8";
    QTest::newRow("Multiple parallel parentheses") << "(1+1)*(1+1)*(1+1)*(1+1)*(1+1)" << "32";

    QTest::newRow("Mismatched open parentheses") << "((1)" << "x";
    QTest::newRow("Mismatched closed parentheses") << "(1))" << "x";
    QTest::newRow("Parentheses wrong way") << ")1(" << "x";
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testGetBaseForExpression()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testGetBaseForExpression()
{
    QFETCH(QString, expression);
    QFETCH(int, expectedBase);

    SystemVerilogExpressionParser parser;
    
    QCOMPARE(parser.baseForExpression(expression), expectedBase);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testGetBaseForExpression_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testGetBaseForExpression_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<int>("expectedBase");

    QTest::newRow("Decimal constant has base 10") << "1" << 10;
    QTest::newRow("Hexadecimal constant has base 16") << "'h1" << 16;
    QTest::newRow("Octal constant has base 8") << "'o1" << 8;
    QTest::newRow("Binary constant has base 2") << "'b1" << 2;

    QTest::newRow("String constant has base 0") << "\"text\"" << 0;

    QTest::newRow("Expression with binary values has base 2") << "'b1 + 'b1" << 2;
    QTest::newRow("Expression with hexadecimal values has base 2") << "'h0F + 'h01" << 16;

    QTest::newRow("Expression with binary and hexadecimal values has base 16") << "'b01 + 'h01" << 16;
    QTest::newRow("Expression with decimal and hexadecimal values has base 16") << "'h01 + 12" << 16;
    QTest::newRow("Expression with multiple bases values has the greatest base") << "'b01 + 'o1 + 'h01 + 'd1" << 16;
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testIsPlainValue()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testIsPlainValue()
{
    QFETCH(QString, expression);
    QFETCH(bool, explectedPlain);

    SystemVerilogExpressionParser parser;

    QCOMPARE(parser.isPlainValue(expression), explectedPlain);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testIsPlainValue_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testIsPlainValue_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<bool>("explectedPlain");

   QTest::newRow("Decimal constant is plain") << "1" << true;
   QTest::newRow("Hexadecimal constant is plain") << "'h1" << true;
   QTest::newRow("Octal constant is plain") << "'o1" << true;
   QTest::newRow("Binary constant is plain") << "'b1" << true;
   QTest::newRow("String is plain") << "\"text\"" << true;
   QTest::newRow("Empty value is plain") << "" << true;

   QTest::newRow("Addition is not plain") << "'h1 + 'h1" << false;
   QTest::newRow("Subtraction is not plain") << "'o2 - 'o1" << false;
   QTest::newRow("Multiply is not plain") << "2 * 2" << false;
   QTest::newRow("Division is not plain") << "4/2" << false;
   QTest::newRow("Power is not plain") << "2**8" << false;
   QTest::newRow("clog2 function is not plain") << "$clog2(8)" << false;

   QTest::newRow("Constant in parentheses is plain") << "(8)" << true;
   QTest::newRow("Addition in parentheses is not plain") << "(8 + 2)" << false;
}

QTEST_APPLESS_MAIN(tst_SystemVerilogExpressionParser)

#include "tst_SystemVerilogExpressionParser.moc"

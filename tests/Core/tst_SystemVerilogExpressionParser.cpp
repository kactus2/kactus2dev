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

#include <QTest>
#include <QtMath>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

class tst_SystemVerilogExpressionParser : public QObject
{
    Q_OBJECT

public:
    tst_SystemVerilogExpressionParser();

    void testInputs();

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

    void testStringValues();
    void testStringValues_data();

    void testParseComparison();
    void testParseComparison_data();

    void testParseMathFunctions();
    void testParseMathFunctions_data();

    void testBitwiseOperations();
    void testBitwiseOperations_data();

    void testTernaryOperations();
    void testTernaryOperations_data();

    void testParserPerformance();
    void testParserPerformance_data();
};

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::tst_SystemVerilogExpressionParser()
//-----------------------------------------------------------------------------
tst_SystemVerilogExpressionParser::tst_SystemVerilogExpressionParser()
{

}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testInputs()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testInputs()
{
    QFETCH(QString, expression);
    QFETCH(QString, expectedResult);
    QFETCH(bool, expectedValid);

    SystemVerilogExpressionParser parser;

    bool isValid = false;
    QString parserResult = parser.parseExpression(expression, &isValid);

    QCOMPARE(isValid, expectedValid);
    if (expectedValid)
    {
        QCOMPARE(parserResult, expectedResult);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseConstant()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseConstant()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseConstant_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseConstant_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty value evaluates to empty") << "" << "" << true;

    QTest::newRow("Operator only evaluates to invalid") << "-" << "x" << false;

	//! Booleans
    QTest::newRow("Boolean true should evaluate to 1") << "true" << "1" << true;
    QTest::newRow("Boolean false should evaluate to 0") << "false" << "0" << true;
    QTest::newRow("Array of {true,false} should evaluate to an array of {1,0}") << "{true,false}" << "{1,0}" << true;
    QTest::newRow("Boolean True should evaluate to 1") << "True" << "1" << true;
    QTest::newRow("Boolean False should evaluate to 0") << "False" << "0" << true;
    QTest::newRow("Boolean True should evaluate to 1") << "TRUe" << "1" << true;
    QTest::newRow("Boolean False should evaluate to 0") << "fALSe" << "0" << true;

    //! Array.
    QTest::newRow("Array of {1,1} should evaluate to an array of {1,1}") << "{1,1}" << "{1,1}" << true;
    QTest::newRow("Array with one value is a valid array") << "{2}" << "{2}" << true;
    QTest::newRow("Inadequate array of {1,1,#} should evaluate to unknown") << "{1,1,#}" << "x" << false;
    //QTest::newRow("Inadequate array of {1,} should evaluate to unknown") << "{1,}" << "x" << false;
    QTest::newRow("Inadequate array of ,1} should evaluate to unknown") << ",1}" << "x" << false;
    QTest::newRow("Hexadecimal values in array are evaluated as decimal values") << "{'h10,'h14}" << "{16,20}" << true;
    QTest::newRow("Array {1,1} should evaluate to an array of {1,1}") << "{1,1}" << "{1,1}" << true;
    //QTest::newRow("Array consisting of multiple types of data is unknown") << "{1.1,1,\"helloWorld\"}" << "x"  << false;
    QTest::newRow("Array inside an array is ok") << "{1,{1,1}}" << "{1,{1,1}}"  << true;
    QTest::newRow("Array inside an array as first is ok") << "{{1,1},1}" << "{{1,1},1}"  << true;

    //! Decimal numbers.
    QTest::newRow("Decimal number 0 should evaluate to 0") << "0" << "0" << true;
    QTest::newRow("Decimal number 1 should evaluate to 1") << "1" << "1" << true;
    QTest::newRow("Decimal number 7 should evaluate to 7") << "7" << "7" << true;
    QTest::newRow("Decimal number '4 should evaluate to 4") << "4" << "4" << true;

    QTest::newRow("Positive decimal number") << "+1" << "1" << true;
    QTest::newRow("Negative decimal number") << "-1" << "-1" << true;

    QTest::newRow("Decimal number 'd2 should evaluate to 2") << "'d2" << "2" << true;
    QTest::newRow("Decimal number 'D8 should evaluate to 8") << "'D8" << "8" << true;

    QTest::newRow("Decimal number 'sd2 should evaluate to 2") << "'sd2" << "2" << true;
    QTest::newRow("Decimal number 'sD2 should evaluate to 2") << "'sD2" << "2" << true;

    QTest::newRow("Decimal number 'Sd3 should evaluate to 3") << "'Sd3" << "3" << true;
    QTest::newRow("Decimal number 'SD3 should evaluate to 3") << "'SD3" << "3" << true;
    
    QTest::newRow("Decimal number 1'd3 with size should evaluate to 3") << "1'd3" << "3" << true;

    QTest::newRow("Decimal number '2 should evaluate to 2") << "'2" << "2" << true;

    QTest::newRow("Decimal number with underscore should evaluate without underscore") << "10_000" << "10000" << true;
    QTest::newRow("Decimal number with multiple underscores should evaluate without underscores") 
        << "1_000_000" << "1000000" << true;
    QTest::newRow("Decimal number with base and underscores should evaluate") << "'sd10_000" << "10000" << true;
    QTest::newRow("Decimal number 1000005 evaluates to 1000005") << "1000005" << "1000005" << true;

    //! Fixed-point numbers.
    QTest::newRow("Fixed-point number 0.0 should evaluate to 0.0") << "0.0" << "0.0" << true;
    QTest::newRow("Fixed-point number 0.5 should evaluate to 0.5") << "0.5" << "0.5" << true;
    QTest::newRow("Fixed-point number 0.25 should evaluate to 0.25") << "0.25" << "0.25" << true;
    QTest::newRow("Fixed-point number 1.0 should evaluate to 1.0") << "1.0" << "1.0" << true;
    QTest::newRow("Negative fixed-point number") << "-1.0" << "-1.0" << true;

    //! Hexadecimal numbers.
    QTest::newRow("Hexadecimal number without base should evaluate to unknown") << "ff" << "x" << false;
    QTest::newRow("Hexadecimal number 'h1 should evaluate to 1") << "'h1" << "1" << true;
    QTest::newRow("Hexadecimal number 'hA should evaluate to 10") << "'hA" << "10" << true;
    QTest::newRow("Hexadecimal number 'Hf should evaluate to 15") << "'Hf" << "15" << true;

    QTest::newRow("Hexadecimal number 'shf should evaluate to 15") << "'shf" << "15" << true;
    QTest::newRow("Hexadecimal number 'sH2 should evaluate to 2") << "'sH2" << "2" << true;
    
    QTest::newRow("Hexadecimal number 'Shb should evaluate to 11") << "'Shb" << "11" << true;
    QTest::newRow("Hexadecimal number 'SH5 should evaluate to 5") << "'SH5" << "5" << true;

    QTest::newRow("Hexadecimal number 8'h02 with size should evaluate to 2") << "8'd02" << "2" << true;
    QTest::newRow("Hexadecimal number 32'h00000001 with size should evaluate to 1") << "32'h00000001" << "1" << true;

    QTest::newRow("Hexadecimal number with underscore should evaluate to decimal without underscore") 
        << "'h1_F" << "31" << true;
    QTest::newRow("Hexadecimal number with multiple underscores should evaluate to decimal without underscores") 
        << "'h0_F_F" << "255" << true;

    //! Binary numbers.
    QTest::newRow("Binary number 'b1 should evaluate to 1") << "'b1" << "1" << true;
    QTest::newRow("Binary number 'b10 should evaluate to 2") << "'b10" << "2" << true;
    QTest::newRow("Binary number 'b111 should evaluate to 7") << "'b111" << "7" << true;

    QTest::newRow("Binary number 'sb11 should evaluate to 3") << "'sb11" << "3" << true;
    QTest::newRow("Binary number 'sB10 should evaluate to 2") << "'sB10" << "2" << true;

    QTest::newRow("Binary number 'Sb110 should evaluate to 6") << "'Sb110" << "6" << true;
    QTest::newRow("Binary number 'SB100 should evaluate to 4") << "'sB100" << "4" << true;

    QTest::newRow("Binary number 4'b0111 with size should evaluate to 7") << "4'b0111" << "7" << true;

    QTest::newRow("Binary number with underscore should evaluate to decimal without underscore") 
        << "'b1_1" << "3" << true;
    QTest::newRow("Binary number with multiple underscores should evaluate to decimal without underscores") 
        << "'b1_1_1_1" << "15" << true;

    //! Octal numbers.
    QTest::newRow("Octal number 'o1 should evaluate to 1") << "'o1" << "1" << true;
    QTest::newRow("Octal number 'o7 should evaluate to 7") << "'o7" << "7" << true;
    QTest::newRow("Octal number 'o10 should evaluate to 8") << "'o10" << "8" << true;
    QTest::newRow("Octal number 'O12 should evaluate to 10") << "'O12" << "10" << true;

    QTest::newRow("Octal number 'so4 should evaluate to 4") << "'so4" << "4" << true;
    QTest::newRow("Octal number 'sO17 should evaluate to 15") << "'sO17" << "15" << true;

    QTest::newRow("Octal number 'So20 should evaluate to 16") << "'So20" << "16" << true;
    QTest::newRow("Octal number 'SO2 should evaluate to 2") << "'SO2" << "2" << true;

    QTest::newRow("Octal number 3'o011 with size should evaluate to 9") << "3'o011" << "9" << true;

    QTest::newRow("Octal number with underscore should evaluate to decimal without underscore") 
        << "'o1_0" << "8" << true;
    QTest::newRow("Octal number with multiple underscores should evaluate to decimal without underscores") 
        << "'o1_0_0" << "64" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseAddition()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseAddition()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseAddition_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseAddition_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("No addition on constant equals constant") << "1" << "1" << true;
    QTest::newRow("Addition without second operand is unknown") << "1+" << "x" << false;
    QTest::newRow("Addition without second operand and whitespaces is unknown") << " 1 + " << "x" << false;

	//! Booleans
	QTest::newRow("True + false + true equals two") << "True + false + true" << "2" << true;
	QTest::newRow("true+true inside array equals 2") << "{false,true+true}" << "{0,2}" << true;

    //! Array.
    QTest::newRow("Addition of arrays should evaluate to unknown") << "{1,1}+{1,1}" << "x" << false;
    QTest::newRow("1+1 inside array equals 2") << "{1+1,1}" << "{2,1}" << true;
    QTest::newRow("Constant added to an array is unknown") << "{1,1} + 1" << "x" << false;

    //! Decimal numbers.
    QTest::newRow("Constant plus zero equals constant") << "1+0" << "1" << true;
    QTest::newRow("One plus one equals two") << "1+1" << "2" << true;
    QTest::newRow("One plus two equals three") << "1+2" << "3" << true;
    QTest::newRow("Two plus one equals three") << "2+1" << "3" << true;
    QTest::newRow("Two plus two with whitespaces equals four") << "2 + 2" << "4" << true;
    QTest::newRow("Multiple whitespaces") << " 1  +     1 " << "2" << true;
    QTest::newRow("Sum of decimal values with sizes and signs") << "8'sd8 + 8'sd9" << "17" << true;
    QTest::newRow("Large decimal sum 1000003+1") << "1000003+1" << "1000004" << true;
    QTest::newRow("Larger decimal sum 1000004+1") << "1000004+1" << "1000005" << true;
    QTest::newRow("Sum of large decimal values 1000003+1000003") << "1000003+1000003" << "2000006" << true;

    //! Fixed-point numbers.
    QTest::newRow("One plus one equals exactly two") << "1.0+1.0" << "2.0" << true;
    QTest::newRow("One plus decimal two equals three") << "1.0+2" << "3.0" << true;
    QTest::newRow("Decimal one plus real two equals three") << "1+2.0" << "3.0" << true;
    QTest::newRow("Half plus one quarter equals 0.75") << "0.5 + 0.25" << "0.75" << true;
    QTest::newRow("Four quarters equals 1") << "0.25 + 0.25 + 0.25 + 0.25" << "1.00" << true;
    QTest::newRow("Real sum 1000004.012 + 1.84444444") << "1000004.012 + 1.84444444" << "1000005.85644444" << true;

    //! Hexadecimal numbers.
    QTest::newRow("Sum of hexadecimal 'h10 + 'h0 equals 16") << "'h10 + 'h0" << "16" << true;
    QTest::newRow("Sum of hexadecimal 'h10 + 'h10 equals 32") << "'h10 + 'h10" << "32" << true;
    QTest::newRow("Sum of hexadecimal values with sizes and signs") << "8'sh10 + 8'sh02" << "18" << true;
    QTest::newRow("Constant hexadecimal with long size") << "32'h00000001" << "1" << true;

    //! Binary numbers.
    QTest::newRow("Sum of binary 'b100 + 'b010 equals 6") << "'b100 + 'b010" << "6" << true;
    QTest::newRow("Sum of binary 'b0001 + 'b0111 equals 8") << "'b0001 + 'b0111" << "8" << true;
    QTest::newRow("Sum of binary values with sizes and signs") << "4'sb0010 + 4'sb0001" << "3" << true;

    //! Octal numbers.
    QTest::newRow("Sum of octal 'o1 + 'o1 equals 2") << "'o1 + 'o1" << "2" << true;
    QTest::newRow("Sum of octal 'o2 + 'o7 equals 9") << "'o2 + 'o7" << "9" << true;
    QTest::newRow("Sum of octal values with sizes and signs") << "3'so3 + 3'so4" << "7" << true;

    QTest::newRow("Sum of decimal and octal 10 + 'o10 equals 18") << "10 + 'o10" << "18" << true;
    QTest::newRow("Sum of hexadecimal and binary 'h10 + 'b10 equals 18") << "'h10 + 'b10" << "18" << true;

    QTest::newRow("Sum of multiple values of same base") << "'h10 + 'h10 + 'h01" << "33" << true;
    QTest::newRow("Sum of multiple values of different bases") << "'hA + 'b1010 + 'o12 + 10" << "40" << true;

    QTest::newRow("Sum of ascii characters and a number is unknown") << "text + 2" << "x" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseSubtraction()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseSubtraction()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseSubtraction_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseSubtraction_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");

    //! Arrays.
    QTest::newRow("One minus one equals zero in array") << "{1,1-1}" << "{1,0}" << true;
    QTest::newRow("Subtraction of arrays evaluates to unknown") << "{1,1} - {1,1}" << "x" << false;
    QTest::newRow("Constant subtracted from an array is unknown") << "{1,1} - 1" << "x" << false;

    //! Decimal numbers.
    QTest::newRow("Constant minus zero equals constant") << "1-0" << "1" << true;
    QTest::newRow("Constant minus constant equals zero") << "1-1" << "0" << true;
    QTest::newRow("Four minus two equals two") << "4-2" << "2" << true;
    QTest::newRow("three minus one equals two") << "3-1" << "2" << true;
    QTest::newRow("Negative result") << "1-2" << "-1" << true;
    QTest::newRow("Negative operands") << "-1-2" << "-3" << true;
    QTest::newRow("Two minus one with whitespaces equals one") << "2 - 1" << "1" << true;
    QTest::newRow("Multiple whitespaces") << " 2  -     1 " << "1" << true;
    QTest::newRow("Subtraction of decimal values with sizes and signs") << "8'sd9 - 8'sd2" << "7" << true;

    //! Fixed-point numbers.
    QTest::newRow("4 - 2.25 two equals 1.75") << "4.0 - 2.25" << "1.75" << true;
    QTest::newRow("Negative fixed-point result") << "0.5 - 2.0" << "-1.5" << true;

    //! Hexadecimal numbers.
    QTest::newRow("Subtraction of hexadecimal 'h10 - 'h1 equals 15") << "'h10 - 'h1" << "15" << true;
    QTest::newRow("Subtraction of hexadecimal 'h0E - 'h4 equals 10") << "'h0E - 'h4" << "10" << true;
    QTest::newRow("Subtraction of hexadecimal values with sizes and signs") << "8'sh0B - 8'sh04" << "7" << true;

    //! Binary numbers.
    QTest::newRow("Subtraction of binary 'b100 - 'b010 equals 2") << "'b100 - 'b010" << "2" << true;
    QTest::newRow("Subtraction of binary 'b0111 - 'b011 equals 4") << "'b0111 - 'b011" << "4" << true;
    QTest::newRow("Subtraction of binary values with sizes and signs") << "4'sb0010 - 4'sb0001" << "1" << true;

    //! Octal numbers.
    QTest::newRow("Subtraction of octal 'o3 - 'o1 equals 2") << "'o3 - 'o1" << "2" << true;
    QTest::newRow("Subtraction of octal 'o7 - 'o2 equals 5") << "'o7 - 'o2" << "5" << true;
    QTest::newRow("Subtraction of octal values with sizes and signs") << "3'so6 - 3'so3" << "3" << true;

    QTest::newRow("Subtraction of multiple values of same base") << "'h12 - 'h8 - 'h01" << "9" << true;
    QTest::newRow("Subtraction of multiple values of different bases") << "'h20 - 'b1010 - 'o12 - 2" << "10" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseMultiply()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseMultiply()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseMultiply_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseMultiply_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Zero times zero equals zero") << "0*0" << "0" << true;
    QTest::newRow("Zero times constant equals zero") << "0*1" << "0" << true;
    QTest::newRow("Constant times zero equals zero") << "1*0" << "0" << true;

    //! Arrays.
    QTest::newRow("Multiplication of arrays is unknown") << "{1,1}*{1,1}" << "x" << false;
    QTest::newRow("Two times two inside array is four") << "{2*2,2*2}" << "{4,4}" << true;
    QTest::newRow("Constant multiplied with an array is unknown") << "{1,1} * 1" << "x" << false;

    //! Decimal numbers.
    QTest::newRow("One times constant equals constant") << "1*2" << "2" << true;
    QTest::newRow("Two times two equals four") << "2*2" << "4" << true;
    QTest::newRow("Two times three with whitespaces equals six") << "2 * 3" << "6" << true;
    QTest::newRow("Multiple whitespaces") << " 4  *     4 " << "16" << true;
    QTest::newRow("Multiply of decimal values with sizes and signs") << "8'sd9 * 8'sd2" << "18" << true;

    QTest::newRow("Negative constant times negative constant gives positive value") << "-2*-2" << "4" << true;
    QTest::newRow("Negative constant times positive constant gives negative value") << "-2*2" << "-4" << true;
    QTest::newRow("Positive constant times negative constant gives negative value") << "2*-2" << "-4" << true;

    //! Fixed-point numbers.
    QTest::newRow("Two times one half equals one") << "2*0.5" << "1.0" << true;
    QTest::newRow("Multiple fraction digits") << "0.240*0.21" << "0.050" << true;
    QTest::newRow("Zero times anything is zero") << "0.0*42" << "0.0" << true;
    QTest::newRow("Large real multiplication 10000.1 * 20002") << "10000.1 * 20002" << "200022000.2" << true;

    //! Hexadecimal numbers.
    QTest::newRow("Multiply of hexadecimal 'h10 * 'h2 equals 32") << "'h10 * 'h2" << "32" << true;
    QTest::newRow("Multiply of hexadecimal 'hA * 'h3 equals 30") << "'hA * 'h3" << "30" << true;
    QTest::newRow("Multiply of hexadecimal values with sizes and signs") << "8'sh0A * 8'sh05" << "50" << true;
    
    //! Binary numbers.
    QTest::newRow("Multiply of binary 'b100 * 'b010 equals 8") << "'b100 * 'b010" << "8" << true;
    QTest::newRow("Multiply of binary 'b0111 * 'b001 equals 7") << "'b0111 * 'b001" << "7" << true;
    QTest::newRow("Multiply of binary values with sizes and signs") << "4'sb0010 * 4'sb0011" << "6" << true;
    
    //! Octal numbers.
    QTest::newRow("Multiply of octal 'o3 * 'o7 equals 21") << "'o3 * 'o7" << "21" << true;
    QTest::newRow("Multiply of octal 'o7 * 'o10 equals 56") << "'o7 * 'o10" << "56" << true;
    QTest::newRow("Multiply of octal values with sizes and signs") << "6'so6 * 6'so3" << "18" << true;

    QTest::newRow("Multiply of multiple values of same base") << "'h2 * 'h8 * 'h01" << "16" << true;
    QTest::newRow("Multiply of multiple values of different bases") << "'h04 * 'b0100 * 'o2 * 2" << "64" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseDivision()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseDivision()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseDivision_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseDivision_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Constant divided by same constant equals one") << "2/2" << "1" << true;
    QTest::newRow("Constant divided by one equals the same constant") << "2/1" << "2" << true;
    QTest::newRow("Constant divided by zero equals x") << "1/0" << "x" << false;

    //! Arrays.
    QTest::newRow("Division of arrays evaluates to unknown") << "{1,1}/{1,1}" << "x" << false;
    QTest::newRow("Ten divided by two inside array is five") << "{10/2,1,1}" << "{5,1,1}" << true;
    QTest::newRow("Constant divided by an array is unknown") << "1 / {1,1}" << "x" << false;
    QTest::newRow("An array divided by a constant is unknown") << "{1,1} / 1" << "x" << false;

    //! Decimal numbers.
    QTest::newRow("Division result for integer truncates towards zero") << "3/2" << "1" << true;

    QTest::newRow("Four divided by two equals two") << "4/2" << "2" << true;
    QTest::newRow("Ten divided by two equals five") << "10/2" << "5" << true;
    QTest::newRow("Division with whitespaces") << "7 / 3" << "2" << true;
    QTest::newRow("Division with multiple whitespaces") << " 4   /  2 " << "2" << true;
    QTest::newRow("Division of decimal values with sizes and signs") << "8'sd9 / 8'sd3" << "3" << true;

    QTest::newRow("Negative constant divided by negative constant gives positive value") << "-2/-2" << "1" << true;
    QTest::newRow("Negative constant divided by positive constant gives negative value") << "-2/2" << "-1" << true;
    QTest::newRow("Positive constant divided by negative constant gives negative value") << "2/-2" << "-1" << true;

    //! Fixed-point numbers.
    QTest::newRow("Division result for integer divisor truncates towards zero") << "3/2.0" << "1" << true;
    QTest::newRow("Division result for real does not truncate towards zero") << "3.0/2" << "1.5" << true;
    QTest::newRow("Number of fractional digits is five by default") << "10.0/3" << "3.3" << true;
    QTest::newRow("Division by zero is unknown") << "3.0/0.0" << "x" << false;
    QTest::newRow("Large real division 10000.1 / 20002") << "10000.1 / 20002" << "0.5" << true;
    QTest::newRow("Real division 1 / 7 ") << "1 / 7" << "0" << true;
    QTest::newRow("Real division 1.0 / 7 ") << "1.0 / 7" << "0.1" << true;
    QTest::newRow("Real division 1.00 / 7 ") << "1.00 / 7" << "0.14" << true;
    QTest::newRow("Real division 1.000 / 7 ") << "1.000 / 7" << "0.143" << true;
    QTest::newRow("Real division 1.0000 / 7 ") << "1.0000 / 7" << "0.1429" << true;

    //! Hexadecimal numbers.
    QTest::newRow("Division of hexadecimal 'h10 / 'h2 equals 8") << "'h10 / 'h2" << "8" << true;
    QTest::newRow("Division of hexadecimal 'hA / 'h3 equals 3") << "'hA / 'h3" << "3" << true;
    QTest::newRow("Division of hexadecimal values with sizes and signs") << "8'sh0A / 8'sh05" << "2" << true;

    //! Binary numbers.
    QTest::newRow("Division of binary 'b100 / 'b010 equals 2") << "'b100 / 'b010" << "2" << true;
    QTest::newRow("Division of binary 'b0111 / 'b010 equals 3") << "'b0111 / 'b010" << "3" << true;
    QTest::newRow("Division of binary values with sizes and signs") << "4'sb1000 / 4'sb0010" << "4" << true;

    //! Octal numbers.
    QTest::newRow("Division of octal 'o10 / 'o2 equals 4") << "'o10 /'o2" << "4" << true;
    QTest::newRow("Division of octal 'o20 / 'o4 equals 4") << "'o20 / 'o4" << "4" << true;
    QTest::newRow("Division of octal values with sizes and signs") << "6'so6 / 6'so3" << "2" << true;

    QTest::newRow("Divide multiple values of same base") << "'h20 / 'h02 / 'h08" << "2" << true;
    QTest::newRow("Divide multiple values of different bases") << "'h40 / 'b0010 / 'o2 / 2" << "8" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParsePower()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParsePower()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParsePower_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParsePower_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");

    //! Arrays.
    QTest::newRow("Array to the power of an array is unknown") << "{1,1}**{1,1}" << "x" << false;
    QTest::newRow("Two to the power of three is eight inside an array") << "{1,2**3,1}" << "{1,8,1}" << true;
    QTest::newRow("Constant to the power of an array is unknown") << "1**{1,1}" << "x" << false;
    QTest::newRow("An array to the power of a constant is unknown") << "{1,1} ** 1" << "x" << false;

    //! Decimal numbers.
    QTest::newRow("One to the power of zero equals one") << "1**0" << "1" << true;
    QTest::newRow("Zero to the power of zero equals one") << "0**0" << "1" << true;
    QTest::newRow("Anything to the power of zero equals one") << "42**0" << "1" << true;

    QTest::newRow("Zero to the power of a negative value is unknown") << "0**-2" << "x" << false;

    QTest::newRow("One to the power of anything equals one") << "1**42" << "1" << true;

    QTest::newRow("Negative one to the power of an even value equals one") << "-1**2" << "1" << true;
    QTest::newRow("Negative one to the power of an odd value equals negative one") << "-1**1" << "-1" << true;

    QTest::newRow("Negative one to the power of an even negative value equals one") << "-1**-2" << "1" << true;
    QTest::newRow("Negative one to the power of an odd negative value equals negative one") << "-1**-1" << "-1" << true;
    
    QTest::newRow("Integer truncates to zero") << "2 ** -1" << "0" << true;

    QTest::newRow("Two to the power of two equals four") << "2**2" << "4" << true;
    QTest::newRow("Negative Two to the power of two equals four") << "-2**2" << "4" << true;
    QTest::newRow("Two to the power of three equals eight") << "2**3" << "8" << true;
    QTest::newRow("Negative two to the power of three equals negative eight") << "-2**3" << "-8" << true;
    QTest::newRow("Two to the power of five equals 32") << "2**5" << "32" << true;
    QTest::newRow("Three to the power of three equals 27") << "3**3" << "27" << true;
    QTest::newRow("Power operation with whitespaces") << "4 ** 3" << "64" << true;

    QTest::newRow("Multiple power operations") << "2**2**2" << "16" << true;
    QTest::newRow("Power of multiple different bases") << "'h02 ** 'b0010 ** 'o2 ** 2" << "256" << true;

    //! Fixed-point numbers.
    QTest::newRow("Real gives real reciprocal") << "2.0 ** -1" << "0.5" << true;
    QTest::newRow("2.5 to the power of two") << "2.50 ** 2" << "6.25" << true;
    QTest::newRow("Large real in power 10000.1 ** 2") << "10000.1 ** 2" << "100002000.0" << true;
    QTest::newRow("Large real in larger power 10000.1**3") << "10000.1**3" << "1000030000300.0" << true;
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
    QTest::newRow("$clog(1) equals 0") << "$clog2(1)" << "0";
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

    QTest::newRow("Nested clog2s") << "$clog2($clog2(256))" << "3";
    //! Arrays.
    QTest::newRow("$clog(3) inside an array equals 2") << "{$clog2(3),1,1,1}" << "{2,1,1,1}";
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

    QTest::newRow("Unary minus in expression") << "-$clog2(32)-($sqrt(25)-(-$pow(2,2)))" << "-14";
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseExpressionWithParathesis()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseExpressionWithParathesis()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseExpressionWithParathesis_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseExpressionWithParathesis_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty parenthesis") << "()" << "" << true;
    QTest::newRow("Single constant in parenthesis") << "(1)" << "1" << true;
    QTest::newRow("Single operation in parenthesis") << "(1+1)" << "2" << true;
    QTest::newRow("Single operation and whitespace in parenthesis") << " ( 1 + 1 ) " << "2" << true;

    QTest::newRow("Addition in parenthesis precedes multiplication") << "(1 + 1)*4" << "8" << true;
    QTest::newRow("Subtraction in parenthesis precedes multiplication") << "(4 - 1)*3" << "9" << true;
    QTest::newRow("Multiplication in parenthesis precedes division") << "9/(3*3)" << "1" << true;

    QTest::newRow("Nested parentheses") << "2*(3*(1 + 1) - 5)" << "2" << true;
    QTest::newRow("Double parentheses") << "((1 + 3)*4)/2" << "8" << true;
    QTest::newRow("Double parentheses with whitespace") << "( (1 + 3)*4 ) /2" << "8" << true;
    QTest::newRow("Triple parentheses") << "(((1 + 3))*4)/2" << "8" << true;
    QTest::newRow("Deeply nested parentheses") << "((((2))))" << "2" << true;
    QTest::newRow("Deeply nested parentheses with whitespace") << " ( ((  (2 ) ))  ) " << "2" << true;

    QTest::newRow("Parallel parentheses") << "(1 + 1)*(1 + 1)" << "4" << true;
    QTest::newRow("Parallel and nested parentheses") << "(2 * (1 + 1))*(1 + 1)" << "8" << true;
    QTest::newRow("Multiple parallel parentheses") << "(1+1)*(1+1)*(1+1)*(1+1)*(1+1)" << "32" << true;

    QTest::newRow("Mismatched open parentheses") << "((1)" << "x" << false;
    QTest::newRow("Mismatched closed parentheses") << "(1))" << "x" << false;
    QTest::newRow("Parentheses wrong way") << ")1(" << "x" << false;
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
    QFETCH(bool, expectedPlain);

    SystemVerilogExpressionParser parser;

    QCOMPARE(parser.isPlainValue(expression), expectedPlain);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testIsPlainValue_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testIsPlainValue_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<bool>("expectedPlain");

    QTest::newRow("Decimal constant is plain") << "1" << true;
    QTest::newRow("Hexadecimal constant is plain") << "'h1" << true;
    QTest::newRow("Octal constant is plain") << "'o1" << true;
    QTest::newRow("Binary constant is plain") << "'b1" << true;
    QTest::newRow("String is plain") << "\"text\"" << true;
    QTest::newRow("Empty value is plain") << "" << true;
    QTest::newRow("Decimal with preceding and trailing spaces is plain") << "  1  " << true;

    QTest::newRow("Addition is not plain") << "'h1 + 'h1" << false;
    QTest::newRow("Subtraction is not plain") << "'o2 - 'o1" << false;
    QTest::newRow("Multiply is not plain") << "2 * 2" << false;
    QTest::newRow("Division is not plain") << "4/2" << false;
    QTest::newRow("Power is not plain") << "2**8" << false;
    QTest::newRow("clog2 function is not plain") << "$clog2(8)" << false;

    QTest::newRow("Addition in parentheses is not plain") << "(8 + 2)" << false;

    QTest::newRow("Large negative number is plain") << "-99999" << true;
    QTest::newRow("Even larger negative number is plain") << "-99999999999999" << true;
    QTest::newRow("Even larger negative number is plain") << "-999999999999999999" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testStringValues()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testStringValues()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testStringValues_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testStringValues_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty string literal") << "\"\"" << "\"\"" << true;
    QTest::newRow("Non-empty string value is parsed as is.") << "\"A string.\"" << "\"A string.\"" << true;
    QTest::newRow("Non-empty string literal with whitespace") << "\" text \"" << "\" text \"" << true;;

    QTest::newRow("String missing a double quote is unknown") << "\"text" << "x" << false;

    QTest::newRow("String comparison for equal strings") << "\"string1\"==\"string1\"" << "1" << true;
    QTest::newRow("String comparison for not equal strings") << "\"string1\"==\"string2\"" << "0" << true;
    QTest::newRow("String not equal comparison for equal strings") << "\"string1\"!=\"string1\"" << "0" << true;
    QTest::newRow("String not equal comparison for not equal strings") << "\"string1\"!=\"string2\"" << "1" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseComparison()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseComparison()
{
    QFETCH(QString, expression);
    QFETCH(int, expectedResult);

    SystemVerilogExpressionParser parser;

    QString parserResult = parser.parseExpression(expression);

    QCOMPARE(parserResult.toInt(), expectedResult);
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseComparison_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseComparison_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<int>("expectedResult");

    QTest::newRow("Single value: 10 is greater than 2 returns 1") << "10 > 2" << 1;
    QTest::newRow("Single value: 2 is greater than 10 returns 0") << "2 > 10" << 0;
    QTest::newRow("Single value: 10 is lesser than 2 returns 0") << "10 < 2" << 0;
    QTest::newRow("Single value: 2 is lesser than 10 returns 1") << "2 < 10" << 1;

    QTest::newRow("5 is greater than 5 returns 0") << "5>5" << 0;
    QTest::newRow("5 is lesser than 5 returns 0") << "5<5" << 0;

    QTest::newRow("Expression: 14*2 is greater than 2-3*2+8 returns 1") << "14*2 > 2-3*2+8" << 1;
    QTest::newRow("Expression: 2-3*2+8 is greater than 14*2 returns 0") << "2 > 10" << 0;
    QTest::newRow("Expression: 14*2 is lesser than 2-3*2+8 returns 0") << "10 < 2" << 0;
    QTest::newRow("Expression: 2-3*2+8 is lesser than 14*2 returns 1") << "2 < 10" << 1;

    QTest::newRow("Single value: 10 is equal to 10 returns 1") << "21==21" << 1;
    QTest::newRow("Single value: 10 is equal to 2 returns 0") << "10==2" << 0;
    QTest::newRow("Single value: 10 is equal to 2 returns 0") << "1==(8==8)" << 1;

    QTest::newRow("Expression: 14-2*2 is equal to 2*5 returns 1") << "14-2*2 == 2*5" << 1;
    QTest::newRow("Expression: 14-2*2+1 is equal to 2*5 returns 0") << "14-2*2+1 == 2*5" << 0;

    QTest::newRow("Single value: 10 is greater or equal than 2 returns 1") << "10>=2" << 1;
    QTest::newRow("Single value: 10 is greater or equal than 10 returns 1") << "10>=10" << 1;
    QTest::newRow("Single value: 10 is greater or equal than 11 returns 0") << "10>=11" << 0;

    QTest::newRow("Expression: 10/2 is greater or equal than 2+1-2 returns 1") << "10/2>=2+1-2" << 1;
    QTest::newRow("Expression: 10/2 is greater or equal than 2*3-1 returns 1") << "10/2>=2*3-1" << 1;
    QTest::newRow("Expression: 10/2 is greater or equal than 11+1*10 returns 0") << "10/2>=11+1*10" << 0;

    QTest::newRow("Single value: 2 is lesser or equal than 10 returns 1") << "2<=10" << 1;
    QTest::newRow("Single value: 10 is lesser or equal than 10 returns 1") << "10<=10" << 1;
    QTest::newRow("Single value: 11 is lesser or equal than 10 returns 0") << "11<=10" << 0;

    QTest::newRow("Expression: 2+1-2 is lesser or equal than 10/2 returns 1") << "2+1-2<=10/2" << 1;
    QTest::newRow("Expression: 2*3-1 is lesser or equal than 10/2 returns 1") << "2*3-1<=10/2" << 1;
    QTest::newRow("Expression: 11+1*10 is lesser or equal than 10/2 returns 0") << "11+1*10<=10/2" << 0;

    QTest::newRow("Single value: 2 is not equal to 10 returns 1") << "2!=10" << 1;
    QTest::newRow("Single value: 10 is not equal to 10 returns 0") << "10!=10" << 0;

    QTest::newRow("Expression: 2+1-2 is not equal to 10/2 returns 1") << "2+1-2!=10/2" << 1;
    QTest::newRow("Expression: 2*3-1 is not equal to 10/2 returns 0") << "2*3-1!=10/2" << 0;
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseMathFunctions()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseMathFunctions()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParseMathFunctions_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParseMathFunctions_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("System function $clog2(), ceil of log2") << "$clog2(17)" << "5" << true;

    QTest::newRow("Simple power function") << "$pow(2, 3)" << "8" << true;

    QTest::newRow("Square root of 0 is 0") << "$sqrt(0)" << "0" << true;
    QTest::newRow("Square root of 4 equals 2") << "$sqrt(4)" << "2" << true;
    QTest::newRow("Square root of negative value is invalid") << "$sqrt(-2)" << "x" << false;

    QTest::newRow("Exp() of 0 is 1") << "$exp(0)" << "1" << true;
    QTest::newRow("Exp() of 1") << "$exp(1)" << QString::number(qExp(1)) << true;
    QTest::newRow("Exp() of -2") << "$exp(-2)" << QString::number(qExp(-2)) << true;
}   

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testBitwiseOperations()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testBitwiseOperations()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testBitwiseOperations_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testBitwiseOperations_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");


    QTest::newRow("Negation") << "~4'b001" << "-2" << true;
    QTest::newRow("Shift left") << "4'b0010 << 2" << "8" << true;
    QTest::newRow("Shift right") << "4'b1000 >> 3" << "1" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testTernaryOperations()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testTernaryOperations()
{
    testInputs();
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testTernaryOperations_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testTernaryOperations_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<QString>("expectedResult");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Ternary true?1:0 should be 1") << "true?1:0" << "1" << true;
    QTest::newRow("Ternary false?1:0 should be 0") << "false?1:0" << "0" << true;
    QTest::newRow("Ternary 1?2:3 should be 2") << "1?2:3" << "2" << true;
    QTest::newRow("Ternary 1-1?2:3+1 should be 4") << "1-1?2:3+1" << "4" << true;
    QTest::newRow("Ternary ? should invalid") << "?" << "0" << false;
    QTest::newRow("Ternary : should invalid") << ":" << "0" << false;
    QTest::newRow("Ternary (3+2)?(4+6) should invalid") << "(3+2)?(4+6)" << "0" << false;
    QTest::newRow("Ternary (3+2):(4+6) should invalid") << "(3+2):(4+6)" << "0" << false;
    QTest::newRow("Ternary (3+2)?((3+3)?(4+7):1):0 should be 11") << "(3+2)?((3+3)?(4+7):1):0" << "11" << true;
    QTest::newRow("Ternary (1>2)?((3+3)?(4+7):1):0 should be 0") << "(1>2)?((3+3)?(4+7):1):0" << "0" << true;
    QTest::newRow("Ternary 3+2?3+3?4+7:1:0 should be 11") << "3+2?3+3?4+7:1:0" << "11" << true;
    QTest::newRow("Ternary 1>2?3+3?4+7:1:0 should be 0") << "1>2?3+3?4+7:1:0" << "0" << true;
    QTest::newRow("Ternary 1?11:2?12:0 should be 11") << "1?11:2?12:0" << "11" << true;
    QTest::newRow("Ternary 1?11:(2?12:0) should be 11") << "1?11:(2?12:0)" << "11" << true;
    QTest::newRow("Ternary 1?12?1:0:22 should be 1") << "1?12?1:0:22" << "1" << true;

}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParserPerformance()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParserPerformance()
{
    QFETCH(QString, expression);
    QFETCH(int, expectedResult);

    SystemVerilogExpressionParser parser;

    QString parserResult;

    QBENCHMARK
    {
        parserResult = parser.parseExpression(expression);
    }

    QCOMPARE(parserResult.toInt(), expectedResult);
    
}

//-----------------------------------------------------------------------------
// Function: tst_SystemVerilogExpressionParser::testParserPerformance_data()
//-----------------------------------------------------------------------------
void tst_SystemVerilogExpressionParser::testParserPerformance_data()
{
    QTest::addColumn<QString>("expression");
    QTest::addColumn<int>("expectedResult");

    QTest::newRow("Constant value") << "'hFF" << 255;
    QTest::newRow("Basic operations") << "12*12 + 8/2 -8" << 140;
    QTest::newRow("clog2 operations") << "$clog2(256) - $clog2(8)" << 5;
    QTest::newRow("Nested clog2 operations") << "$clog2($clog2(256))" << 3;
    QTest::newRow("Sum of multiple values of different bases") << "'hA + 'b1010 + 'o12 + 10" << 40;

}

QTEST_APPLESS_MAIN(tst_SystemVerilogExpressionParser)

#include "tst_SystemVerilogExpressionParser.moc"

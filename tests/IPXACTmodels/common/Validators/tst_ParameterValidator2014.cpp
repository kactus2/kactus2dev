//-----------------------------------------------------------------------------
// File: tst_ParameterValidator2014.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 11.12.2014
//
// Description:
// Unit test for class SystemVerilogValidator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/common/validators/ParameterValidator2014.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/Component/Choice.h>
#include <IPXACTmodels/common/Enumeration.h>

class tst_ParameterValidator2014 : public QObject
{
    Q_OBJECT

public:
    tst_ParameterValidator2014();

private slots:

    void testValueIsValidForGivenType();
    void testValueIsValidForGivenType_data();

    void testValidityWithMaximumValueAndType();
    void testValidityWithMaximumValueAndType_data();

    void testValidityWithMinimumValueAndType();
    void testValidityWithMinimumValueAndType_data();

    void testValidityWithVectorAndType();
    void testValidityWithVectorAndType_data();

    void testValidityWithChoice();
    void testValidityWithChoice_data();

    void testBitVectorIsValid();

private:

    bool errorIsNotFoundInErrorlist(QString const& expectedError, QVector<QString>& errorlist);

};

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::tst_ParameterValidator2014()
//-----------------------------------------------------------------------------
tst_ParameterValidator2014::tst_ParameterValidator2014()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testValueIsValidForGivenType()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testValueIsValidForGivenType()
{
    QFETCH(QString, value);
    QFETCH(QString, type);    
    QFETCH(bool, isValid);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    ParameterValidator2014 validator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());
    QCOMPARE(validator.hasValidValueForType(value, type), isValid);

    if (!isValid && !value.isEmpty())
    {
        QSharedPointer<Parameter> parameter (new Parameter());
        parameter->setName("param");
        parameter->setType(type);
        parameter->setValue(value);
        parameter->setValueId("parameterid");

        QVector<QString> errorList;
        validator.findErrorsIn(errorList, parameter, "test");

        QString expectedError = QObject::tr("Value '%1' is not valid for type %2 in parameter param within test")
            .arg(value).arg(type);
        if (errorIsNotFoundInErrorlist(expectedError, errorList))
        {
            QFAIL("No error message found.");
        }
        parameter.clear();
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testValueIsValidForGivenType_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testValueIsValidForGivenType_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("type");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Hexadecimal expression is valid for bit") << "'ha + 'hb" << "bit" << true;
    QTest::newRow("String is not valid for bit type") << "\"text\"" << "bit" << false;

    QTest::newRow("0 is valid for bit type") << "0" << "bit" << true;
    QTest::newRow("1 is valid for bit type") << "1" << "bit" << true;
//     QTest::newRow("13 is invalid for bit type") << "13" << "bit" << false;
//     QTest::newRow("13*2 is invalid for bit type") << "13*2" << "bit" << false;
    QTest::newRow("13 is valid for bit type") << "13" << "bit" << true;
    QTest::newRow("13*2 is valid for bit type") << "13*2" << "bit" << true;
    QTest::newRow("'1 is valid for bit type") << "'1" << "bit" << true;
    QTest::newRow("Binary 'b0 is valid for bit type") << "'b0" << "bit" << true;
    QTest::newRow("Binary 'b1 is valid for bit type") << "'b1" << "bit" << true;
    QTest::newRow("Binary 2'b11 with size is valid for bit type") << "2'b11" << "bit" << true;
    QTest::newRow("Binary 2'b11+3'b101 is valid for bit type") << "2'b11+3'b101" << "bit" << true;
    QTest::newRow("Array {1,0} is valid for bit type") << "{1,0}" << "bit" << true;
    QTest::newRow("Array {1, 1.0, \"text\"} is invalid for bit type") << "{1, 1.0, \"text\"}" << "bit" << false;
    QTest::newRow("Array '{1,1} is valid for bit type") << "'{1,1}" << "bit" << true;
    QTest::newRow("Array {3'b101,3'b111} is valid for bit type") << "{3'b101,3'b111}" << "bit" << true;
    QTest::newRow("Array {3'b101,2'b10} is invalid for bit type") << "{3'b101,2'b10}" << "bit" << false;
    QTest::newRow("Array {2'b10, 2'b11 } is valid for bit type") << "{2'b10, 2'b11 }" << "bit" << true;
    QTest::newRow("Array {6'b10_1101,6'b11_1010} is valid for bit type") << "{6'b10_1101,6'b11_1010}" << "bit" << true;
    QTest::newRow("Array {'b1, 'b0} is valid for bit type") << "{'b1, 'b0}" << "bit" << true;
    QTest::newRow("Array {'b11, 'b00} is invalid for bit type") << "{'b11, 'b00}" << "bit" << false;
    QTest::newRow("Array {1'b1+1'b1, 2'b10} is valid for bit type") << "{1'b1+1'b1, 2'b10}" << "bit" << true;

    QTest::newRow("Binary 'b11 without size is valid for bit type") << "'b11" << "bit" << true;
    QTest::newRow("Binary 3'b13 is not valid for bit type") << "3'b13" << "bit" << false;
    QTest::newRow("Binary with hexadecimal numbers is not valid for bit type") << "3'b3f" << "bit" << false;

    QTest::newRow("Hexadecimal 'h1 is valid for bit type") << "'h1" << "bit" << true;
    QTest::newRow("Hexadecimal with size is valid for bit type") << "8'h11" << "bit" << true;
    QTest::newRow("Hexadecimal with size and numbers 1-9 is valid for bit type") << "16'h1289" << "bit" << true;
    QTest::newRow("Hexadecimal with size and a-f characters is valid for bit type") << "8'haf" << "bit" << true;
    QTest::newRow("Hexadecimal with size and upper case is valid for bit type") << "8'hED" << "bit" << true;

    QTest::newRow("Hexadecimal without size is valid for bit type") << "'h11" << "bit" << true;
//     QTest::newRow("2 is invalid for bit type") << "2" << "bit" << false;
    QTest::newRow("2 is valid for bit type") << "2" << "bit" << true;
//     QTest::newRow("-1 is invalid for bit type") << "-1" << "bit" << false;
    QTest::newRow("-1 is valid for bit type") << "-1" << "bit" << true;
    QTest::newRow("text is invalid for bit type") << "some text" << "bit" << false;
    QTest::newRow("string is invalid for bit type") << "\"some other text\"" << "bit" << false;
    QTest::newRow("Expression is valid for bit type") << "'h1 + 'h1 - 'b1" << "bit" << true;

    QTest::newRow("0 is valid for byte type") << "0" << "byte" << true;
    QTest::newRow("1 is valid for byte type") << "1" << "byte" << true;
    QTest::newRow("64 is valid for byte type") << "64" << "byte" << true;
    QTest::newRow("127 is valid for byte type") << "127" << "byte" << true;
    QTest::newRow("128 is invalid for byte type") << "128" << "byte" << false;
    QTest::newRow("-1 is valid for byte type") << "-1" << "byte" << true;
    QTest::newRow("-100 is valid for byte type") << "-100" << "byte" << true;
    QTest::newRow("-128 is valid for byte type") << "-128" << "byte" << true;
    QTest::newRow("-129 is invalid for byte type") << "-129" << "byte" << false;
    QTest::newRow("-9999999999 is invalid for byte type") << "-9999999999" << "byte" << false;
    QTest::newRow("Expression is valid for byte type") << "'h01 + 'h0E" << "byte" << true;
    QTest::newRow("Octal number 'o2 is valid for byte type") << "'o2" << "byte" << true;
    QTest::newRow("Negative octal number -'o4 is valid for byte type") << "-'h4" << "byte" << true;
    QTest::newRow("text is invalid for byte type") << "some text" << "byte" << false;
    QTest::newRow("string is invalid for byte type") << "\"some other text\"" << "byte" << false;
    QTest::newRow("Fractional number 0.123 is invalid for byte type") << "0.123" << "byte" << false;
    QTest::newRow("Array {12,12} is valid for byte type") << "{12,12}" << "byte" << true;
    QTest::newRow("Array {1, 1.0, \"text\"} is invalid for byte type") << "{1, 1.0, \"text\"}" << "byte" << false;

    QTest::newRow("0 is valid for shortint type") << "0" << "shortint" << true;
    QTest::newRow("1 is valid for shortint type") << "1" << "shortint" << true;
    QTest::newRow("128 is valid for shortint type") << "128" << "shortint" << true;
    QTest::newRow("32767 is valid for shortint type") << "32767" << "shortint" << true;
    QTest::newRow("32768 is invalid for shortint type") << "32768" << "shortint" << false;
    QTest::newRow("-1 is valid for shortint type") << "-1" << "shortint" << true;
    QTest::newRow("-129 is valid for shortint type") << "-129" << "shortint" << true;
    QTest::newRow("-32768 is valid for shortint type") << "-32768" << "shortint" << true;
    QTest::newRow("-32769 is valid for shortint type") << "-32769" << "shortint" << false;
    QTest::newRow("Expression is valid for shortint type") << "12 + 12" << "shortint" << true;
    QTest::newRow("Hexadecimal number 'ha is valid for shortint type") << "'ha" << "shortint" << true;
    QTest::newRow("Negative octal number -'o5 is valid for shortint type") << "-'o5" << "shortint" << true;
    QTest::newRow("text is invalid for shortint type") << "some text" << "shortint" << false;
    QTest::newRow("string is invalid for shortint type") << "\"some other text\"" << "shortint" << false;
    QTest::newRow("Fractional number 0.123 is invalid for shortint type") << "0.123" << "shortint" << false;
    QTest::newRow("Array {128,128} is valid for shortint type") << "{128,128}" << "shortint" << true;
    QTest::newRow("Array {1, 1.0, \"text\"} is invalid for shortint type") << "{1, 1.0, \"text\"}" << "shortint" << false;

    QTest::newRow("0 is valid for int type") << "0" << "int" << true;
    QTest::newRow("1 is valid for int type") << "1" << "int" << true;
    QTest::newRow("32768 is valid for int type") << "32768" << "int" << true;
    QTest::newRow("2147483647 is valid for int type") << "2147483647" << "int" << true;
    QTest::newRow("2147483648 is invalid for int type") << "2147483648" << "int" << false;
    QTest::newRow("-1 is valid for int type") << "-1" << "int" << true;
    QTest::newRow("-32769 is valid for int type") << "-32769" << "int" << true;
    QTest::newRow("-2147483648 is valid for int type") << "-2147483648" << "int" << true;
    QTest::newRow("-2147483649 is invalid for int type") << "-2147483649" << "int" << false;
    QTest::newRow("Expression is valid for int type") << "12 + 12" << "int" << true;
    QTest::newRow("Binary number 'b01 is valid for int type") << "'b01" << "int" << true;
    QTest::newRow("Negative hexadecimal number -'hf is valid for int type") << "-'hf" << "int" << true;
    QTest::newRow("text is invalid for int type") << "some text" << "int" << false;
    QTest::newRow("string is invalid for int type") << "\"some other text\"" << "int" << false;
    QTest::newRow("Fractional number 0.123 is invalid for int type") << "0.123" << "int" << false;
    QTest::newRow("Array {32768,32768} is valid for int type") << "{32768,32768}" << "int" << true;
    QTest::newRow("Array {'h10,'h10} is valid for int type") << "{'h10,'h10}" << "int" << true;
    QTest::newRow("Array {1, 1.0, \"text\"} is invalid for int type") << "{1, 1.0, \"text\"}" << "int" << false;
    QTest::newRow("Array {1+3, 4+8+5} is valid for int type") << "{1+3, 4+8+5}" << "int" << true;
    QTest::newRow("Array {'h1+3, 'h4+8+5} is valid for int type") << "{'h1+3, 'h4+8+5}" << "int" << true;

    QTest::newRow("0 is valid for longint type") << "0" << "longint" << true;
    QTest::newRow("1 is valid for longint type") << "1" << "longint" << true;
    QTest::newRow("2147483648 is valid for longint type") << "2147483648" << "longint" << true;
//     QTest::newRow("9223372036854775807 is valid for longint type") << "9223372036854775807" << "longint" << true;
    //QTest::newRow("9223372036854775808 is invalid for longint type") << "9223372036854775808" << "longint" << false;
    QTest::newRow("-1 is valid for longint type") << "-1" << "longint" << true;
    QTest::newRow("-2147483649 is valid for longint type") << "-2147483649" << "longint" << true;
//     QTest::newRow("-9223372036854775808 is valid for longint type") << "-9223372036854775808" << "longint" << true;
    //QTest::newRow("-9223372036854775809 is invalid for longint type") << "-9223372036854775809" << "longint" << false;
    QTest::newRow("Expression is valid for longint type") << "12 + 12" << "longint" << true;
    QTest::newRow("Octal number 'o6 is valid for longint type") << "'o6" << "longint" << true;
    QTest::newRow("Negative binary number -'b101 is valid for longint type") << "-'b101" << "longint" << true;
    QTest::newRow("text is invalid for longint type") << "some text" << "longint" << false;
    QTest::newRow("string is invalid for longint type") << "\"some other text\"" << "longint" << false;
    QTest::newRow("Fractional number 0.123 is invalid for longint type") << "0.123" << "longint" << false;
    QTest::newRow("Array {21474836,21474836} is valid for longint type") << "{21474836,21474836}" << "longint" << true;
    QTest::newRow("Array {1, 1.0, \"text\"} is invalid for longint type") << "{1, 1.0, \"text\"}" << "longint" << false;

    QTest::newRow("0.0 is valid for shortreal type") << "0.0" << "shortreal" << true;
    QTest::newRow("10.18 is valid for shortreal type") << "10.18" << "shortreal" << true;
    QTest::newRow("-28.1423 is valid for shortreal type") << "-28.1423" << "shortreal" << true;
    QTest::newRow("Octal number 'o5 is valid for shortreal type") << "'o5" << "shortreal" << true;
    QTest::newRow("Negative binary number -'b010 is valid for shortreal type") << "-'b010" << "shortreal" << true;
    QTest::newRow("text is invalid for shortreal type") << "some text" << "shortreal" << false;
    QTest::newRow("string is invalid for shortreal type") << "\"some other text\"" << "shortreal" << false;
    QTest::newRow("Array {1.2, 1.2} is valid for shortreal type") << "{1.2, 1.2}" << "shortreal" << true;
    QTest::newRow("Array {1, 1.0, \"text\"} is invalid for shortreal type") << "{1, 1.0, \"text\"}" << "shortreal" << false;

    QTest::newRow("0.0 is valid for real type") << "0.0" << "real" << true;
    QTest::newRow("10.18 is valid for real type") << "10.18" << "real" << true;
    QTest::newRow("-28.1423 is valid for real type") << "-28.1423" << "real" << true;
    QTest::newRow("Hexadecimal number 'ha4 is valid for real type") << "'ha4" << "real" << true;
    QTest::newRow("Negative octal number -'o12 is valid for real type") << "-'o12" << "real" << true;
    QTest::newRow("text is invalid for real type") << "some text" << "real" << false;
    QTest::newRow("string is invalid for real type") << "\"some other text\"" << "real" << false;
    QTest::newRow("Array {10.18,10.18} is valid for real type") << "{10.18,10.18}" << "real" << true;
    QTest::newRow("Array {1, 1.0, \"text\"} is invalid for real type") << "{1, 1.0, \"text\"}" << "real" << false;

    QTest::newRow("Empty is invalid for string type") << "" << "string" << false;
    QTest::newRow("String without double quotes is invalid") << "text" << "string" << false;
    QTest::newRow("Empty string is valid for string type") << "\"\"" << "string" << true;
    QTest::newRow("String in double quotes is valid") << "\"text\"" << "string" << true;
    QTest::newRow("Decimal number is invalid for string type") << "1" << "string" << false;
    QTest::newRow("Expression is invalid for string type") << "12 + 12" << "string" << false;
    QTest::newRow("Array {\"text\",\"text\"} is valid for string type") << "{\"text\",\"text\"}" << "string" << true;
    QTest::newRow("Array {1, 1.0, \"text\"} is invalid for string type") << "{1, 1.0, \"text\"}" << "string" << false;

    QTest::newRow("Text is invalid for empty type") << "text" << "" << false;
    QTest::newRow("Text in quotes is valid for empty type") << "\"text\"" << "" << true;
    //QTest::newRow("Reference is invalid for empty type") << "parameterid" << "" << false;
    QTest::newRow("Expression: divide by zero is invalid") << "4/0" << "" << false;
    QTest::newRow("Expression: 1+1 is valid") << "1+1" << "" << true;
    QTest::newRow("Single character x is invalid") << "x" << "" << false;
    QTest::newRow("Parenthesis only is invalid") << "()" << "" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testValidityWithMaximumValueAndType()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testValidityWithMaximumValueAndType()
{  
    QFETCH(QString, type);   
    QFETCH(QString, value);   
    QFETCH(QString, maximum);
    QFETCH(bool, isValid);

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setName("param");
    parameter->setType(type);
    parameter->setValue(value);
    parameter->setMaximumValue(maximum);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    ParameterValidator2014 validator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());
    QCOMPARE(validator.hasValidValue(parameter), isValid);

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testValidityWithMaximumValueAndType_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testValidityWithMaximumValueAndType_data()
{
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("maximum");
    QTest::addColumn<bool>("isValid");

    //QTest::newRow("Any value is valid for empty type") << "" << "value42" << "" << true;
    QTest::newRow("Any double quoted string is valid for string type") << "string" << "\"42\"" << "1" << true;

    QTest::newRow("Real value is valid for real type") << "real" << "2.0" << "" << true;
    QTest::newRow("Real value is valid for shortreal type") << "shortreal" << "2.0" << "4.0" << true;
    QTest::newRow("Real value is valid for greater maximum") << "real" << "2.0" << "4.0" << true;
    QTest::newRow("Real value is not valid for smaller maximum") << "real" << "2.0" << "1.0" << false;
   
    QTest::newRow("Real value is not valid for int type") << "int" << "2.0" << "" << false;

    QTest::newRow("Int value is valid") << "int" << "1" << "" << true;
    QTest::newRow("Int value is valid with greater maximum") << "int" << "1" << "3" << true;
    QTest::newRow("Int value is not valid with smaller maximum") << "int" << "1" << "0" << false;
    QTest::newRow("Int value is not valid with invalid maximum") << "int" << "1" << "f" << true;

    QTest::newRow("Array {2,2,2} is valid with valid maximum 3") << "int" << "{2,2,2}" << "3" << true;
    QTest::newRow("Array '{2,2,2} is valid with valid maximum 3") << "int" << "'{2,2,2}" << "3" << true;
    QTest::newRow("Array {1,3,4} is invalid with valid maximum 3") << "int" << "{1,3,4}" << "3" << false;
    QTest::newRow("Array '{4,4,2} is valid with valid maximum 3") << "int" << "'{4,4,2}" << "3" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testValidityWithMinimumValueAndType()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testValidityWithMinimumValueAndType()
{
    QFETCH(QString, type);   
    QFETCH(QString, value);   
    QFETCH(QString, minimum);
    QFETCH(bool, isValid);

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setName("param");
    parameter->setType(type);
    parameter->setValue(value);
    parameter->setMinimumValue(minimum);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    ParameterValidator2014 validator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());
    QCOMPARE(validator.hasValidValue(parameter), isValid);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testValidityWithMinimumValueAndType_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testValidityWithMinimumValueAndType_data()
{
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("minimum");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Any double quoted string is valid for string type") << "string" << "\"42\"" << "1" << true;

    QTest::newRow("Real value is valid for real type") << "real" << "2.0" << "" << true;
    QTest::newRow("Real value is valid for shortreal type") << "shortreal" << "2.0" << "1.0" << true;
    QTest::newRow("Real value is valid for smaller minimum") << "real" << "2.0" << "1.0" << true;
    QTest::newRow("Real value is not valid for greater minimum") << "real" << "2.0" << "3.0" << false;

    QTest::newRow("Real value is not valid for int type") << "int" << "2.0" << "" << false;

    QTest::newRow("Int value is valid") << "int" << "1" << "" << true;
    QTest::newRow("Int value is valid with smaller minimum") << "int" << "1" << "0" << true;
    QTest::newRow("Int value is not valid with greater minimum") << "int" << "1" << "2" << false;
    QTest::newRow("Int value is not valid with invalid minimum") << "int" << "1" << "f" << true;

    QTest::newRow("Array {2,2,2} is valid with valid minimum 1") << "int" << "{2,2,2}" << "1" << true;
    QTest::newRow("Array '{2,2,2} is valid with valid minimum 1") << "int" << "'{2,2,2}" << "1" << true;
    QTest::newRow("Array {1,3,4} is invalid with valid minimum 2") << "int" << "{1,3,4}" << "2" << false;
    QTest::newRow("Array '{2,2,2} is valid with valid minimum 3") << "int" << "'{2,2,2}" << "3" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testValidityWithVectorAndType()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testValidityWithVectorAndType()
{
    QFETCH(QString, vectorLeft);
    QFETCH(QString, vectorRight);
    QFETCH(QString, type);
    QFETCH(bool, isValid);

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setName("param");
    parameter->setType(type);
    parameter->setVectorLeft(vectorLeft);
    parameter->setVectorRight(vectorRight);

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());
    ParameterValidator2014 validator(parser, QSharedPointer<QList<QSharedPointer<Choice> > > ());
    QCOMPARE(validator.hasValidVector(parameter), isValid);

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testValidityWithVectorAndType_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testValidityWithVectorAndType_data()
{
    QTest::addColumn<QString>("type");
    QTest::addColumn<QString>("vectorLeft");
    QTest::addColumn<QString>("vectorRight");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Vector is valid for a bit-type parameter") << "bit" << "4" << "5" << true;
    QTest::newRow("Vector is invalid for int-type parameter") << "int" << "4" << "5" << false;
    QTest::newRow("Vector is invalid for real-type parameter") << "real" << "4" << "5" << false;
    QTest::newRow("Vector is invalid for string-type parameter") << "string" << "4" << "5" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testValidityWithChoice()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testValidityWithChoice()
{
    QFETCH(QString, value);
    QFETCH(QString, enumerationValue1);
    QFETCH(QString, enumerationvalue2);
    QFETCH(QString, type);
    QFETCH(bool, isValid);

    QSharedPointer<Enumeration> enumeration1 (new Enumeration());
    enumeration1->setValue(enumerationValue1);
    QSharedPointer<Enumeration> enumeration2 (new Enumeration());
    enumeration2->setValue(enumerationvalue2);

    QSharedPointer<Choice> testChoice(new Choice());
    testChoice->setName("TestChoice");
    testChoice->enumerations()->append(enumeration1);
    testChoice->enumerations()->append(enumeration2);

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setName("param");
    parameter->setType(type);
    parameter->setValue(value);
    parameter->setChoiceRef(testChoice->name());

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    QSharedPointer<QList<QSharedPointer<Choice> > > choiceList (new QList<QSharedPointer<Choice> > ());
    choiceList->append(testChoice);

    ParameterValidator2014 validator(parser, choiceList);
    QCOMPARE(validator.validate(parameter), isValid);

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testValidityWithChoice_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testValidityWithChoice_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<QString>("enumerationValue1");
    QTest::addColumn<QString>("enumerationvalue2");
    QTest::addColumn<QString>("type");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Value 4 is valid for enumerations 4 and 5, with type int") << "4" << "4" << "5" << "int" << true;
    QTest::newRow("Value 3 is invalid for enumerations 4 and 5, with type int") << "3" << "4" << "5" << "int" << false;
    QTest::newRow("Value 5 is invalid for enumerations 4 and 5, with type string") << "5" << "4" << "5" << "string" << false;

    QTest::newRow("Value test is valid for enumerations test and test1, with type string") << "\"test\"" <<
        "\"test\"" << "\"test1\"" << "string" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::testBitVectorIsValid()
//-----------------------------------------------------------------------------
void tst_ParameterValidator2014::testBitVectorIsValid()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setType("bit");
    testParameter->setValue("1");

    QSharedPointer<ExpressionParser> parser(new SystemVerilogExpressionParser());

    QSharedPointer<QList<QSharedPointer<Choice> > > choiceList (new QList<QSharedPointer<Choice> > ());

    ParameterValidator2014 validator(parser, choiceList);
    QCOMPARE(validator.validate(testParameter), true);

    testParameter->setVectorLeft("0");
    testParameter->setVectorRight("4");

    QCOMPARE(validator.validate(testParameter), true);

    testParameter->setType("int");

    QCOMPARE(validator.validate(testParameter), false);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, testParameter, "test");

    QString expectedError = QObject::tr("Invalid bit vector values specified for %1 %2 within %3")
        .arg(testParameter->elementName()).arg(testParameter->name()).arg("test");
    if (errorIsNotFoundInErrorlist(expectedError, errorList))
    {
        QFAIL("No error message found.");
    }

    testParameter->setVectorLeft("");
    testParameter->setVectorRight("");

    QCOMPARE(validator.validate(testParameter), true);

    testParameter->setType("bit");
    testParameter->setVectorLeft("1");
    testParameter->setVectorRight("2");

    QCOMPARE(validator.validate(testParameter), true);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator2014::errorIsNotFoundInErrorlist()
//-----------------------------------------------------------------------------
bool tst_ParameterValidator2014::errorIsNotFoundInErrorlist(QString const& expectedError,
    QVector<QString>& errorlist)
{
    if (!errorlist.contains(expectedError))
    {
        qDebug() << "The following error:" << endl << expectedError << endl << "was not found in errorlist:";
        foreach(QString error, errorlist)
        {
            qDebug() << error; 
        }
        return true;
    }

    return false;
}

QTEST_APPLESS_MAIN(tst_ParameterValidator2014)

#include "tst_ParameterValidator2014.moc"

//-----------------------------------------------------------------------------
// File: tst_ParameterValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.11.2014
//
// Description:
// Unit test for class ParameterValidator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/choice.h>
#include <IPXACTmodels/Enumeration.h>
#include <IPXACTmodels/parameter.h>
#include <IPXACTmodels/validators/ParameterValidator.h>

class tst_ParameterValidator : public QObject
{
    Q_OBJECT

public:
    tst_ParameterValidator();

private slots:

    void testResolve();
    void testResolve_data();

    void testIdIsSpecifiedForResolveUserAndGenerated();
    void testIdIsSpecifiedForResolveUserAndGenerated_data();

    void testFormat();
    void testFormat_data();

    void testValueForBoolFormat();
    void testValueForBoolFormat_data();

    void testBitStringLengthIsSetForBitStringFormatAndNotSetForOtherFormats();
    void testBitStringLengthIsSetForBitStringFormatAndNotSetForOtherFormats_data();

    void testMinimumValueNotCheckedForEmptyBoolOrStringFormat();
    void testMinimumValueNotCheckedForEmptyBoolOrStringFormat_data();

    void testMinimumValueIsValidForGivenFormat();
    void testMinimumValueIsValidForGivenFormat_data();

    void testValidityWithLongMinimumValue();
    void testValidityWithLongMinimumValue_data();

    void testValidityWithFloatMinimumValue();
    void testValidityWithFloatMinimumValue_data();

    void testMaximumValueIsValidForGivenFormat();
    void testMaximumValueIsValidForGivenFormat_data();

    void testValidityWithLongMaximumValue();
    void testValidityWithLongMaximumValue_data();

    void testChoiceReference();
    void testChoiceReference_data();

    void testValueUsingChoice();
    void testValueUsingChoice_data();

private:
        
    Parameter* createParameterWithName();

    QStringList findErrors(Parameter* parameter);

    bool errorIsNotFoundInErrorlist(QString const& expectedError, QStringList const& errorlist) const;

    void testValidityWithMinimumValueAndFormat(QString const& format);
    void testValidityWithMaximumValueAndFormat(QString const& format);
   

};

//-----------------------------------------------------------------------------
// Function: tst_Parameter::tst_Parameter()
//-----------------------------------------------------------------------------
tst_ParameterValidator::tst_ParameterValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testResolve()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testResolve()
{
    QFETCH(QString, resolve);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();
    parameter->setValueResolve(resolve);

    ParameterValidator validator;
    QVERIFY(validator.hasValidResolve(parameter) == expectedValid);

    if (!expectedValid)
    {
        QStringList errorlist =  findErrors(parameter);


        QString expectedError = "Invalid resolve " + resolve + " specified for parameter param within test";
        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
    delete parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testResolve_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testResolve_data()
{
    QTest::addColumn<QString>("resolve");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("empty resolve is valid") << "" << true;
    QTest::newRow("immediate resolve is valid") << "immediate" << true;
    QTest::newRow("user resolve is valid") << "user" << true;
    QTest::newRow("generated resolve is valid") << "generated" << true;
    QTest::newRow("dependent resolve is valid") << "dependent" << true;

    QTest::newRow("other resolve is invalid") << "other" << false;
    QTest::newRow("number resolve is invalid") << "1" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testIdIsSpecifiedForResolveUserAndGenerated()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testIdIsSpecifiedForResolveUserAndGenerated()
{
    QFETCH(QString, id);
    QFETCH(QString, resolve);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();
    parameter->setValueId(id);
    parameter->setValueResolve(resolve);

    ParameterValidator validator;
    QVERIFY(validator.hasValidValueId(parameter) == expectedValid);

   if (!expectedValid)
    {
        QStringList errorlist =  findErrors(parameter);

        QString expectedError = "No id specified for parameter param with resolve " + resolve + " within test";
        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
    delete parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testIdIsSpecifiedForResolveUserAndGenerated()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testIdIsSpecifiedForResolveUserAndGenerated_data()
{
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("resolve");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("No id for unspecified resolve is valid") << "" << "" << true;
    QTest::newRow("Id for unspecified resolve is valid") << "1" << "" << true;

    QTest::newRow("No id for resolve user is invalid") << "" << "user" << false;
    QTest::newRow("Id for resolve user is valid") << "1" << "user" << true;

    QTest::newRow("No id for resolve generated is invalid") << "" << "generated" << false;
    QTest::newRow("Id for resolve generated is valid") << "1" << "generated" << true;

    QTest::newRow("No id for resolve immediate is valid") << "" << "immediate" << true;
    QTest::newRow("Id for resolve immediate is valid") << "1" << "immediate" << true;

    QTest::newRow("No id for resolve dependent is valid") << "" << "dependent" << true;
    QTest::newRow("Id for resolve dependent is valid") << "1" << "dependent" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testFormat()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testFormat()
{
    QFETCH(QString, format);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();
    parameter->setValueFormat(format);

    ParameterValidator validator;
    QVERIFY(validator.hasValidFormat(parameter) == expectedValid);

    if (!expectedValid)
    {
        QStringList errorlist = findErrors(parameter);

        QString expectedError = "Invalid format " + format + " specified for parameter param within test";
        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
    delete parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testFormat_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testFormat_data()
{
    QTest::addColumn<QString>("format");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("empty format is valid") << "" << true;
    QTest::newRow("bitString format is valid") << "bitString" << true;
    QTest::newRow("bool format is valid") << "bool" << true;
    QTest::newRow("float format is valid") << "float" << true;
    QTest::newRow("long format is valid") << "long" << true;
    QTest::newRow("string format is valid") << "string" << true;

    QTest::newRow("other format is invalid") << "other" << false;
    QTest::newRow("number format is invalid") << "1" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValueForBoolFormat()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValueForBoolFormat()
{
    QFETCH(QString, value);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();
    parameter->setValueFormat("bool");
    parameter->setValue(value);

    ParameterValidator validator;
    QVERIFY(validator.hasValidValueForFormat(parameter) == expectedValid);

    if (!expectedValid)
    {
        QStringList errorlist =  findErrors(parameter);

        QString expectedError = "Value " + value + " violates format bool in parameter param within test";
        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }

    delete parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValueForBoolFormat_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValueForBoolFormat_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("True is acceptable") << "true" << true;
    QTest::newRow("False is acceptable") << "false" << true;
    QTest::newRow("Other text is not acceptable") << "int" << false;
    QTest::newRow("Number is not acceptable") << "1" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testBitStringLengthIsSetForBitStringFormatAndNotSetForOtherFormats()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testBitStringLengthIsSetForBitStringFormatAndNotSetForOtherFormats()
{
    QFETCH(QString, bitStringLength);
    QFETCH(QString, format);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();
    parameter->setValueFormat(format);
    parameter->setBitStringLength(bitStringLength);

    ParameterValidator validator;
    QVERIFY(validator.hasValidBitStringLength(parameter) == expectedValid);

    if (!expectedValid)
    {
        QStringList errorlist = findErrors(parameter);

        QString expectedError;
        if (!bitStringLength.isEmpty())
        {
            expectedError = "Bit string length specified for format other than bitString "
                "for parameter param within test";
        }
        else
        {
            expectedError = "No bit string length specified for parameter param within test";
        }

        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }

    delete parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testBitStringLengthIsSetForBitStringFormatAndNotSetForOtherFormats_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testBitStringLengthIsSetForBitStringFormatAndNotSetForOtherFormats_data()
{
    QTest::addColumn<QString>("bitStringLength");
    QTest::addColumn<QString>("format");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("No length and no format set is valid") << "" << "" << true;
    QTest::newRow("No length and bool format set is valid") << "" << "bool" << true;
    QTest::newRow("No length and float format set is valid") << "" << "float" << true;
    QTest::newRow("No length and long format set is valid") << "" << "long" << true;
    QTest::newRow("No length and string format set is valid") << "" << "string" << true;

    QTest::newRow("Bit string length and bitString format set is valid") << "1" << "bitString" << true;
    QTest::newRow("No Bit string length and bitString format set is invalid") << "" << "bitString"  << false;

    QTest::newRow("Bit string length set and no format set is invalid") << "1" << "" << false;
    QTest::newRow("Bit string length and bool format set is invalid") << "1" << "bool" <<  false;
    QTest::newRow("Bit string length and float format set is invalid") << "1" << "float" << false;
    QTest::newRow("Bit string length and long format set is invalid") << "1" << "long" << false;
    QTest::newRow("Bit string length and string format set is invalid") << "1" << "string" << false;    
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testMinimumValueIsValidForGivenFormat()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testMinimumValueNotCheckedForEmptyBoolOrStringFormat()
{
    QFETCH(QString, format);

    Parameter* parameter = createParameterWithName();
    parameter->setMinimumValue("100");

    parameter->setValueFormat(format);

    ParameterValidator validator;
    QVERIFY(validator.hasValidMinimumValue(parameter) == true);
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testMinimumValueIsValidForGivenFormat()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testMinimumValueNotCheckedForEmptyBoolOrStringFormat_data()
{
    QTest::addColumn<QString>("format");

    QTest::newRow("no format set") << "";
    QTest::newRow("bool format set") << "bool";
    QTest::newRow("string format set") << "string";
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testMinimumValueIsValidForGivenFormat()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testMinimumValueIsValidForGivenFormat()
{
    QFETCH(QString, format);
    QFETCH(QString, boundary);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();


    parameter->setValue("1000000000");

    parameter->setValueFormat(format);
    parameter->setMinimumValue(boundary);

    QStringList errorList;
    ParameterValidator validator;

    QVERIFY(validator.hasValidMinimumValue(parameter) == expectedValid);

    if (!expectedValid)
    {
        QStringList errorlist = findErrors(parameter);

        QString expectedError = "Minimum value " + boundary + " is not valid for format " + format + 
            " in parameter param within test";

        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }

    delete parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testMinimumValueIsValidForGivenFormat()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testMinimumValueIsValidForGivenFormat_data()
{
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("boundary");
    QTest::addColumn<bool>("expectedValid");

    //! No format.
    QTest::newRow("empty input") << "" << "" << true;
    QTest::newRow("single zero") << "" << "0" << true;
    QTest::newRow("single number") << "" << "9" << true;
    QTest::newRow("single character") << "" << "c" << true;
    QTest::newRow("text") << "" << "input" << true;
    QTest::newRow("zeros and ones") << "" << "01010101" << true;
    QTest::newRow("zeros and other numbers") << "" << "002003008" << true;
    QTest::newRow("zeros and ones in double quotes") << "" << "\"0011\"" << true;
    QTest::newRow("zeros and ones missing first double quote") << "" << "0011\"" << true;
    QTest::newRow("zeros and ones missing last double quote") << "" << "\"0011" << true;

    //! bitString format.
    QTest::newRow("empty bitString minimum") << "bitString" << "" << true;
    QTest::newRow("single zero") << "bitString" << "0" << true;
    QTest::newRow("single number") << "bitString" << "9" << false;
    QTest::newRow("single character") << "bitString" << "c" << false;
    QTest::newRow("text") << "bitString" << "invalid input" << false;
    QTest::newRow("zeros and ones") << "bitString" << "01010101" << true;
    QTest::newRow("zeros and other numbers") << "bitString" << "002003008" << false;
    QTest::newRow("zeros and ones in double quotes") << "bitString" << "\"0011\"" << true;
    QTest::newRow("zeros and ones missing first double quote") << "bitString" << "0011\"" << false;
    QTest::newRow("zeros and ones missing last double quote") << "bitString" << "\"0011" << false;

    //! Long format.
    QTest::newRow("empty long minimum") << "long"  << "" << true;
    QTest::newRow("single number") << "long"  << "0" << true;
    QTest::newRow("negative number") << "long"  << "-1" << true;
    QTest::newRow("positive number") << "long"  << "+1" << true;
    QTest::newRow("long number") << "long"  << "1234567" << true;
    QTest::newRow("expression") << "long"  << "12+12" << false;

    QTest::newRow("single hexadecimal character") << "long"  << "A" << true;
    QTest::newRow("single non-hexadecimal character") << "long"  << "z" << false;
    QTest::newRow("only characters a-f are valid") << "long"  << "nonsense" << false;
    QTest::newRow("hexadecimal with preceding 0x") << "long"  << "0xFFFF" << true;
    QTest::newRow("hexadecimal with preceding 0X") << "long"  << "0XAAAA" << true;
    QTest::newRow("hexadecimal with invalid prefix") << "long"  << "000x000" << false;
    QTest::newRow("hexadecimal with preceding #") << "long"  << "#010" << true;

    QTest::newRow("number with magnitude suffix k") << "long"  << "10k" << true;
    QTest::newRow("number with magnitude suffix K") << "long"  << "10K" << true;
    QTest::newRow("number with magnitude suffix m") << "long"  << "1m" << true;
    QTest::newRow("number with magnitude suffix M") << "long"  << "1M" << true;
    QTest::newRow("number with invalid magnitude suffix") << "long"  << "10Q" << false;
    QTest::newRow("number with multiple magnitude suffixes") << "long"  << "10Mk" << false;

    //! Float format.
    QTest::newRow("single number") << "float" << "0" << true;
    QTest::newRow("negative number") << "float" << "-1" << true;
    QTest::newRow("positive number") << "float" << "+1" << true;
    QTest::newRow("long number") << "float" << "1234567" << true;
    QTest::newRow("expression") << "float" << "12+12" << false;

    QTest::newRow("single decimal") << "float" << "1.0" << true;
    QTest::newRow("four decimals") << "float" << "0.0005" << true;
    QTest::newRow("scientific format") << "float" << "2e5" << true;
    QTest::newRow("scientific format with negative exponent") << "float" << "1E-3" << true;
    QTest::newRow("text is not valid") << "float" << "nonsense" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithLongMinimumValue()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithLongMinimumValue()
{
    testValidityWithMinimumValueAndFormat("long");

}
//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithMinimuValueAndFormat()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithMinimumValueAndFormat(QString const& format)
{
    QFETCH(QString, boundary);
    QFETCH(QString, value);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();
    parameter->setValueFormat(format);
    parameter->setMinimumValue(boundary);
    parameter->setValue(value);

    QStringList errorList;
    QString identifier = "test";

     ParameterValidator validator;

     QVERIFY(validator.hasValidValue(parameter, QSharedPointer<QList<QSharedPointer<Choice> >>()) == expectedValid);

     if (!expectedValid)
     {
         QStringList errorlist =  findErrors(parameter);

         QString expectedError;

         if (value.isEmpty())
         {
             expectedError = "No value specified for parameter param within test";
         }
         else
         {
             expectedError = "Value " + value + " violates minimum value " + boundary + " in parameter "
                 "param within test";
         }

         if (errorIsNotFoundInErrorlist(expectedError, errorlist))
         {
             QFAIL("No error message found.");
         }
     }

     delete parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithLongMinimumValue_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithLongMinimumValue_data()
{
    QTest::addColumn<QString>("boundary");
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("empty values") << "" << "" << false;

    QTest::newRow("decimal zeros") << "0" << "0" << true;
    QTest::newRow("hexadecimal zeros") << "0x0" << "0x0" << true;

    QTest::newRow("decimal minimum compared to lesser decimal") << "2" << "1" << false;
    QTest::newRow("decimal minimum compared to equal decimal") << "1" << "1" << true;
    QTest::newRow("decimal minimum compared to greater decimal") << "1" << "2" << true;

    QTest::newRow("hexadecimal minimum compared to lesser decimal") << "0x2" << "1" << false;
    QTest::newRow("hexadecimal minimum compared to equal decimal") << "0x1" << "1" << true;
    QTest::newRow("hexadecimal minimum compared to greater decimal") << "0x1" << "2" << true;

    QTest::newRow("decimal minimum compared to lesser hexadecimal") << "2" << "0x01" << false;
    QTest::newRow("decimal minimum compared to equal hexadecimal") << "1" << "0x01" << true;
    QTest::newRow("decimal minimum compared to greater hexadecimal") << "1" << "0x02" << true;

    QTest::newRow("hexadecimal minimum compared to lesser hexadecimal") << "0x2" << "0x1" << false;
    QTest::newRow("hexadecimal minimum compared to equal hexadecimal") << "0x1" << "0x1" << true;
    QTest::newRow("hexadecimal minimum compared to greater hexadecimal") << "0x1" << "0x2" << true;

    QTest::newRow("hexadecimal minimum compared to lesser hexadecimal using X") << "0X2" << "0X1" << false;
    QTest::newRow("hexadecimal minimum compared to equal hexadecimal using X") << "0X1" << "0X1" << true;
    QTest::newRow("hexadecimal minimum compared to greater hexadecimal using X") << "0X1" << "0X2" << true;

    QTest::newRow("hexadecimal minimum compared to lesser hexadecimal using #") << "#2" << "#1" << false;
    QTest::newRow("hexadecimal minimum compared to equal hexadecimal using X") << "#1" << "#1" << true;
    QTest::newRow("hexadecimal minimum compared to greater hexadecimal using X") << "#1" << "#2" << true;

    QTest::newRow("positive decimal minimum compared to negative decimal") << "1" << "-1" << false;

    QTest::newRow("decimal minimum compared to lesser decimal with magnitude") << "1025" << "1k" << false;
    QTest::newRow("decimal minimum compared to equal decimal with magnitude") << "1024" << "1k" << true;
    QTest::newRow("decimal minimum compared to greater decimal with magnitude") << "1k" << "1M" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithFloatMinimumValue()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithFloatMinimumValue()
{
    testValidityWithMinimumValueAndFormat("float");
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithFloatMinimumValue_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithFloatMinimumValue_data()
{
    QTest::addColumn<QString>("boundary");
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("empty values") << "" << "" << false;

    QTest::newRow("decimal zeros") << "0" << "0" << true;
    QTest::newRow("multiple zero decimals") << "0.000" << "0.000" << true;

    QTest::newRow("minimum compared to lesser decimal") << "1.11" << "1.10" << false;
    QTest::newRow("minimum compared to equal decimal") << "0.01" << "0.010" << true;
    QTest::newRow("minimum compared to greater decimal") << "0.001" << "0.010" << true;

    QTest::newRow("minimum compared to lesser scientific decimal") << "1.0" << "1e-1" << false;
    QTest::newRow("minimum compared to equal scientific decimal") << "0.01" << "1e-2" << true;
    QTest::newRow("minimum compared to greater scientific decimal") << "0.001" << "1e-2" << true;

    QTest::newRow("scientific minimum compared to lesser decimal") << "1e1" << "0.0" << false;
    //! Comparing two floating point numbers does not work for different formats.
    //QTest::newRow("scientific minimum compared to equal decimal") << "1e-2" << "0.01" << true;
    QTest::newRow("scientific minimum compared to greater decimal") << "1e-3" << "0.010" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::tesMaximumValueIsValidForGivenFormat()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testMaximumValueIsValidForGivenFormat()
{
    QFETCH(QString, format);
    QFETCH(QString, boundary);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();

    parameter->setValue("0");

    parameter->setValueFormat(format);
    parameter->setMaximumValue(boundary);

    QStringList errorList;
    QString identifier = "testMaximumValueIsValidForGivenFormat";

    ParameterValidator validator;

    QVERIFY(validator.hasValidMaximumValue(parameter) == expectedValid);

    if (!expectedValid)
    {
        QStringList errorlist = findErrors(parameter);

        QString expectedError = "Maximum value " + boundary + " is not valid for format " + format + 
            " in parameter param within test";

        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
    delete parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testMaximumValueIsValidForGivenFormat_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testMaximumValueIsValidForGivenFormat_data()
{
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("boundary");
    QTest::addColumn<bool>("expectedValid");

    //! No format.
    QTest::newRow("empty input") << "" << "" << true;
    QTest::newRow("single zero") << "" << "0" << true;
    QTest::newRow("single number") << "" << "9" << true;
    QTest::newRow("single character") << "" << "c" << true;
    QTest::newRow("text") << "" << "input" << true;
    QTest::newRow("zeros and ones") << "" << "01010101" << true;
    QTest::newRow("zeros and other numbers") << "" << "002003008" << true;
    QTest::newRow("zeros and ones in double quotes") << "" << "\"0011\"" << true;
    QTest::newRow("zeros and ones missing first double quote") << "" << "0011\"" << true;
    QTest::newRow("zeros and ones missing last double quote") << "" << "\"0011" << true;

    //! bitString format.
    QTest::newRow("empty bitString minimum") << "bitString" << "" << true;
    QTest::newRow("single zero") << "bitString" << "0" << true;
    QTest::newRow("single number") << "bitString" << "9" << false;
    QTest::newRow("single character") << "bitString" << "c" << false;
    QTest::newRow("text") << "bitString" << "invalid input" << false;
    QTest::newRow("zeros and ones") << "bitString" << "01010101" << true;
    QTest::newRow("zeros and other numbers") << "bitString" << "002003008" << false;
    QTest::newRow("zeros and ones in double quotes") << "bitString" << "\"0011\"" << true;
    QTest::newRow("zeros and ones missing first double quote") << "bitString" << "0011\"" << false;
    QTest::newRow("zeros and ones missing last double quote") << "bitString" << "\"0011" << false;

    //! Long format.
    QTest::newRow("empty long minimum") << "long"  << "" << true;
    QTest::newRow("single number") << "long"  << "0" << true;
    QTest::newRow("positive number") << "long"  << "+1" << true;
    QTest::newRow("long number") << "long"  << "1234567" << true;
    QTest::newRow("expression") << "long"  << "12+12" << false;

    QTest::newRow("single hexadecimal character") << "long"  << "A" << true;
    QTest::newRow("single non-hexadecimal character") << "long"  << "z" << false;
    QTest::newRow("only characters a-f are valid") << "long"  << "nonsense" << false;
    QTest::newRow("hexadecimal with preceding 0x") << "long"  << "0xFFFF" << true;
    QTest::newRow("hexadecimal with preceding 0X") << "long"  << "0XAAAA" << true;
    QTest::newRow("hexadecimal with invalid prefix") << "long"  << "000x000" << false;
    QTest::newRow("hexadecimal with preceding #") << "long"  << "#010" << true;

    QTest::newRow("number with magnitude suffix k") << "long"  << "10k" << true;
    QTest::newRow("number with magnitude suffix K") << "long"  << "10K" << true;
    QTest::newRow("number with magnitude suffix m") << "long"  << "10m" << true;
    QTest::newRow("number with magnitude suffix M") << "long"  << "10M" << true;
    QTest::newRow("number with magnitude suffix g") << "long"  << "1g" << true;
    QTest::newRow("number with magnitude suffix G") << "long"  << "1G" << true;
    QTest::newRow("number with magnitude suffix t") << "long"  << "1t" << true;
    QTest::newRow("number with magnitude suffix T") << "long"  << "1T" << true;
    QTest::newRow("number with invalid magnitude suffix") << "long"  << "10Q" << false;
    QTest::newRow("number with multiple magnitude suffixes") << "long"  << "10Mk" << false;

    //! Float format.
    QTest::newRow("single number") << "float" << "0" << true;
    QTest::newRow("positive number") << "float" << "+1" << true;
    QTest::newRow("long number") << "float" << "1234567" << true;
    QTest::newRow("expression") << "float" << "12+12" << false;

    QTest::newRow("single decimal") << "float" << "1.0" << true;
    QTest::newRow("four decimals") << "float" << "0.0005" << true;
    QTest::newRow("scientific format") << "float" << "2e5" << true;
    QTest::newRow("scientific format with negative exponent") << "float" << "1E-3" << true;
    QTest::newRow("text is not valid") << "float" << "nonsense" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithLongMaximumValue()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithLongMaximumValue()
{
    testValidityWithMaximumValueAndFormat("long");
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithMinimuValueAndFormat()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithMaximumValueAndFormat(QString const& format)
{
    QFETCH(QString, boundary);
    QFETCH(QString, value);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();
    parameter->setValueFormat(format);
    parameter->setMaximumValue(boundary);
    parameter->setValue(value);

    QStringList errorList;
    QString identifier = "test";

    ParameterValidator validator;

    QVERIFY(validator.hasValidValue(parameter, QSharedPointer<QList<QSharedPointer<Choice> > >()) == expectedValid);

    if (!expectedValid)
    {
        QStringList errorlist =  findErrors(parameter);

        QString expectedError;
        if (value.isEmpty())
        {
            expectedError = "No value specified for parameter param within test";
        }
        else
        {
            expectedError = "Value " + value + " violates maximum value " + boundary + " in parameter "
                "param within test";
        }

        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
    delete parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithLongMaximumValue_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValidityWithLongMaximumValue_data()
{
    QTest::addColumn<QString>("boundary");
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("empty values") << "" << "" << false;

    QTest::newRow("decimal zeros") << "0" << "0" << true;
    QTest::newRow("hexadecimal zeros") << "0x0" << "0x0" << true;

    QTest::newRow("decimal maximum compared to lesser decimal") << "2" << "1" << true;
    QTest::newRow("decimal maximum compared to equal decimal") << "1" << "1" << true;
    QTest::newRow("decimal maximum compared to greater decimal") << "1" << "2" << false;

    QTest::newRow("hexadecimal maximum compared to lesser decimal") << "0x2" << "1" << true;
    QTest::newRow("hexadecimal maximum compared to equal decimal") << "0x1" << "1" << true;
    QTest::newRow("hexadecimal maximum compared to greater decimal") << "0x1" << "2" << false;

    QTest::newRow("decimal maximum compared to lesser hexadecimal") << "2" << "0x01" << true;
    QTest::newRow("decimal maximum compared to equal hexadecimal") << "1" << "0x01" << true;
    QTest::newRow("decimal maximum compared to greater hexadecimal") << "1" << "0x02" << false;

    QTest::newRow("hexadecimal maximum compared to lesser hexadecimal") << "0x2" << "0x1" << true;
    QTest::newRow("hexadecimal maximum compared to equal hexadecimal") << "0x1" << "0x1" << true;
    QTest::newRow("hexadecimal maximum compared to greater hexadecimal") << "0x1" << "0x2" << false;

    QTest::newRow("hexadecimal maximum compared to lesser hexadecimal using X") << "0X2" << "0X1" << true;
    QTest::newRow("hexadecimal maximum compared to equal hexadecimal using X") << "0X1" << "0X1" << true;
    QTest::newRow("hexadecimal maximum compared to greater hexadecimal using X") << "0X1" << "0X2" << false;

    QTest::newRow("hexadecimal maximum compared to lesser hexadecimal using #") << "#2" << "#1" << true;
    QTest::newRow("hexadecimal maximum compared to equal hexadecimal using X") << "#1" << "#1" << true;
    QTest::newRow("hexadecimal maximum compared to greater hexadecimal using X") << "#1" << "#2" << false;

    QTest::newRow("positive decimal maximum compared to negative decimal") << "1" << "-1" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testChoiceReference()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testChoiceReference()
{
    QFETCH(QStringList, choiceNames);
    QFETCH(QString, choiceRef);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();
    parameter->setChoiceRef(choiceRef);

    QSharedPointer<QList<QSharedPointer<Choice> > > choices(new QList<QSharedPointer<Choice> >());

    foreach(QString choiceName, choiceNames)
    {
        QSharedPointer<Choice> choice(new Choice(QDomNode()));
        choice->setName(choiceName);
        choices->append(choice);
    }

    ParameterValidator validator;
    QVERIFY(validator.hasValidChoice(parameter, choices) == expectedValid);

    if (!expectedValid)
    {
        QStringList errorlist =  validator.findErrorsIn(parameter, "test", choices);

        QString expectedError = "Choice choice1 referenced in parameter param is not specified within test";

        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testChoiceReference_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testChoiceReference_data()
{
    QTest::addColumn<QStringList>("choiceNames");
    QTest::addColumn<QString>("choiceRef");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty choice is valid") << QStringList() << "" << true;
    QTest::newRow("Empty choice is valid if choices available") << QStringList("choice1") << "" << true;

    QTest::newRow("No choices available but reference set is not valid") << QStringList() << "choice1" << false;
    QTest::newRow("Reference to unavailable choice is not valid") << QStringList("choice0") << "choice1" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::testValueUsingChoice()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValueUsingChoice()
{
    QFETCH(QString, choiceRef);
    QFETCH(QStringList, enumerations);
    QFETCH(QString, value);
    QFETCH(bool, expectedValid);

    Parameter* parameter = createParameterWithName();
    parameter->setChoiceRef(choiceRef);
    parameter->setValue(value);

    QSharedPointer<QList<QSharedPointer<Choice> > > choices(new QList<QSharedPointer<Choice> > ());

    QSharedPointer<Choice> referencedChoice(new Choice(QDomNode()));
    referencedChoice->setName(choiceRef);
    foreach(QString enumerationValue, enumerations)
    {
        QSharedPointer<Enumeration> enumeration(new Enumeration());
        enumeration->setValue(enumerationValue);
        referencedChoice->enumerations()->append(enumeration);
    }
    choices->append(referencedChoice);

    ParameterValidator validator;
    QVERIFY(validator.hasValidValue(parameter, choices) == expectedValid);

    if (!expectedValid && !value.isEmpty())
    {
        QStringList errorlist =  validator.findErrorsIn(parameter, "test", choices);

        QString expectedError = "Value " + value + " references unknown enumeration "
            "for choice " + choiceRef + " in parameter param within test";

        if (errorIsNotFoundInErrorlist(expectedError, errorlist))
        {
            QFAIL("No error message found.");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValueUsingChoice_data()
//-----------------------------------------------------------------------------
void tst_ParameterValidator::testValueUsingChoice_data()
{
    QTest::addColumn<QString>("choiceRef");
    QTest::addColumn<QStringList>("enumerations");
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("Empty choice and numeric value is valid") << "" << QStringList() << "1" << true;
    QTest::newRow("Choice and empty value is not valid") << "choice1" << QStringList() << "" << false;

    QTest::newRow("Choice with no enumerations is not valid") << "choice1" << QStringList() << "1" << false;
    QTest::newRow("Choice and value not in enumerations is not valid") 
        << "choice1" << QStringList("0") << "1" << false;

    QTest::newRow("Choice and value in enumeration is valid") << "choice1" << QStringList("1") << "1" << true;
    QTest::newRow("Choice and value in enumerations is valid") 
        << "choice1" << QString("0,1,2").split(',') << "1" << true;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::createParameterWithName()
//-----------------------------------------------------------------------------
Parameter* tst_ParameterValidator::createParameterWithName()
{
    Parameter* parameter = new Parameter();
    parameter->setName("param");

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::verifyErrorIsFoundInErrorlist()
//-----------------------------------------------------------------------------
bool tst_ParameterValidator::errorIsNotFoundInErrorlist(QString const& expectedError, QStringList const& errorlist) const
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

//-----------------------------------------------------------------------------
// Function: tst_ParameterValidator::findErrors()
//-----------------------------------------------------------------------------
QStringList tst_ParameterValidator::findErrors(Parameter* parameter)
{
    ParameterValidator validator; 
    return validator.findErrorsIn(parameter, "test", QSharedPointer<QList<QSharedPointer<Choice> > >());
}

QTEST_APPLESS_MAIN(tst_ParameterValidator)

#include "tst_ParameterValidator.moc"

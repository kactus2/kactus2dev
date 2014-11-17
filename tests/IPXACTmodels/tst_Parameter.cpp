//-----------------------------------------------------------------------------
// File: tst_Parameter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.11.2014
//
// Description:
// Unit test for class Parameter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/parameter.h>

class tst_Parameter : public QObject
{
    Q_OBJECT

public:
    tst_Parameter();

private slots:

    void testValueForBoolFormat();
    void testValueForBoolFormat_data();

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

private:
    
    void testValidityWithMinimumValueAndFormat(QString const& format);
    void testValidityWithMaximumValueAndFormat(QString const& format);
   
};

//-----------------------------------------------------------------------------
// Function: tst_Parameter::tst_Parameter()
//-----------------------------------------------------------------------------
tst_Parameter::tst_Parameter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValueForBoolFormat()
//-----------------------------------------------------------------------------
void tst_Parameter::testValueForBoolFormat()
{
    QFETCH(QString, value);
    QFETCH(bool, expectedValid);

    Parameter parameter;
    parameter.setName("param");
    parameter.setValueFormat("bool");
    parameter.setValue(value);

    QVERIFY(parameter.isValid() == expectedValid);
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValueForBoolFormat_data()
//-----------------------------------------------------------------------------
void tst_Parameter::testValueForBoolFormat_data()
{
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("expectedValid");

    QTest::newRow("True is acceptable") << "true" << true;
    QTest::newRow("False is acceptable") << "false" << true;
    QTest::newRow("Other text is not acceptable") << "int" << false;
    QTest::newRow("Number is not acceptable") << "1" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testMinimumValueIsValidForGivenFormat()
//-----------------------------------------------------------------------------
void tst_Parameter::testMinimumValueNotCheckedForEmptyBoolOrStringFormat()
{
    QFETCH(QString, format);
    QFETCH(QString, validValue);

    Parameter parameter;
    parameter.setName("param");
    parameter.setValue(validValue);
    parameter.setMinimumValue("100");

    parameter.setValueFormat(format);
    QVERIFY(parameter.isValid(QStringList(), QString()) == true);
    QVERIFY(parameter.isValid() == true);

}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testMinimumValueIsValidForGivenFormat()
//-----------------------------------------------------------------------------
void tst_Parameter::testMinimumValueNotCheckedForEmptyBoolOrStringFormat_data()
{
    QTest::addColumn<QString>("format");
    QTest::addColumn<QString>("validValue");

    QTest::newRow("no format set") << "" << "1";
    QTest::newRow("bool format set") << "bool" << "true";
    QTest::newRow("string format set") << "string" << "some text";
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testMinimumValueIsValidForGivenFormat()
//-----------------------------------------------------------------------------
void tst_Parameter::testMinimumValueIsValidForGivenFormat()
{
    QFETCH(QString, format);
    QFETCH(QString, boundary);
    QFETCH(bool, expectedValid);

    Parameter parameter;
    parameter.setName("param");
    parameter.setBitStringLength("4");
    parameter.setValue("1000000000");

    parameter.setValueFormat(format);
    parameter.setMinimumValue(boundary);

    QStringList errorList;
    QString identifier = "testMinimumValueIsValidForGivenFormat";
    
    QVERIFY(parameter.isValid(errorList, identifier) == expectedValid);
    QVERIFY(parameter.isValid() == expectedValid);

    if (expectedValid)
    {
        QVERIFY(errorList.isEmpty());
    }
    else
    {
        QCOMPARE(errorList.count(), 1);
        QCOMPARE(errorList.first(), 
            QString("Minimum value %1 is not valid for format %2 in parameter param within "
            "testMinimumValueIsValidForGivenFormat").arg(parameter.getMinimumValue(), parameter.getValueFormat()));
    }
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testMinimumValueIsValidForGivenFormat()
//-----------------------------------------------------------------------------
void tst_Parameter::testMinimumValueIsValidForGivenFormat_data()
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
void tst_Parameter::testValidityWithLongMinimumValue()
{
    testValidityWithMinimumValueAndFormat("long");

}
//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithMinimuValueAndFormat()
//-----------------------------------------------------------------------------
void tst_Parameter::testValidityWithMinimumValueAndFormat(QString const& format)
{
    QFETCH(QString, boundary);
    QFETCH(QString, value);
    QFETCH(bool, expectedValid);

    Parameter parameter;
    parameter.setName("param");
    parameter.setValueFormat(format);
    parameter.setMinimumValue(boundary);
    parameter.setValue(value);

    QStringList errorList;
    QString identifier = "test";

    QVERIFY(parameter.isValid(errorList, identifier) == expectedValid);
    QVERIFY(parameter.isValid() == expectedValid);

    if (expectedValid)
    {
        QVERIFY(errorList.isEmpty());
    }
    else if (value.isEmpty())
    {
        QCOMPARE(errorList.first(), 
            QString("No value specified for parameter param within test"));
    }
    else
    {
        QCOMPARE(errorList.count(), 1);
        QCOMPARE(errorList.first(), 
            QString("Value %1 violates minimum value %2 in parameter param within "
            "test").arg(parameter.getValue(), parameter.getMinimumValue()));
    }
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithLongMinimumValue_data()
//-----------------------------------------------------------------------------
void tst_Parameter::testValidityWithLongMinimumValue_data()
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
void tst_Parameter::testValidityWithFloatMinimumValue()
{
    testValidityWithMinimumValueAndFormat("float");
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithFloatMinimumValue_data()
//-----------------------------------------------------------------------------
void tst_Parameter::testValidityWithFloatMinimumValue_data()
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
void tst_Parameter::testMaximumValueIsValidForGivenFormat()
{
    QFETCH(QString, format);
    QFETCH(QString, boundary);
    QFETCH(bool, expectedValid);

    Parameter parameter;
    parameter.setName("param");
    parameter.setBitStringLength("4");
    parameter.setValue("0");

    parameter.setValueFormat(format);
    parameter.setMaximumValue(boundary);

    QStringList errorList;
    QString identifier = "testMaximumValueIsValidForGivenFormat";

    QVERIFY(parameter.isValid(errorList, identifier) == expectedValid);
    QVERIFY(parameter.isValid() == expectedValid);

    if (expectedValid)
    {
        QVERIFY(errorList.isEmpty());
    }
    else
    {
        QCOMPARE(errorList.count(), 1);
        QCOMPARE(errorList.first(), 
            QString("Maximum value %1 is not valid for format %2 in parameter param within "
            "testMaximumValueIsValidForGivenFormat").arg(parameter.getMaximumValue(), parameter.getValueFormat()));
    }
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testMaximumValueIsValidForGivenFormat_data()
//-----------------------------------------------------------------------------
void tst_Parameter::testMaximumValueIsValidForGivenFormat_data()
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
void tst_Parameter::testValidityWithLongMaximumValue()
{
    testValidityWithMaximumValueAndFormat("long");

}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithMinimuValueAndFormat()
//-----------------------------------------------------------------------------
void tst_Parameter::testValidityWithMaximumValueAndFormat(QString const& format)
{
    QFETCH(QString, boundary);
    QFETCH(QString, value);
    QFETCH(bool, expectedValid);

    Parameter parameter;
    parameter.setName("param");
    parameter.setValueFormat(format);
    parameter.setMaximumValue(boundary);
    parameter.setValue(value);

    QStringList errorList;
    QString identifier = "test";

    QVERIFY(parameter.isValid(errorList, identifier) == expectedValid);
    QVERIFY(parameter.isValid() == expectedValid);

    if (expectedValid)
    {
        QVERIFY(errorList.isEmpty());
    }
    else if (value.isEmpty())
    {
        QCOMPARE(errorList.first(), 
            QString("No value specified for parameter param within test"));
    }
    else
    {
        QCOMPARE(errorList.count(), 1);
        QCOMPARE(errorList.first(), 
            QString("Value %1 violates maximum value %2 in parameter param within "
            "test").arg(parameter.getValue(), parameter.getMaximumValue()));
    }
}

//-----------------------------------------------------------------------------
// Function: tst_Parameter::testValidityWithLongMaximumValue_data()
//-----------------------------------------------------------------------------
void tst_Parameter::testValidityWithLongMaximumValue_data()
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

QTEST_APPLESS_MAIN(tst_Parameter)

#include "tst_Parameter.moc"

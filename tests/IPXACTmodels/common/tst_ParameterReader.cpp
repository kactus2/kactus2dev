//-----------------------------------------------------------------------------
// File: tst_ParameterReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// Unit test for class ParameterReader.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QDomNode>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ParameterReader.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
//! Unit test for class ParameterReader.
//-----------------------------------------------------------------------------
class tst_ParameterReader : public QObject
{
    Q_OBJECT

public:
    tst_ParameterReader();

private slots:
    void testReadNameGroup();
    void testReadAttributes();
    void testReadValue();    
    void testReadVector();
    void testReadArray();
    void testVendorExtensions();
    void testReadAttributesAndValue();
};

//-----------------------------------------------------------------------------
// Function: tst_ParameterReader::tst_ParameterReader()
//-----------------------------------------------------------------------------
tst_ParameterReader::tst_ParameterReader()
{
    
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReader::testReadNameGroup()
//-----------------------------------------------------------------------------
void tst_ParameterReader::testReadNameGroup()
{
    QDomDocument document;
    document.setContent(QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:displayName>firstParameter</ipxact:displayName>"
            "<ipxact:description>This is parameter description.</ipxact:description>"
        "</ipxact:parameter>"));

    QDomElement parameterElement = document.firstChildElement();

    ParameterReader parameterReader;
    QSharedPointer<Parameter> testParameter = parameterReader.createParameterFrom(parameterElement);
   
    QCOMPARE(testParameter->name(), QString("testParameter"));
    QCOMPARE(testParameter->displayName(), QString("firstParameter"));
    QCOMPARE(testParameter->description(), QString("This is parameter description."));
}


//-----------------------------------------------------------------------------
// Function: tst_ParameterReader::testReadAttributes()
//-----------------------------------------------------------------------------
void tst_ParameterReader::testReadAttributes()
{
    QDomDocument document;
    document.setContent(QString(
        "<ipxact:parameter parameterId=\"id\" prompt=\"please define a value\" choiceRef=\"choice1\" "
            "order=\"1.0\" minimum=\"0\" maximum=\"1\" type=\"int\" sign=\"signed\" prefix=\"kilo\" "
            "unit=\"hertz\" vendorAttribute=\"custom\">"
        "</ipxact:parameter>"));

    QDomElement parameterElement = document.firstChildElement();

    ParameterReader parameterReader;
    QSharedPointer<Parameter> testParameter = parameterReader.createParameterFrom(parameterElement);

    QCOMPARE(testParameter->getAttributeNames().size(), 11);

    QCOMPARE(testParameter->getValueId(), QString("id"));
    QCOMPARE(testParameter->getType(), QString("int"));
    QCOMPARE(testParameter->getChoiceRef(), QString("choice1"));
    QCOMPARE(testParameter->getMinimumValue(), QString("0"));
    QCOMPARE(testParameter->getMaximumValue(), QString("1"));

    QCOMPARE(testParameter->getAttribute("prompt"), QString("please define a value"));    
    QCOMPARE(testParameter->getAttribute("order"), QString("1.0"));
    QCOMPARE(testParameter->getAttribute("sign"), QString("signed"));
    QCOMPARE(testParameter->getAttribute("prefix"), QString("kilo"));
    QCOMPARE(testParameter->getAttribute("unit"), QString("hertz"));
    QCOMPARE(testParameter->getAttribute("vendorAttribute"), QString("custom"));
}


//-----------------------------------------------------------------------------
// Function: tst_ParameterReader::testReadValue()
//-----------------------------------------------------------------------------
void tst_ParameterReader::testReadValue()
{
    QDomDocument document;
    document.setContent(QString(
        "<ipxact:parameter>"
        "<ipxact:value vendorAttribute=\"attributeValue\">parameterValue</ipxact:value>"
        "</ipxact:parameter>"));

    QDomElement parameterElement = document.firstChildElement();

    ParameterReader parameterReader;
    QSharedPointer<Parameter> testParameter = parameterReader.createParameterFrom(parameterElement);

    QCOMPARE(testParameter->getValue(), QString("parameterValue"));
    
    QCOMPARE(testParameter->getValueAttributeNames().size(), 1);
    QCOMPARE(testParameter->getValueAttribute("vendorAttribute"), QString("attributeValue"));
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReader::testReadVector()
//-----------------------------------------------------------------------------
void tst_ParameterReader::testReadVector()
{
    QDomDocument document;
    document.setContent(QString(
        "<ipxact:parameter>"
            "<ipxact:vectors>"
                "<ipxact:vector>"
                    "<ipxact:left>leftExpression</ipxact:left>"
                    "<ipxact:right>rightExpression</ipxact:right>"
                "</ipxact:vector>"
            "</ipxact:vectors>"
        "</ipxact:parameter>"));

    QDomElement parameterElement = document.firstChildElement();

    ParameterReader parameterReader;
    QSharedPointer<Parameter> testParameter = parameterReader.createParameterFrom(parameterElement);

    QCOMPARE(testParameter->getVectors()->size(), 1);
    QCOMPARE(testParameter->getVectors()->first()->getLeft(), QString("leftExpression"));
    QCOMPARE(testParameter->getVectors()->first()->getRight(), QString("rightExpression"));
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReader::testReadVector()
//-----------------------------------------------------------------------------
void tst_ParameterReader::testReadArray()
{
    QDomDocument document;
    document.setContent(QString(
        "<ipxact:parameter>"
            "<ipxact:arrays>"
                "<ipxact:array>"
                    "<ipxact:left>leftExpression</ipxact:left>"
                    "<ipxact:right>rightExpression</ipxact:right>"
                "</ipxact:array>"
            "</ipxact:arrays>"
        "</ipxact:parameter>"));

    QDomElement parameterElement = document.firstChildElement();

    ParameterReader parameterReader;
    QSharedPointer<Parameter> testParameter = parameterReader.createParameterFrom(parameterElement);

    QCOMPARE(testParameter->getArrays()->size(), 1);
    QCOMPARE(testParameter->getArrayLeft(), QString("leftExpression"));
    QCOMPARE(testParameter->getArrayRight(), QString("rightExpression"));
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReader::testVendorExtensions()
//-----------------------------------------------------------------------------
void tst_ParameterReader::testVendorExtensions()
{
    QDomDocument document;
    document.setContent(QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:parameter>"));

    QDomElement parameterElement = document.firstChildElement();

    ParameterReader parameterReader;
    QSharedPointer<Parameter> testParameter = parameterReader.createParameterFrom(parameterElement);

    QCOMPARE(testParameter->getVendorExtensions()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterReader::testReadAttributesAndValue()
//-----------------------------------------------------------------------------
void tst_ParameterReader::testReadAttributesAndValue()
{
    QDomDocument document;
    document.setContent(QString(
        "<ipxact:parameter parameterId=\"id\" prompt=\"please define a value:\">"
            "<ipxact:name>parameter1</ipxact:name>"
            "<ipxact:value vendorAttribute=\"attributeValue\">parameterValue</ipxact:value>"
        "</ipxact:parameter>"));

    QDomElement parameterElement = document.firstChildElement();

    ParameterReader parameterReader;
    QSharedPointer<Parameter> testParameter = parameterReader.createParameterFrom(parameterElement);

    QCOMPARE(testParameter->name(), QString("parameter1"));

    QCOMPARE(testParameter->getAttributeNames().size(), 2);

    QCOMPARE(testParameter->getValueId(), QString("id"));
    QCOMPARE(testParameter->getAttribute("prompt"), QString("please define a value:"));    
    
    QCOMPARE(testParameter->getValue(), QString("parameterValue"));
    QCOMPARE(testParameter->getValueAttribute("vendorAttribute"), QString("attributeValue"));
}

QTEST_APPLESS_MAIN(tst_ParameterReader)

#include "tst_ParameterReader.moc"

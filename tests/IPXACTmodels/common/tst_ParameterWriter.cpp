//-----------------------------------------------------------------------------
// File: tst_ParameterWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 31.07.2015
//
// Description:
// Unit test for class ParameterWriter.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QXmlStreamWriter>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
//! Unit test for class ParameterWriter.
//-----------------------------------------------------------------------------
class tst_ParameterWriter : public QObject
{
    Q_OBJECT

public:
    tst_ParameterWriter();

private slots:
    void testWriteNameGroup();
    void testWriteAttributes();
    void testWriteValue();    
    void testWriteVector();
    void testWriteArray();
    void testVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_ParameterWriter::tst_ParameterWriter()
//-----------------------------------------------------------------------------
tst_ParameterWriter::tst_ParameterWriter()
{
    
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterWriter::testWriteNameGroup()
//-----------------------------------------------------------------------------
void tst_ParameterWriter::testWriteNameGroup()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    ParameterWriter parameterWriter;
    
    parameterWriter.writeParameter(xmlStreamWriter, testParameter);
    QCOMPARE(output, QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:value>1</ipxact:value>"
        "</ipxact:parameter>"));
    
    testParameter->setDisplayName("firstParameter");

    output.clear();
    parameterWriter.writeParameter(xmlStreamWriter, testParameter);
    QCOMPARE(output, QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:displayName>firstParameter</ipxact:displayName>"
            "<ipxact:value>1</ipxact:value>"
        "</ipxact:parameter>"));

    testParameter->setDescription("This is parameter description.");

    output.clear();
    parameterWriter.writeParameter(xmlStreamWriter, testParameter);
    QCOMPARE(output, QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:displayName>firstParameter</ipxact:displayName>"
            "<ipxact:description>This is parameter description.</ipxact:description>"
            "<ipxact:value>1</ipxact:value>"
        "</ipxact:parameter>"));
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterWriter::testWriteAttributes()
//-----------------------------------------------------------------------------
void tst_ParameterWriter::testWriteAttributes()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("testParameter");
    testParameter->setChoiceRef("choice1");
    testParameter->setMaximumValue("1");
    testParameter->setMinimumValue("0");
    testParameter->setValueId("id");
    testParameter->setType("int");
    testParameter->setValueResolve("user");
    testParameter->setValue("1");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    ParameterWriter parameterWriter;

    parameterWriter.writeParameter(xmlStreamWriter, testParameter);
    QCOMPARE(output, QString(
        "<ipxact:parameter choiceRef=\"choice1\" maximum=\"1\" minimum=\"0\" parameterId=\"id\" resolve=\"user\" type=\"int\">"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:value>1</ipxact:value>"
        "</ipxact:parameter>"));
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterWriter::testWriteValue()
//-----------------------------------------------------------------------------
void tst_ParameterWriter::testWriteValue()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("");
    testParameter->setName("testParameter");
    testParameter->setValue("parameterValue");
    testParameter->setValueAttribute("vendorAttribute", "attributeValue");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    ParameterWriter parameterWriter;

    parameterWriter.writeParameter(xmlStreamWriter, testParameter);
    QCOMPARE(output, QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:value vendorAttribute=\"attributeValue\">parameterValue</ipxact:value>"
        "</ipxact:parameter>"));
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterWriter::testWriteVector()
//-----------------------------------------------------------------------------
void tst_ParameterWriter::testWriteVector()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    ParameterWriter parameterWriter;

    parameterWriter.writeParameter(xmlStreamWriter, testParameter);
    QCOMPARE(output, QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:value>1</ipxact:value>"
        "</ipxact:parameter>"));

    testParameter->setVectorLeft("leftExpression");
    testParameter->setVectorRight("rightExpression");

    output.clear();
    parameterWriter.writeParameter(xmlStreamWriter, testParameter);
    QCOMPARE(output, QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:vectors>"
                "<ipxact:vector>"
                    "<ipxact:left>leftExpression</ipxact:left>"
                    "<ipxact:right>rightExpression</ipxact:right>"
                "</ipxact:vector>"
            "</ipxact:vectors>"
            "<ipxact:value>1</ipxact:value>"
        "</ipxact:parameter>"));
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterWriter::testWriteArray()
//-----------------------------------------------------------------------------
void tst_ParameterWriter::testWriteArray()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    ParameterWriter parameterWriter;

    parameterWriter.writeParameter(xmlStreamWriter, testParameter);
    QCOMPARE(output, QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:value>1</ipxact:value>"
        "</ipxact:parameter>"));

    testParameter->setArrayLeft("leftExpression");
    testParameter->setArrayRight("rightExpression");

    output.clear();
    parameterWriter.writeParameter(xmlStreamWriter, testParameter);
    QCOMPARE(output, QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:arrays>"
                "<ipxact:array>"
                    "<ipxact:left>leftExpression</ipxact:left>"
                    "<ipxact:right>rightExpression</ipxact:right>"
                "</ipxact:array>"
            "</ipxact:arrays>"
            "<ipxact:value>1</ipxact:value>"
        "</ipxact:parameter>"));
}

//-----------------------------------------------------------------------------
// Function: tst_ParameterWriter::testVendorExtensions()
//-----------------------------------------------------------------------------
void tst_ParameterWriter::testVendorExtensions()
{
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("");
    testParameter->setName("testParameter");
    testParameter->setValue("1");
    
    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("vendorAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > vendorExtensions = testParameter->getVendorExtensions();
    vendorExtensions->append(testExtension);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    ParameterWriter parameterWriter;

    parameterWriter.writeParameter(xmlStreamWriter, testParameter);
    QCOMPARE(output, QString(
        "<ipxact:parameter>"
            "<ipxact:name>testParameter</ipxact:name>"
            "<ipxact:value>1</ipxact:value>"
            "<ipxact:vendorExtensions>"
                "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:parameter>"));
}

QTEST_APPLESS_MAIN(tst_ParameterWriter)

#include "tst_ParameterWriter.moc"

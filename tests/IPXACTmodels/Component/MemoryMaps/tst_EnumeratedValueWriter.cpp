//-----------------------------------------------------------------------------
// File: tst_EnumeratedValueWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Unit test for class EnumeratedValueWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/EnumeratedValueWriter.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>
#include <QSharedPointer>
#include <QDomDocument>

class tst_EnumeratedValueWriter : public QObject
{
    Q_OBJECT

public:
    tst_EnumeratedValueWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void writeSimpleEnumeration();
    void writeUsage();
    void writeVendorExtensions();

private:

    QSharedPointer<EnumeratedValue> testEnumeration_;
};

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueWriter::tst_EnumeratedValueWriter()
//-----------------------------------------------------------------------------
tst_EnumeratedValueWriter::tst_EnumeratedValueWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueWriter::init()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueWriter::init()
{
    testEnumeration_ = QSharedPointer<EnumeratedValue> (new EnumeratedValue("testEnumeration", "testValue"));
}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueWriter::cleanup()
{

}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueWriter::writeSimpleEnumeration()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueWriter::writeSimpleEnumeration()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QString expectedOutput(
        "<ipxact:enumeratedValue>"
            "<ipxact:name>testEnumeration</ipxact:name>"
            "<ipxact:value>testValue</ipxact:value>"
        "</ipxact:enumeratedValue>"
        );

    EnumeratedValueWriter enumerationWriter;
    enumerationWriter.writeEnumeratedValue(xmlStreamWriter, testEnumeration_);
    QCOMPARE(output, expectedOutput);

    output.clear();

    testEnumeration_->setDisplayName("enumDisplay");
    testEnumeration_->setDescription("enumDescription");

    expectedOutput =
        "<ipxact:enumeratedValue>"
            "<ipxact:name>testEnumeration</ipxact:name>"
            "<ipxact:displayName>enumDisplay</ipxact:displayName>"
            "<ipxact:description>enumDescription</ipxact:description>"
            "<ipxact:value>testValue</ipxact:value>"
        "</ipxact:enumeratedValue>"
        ;

    enumerationWriter.writeEnumeratedValue(xmlStreamWriter, testEnumeration_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueWriter::writeUsage()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueWriter::writeUsage()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testEnumeration_->setUsage(EnumeratedValue::WRITE);

    QString expectedOutput(
        "<ipxact:enumeratedValue usage=\"write\">"
            "<ipxact:name>testEnumeration</ipxact:name>"
            "<ipxact:value>testValue</ipxact:value>"
        "</ipxact:enumeratedValue>"
        );

    EnumeratedValueWriter enumerationWriter;
    enumerationWriter.writeEnumeratedValue(xmlStreamWriter, testEnumeration_);
    QCOMPARE(output, expectedOutput);

    output.clear();

    testEnumeration_->setUsage(EnumeratedValue::READWRITE);

    expectedOutput =
        "<ipxact:enumeratedValue usage=\"read-write\">"
            "<ipxact:name>testEnumeration</ipxact:name>"
            "<ipxact:value>testValue</ipxact:value>"
         "</ipxact:enumeratedValue>"
        ;

    enumerationWriter.writeEnumeratedValue(xmlStreamWriter, testEnumeration_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueWriter::writeVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testEnumeration_->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:enumeratedValue>"
            "<ipxact:name>testEnumeration</ipxact:name>"
            "<ipxact:value>testValue</ipxact:value>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:enumeratedValue>"
        );

    EnumeratedValueWriter enumerationWriter;
    enumerationWriter.writeEnumeratedValue(xmlStreamWriter, testEnumeration_);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_EnumeratedValueWriter)

#include "tst_EnumeratedValueWriter.moc"

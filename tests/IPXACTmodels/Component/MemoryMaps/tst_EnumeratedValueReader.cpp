//-----------------------------------------------------------------------------
// File: tst_EnumeratedValueReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Unit test for class EnumeratedValueReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/EnumeratedValueReader.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_EnumeratedValueReader : public QObject
{
    Q_OBJECT

public:
    tst_EnumeratedValueReader();

private slots:

    void readSimpleEnumeratedValue();
    void readUsage();
    void readVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueReader::tst_EnumeratedValueReader()
//-----------------------------------------------------------------------------
tst_EnumeratedValueReader::tst_EnumeratedValueReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueReader::readSimpleEnumeratedValue()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueReader::readSimpleEnumeratedValue()
{
    QString documentContent(
        "<ipxact:enumeratedValue>"
            "<ipxact:name>testEnumeration</ipxact:name>"
            "<ipxact:displayName>enumDisplay</ipxact:displayName>"
            "<ipxact:description>enumDescription</ipxact:description>"
            "<ipxact:value>testValue</ipxact:value>"
        "</ipxact:enumeratedValue>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode enumeratedValueNode = document.firstChildElement("ipxact:enumeratedValue");

    EnumeratedValueReader enumeratedValueReader;
    QSharedPointer<EnumeratedValue> enumeratedValue =
        enumeratedValueReader.createEnumeratedValueFrom(enumeratedValueNode);

    QCOMPARE(enumeratedValue->name(), QString("testEnumeration"));
    QCOMPARE(enumeratedValue->displayName(), QString("enumDisplay"));
    QCOMPARE(enumeratedValue->description(), QString("enumDescription"));
    QCOMPARE(enumeratedValue->getValue(), QString("testValue"));
}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueReader::readUsage()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueReader::readUsage()
{
    QString documentContent(
        "<ipxact:enumeratedValue usage=\"write\">"
            "<ipxact:name>testEnumeration</ipxact:name>"
            "<ipxact:value>testValue</ipxact:value>"
        "</ipxact:enumeratedValue>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode enumeratedValueNode = document.firstChildElement("ipxact:enumeratedValue");

    EnumeratedValueReader enumeratedValueReader;
    QSharedPointer<EnumeratedValue> enumeratedValue =
        enumeratedValueReader.createEnumeratedValueFrom(enumeratedValueNode);

    QCOMPARE(enumeratedValue->name(), QString("testEnumeration"));
    QCOMPARE(enumeratedValue->getValue(), QString("testValue"));
    QCOMPARE(enumeratedValue->getUsage(), EnumeratedValue::WRITE);
}

//-----------------------------------------------------------------------------
// Function: tst_EnumeratedValueReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_EnumeratedValueReader::readVendorExtensions()
{
    QString documentContent(
        "<ipxact:enumeratedValue>"
            "<ipxact:name>testEnumeration</ipxact:name>"
            "<ipxact:value>testValue</ipxact:value>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:enumeratedValue>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode enumeratedValueNode = document.firstChildElement("ipxact:enumeratedValue");

    EnumeratedValueReader enumeratedValueReader;
    QSharedPointer<EnumeratedValue> enumeratedValue =
        enumeratedValueReader.createEnumeratedValueFrom(enumeratedValueNode);

    QCOMPARE(enumeratedValue->name(), QString("testEnumeration"));
    QCOMPARE(enumeratedValue->getValue(), QString("testValue"));
    QCOMPARE(enumeratedValue->getVendorExtensions()->size(), 1);
    QCOMPARE(enumeratedValue->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_EnumeratedValueReader)

#include "tst_EnumeratedValueReader.moc"

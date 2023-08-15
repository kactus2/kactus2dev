//-----------------------------------------------------------------------------
// File: tst_FieldReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Unit test for class AccessPolicyReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/AccessPolicyReader.h>

#include <IPXACTmodels/Component/AccessPolicy.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_AccessPolicyReader : public QObject
{
    Q_OBJECT

public:
    tst_AccessPolicyReader();

private slots:

    void readModeRefs();
    void readAccess();
    void readVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_AccessPolicyReader::tst_AccessPolicyReader()
//-----------------------------------------------------------------------------
tst_AccessPolicyReader::tst_AccessPolicyReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AccessPolicyReader::readModeRefs()
//-----------------------------------------------------------------------------
void tst_AccessPolicyReader::readModeRefs()
{
    QString doc(
        "<ipxact:accessPolicy>"
            "<ipxact:modeRef priority=\"0\">normal</ipxact:modeRef>"
        "</ipxact:accessPolicy>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto accessPolicy = AccessPolicyReader::createAccessPolicyFrom(document.firstChildElement("ipxact:accessPolicy"));

    QCOMPARE(accessPolicy->getModeReferences()->size(), 1);
    QCOMPARE(accessPolicy->getModeReferences()->first()->getReference(), QString("normal"));

     doc =
        "<ipxact:accessPolicy>"
            "<ipxact:modeRef priority=\"0\">normal</ipxact:modeRef>"
            "<ipxact:modeRef priority=\"1\">test</ipxact:modeRef>"
        "</ipxact:accessPolicy>"
        ;

    document.setContent(doc);

    accessPolicy = AccessPolicyReader::createAccessPolicyFrom(document.firstChildElement("ipxact:accessPolicy"));

    QCOMPARE(accessPolicy->getModeReferences()->size(), 2);
    QCOMPARE(accessPolicy->getModeReferences()->first()->getReference(), QString("normal"));
    QCOMPARE(accessPolicy->getModeReferences()->first()->getPriority(), QString("0"));
    QCOMPARE(accessPolicy->getModeReferences()->at(1)->getReference(), QString("test"));
    QCOMPARE(accessPolicy->getModeReferences()->at(1)->getPriority(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_AccessPolicyReader::readAccess()
//-----------------------------------------------------------------------------
void tst_AccessPolicyReader::readAccess()
{
    QString documentContent(
        "<ipxact:accessPolicy>"
            "<ipxact:access>read-write</ipxact:access>"
        "</ipxact:accessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    auto accessPolicy = AccessPolicyReader::createAccessPolicyFrom(document.firstChildElement("ipxact:accessPolicy"));

    QCOMPARE(accessPolicy->getAccess(), AccessTypes::Access::READ_WRITE);
}

//-----------------------------------------------------------------------------
// Function: tst_AccessPolicyReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_AccessPolicyReader::readVendorExtensions()
{
    QString documentContent(
        "<ipxact:accessPolicy>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:accessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    auto accessPolicy = AccessPolicyReader::createAccessPolicyFrom(document.firstChildElement("ipxact:accessPolicy"));

    QCOMPARE(accessPolicy->getVendorExtensions()->size(), 1);
    QCOMPARE(accessPolicy->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_AccessPolicyReader)

#include "tst_AccessPolicyReader.moc"

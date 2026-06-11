//-----------------------------------------------------------------------------
// File: tst_ClearboxElementReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 10.06.2026
//
// Description:
// Unit test for ClearboxElementReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ClearboxElementReader.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_ClearboxElementReader : public QObject
{
    Q_OBJECT

public:
    tst_ClearboxElementReader();

private slots:

    void tst_readClearboxElement();
    void tst_readWhiteboxElement();
};

//-----------------------------------------------------------------------------
// Function: tst_ClearboxElementReader::tst_ClearboxElementReader()
//-----------------------------------------------------------------------------
tst_ClearboxElementReader::tst_ClearboxElementReader()
{

}

void tst_ClearboxElementReader::tst_readClearboxElement()
{
    QString documentContent(
        "<ipxact:clearboxElement>"
            "<ipxact:name>a_name</ipxact:name>"
            "<ipxact:isPresent>false</ipxact:isPresent>" // should not be read with std22
            "<ipxact:clearboxType>signal</ipxact:clearboxType>"
            "<ipxact:driveable>true</ipxact:driveable>"
            "<ipxact:parameters>"
                "<ipxact:parameter>"
                    "<ipxact:name>testParam</ipxact:name>"
                    "<ipxact:value>32</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:clearboxElement>"
    );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode elementNode = document.firstChildElement("ipxact:clearboxElement");
    auto testElement = ClearboxElementReader::createClearboxElementFrom(elementNode, Document::Revision::Std22);
    
    QCOMPARE(testElement->name(), QString("a_name"));
    QCOMPARE(testElement->getIsPresent(), QString(""));
    QCOMPARE(testElement->getType(), ClearboxElement::Type::SIGNAL);
    QCOMPARE(testElement->getDriveable(), QString("true"));
    QCOMPARE(testElement->getParameters()->size(), 1);
    QCOMPARE(testElement->getParameters()->first()->getValue(), QString("32"));
    QCOMPARE(testElement->getVendorExtensions()->size(), 1);
    QCOMPARE(testElement->getVendorExtensions()->first()->type(), QString("testExtension"));
}

void tst_ClearboxElementReader::tst_readWhiteboxElement()
{
    QString documentContent(
        "<ipxact:whiteboxElement>"
            "<ipxact:name>a_name</ipxact:name>"
            "<ipxact:isPresent>false</ipxact:isPresent>"
            "<ipxact:whiteboxType>signal</ipxact:whiteboxType>"
            "<ipxact:driveable>true</ipxact:driveable>"
            "<ipxact:parameters>"
                "<ipxact:parameter>"
                    "<ipxact:name>testParam</ipxact:name>"
                    "<ipxact:value>32</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:whiteboxElement>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode elementNode = document.firstChildElement("ipxact:whiteboxElement");
    auto testElement = ClearboxElementReader::createClearboxElementFrom(elementNode, Document::Revision::Std14);
    
    QCOMPARE(testElement->name(), QString("a_name"));
    QCOMPARE(testElement->getIsPresent(), QString("false"));
    QCOMPARE(testElement->getType(), ClearboxElement::Type::SIGNAL);
    QCOMPARE(testElement->getDriveable(), QString("true"));
    QCOMPARE(testElement->getParameters()->size(), 1);
    QCOMPARE(testElement->getParameters()->first()->getValue(), QString("32"));
    QCOMPARE(testElement->getVendorExtensions()->size(), 1);
    QCOMPARE(testElement->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_ClearboxElementReader)

#include "tst_ClearboxElementReader.moc"

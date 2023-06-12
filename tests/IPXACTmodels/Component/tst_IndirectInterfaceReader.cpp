//-----------------------------------------------------------------------------
// File: tst_IndirectInterfaceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 02.08.2017
//
// Description:
// Unit test for class IndirectInterfaceReader.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/Component/IndirectInterface.h>
#include <IPXACTmodels/Component/IndirectInterfaceReader.h>
#include <IPXACTmodels/Component/TransparentBridge.h>
#include <IPXACTmodels/common/Document.h>

#include <QDomDocument>
#include <QDomNode>

class tst_IndirectInterfaceReader : public QObject
{
    Q_OBJECT

public:
    tst_IndirectInterfaceReader();

private slots:
    void simpleIndirectInterface();
    void fullIndirectInterface();
    void indirectInterfaceWithTransparentBridges();
    void indirectInterfaceWithTransparentBridges2022();
};

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceReader::tst_IndirectInterfaceReader()
//-----------------------------------------------------------------------------
tst_IndirectInterfaceReader::tst_IndirectInterfaceReader()
{
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceReader::simpleIndirectInterface()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceReader::simpleIndirectInterface()
{
    QString documentContent(
        "<ipxact:indirectInterface>"
            "<ipxact:name>interface1</ipxact:name>"
            "<ipxact:displayName>interfaceDisplay</ipxact:displayName>"
            "<ipxact:description>interfaceDescription</ipxact:description>"
            "<ipxact:indirectAddressRef>addressId</ipxact:indirectAddressRef>"
            "<ipxact:indirectDataRef>dataId</ipxact:indirectDataRef>"
            "<ipxact:memoryMapRef>targetMap</ipxact:memoryMapRef>"
        "</ipxact:indirectInterface>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode interfaceNode = document.firstChildElement("ipxact:indirectInterface");

    QSharedPointer<IndirectInterface> testInterface = IndirectInterfaceReader::createIndirectInterfaceFrom(interfaceNode, Document::Revision::Std14);

    QCOMPARE(testInterface->name(), QString("interface1"));
    QCOMPARE(testInterface->displayName(), QString("interfaceDisplay"));
    QCOMPARE(testInterface->description(), QString("interfaceDescription"));

    QCOMPARE(testInterface->getIndirectAddressRef(), QString("addressId"));
    QCOMPARE(testInterface->getIndirectDataRef(), QString("dataId"));
    QCOMPARE(testInterface->getMemoryMapRef(), QString("targetMap"));
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceReader::fullIndirectInterface()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceReader::fullIndirectInterface()
{
    QString documentContent(
        "<ipxact:indirectInterface testAttribute=\"testValue\">"
            "<ipxact:name>interface1</ipxact:name>"
            "<ipxact:displayName>interfaceDisplay</ipxact:displayName>"
            "<ipxact:description>interfaceDescription</ipxact:description>"
            "<ipxact:indirectAddressRef>addressId</ipxact:indirectAddressRef>"
            "<ipxact:indirectDataRef>dataId</ipxact:indirectDataRef>"
            "<ipxact:memoryMapRef>targetMap</ipxact:memoryMapRef>"
            "<ipxact:bitsInLau>16</ipxact:bitsInLau>"
            "<ipxact:endianness>big</ipxact:endianness>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"id1\">"
                    "<ipxact:name>testParameter</ipxact:name>"
                    "<ipxact:description>First generator parameter.</ipxact:description>"
                    "<ipxact:value>5</ipxact:value>"
                "</ipxact:parameter>"
                "<ipxact:parameter parameterId=\"id2\">"
                    "<ipxact:name>secondParameter</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
            "<ipxact:vendorExtensions>"
                "<testExtension testAttribute=\"attributeValue\"/>"
            "</ipxact:vendorExtensions>"
        "</ipxact:indirectInterface>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode interfaceNode = document.firstChildElement("ipxact:indirectInterface");

    QSharedPointer<IndirectInterface> testInterface = IndirectInterfaceReader::createIndirectInterfaceFrom(interfaceNode, Document::Revision::Std14);

    QCOMPARE(testInterface->getAttributes().count(), 1);
    QCOMPARE(testInterface->getAttributes().value("testAttribute"), QString("testValue"));

    QCOMPARE(testInterface->name(), QString("interface1"));
    QCOMPARE(testInterface->displayName(), QString("interfaceDisplay"));
    QCOMPARE(testInterface->description(), QString("interfaceDescription"));

    QCOMPARE(testInterface->getIndirectAddressRef(), QString("addressId"));
    QCOMPARE(testInterface->getIndirectDataRef(), QString("dataId"));
    QCOMPARE(testInterface->getMemoryMapRef(), QString("targetMap"));

    QCOMPARE(testInterface->getBitsInLau(), QString("16"));
    QCOMPARE(testInterface->getEndianness(), QString("big"));

    QCOMPARE(testInterface->getParameters()->count(), 2);
    QCOMPARE(testInterface->getVendorExtensions()->count(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceReader::indirectInterfaceWithTransparentBridges()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceReader::indirectInterfaceWithTransparentBridges()
{
    QString documentContent(
        "<ipxact:indirectInterface>"
            "<ipxact:name>interface2</ipxact:name>"
            "<ipxact:indirectAddressRef>addressId</ipxact:indirectAddressRef>"
            "<ipxact:indirectDataRef>dataId</ipxact:indirectDataRef>"
            "<ipxact:transparentBridge masterRef=\"firstInterface\">"                         
                "<ipxact:isPresent>true</ipxact:isPresent>"
            "</ipxact:transparentBridge>"
            "<ipxact:transparentBridge masterRef=\"secondInterface\">"                         
                "<ipxact:isPresent>1</ipxact:isPresent>"
            "</ipxact:transparentBridge>"
        "</ipxact:indirectInterface>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode interfaceNode = document.firstChildElement("ipxact:indirectInterface");

    QSharedPointer<IndirectInterface> testInterface = IndirectInterfaceReader::createIndirectInterfaceFrom(interfaceNode, Document::Revision::Std14);

    QCOMPARE(testInterface->getTransparentBridges()->count(), 2);

    QCOMPARE(testInterface->getTransparentBridges()->first()->getMasterRef(), QString("firstInterface"));
    QCOMPARE(testInterface->getTransparentBridges()->first()->getIsPresent(), QString("true"));

    QCOMPARE(testInterface->getTransparentBridges()->last()->getMasterRef(), QString("secondInterface"));
    QCOMPARE(testInterface->getTransparentBridges()->last()->getIsPresent(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceReader::indirectInterfaceWithTransparentBridges2022()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceReader::indirectInterfaceWithTransparentBridges2022()
{
    QString documentContent(
        "<ipxact:indirectInterface>"
            "<ipxact:name>interface2</ipxact:name>"
            "<ipxact:indirectAddressRef>addressId</ipxact:indirectAddressRef>"
            "<ipxact:indirectDataRef>dataId</ipxact:indirectDataRef>"
            "<ipxact:transparentBridge initiatorRef=\"firstInterface\">"                         
                "<ipxact:isPresent>true</ipxact:isPresent>"
            "</ipxact:transparentBridge>"
            "<ipxact:transparentBridge initiatorRef=\"secondInterface\">"                         
                "<ipxact:isPresent>1</ipxact:isPresent>"
                "<ipxact:vendorExtensions>"
                    "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                "</ipxact:vendorExtensions>"
            "</ipxact:transparentBridge>"
        "</ipxact:indirectInterface>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode interfaceNode = document.firstChildElement("ipxact:indirectInterface");

    QSharedPointer<IndirectInterface> testInterface = IndirectInterfaceReader::createIndirectInterfaceFrom(interfaceNode, Document::Revision::Std22);

    QCOMPARE(testInterface->getTransparentBridges()->count(), 2);

    QCOMPARE(testInterface->getTransparentBridges()->first()->getInitiatorRef(), QString("firstInterface"));
    QCOMPARE(testInterface->getTransparentBridges()->first()->getIsPresent(), QString("")); //!< No isPresent should be available.

    QCOMPARE(testInterface->getTransparentBridges()->last()->getInitiatorRef(), QString("secondInterface"));
    QCOMPARE(testInterface->getTransparentBridges()->last()->getIsPresent(), QString("")); //!< No isPresent should be available.
    QCOMPARE(testInterface->getTransparentBridges()->last()->getVendorExtensions()->count(), 1);
    QCOMPARE(testInterface->getTransparentBridges()->last()->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_IndirectInterfaceReader)

#include "tst_IndirectInterfaceReader.moc"

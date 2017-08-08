//-----------------------------------------------------------------------------
// File: tst_IndirectInterfaceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.08.2017
//
// Description:
// Unit test for class IndirectInterfaceWriter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/Component/IndirectInterface.h>
#include <IPXACTmodels/Component/IndirectInterfaceWriter.h>
#include <IPXACTmodels/Component/TransparentBridge.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QDomDocument>
#include <QDomNode>

class tst_IndirectInterfaceWriter : public QObject
{
    Q_OBJECT

public:
    tst_IndirectInterfaceWriter();

private slots:
    void writeSimpleIndirectInterface();

    void writeEndiannessAndLauInInterface();

    void writeIndirectInterfaceWithBridges();

    void writeIndirectInterfaceWithParameters();

    void writeIndirectInterfaceWithVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceWriter::tst_IndirectInterfaceWriter()
//-----------------------------------------------------------------------------
tst_IndirectInterfaceWriter::tst_IndirectInterfaceWriter()
{
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceWriter::writeSimpleIndirectInterface()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceWriter::writeSimpleIndirectInterface()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    IndirectInterfaceWriter writer;

    QSharedPointer<IndirectInterface> testInterface(new IndirectInterface("interface1"));
    testInterface->setDisplayName("testDisplay");
    testInterface->setDescription("testDescription");
    
    testInterface->setIndirectAddressRef("addressId");
    testInterface->setIndirectDataRef("dataId");

    testInterface->setMemoryMapRef("targetMap");

    QString expectedOutput( 
        "<ipxact:indirectInterface>"
            "<ipxact:name>interface1</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:description>testDescription</ipxact:description>"
            "<ipxact:indirectAddressRef>addressId</ipxact:indirectAddressRef>"
            "<ipxact:indirectDataRef>dataId</ipxact:indirectDataRef>"
            "<ipxact:memoryMapRef>targetMap</ipxact:memoryMapRef>"
        "</ipxact:indirectInterface>");

    writer.writeIndirectInterface(xmlStreamWriter, testInterface);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceWriter::writeEndiannessAndLauInInterface()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceWriter::writeEndiannessAndLauInInterface()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    IndirectInterfaceWriter writer;

    QSharedPointer<IndirectInterface> testInterface(new IndirectInterface("interface1"));
    testInterface->setIndirectAddressRef("addressId");
    testInterface->setIndirectDataRef("dataId");
    testInterface->setMemoryMapRef("targetMap");
    testInterface->setBitsInLau("16");
    testInterface->setEndianness("big");

    QString expectedOutput( 
        "<ipxact:indirectInterface>"
            "<ipxact:name>interface1</ipxact:name>"
            "<ipxact:indirectAddressRef>addressId</ipxact:indirectAddressRef>"
            "<ipxact:indirectDataRef>dataId</ipxact:indirectDataRef>"
            "<ipxact:memoryMapRef>targetMap</ipxact:memoryMapRef>"
            "<ipxact:bitsInLau>16</ipxact:bitsInLau>"
            "<ipxact:endianness>big</ipxact:endianness>"
        "</ipxact:indirectInterface>");

    writer.writeIndirectInterface(xmlStreamWriter, testInterface);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceWriter::writeIndirectInterfaceWithBridges()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceWriter::writeIndirectInterfaceWithBridges()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    IndirectInterfaceWriter writer;

    QSharedPointer<IndirectInterface> testInterface(new IndirectInterface("interface"));
    testInterface->setIndirectAddressRef("addr");
    testInterface->setIndirectDataRef("data");

    QSharedPointer<TransparentBridge> firstBridge(new TransparentBridge("master1"));
    testInterface->getTransparentBridges()->append(firstBridge);

    QSharedPointer<TransparentBridge> conditionalBridge(new TransparentBridge("master2"));
    conditionalBridge->setIsPresent("true");
    testInterface->getTransparentBridges()->append(conditionalBridge);

    QString expectedOutput( 
        "<ipxact:indirectInterface>"
            "<ipxact:name>interface</ipxact:name>"
            "<ipxact:indirectAddressRef>addr</ipxact:indirectAddressRef>"
            "<ipxact:indirectDataRef>data</ipxact:indirectDataRef>"
            "<ipxact:transparentBridge masterRef=\"master1\"/>"
            "<ipxact:transparentBridge masterRef=\"master2\">"
                "<ipxact:isPresent>true</ipxact:isPresent>"
            "</ipxact:transparentBridge>"
        "</ipxact:indirectInterface>");

    writer.writeIndirectInterface(xmlStreamWriter, testInterface);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceWriter::writeIndirectInterfaceWithParameters()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceWriter::writeIndirectInterfaceWithParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    IndirectInterfaceWriter writer;

    QSharedPointer<IndirectInterface> testInterface(new IndirectInterface("interface1"));
    testInterface->setIndirectAddressRef("addressId");
    testInterface->setIndirectDataRef("dataId");

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("");
    testParameter->setName("testParameter");
    testParameter->setValue("testValue");

    testInterface->getParameters()->append(testParameter);

    QString expectedOutput( 
        "<ipxact:indirectInterface>"
            "<ipxact:name>interface1</ipxact:name>"
            "<ipxact:indirectAddressRef>addressId</ipxact:indirectAddressRef>"
            "<ipxact:indirectDataRef>dataId</ipxact:indirectDataRef>"
            "<ipxact:parameters>"
                "<ipxact:parameter>"
                    "<ipxact:name>testParameter</ipxact:name>"
                    "<ipxact:value>testValue</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:indirectInterface>");

    writer.writeIndirectInterface(xmlStreamWriter, testInterface);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_IndirectInterfaceWriter::writeIndirectInterfaceWithVendorExtensions()
//-----------------------------------------------------------------------------
void tst_IndirectInterfaceWriter::writeIndirectInterfaceWithVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    IndirectInterfaceWriter writer;

    QSharedPointer<IndirectInterface> testInterface(new IndirectInterface("interface1"));
    testInterface->setIndirectAddressRef("addressId");
    testInterface->setIndirectDataRef("dataId");

    QSharedPointer<Kactus2Value> testExtension(new Kactus2Value("testExtension", "testValue"));
    testInterface->getVendorExtensions()->append(testExtension);

    QString expectedOutput( 
        "<ipxact:indirectInterface>"
            "<ipxact:name>interface1</ipxact:name>"
            "<ipxact:indirectAddressRef>addressId</ipxact:indirectAddressRef>"
            "<ipxact:indirectDataRef>dataId</ipxact:indirectDataRef>"
            "<ipxact:vendorExtensions>"
                "<testExtension>testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:indirectInterface>");

    writer.writeIndirectInterface(xmlStreamWriter, testInterface);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_IndirectInterfaceWriter)

#include "tst_IndirectInterfaceWriter.moc"

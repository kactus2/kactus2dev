//-----------------------------------------------------------------------------
// File: tst_tst_ComponentPortWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.09.2015
//
// Description:
// Unit test for class tst_ComponentPortWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/PortWriter.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/common/Qualifier.h>

#include <QtTest>
#include <QDomDocument>

class tst_ComponentPortWriter : public QObject
{
    Q_OBJECT

public:
    tst_ComponentPortWriter();

private slots:
    void writeSimplePort();
    void writeSimpleTransactionalPort();
    void writePortPresence();
    void writePortArrays();
    void writePortExtensions();
    void writePortAdHocVisible();
    void writePortDefaultPosition();
    void writePortTags();

    void writeWirePortAllLogicalDirectionsAllowed();
    void writeWirePortVectors();
    void writeWirePortVectors_2022();
    void emptyVectorIsNotWritten();
    void writeWireTypeDefinitions();
    void emptyWireTypeDefinitionIsNotWritten();
    void writeWireDefaultDriver();
    void writeWireQualifiers_2022();

    void writeTransactionalAllLogicalInitiativesAllowed();
    void writeTransactionalKind();
    void writeTransactionalBusWidth();
    void writeTransactionalQualifier_2022();
    void writeTransactionalProtocol();
    void writeTransactionalTypeDefinitions();
    void writeTransactionalConnectionMinMax();
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::tst_ComponentPortWriter()
//-----------------------------------------------------------------------------
tst_ComponentPortWriter::tst_ComponentPortWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeSimplePort()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeSimplePort()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDirection(DirectionTypes::IN);
    testPort->setDisplayName("PortorDisplay");
    testPort->setDescription("PortorDescription");

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:displayName>PortorDisplay</ipxact:displayName>"
            "<ipxact:description>PortorDescription</ipxact:description>"
            "<ipxact:wire>"
                "<ipxact:direction>in</ipxact:direction>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeSimpleTransactionalPort()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeSimpleTransactionalPort()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDisplayName("PortorDisplay");
    testPort->setDescription("PortorDescription");

    QSharedPointer<Transactional> testTransactional(new Transactional());
    testTransactional->setInitiative("provides");

    testPort->setTransactional(testTransactional);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:displayName>PortorDisplay</ipxact:displayName>"
            "<ipxact:description>PortorDescription</ipxact:description>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writePortPresence()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writePortPresence()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort (new Port("testPort"));
    testPort->setDirection(DirectionTypes::IN);
    testPort->setIsPresent("4-2*2+1");

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:isPresent>4-2*2+1</ipxact:isPresent>"
            "<ipxact:wire>"
                "<ipxact:direction>in</ipxact:direction>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writePortArrays()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writePortArrays()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Transactional> testTransactional(new Transactional());
    testTransactional->setInitiative("provides");

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setArrayLeft("4+4");
    testPort->setArrayRight("18");
    testPort->setTransactional(testTransactional);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
            "</ipxact:transactional>"
            "<ipxact:arrays>"
                "<ipxact:array>"
                    "<ipxact:left>4+4</ipxact:left>"
                    "<ipxact:right>18</ipxact:right>"
                "</ipxact:array>"
            "</ipxact:arrays>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writePortExtensions()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writePortExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<Port> testPort (new Port("testPort"));
    testPort->setDirection(DirectionTypes::IN);
    testPort->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>in</ipxact:direction>"
            "</ipxact:wire>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writePortAdHocVisible()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writePortAdHocVisible()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));  
    testPort->setDirection(DirectionTypes::IN);
    testPort->setAdHocVisible(true);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>in</ipxact:direction>"
            "</ipxact:wire>"
            "<ipxact:vendorExtensions>"
                "<kactus2:adHocVisible/>"
            "</ipxact:vendorExtensions>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writePortDefaultPosition()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writePortDefaultPosition()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);


    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDirection(DirectionTypes::IN);
    testPort->setDefaultPos(QPointF(1, 1));

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>in</ipxact:direction>"
            "</ipxact:wire>"
            "<ipxact:vendorExtensions>"
                "<kactus2:position x=\"1\" y=\"1\"/>"
            "</ipxact:vendorExtensions>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writePortTags()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writePortTags()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDirection(DirectionTypes::IN);
    testPort->setPortTags("oyoroi,yuJing,guilang,tags");

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>in</ipxact:direction>"
            "</ipxact:wire>"
            "<ipxact:vendorExtensions>"
                "<kactus2:portTags>oyoroi,yuJing,guilang,tags</kactus2:portTags>"
            "</ipxact:vendorExtensions>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeWirePortAllLogicalDirectionsAllowed()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeWirePortAllLogicalDirectionsAllowed()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDirection(DirectionTypes::IN);
    testPort->setAllLogicalDirectionsAllowed(true);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire allLogicalDirectionsAllowed=\"true\">"
                "<ipxact:direction>in</ipxact:direction>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeWirePortVectors()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeWirePortVectors()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDirection(DirectionTypes::OUT);
    testPort->getWire()->setVectorLeftBound("4+18-Yaoxao");
    testPort->getWire()->setVectorRightBound("Yaoxao");

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:vectors>"
                    "<ipxact:vector>"
                        "<ipxact:left>4+18-Yaoxao</ipxact:left>"
                        "<ipxact:right>Yaoxao</ipxact:right>"
                    "</ipxact:vector>"
                "</ipxact:vectors>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeWirePortVectors_2022()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeWirePortVectors_2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDirection(DirectionTypes::OUT);
    testPort->getWire()->setVectorLeftBound("4+18-Yaoxao");
    testPort->getWire()->setVectorRightBound("Yaoxao");
    testPort->getWire()->getVector()->setId("testID");

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:vectors>"
                    "<ipxact:vector vectorId=\"testID\">"
                        "<ipxact:left>4+18-Yaoxao</ipxact:left>"
                        "<ipxact:right>Yaoxao</ipxact:right>"
                    "</ipxact:vector>"
                "</ipxact:vectors>"
            "</ipxact:wire>"
        "</ipxact:port>"
    );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::emptyVectorIsNotWritten()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::emptyVectorIsNotWritten()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDirection(DirectionTypes::OUT);
    testPort->getWire()->setVectorLeftBound("");
    testPort->getWire()->setVectorRightBound("");

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"              
            "</ipxact:wire>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeWireTypeDefinitions()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeWireTypeDefinitions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDirection(DirectionTypes::OUT);

    QSharedPointer<WireTypeDef> testTypeDefinition (new WireTypeDef("testType"));
    testTypeDefinition->getViewRefs()->append(QStringList{ "testView" });
    testTypeDefinition->setConstrained(true);
    QStringList typeDefinitions;
    typeDefinitions.append("testTypeDefinition");
    testTypeDefinition->getTypeDefinitions()->append(typeDefinitions);

    testPort->getWire()->getWireTypeDefs()->append(testTypeDefinition);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:wireTypeDefs>"
                    "<ipxact:wireTypeDef>"
                        "<ipxact:typeName constrained=\"true\">testType</ipxact:typeName>"
                        "<ipxact:typeDefinition>testTypeDefinition</ipxact:typeDefinition>"
                        "<ipxact:viewRef>testView</ipxact:viewRef>"
                    "</ipxact:wireTypeDef>"
                "</ipxact:wireTypeDefs>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::emptyWireTypeDefinitionIsNotWritten()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::emptyWireTypeDefinitionIsNotWritten()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDirection(DirectionTypes::OUT);

    QSharedPointer<WireTypeDef> testTypeDefinition(new WireTypeDef("testType"));
    testTypeDefinition->getViewRefs()->append(QStringList{ "testView" });
    testTypeDefinition->setConstrained(true);
    QStringList typeDefinitions;
    typeDefinitions.append("testTypeDefinition");
    testTypeDefinition->getTypeDefinitions()->append(typeDefinitions);

    testPort->getWire()->getWireTypeDefs()->append(testTypeDefinition);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:wireTypeDefs>"
                    "<ipxact:wireTypeDef>"
                        "<ipxact:typeName constrained=\"true\">testType</ipxact:typeName>"
                        "<ipxact:typeDefinition>testTypeDefinition</ipxact:typeDefinition>"
                        "<ipxact:viewRef>testView</ipxact:viewRef>"
                    "</ipxact:wireTypeDef>"
                "</ipxact:wireTypeDefs>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();
    QSharedPointer<WireTypeDef> testWireDefinition = testPort->getWire()->getWireTypeDefs()->first();
    testWireDefinition->setTypeName(QStringLiteral(""));

    expectedOutput =
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:wireTypeDefs>"
                    "<ipxact:wireTypeDef>"
                        "<ipxact:typeDefinition>testTypeDefinition</ipxact:typeDefinition>"
                        "<ipxact:viewRef>testView</ipxact:viewRef>"
                    "</ipxact:wireTypeDef>"
                "</ipxact:wireTypeDefs>"
            "</ipxact:wire>"
        "</ipxact:port>";

    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeWireDefaultDriver()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeWireDefaultDriver()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setDirection(DirectionTypes::OUT);
    testPort->setDefaultValue("4+4-2");
    auto driver = testPort->getWire()->getDriver();
    driver->setRange(Range("1", "0"));

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:drivers>"
                    "<ipxact:driver>"
                        "<ipxact:range>"
                            "<ipxact:left>1</ipxact:left>"
                            "<ipxact:right>0</ipxact:right>"
                        "</ipxact:range>"
                        "<ipxact:defaultValue>4+4-2</ipxact:defaultValue>"
                    "</ipxact:driver>"
                "</ipxact:drivers>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeWireQualifiers_2022()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeWireQualifiers_2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Port> testPort(new Port("testPort"));
    testPort->setWire(QSharedPointer<Wire>(new Wire()));
    testPort->setDirection(DirectionTypes::OUT);
    
    auto wire = testPort->getWire(); 
    auto qualifier = wire->getQualifier();
    qualifier->setType(Qualifier::Type::Reset);
    qualifier->setAttribute(Qualifier::Attribute::ResetLevel, "low");
    qualifier->setType(Qualifier::Type::FlowControl);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:qualifier>"
                    "<ipxact:isReset level=\"low\">true</ipxact:isReset>"
                    "<ipxact:isFlowControl>true</ipxact:isFlowControl>"
                "</ipxact:qualifier>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeTransactionalAllLogicalInitiativesAllowed()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeTransactionalAllLogicalInitiativesAllowed()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Transactional> testTransactional(new Transactional());
    testTransactional->setInitiative("provides");
    testTransactional->setAllLogicalInitiativesAllowed(true);

    QSharedPointer<Port> testPort(new Port("testPort"));   
    testPort->setTransactional(testTransactional);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional allLogicalInitiativesAllowed=\"true\">"
                "<ipxact:initiative>provides</ipxact:initiative>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeTransactionalKind()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeTransactionalKind()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Transactional> testTransactional(new Transactional());
    testTransactional->setInitiative("provides");
    testTransactional->setKind("tlm_port");
    
    QSharedPointer<Port> testPort(new Port("testPort"));    
    testPort->setTransactional(testTransactional);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
                "<ipxact:kind>tlm_port</ipxact:kind>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testTransactional->setKind("customKind");
    
    expectedOutput =
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
                "<ipxact:kind custom=\"customKind\">custom</ipxact:kind>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        ;

    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeTransactionalBusWidth()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeTransactionalBusWidth()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Transactional> testTransactional(new Transactional());
    testTransactional->setInitiative("provides");
    testTransactional->setBusWidth("8");


    QSharedPointer<Port> testPort(new Port("testPort"));    
    testPort->setTransactional(testTransactional);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
                "<ipxact:busWidth>8</ipxact:busWidth>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeTransactionalQualifier_2022()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeTransactionalQualifier_2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Transactional> testTransactional(new Transactional());
    auto qualifier = testTransactional->getQualifier();
    qualifier->setType(Qualifier::Type::Data);
    qualifier->setType(Qualifier::Type::User);
    qualifier->setAttribute(Qualifier::Attribute::UserDefined, "test");

    QSharedPointer<Port> testPort(new Port("testPort"));    
    testPort->setTransactional(testTransactional);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative></ipxact:initiative>"
                "<ipxact:qualifier>"
                    "<ipxact:isData>true</ipxact:isData>"
                    "<ipxact:isUser user=\"test\">true</ipxact:isUser>"
                "</ipxact:qualifier>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeTransactionalProtocol()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeTransactionalProtocol()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<Protocol> testProtocol (new Protocol());
    testProtocol->setProtocolType("tlm");
    testProtocol->setPayloadName("testPayload");
    testProtocol->setPayloadType("generic");
    testProtocol->setPayloadExtension("extended", true);
    testProtocol->getVendorExtensions()->append(testExtension);

    QSharedPointer<Transactional> testTransactional(new Transactional());
    testTransactional->setInitiative("provides");
    testTransactional->setProtocol(testProtocol);

    QSharedPointer<Port> testPort(new Port("testPort"));    
    testPort->setTransactional(testTransactional);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
                "<ipxact:protocol>"
                    "<ipxact:protocolType>tlm</ipxact:protocolType>"
                    "<ipxact:payload>"
                        "<ipxact:name>testPayload</ipxact:name>"
                        "<ipxact:type>generic</ipxact:type>"
                        "<ipxact:extension mandatory=\"true\">extended</ipxact:extension>"
                        "<ipxact:vendorExtensions>"
                            "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                        "</ipxact:vendorExtensions>"
                    "</ipxact:payload>"
                "</ipxact:protocol>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();
    expectedOutput.clear();

    testProtocol->setProtocolType("customType");
    testProtocol->setPayloadExtension("extend", false);
    testProtocol->getVendorExtensions()->removeAll(testExtension);

    expectedOutput =
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
                "<ipxact:protocol>"
                    "<ipxact:protocolType custom=\"customType\">custom</ipxact:protocolType>"
                    "<ipxact:payload>"
                        "<ipxact:name>testPayload</ipxact:name>"
                        "<ipxact:type>generic</ipxact:type>"
                        "<ipxact:extension>extend</ipxact:extension>"
                    "</ipxact:payload>"
                "</ipxact:protocol>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        ;

    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeTransactionalTypeDefinitions()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeTransactionalTypeDefinitions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<WireTypeDef> testTypeDefinition(new WireTypeDef("testType"));
    testTypeDefinition->getViewRefs()->append(QStringList{ "testView" });
    testTypeDefinition->setConstrained(true);
    QStringList typeDefinitions;
    typeDefinitions.append("testTypeDefinition");
    testTypeDefinition->getTypeDefinitions()->append(typeDefinitions);

    QSharedPointer<Transactional> testTransactional(new Transactional());
    testTransactional->setInitiative("provides");
    testTransactional->getTransTypeDef()->append(testTypeDefinition);

    QSharedPointer<Port> testPort(new Port("testPort"));    
    testPort->setTransactional(testTransactional);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
                "<ipxact:transTypeDefs>"
                    "<ipxact:transTypeDef>"
                        "<ipxact:typeName exact=\"true\">testType</ipxact:typeName>"
                        "<ipxact:typeDefinition>testTypeDefinition</ipxact:typeDefinition>"
                        "<ipxact:viewRef>testView</ipxact:viewRef>"
                    "</ipxact:transTypeDef>"
                "</ipxact:transTypeDefs>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortWriter::writeTransactionalConnectionMinMax()
//-----------------------------------------------------------------------------
void tst_ComponentPortWriter::writeTransactionalConnectionMinMax()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Transactional> testTransactional(new Transactional());
    testTransactional->setInitiative("provides");
    testTransactional->setMinConnections("8*2");
    testTransactional->setMaxConnections("16*2");

    QSharedPointer<Port> testPort(new Port("testPort"));    
    testPort->setTransactional(testTransactional);

    QString expectedOutput(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
                "<ipxact:connection>"
                    "<ipxact:maxConnections>16*2</ipxact:maxConnections>"
                    "<ipxact:minConnections>8*2</ipxact:minConnections>"
                "</ipxact:connection>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );

    PortWriter portWriter;
    portWriter.writePort(xmlStreamWriter, testPort, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ComponentPortWriter)

#include "tst_ComponentPortWriter.moc"

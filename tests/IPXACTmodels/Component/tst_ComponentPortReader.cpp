//-----------------------------------------------------------------------------
// File: tst_ComponentPortReader::cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.09.2015
//
// Description:
// Unit test for class tst_ComponentPortReader::
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/PortReader.h>
#include <IPXACTmodels/Component/Structured.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>
#include <QDomDocument>

class tst_ComponentPortReader : public QObject
{
    Q_OBJECT


private slots:

    void readSimplePort();
    void readSimpleTransactionalPort();
    void readPortPresence();
    void readPortArrays();
    void readPortExtensions();
    void readPortAdHocVisibility();
    void readPortPosition();
    void readPortTags();

    void readWirePortAllLogicalDirectionsAllowed();
    void readWirePortVectors();
    void readWirePortVectors_2022();
    void readWireTypeDefinitions();
    void readWireDriver();
    void readWireDefaultDriver();
    void readWireQualifiers_2022();

    void readTransactionalAllLogicalInitiativesAllowed();
    void readTransactionalKind();
    void readTransactionalBusWidth();
    void readTransactionalProtocol();
    void readTransactionalTypeDefinitions();
    void readTransactionalConnectionMinMax();

    void readStrucuredType_2022();
    void readStrucuredType_2022_data();
    void readStructuredVectors_2022();
    void readStructuredSubPortWire_2022();
    void readNestedStructuredSubPort_2022();
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readSimplePort()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readSimplePort()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:displayName>PortorDisplay</ipxact:displayName>"
            "<ipxact:description>PortorDescription</ipxact:description>"
            "<ipxact:wire>"
                "<ipxact:direction>in</ipxact:direction>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->displayName(), QString("PortorDisplay"));
    QCOMPARE(testPort->description(), QString("PortorDescription"));

    QCOMPARE(testPort->getWire().isNull(), false);
    QCOMPARE(testPort->getTransactional().isNull(), true);
    QCOMPARE(testPort->getDirection(), DirectionTypes::IN);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readSimpleTransactionalPort()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readSimpleTransactionalPort()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:displayName>PortorDisplay</ipxact:displayName>"
            "<ipxact:description>PortorDescription</ipxact:description>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->displayName(), QString("PortorDisplay"));
    QCOMPARE(testPort->description(), QString("PortorDescription"));

    QCOMPARE(testPort->getWire().isNull(), true);
    QCOMPARE(testPort->getTransactional().isNull(), false);
    QCOMPARE(testPort->getTransactional()->getInitiative(), QString("provides"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readPortPresence()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readPortPresence()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:isPresent>4-2*2+1</ipxact:isPresent>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->getIsPresent(), QString("4-2*2+1"));

    QCOMPARE(testPort->getWire().isNull(), false);
    QCOMPARE(testPort->getTransactional().isNull(), true);
    QCOMPARE(testPort->getDirection(), DirectionTypes::OUT);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readPortArrays()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readPortArrays()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>requires</ipxact:initiative>"
            "</ipxact:transactional>"
            "<ipxact:arrays>"
                "<ipxact:array>"
                    "<ipxact:left>4+4</ipxact:left>"
                    "<ipxact:right>18</ipxact:right>"
                "</ipxact:array>"
            "</ipxact:arrays>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->getArrays()->size(), 1);
    QCOMPARE(testPort->getArrayLeft(), QString("4+4"));
    QCOMPARE(testPort->getArrayRight(), QString("18"));

    QCOMPARE(testPort->getWire().isNull(), true);
    QCOMPARE(testPort->getTransactional().isNull(), false);
    QCOMPARE(testPort->getTransactional()->getInitiative(), QString("requires"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readPortExtensions()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readPortExtensions()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
            "</ipxact:wire>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->getVendorExtensions()->size(), 1);
    QCOMPARE(testPort->getVendorExtensions()->first()->type(), QString("testExtension"));

    QCOMPARE(testPort->getWire().isNull(), false);
    QCOMPARE(testPort->getTransactional().isNull(), true);
    QCOMPARE(testPort->getDirection(), DirectionTypes::OUT);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readPortAdHocVisibility()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readPortAdHocVisibility()
{
    QString documentContent(
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


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->getVendorExtensions()->size(), 1);
    QCOMPARE(testPort->isAdHocVisible(), true);

    QCOMPARE(testPort->getWire().isNull(), false);
    QCOMPARE(testPort->getTransactional().isNull(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readPortPosition()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readPortPosition()
{
    QString documentContent(
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


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->getVendorExtensions()->size(), 1);
    QCOMPARE(testPort->getDefaultPos().x(), qreal(1));
    QCOMPARE(testPort->getDefaultPos().y(), qreal(1));

    QCOMPARE(testPort->getWire().isNull(), false);
    QCOMPARE(testPort->getTransactional().isNull(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readPortTags()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readPortTags()
{
    QString documentContent(
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


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->getVendorExtensions()->size(), 1);
    QCOMPARE(testPort->getPortTags(), QString("oyoroi,yuJing,guilang,tags"));

    QCOMPARE(testPort->getWire().isNull(), false);
    QCOMPARE(testPort->getTransactional().isNull(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readWirePortAllLogicalDirectionsAllowed()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readWirePortAllLogicalDirectionsAllowed()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire allLogicalDirectionsAllowed=\"true\">"
                "<ipxact:direction>in</ipxact:direction>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));

    QCOMPARE(testPort->getTransactional().isNull(), true);
    QCOMPARE(testPort->getWire().isNull(), false);

    QCOMPARE(testPort->allLogicalDirectionsAllowed(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readWirePortVectors()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readWirePortVectors()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:vectors>"
                    "<ipxact:vector>"
                        "<ipxact:left>4+18-Yaoxao</ipxact:left>"
                        "<ipxact:right>Yaoxao</ipxact:right>"
                    "</ipxact:vector>"
                    "<ipxact:vector>"
                        "<ipxact:left>1</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:vector>"
                "</ipxact:vectors>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));

    QCOMPARE(testPort->getTransactional().isNull(), true);
    QCOMPARE(testPort->getWire().isNull(), false);
    QCOMPARE(testPort->getWire()->getVectors()->count(), 2);

    auto const& vector = testPort->getWire()->getVectors()->first();
    QCOMPARE(vector.getLeft(), QStringLiteral("4+18-Yaoxao"));
    QCOMPARE(vector.getRight(), QStringLiteral("Yaoxao"));
    QCOMPARE(vector.getId(), QString());
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readWirePortVectors_2022()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readWirePortVectors_2022()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:vectors>"
                    "<ipxact:vector vectorId=\"testVector\">"
                        "<ipxact:left>1</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:vector>"
                "</ipxact:vectors>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std22);

    QCOMPARE(testPort->name(), QString("testPort"));

    QCOMPARE(testPort->getTransactional().isNull(), true);
    QCOMPARE(testPort->getWire().isNull(), false);

    auto const& vector = testPort->getWire()->getVectors()->first();
    QCOMPARE(vector.getLeft(), QStringLiteral("1"));
    QCOMPARE(vector.getRight(), QStringLiteral("0"));
    QCOMPARE(vector.getId(), QStringLiteral("testVector"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readWireTypeDefinitions()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readWireTypeDefinitions()
{
    QString documentContent(
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


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));

    QCOMPARE(testPort->getTransactional().isNull(), true);
    QCOMPARE(testPort->getWire().isNull(), false);

    QCOMPARE(testPort->getWire()->getWireTypeDefs()->size(), 1);
    QSharedPointer<WireTypeDef> typeDefinition = testPort->getWire()->getWireTypeDefs()->first();
    QCOMPARE(typeDefinition->getTypeName(), QString("testType"));
    QCOMPARE(typeDefinition->getTypeDefinitions()->size(), 1);
    QCOMPARE(typeDefinition->getTypeDefinitions()->first(), QString("testTypeDefinition"));
    QCOMPARE(typeDefinition->getViewRefs()->size(), 1);
    QCOMPARE(typeDefinition->getViewRefs()->first(), QString("testView"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readWireDriver()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readWireDriver()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:drivers>"
                    "<ipxact:driver>"
                        "<ipxact:range>"
                            "<ipxact:left>7</ipxact:left>"
                            "<ipxact:right>0</ipxact:right>"
                        "</ipxact:range>"
                        "<ipxact:defaultValue>4+4-2</ipxact:defaultValue>"
                    "</ipxact:driver>"
                "</ipxact:drivers>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->getTransactional().isNull(), true);
    QCOMPARE(testPort->getWire().isNull(), false);
    QCOMPARE(testPort->getDefaultValue(), QString("4+4-2"));

    auto wireRange = testPort->getWire()->getDriver()->getRange();
    QCOMPARE(wireRange.getLeft(), QString("7"));
    QCOMPARE(wireRange.getRight(), QString("0"));

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readWireDefaultDriver()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readWireDefaultDriver()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:drivers>"
                    "<ipxact:driver>"
                        "<ipxact:defaultValue>4+4-2</ipxact:defaultValue>"
                    "</ipxact:driver>"
                "</ipxact:drivers>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));

    QCOMPARE(testPort->getTransactional().isNull(), true);
    QCOMPARE(testPort->getWire().isNull(), false);
    QCOMPARE(testPort->getDefaultValue(), QString("4+4-2"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readWireQualifiers_2022()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readWireQualifiers_2022()
{
     QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:wire>"
                "<ipxact:direction>out</ipxact:direction>"
                "<ipxact:qualifier>"
                    "<ipxact:isData>true</ipxact:isData>"
                    "<ipxact:isValid>true</ipxact:isValid>"
                    "<ipxact:isInterrupt>false</ipxact:isInterrupt>"
                    "<ipxact:isClockEn level=\"high\">true</ipxact:isClockEn>"
                "</ipxact:qualifier>"
            "</ipxact:wire>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std22);

    QCOMPARE(testPort->getTransactional().isNull(), true);
    
    auto wire = testPort->getWire();
    QCOMPARE(wire.isNull(), false);
    QCOMPARE(wire->getQualifier()->hasType(Qualifier::Type::Data), true);
    QCOMPARE(wire->getQualifier()->hasType(Qualifier::Type::Valid), true);
    QCOMPARE(wire->getQualifier()->hasType(Qualifier::Type::Interrupt), false);
    QCOMPARE(wire->getQualifier()->hasType(Qualifier::Type::ClockEnable), true);
    QCOMPARE(wire->getQualifier()->getAttribute(Qualifier::Attribute::ClockEnableLevel), QString("high"));
}


//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readTransactionalAllLogicalInitiativesAllowed()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readTransactionalAllLogicalInitiativesAllowed()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional allLogicalInitiativesAllowed=\"true\">"
                "<ipxact:initiative>provides</ipxact:initiative>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));

    QCOMPARE(testPort->getWire().isNull(), true);
    QCOMPARE(testPort->getTransactional().isNull(), false);
    QCOMPARE(testPort->getTransactional()->getAllLogicalInitiativesAllowed(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readTransactionalKind()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readTransactionalKind()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
                "<ipxact:kind>tlm_port</ipxact:kind>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));

    QCOMPARE(testPort->getWire().isNull(), true);
    QCOMPARE(testPort->getTransactional().isNull(), false);
    QCOMPARE(testPort->getTransactional()->getAllLogicalInitiativesAllowed(), false);
    QCOMPARE(testPort->getTransactional()->getKind(), QString("tlm_port"));

    documentContent =
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
                "<ipxact:kind custom=\"customKind\">custom</ipxact:kind>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        ;

    document.setContent(documentContent);
    portNode = document.firstChildElement("ipxact:port");
    testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));

    QCOMPARE(testPort->getWire().isNull(), true);
    QCOMPARE(testPort->getTransactional().isNull(), false);
    QCOMPARE(testPort->getTransactional()->getKind(), QString("customKind"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readTransactionalBusWidth()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readTransactionalBusWidth()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:transactional>"
                "<ipxact:initiative>provides</ipxact:initiative>"
                "<ipxact:busWidth>8</ipxact:busWidth>"
            "</ipxact:transactional>"
        "</ipxact:port>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));

    QCOMPARE(testPort->getWire().isNull(), true);
    QCOMPARE(testPort->getTransactional().isNull(), false);
    QCOMPARE(testPort->getTransactional()->getBusWidth(), QString("8"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readTransactionalProtocol()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readTransactionalProtocol()
{
    QString documentContent(
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


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));

    QCOMPARE(testPort->getWire().isNull(), true);
    QCOMPARE(testPort->getTransactional().isNull(), false);
    QCOMPARE(testPort->getTransactional()->getProtocol().isNull(), false);

    QSharedPointer<Protocol> transactionalProtocol = testPort->getTransactional()->getProtocol();
    QCOMPARE(transactionalProtocol->getProtocolType(), QString("tlm"));
    QCOMPARE(transactionalProtocol->getPayloadName(), QString("testPayload"));
    QCOMPARE(transactionalProtocol->getPayloadType(), QString("generic"));
    QCOMPARE(transactionalProtocol->hasMandatoryPayloadExtension(), true);
    QCOMPARE(transactionalProtocol->getPayloadExtension(), QString("extended"));
    QCOMPARE(transactionalProtocol->getVendorExtensions()->size(), 1);
    QCOMPARE(transactionalProtocol->getVendorExtensions()->first()->type(), QString("testExtension"));

    documentContent =
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

    document.setContent(documentContent);
    portNode = document.firstChildElement("ipxact:port");
    testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->getTransactional()->getProtocol().isNull(), false);

    transactionalProtocol = testPort->getTransactional()->getProtocol();
    QCOMPARE(transactionalProtocol->getProtocolType(), QString("custom"));
    QCOMPARE(transactionalProtocol->getCustomProtocolType(), QString("customType"));
    QCOMPARE(transactionalProtocol->getPayloadName(), QString("testPayload"));
    QCOMPARE(transactionalProtocol->getPayloadType(), QString("generic"));
    QCOMPARE(transactionalProtocol->hasMandatoryPayloadExtension(), false);
    QCOMPARE(transactionalProtocol->getPayloadExtension(), QString("extend"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readTransactionalTypeDefinitions()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readTransactionalTypeDefinitions()
{
    QString documentContent(
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


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->getTransactional().isNull(), false);
    QCOMPARE(testPort->getTransactional()->getTransTypeDef()->size(), 1);
    
    QSharedPointer<WireTypeDef> transTypeDefinition = testPort->getTransactional()->getTransTypeDef()->first();
    QCOMPARE(transTypeDefinition->getTypeName(), QString("testType"));
    QCOMPARE(transTypeDefinition->isConstrained(), true);
    QCOMPARE(transTypeDefinition->getTypeDefinitions()->size(), 1);
    QCOMPARE(transTypeDefinition->getTypeDefinitions()->first(), QString("testTypeDefinition"));
    QCOMPARE(transTypeDefinition->getViewRefs()->size(), 1);
    QCOMPARE(transTypeDefinition->getViewRefs()->first(), QString("testView"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readTransactionalConnectionMinMax()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readTransactionalConnectionMinMax()
{
    QString documentContent(
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


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std14);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->getTransactional().isNull(), false);

    QCOMPARE(testPort->getTransactional()->getMaxConnections(), QString("16*2"));
    QCOMPARE(testPort->getTransactional()->getMinConnections(), QString("8*2"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readStrucuredType_2022()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readStrucuredType_2022()
{
    QFETCH(QString, content);
    QFETCH(Structured::Type, expectedType);
    QFETCH(DirectionTypes::Direction, expectedDirection);

     QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:structured>" +
            content +
            "</ipxact:structured>"
        "</ipxact:port>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std22);

    QCOMPARE(testPort->name(), QString("testPort"));
    QCOMPARE(testPort->getWire().isNull(), true);
    QCOMPARE(testPort->getTransactional().isNull(), true);
    QCOMPARE(testPort->getStructured().isNull(), false);

    auto structured = testPort->getStructured();
    QCOMPARE(structured->isPacked(), false);
    QCOMPARE(structured->getType(), expectedType);
    QCOMPARE(structured->getDirection(), expectedDirection);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readStrucuredType_2022_data()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readStrucuredType_2022_data()
{
    QTest::addColumn<QString>("content");
    QTest::addColumn<Structured::Type>("expectedType");
    QTest::addColumn<DirectionTypes::Direction>("expectedDirection");

    QTest::addRow("Struct without direction") << 
        "<ipxact:struct>" << Structured::Type::Struct << DirectionTypes::DIRECTION_INVALID;
    QTest::addRow("Struct with in direction") << 
        "<ipxact:struct direction = \"in\">" << Structured::Type::Struct << DirectionTypes::IN;

    QTest::addRow("Union without direction") << 
        "<ipxact:union>" << Structured::Type::Union << DirectionTypes::DIRECTION_INVALID;
    QTest::addRow("Union with out direction") << 
        "<ipxact:union direction = \"out\">" << Structured::Type::Union << DirectionTypes::OUT;

    QTest::addRow("Interface without phantom attribute") <<
        "<ipxact:interface>" << Structured::Type::Interface << DirectionTypes::DIRECTION_INVALID;
    QTest::addRow("Interface with true phantom attribute") <<
        "<ipxact:interface phantom=\"true\">" << Structured::Type::Interface << DirectionTypes::DIRECTION_PHANTOM;
    QTest::addRow("Interface with false phantom attribute") <<
        "<ipxact:interface phantom=\"false\">" << Structured::Type::Interface << DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readStructuredVectors_2022()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readStructuredVectors_2022()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:structured packed=\"true\">"
                "<ipxact:vectors>"
                    "<ipxact:vector>"
                        "<ipxact:left>1</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:vector>"
                    "<ipxact:vector>"
                        "<ipxact:left>2</ipxact:left>"
                        "<ipxact:right>0</ipxact:right>"
                    "</ipxact:vector>"
                "</ipxact:vectors>"
            "</ipxact:structured>"
        "</ipxact:port>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std22);

    QCOMPARE(testPort->getStructured().isNull(), false);

    auto structured = testPort->getStructured();
    QCOMPARE(structured->isPacked(), true);
    QCOMPARE(structured->getVectors()->count(), 2);

    auto const& lastVector = structured->getVectors()->last();
    QCOMPARE(lastVector.getLeft(), "2");
    QCOMPARE(lastVector.getRight(), "0");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readStructuredSubPortWire_2022()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readStructuredSubPortWire_2022()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:structured>"
                "<ipxact:interface/>"
                "<ipxact:subPorts>"
                    "<ipxact:subPort isIO=\"true\">"
                        "<ipxact:name>minimalWire</ipxact:name>"
                        "<ipxact:wire>"
                            "<ipxact:direction>in</ipxact:direction>"
                        "</ipxact:wire>"
                    "</ipxact:subPort>"
                "</ipxact:subPorts>"
            "</ipxact:structured>"
        "</ipxact:port>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std22);

    QCOMPARE(testPort->getStructured().isNull(), false);

    auto structured = testPort->getStructured();
    QCOMPARE(structured->isPacked(), false);
    QCOMPARE(structured->getSubPorts()->count(), 1);

    auto subPort = structured->getSubPorts()->first();
    QCOMPARE(subPort->isIO(), true);
    QCOMPARE(subPort->name(), QString("minimalWire"));

    QCOMPARE(subPort->getWire().isNull(), false);
    QCOMPARE(subPort->getWire()->getDirection(), DirectionTypes::IN);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortReader::readNestedStructuredSubPort_2022()
//-----------------------------------------------------------------------------
void tst_ComponentPortReader::readNestedStructuredSubPort_2022()
{
    QString documentContent(
        "<ipxact:port>"
            "<ipxact:name>testPort</ipxact:name>"
            "<ipxact:structured>"
                "<ipxact:interface/>"
                "<ipxact:subPorts>"
                    "<ipxact:subPort>"
                    "<ipxact:name>firstLayer</ipxact:name>"
                        "<ipxact:structured>"
                        "<ipxact:interface/>"
                            "<ipxact:subPorts>"
                                "<ipxact:subPort>"
                                "<ipxact:name>middleLayer</ipxact:name>"
                                    "<ipxact:structured>"
                                        "<ipxact:interface/>"
                                        "<ipxact:subPorts>"
                                            "<ipxact:subPort>"
                                                "<ipxact:name>wireLayer</ipxact:name>"
                                                "<ipxact:wire>"
                                                    "<ipxact:direction>in</ipxact:direction>"
                                                "</ipxact:wire>"
                                            "</ipxact:subPort>"
                                        "</ipxact:subPorts>"
                                    "</ipxact:structured>"
                                "</ipxact:subPort>"
                            "</ipxact:subPorts>"
                        "</ipxact:structured>"
                    "</ipxact:subPort>"
                "</ipxact:subPorts>"
            "</ipxact:structured>"
        "</ipxact:port>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode portNode = document.firstChildElement("ipxact:port");

    QSharedPointer<Port> testPort = PortReader::createPortFrom(portNode, Document::Revision::Std22);

    QCOMPARE(testPort->getStructured().isNull(), false);

    auto structured = testPort->getStructured();
    QCOMPARE(structured->getSubPorts()->count(), 1);

    auto const& firstSubPort = structured->getSubPorts()->first();
    QCOMPARE(firstSubPort->name(), "firstLayer");
    QCOMPARE(firstSubPort->getWire().isNull(), true);
    QCOMPARE(firstSubPort->getStructured().isNull(), false);
    QCOMPARE(firstSubPort->getStructured()->getSubPorts()->count(), 1);

    auto const& middleSubPort = firstSubPort->getStructured()->getSubPorts()->first();
    QCOMPARE(middleSubPort->name(), "middleLayer");
    QCOMPARE(middleSubPort->getWire().isNull(), true);
    QCOMPARE(middleSubPort->getStructured().isNull(), false);
    QCOMPARE(middleSubPort->getStructured()->getSubPorts()->count(), 1);

    auto const& finalSubPort = middleSubPort->getStructured()->getSubPorts()->first();
    QCOMPARE(finalSubPort->name(), QString("wireLayer"));
    QCOMPARE(finalSubPort->getStructured().isNull(), true);
    QCOMPARE(finalSubPort->getWire().isNull(), false);

    auto const& wire = finalSubPort->getWire();
    QCOMPARE(wire->getDirection(), DirectionTypes::IN);
}

QTEST_APPLESS_MAIN(tst_ComponentPortReader)

#include "tst_ComponentPortReader.moc"

//-----------------------------------------------------------------------------
// File: tst_DesignWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 17.08.2015
//
// Description:
// Unit test for class DesignWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Design/DesignWriter.h>
#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Group.h>

#include <editors/common/ColumnTypes.h>

#include <QtTest>
#include <QSharedPointer>

class tst_DesignWriter : public QObject
{
    Q_OBJECT

public:
    tst_DesignWriter();

private slots:
    
    void testWriteSimpleDesign();
    void testWriteSimple2022Design();
    void testWriteProcessingInstructions();

    void testWriteComponentInstances();
    void testWrite2022ComponentInstances();
    void testWriteComponentInstanceExtensions();

    void testWriteInterconnections();
    void testWrite2022Interconnections();
    void testWriteInterconnectionExtensions();

    void testWriteMonitorInterconnections();
    void testWrite2022MonitorInterconnections();

    void testWriteAdHocConnections();
    void testWrite2022AdHocConnections();
    void testWrite2022SubPortReferences();
    void testWriteAdHocConnectionExtensions();

    void testWriteParameters();
    void testWriteAssertions();
    void testWriteVendorExtensions();

    void testWriteColumns();
    void testWritePortAdHocVisibilitiesAndPositions();
    void testWriteApiDependencies();
    void testWriteHierApiDependencies();
    void testWriteComConnections();
    void testWriteHierComConnections();

private:

    void compareOutputToExpected(QString const& output, QString const& expectedOutput);

};

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::tst_DesignWriter()
//-----------------------------------------------------------------------------
tst_DesignWriter::tst_DesignWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testSimpleDesign()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteSimpleDesign()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    //compareOutputToExpected(output, expectedOutput);
    QCOMPARE(output, expectedOutput);

    expectedOutput.clear();
    output.clear();

    testDesign->setDescription("testDescription");
    expectedOutput = 
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:description>testDescription</ipxact:description>\n"
        "</ipxact:design>\n"
        ;

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteSimple2022Design()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteSimple2022Design()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std22));

    testDesign->setDisplayName("display");
    testDesign->setShortDescription("brief");
    testDesign->setDescription("testDescription");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\">\n"
        "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
        "\t<ipxact:library>TestLibrary</ipxact:library>\n"
        "\t<ipxact:name>TestDesign</ipxact:name>\n"
        "\t<ipxact:version>0.1</ipxact:version>\n"
        "\t<ipxact:displayName>display</ipxact:displayName>\n"
        "\t<ipxact:shortDescription>brief</ipxact:shortDescription>\n"
        "\t<ipxact:description>testDescription</ipxact:description>\n"
        "</ipxact:design>\n"
    );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteProcessingInstructions()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteProcessingInstructions()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    testDesign->setTopComments("Header comment");
    testDesign->addXmlProcessingInstructions("xml-stylesheet", "href=\"style.css\"");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<!--Header comment-->\n"
        "<?xml-stylesheet href=\"style.css\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteComponentInstances()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteComponentInstances()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<ConfigurableVLNVReference> testComponentReference (
        new ConfigurableVLNVReference(VLNV::COMPONENT, "tuni.fi", "TestLibrary", "testComponent", "1.0"));

    QSharedPointer<ConfigurableElementValue> componentElement (
        new ConfigurableElementValue("10", "testReferenceID"));
    testComponentReference->getConfigurableElementValues()->append(componentElement);

    QSharedPointer<ComponentInstance> testComponentInstance(new ComponentInstance("testInstance",
        testComponentReference));
    testComponentInstance->setDisplayName("displayName");
    testComponentInstance->setDescription("described");
    testComponentInstance->setPosition(QPointF(10, 10));
    testComponentInstance->setUuid("testUUID");
    testComponentInstance->setIsPresent("2-1");

    testDesign->getComponentInstances()->append(testComponentInstance);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:componentInstances>\n"
                "\t\t<ipxact:componentInstance>\n"
                    "\t\t\t<ipxact:instanceName>testInstance</ipxact:instanceName>\n"
                    "\t\t\t<ipxact:displayName>displayName</ipxact:displayName>\n"
                    "\t\t\t<ipxact:description>described</ipxact:description>\n"
                    "\t\t\t<ipxact:isPresent>2-1</ipxact:isPresent>\n"
                    "\t\t\t<ipxact:componentRef vendor=\"tuni.fi\" library=\"TestLibrary\""
                            " name=\"testComponent\" version=\"1.0\">\n"
                        "\t\t\t\t<ipxact:configurableElementValues>\n"
                            "\t\t\t\t\t<ipxact:configurableElementValue referenceId=\"testReferenceID\">10"
                                      "</ipxact:configurableElementValue>\n"
                        "\t\t\t\t</ipxact:configurableElementValues>\n"
                    "\t\t\t</ipxact:componentRef>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<kactus2:uuid>testUUID</kactus2:uuid>\n"
                        "\t\t\t\t<kactus2:position x=\"10\" y=\"10\"/>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:componentInstance>\n"
            "\t</ipxact:componentInstances>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWrite2022ComponentInstances()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWrite2022ComponentInstances()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std22));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<ConfigurableVLNVReference> testComponentReference (
        new ConfigurableVLNVReference(VLNV::COMPONENT, "tuni.fi", "TestLibrary", "testComponent", "1.0"));

    QSharedPointer<ConfigurableElementValue> componentElement (
        new ConfigurableElementValue("10", "testReferenceID"));
    testComponentReference->getConfigurableElementValues()->append(componentElement);

    QSharedPointer<ComponentInstance> testComponentInstance(new ComponentInstance("testInstance",
        testComponentReference));
    testComponentInstance->setDisplayName("displayName");
    testComponentInstance->setShortDescription("brief");
    testComponentInstance->setDescription("described");
    testComponentInstance->setPosition(QPointF(10, 10));
    testComponentInstance->setUuid("testUUID");

    QSharedPointer<ComponentInstance::PowerDomainLink> firstLink(new ComponentInstance::PowerDomainLink({ "ext", QStringList("int") }));
    QSharedPointer<ComponentInstance::PowerDomainLink> secondLink(new ComponentInstance::PowerDomainLink({ "outside", QStringList{"inside", "in"}}));
    testComponentInstance->getPowerDomainLinks()->append(firstLink);
    testComponentInstance->getPowerDomainLinks()->append(secondLink);

    testDesign->getComponentInstances()->append(testComponentInstance);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:componentInstances>\n"
                "\t\t<ipxact:componentInstance>\n"
                    "\t\t\t<ipxact:instanceName>testInstance</ipxact:instanceName>\n"
                    "\t\t\t<ipxact:displayName>displayName</ipxact:displayName>\n"
                    "\t\t\t<ipxact:shortDescription>brief</ipxact:shortDescription>\n"
                    "\t\t\t<ipxact:description>described</ipxact:description>\n"
                    "\t\t\t<ipxact:componentRef vendor=\"tuni.fi\" library=\"TestLibrary\""
                            " name=\"testComponent\" version=\"1.0\">\n"
                        "\t\t\t\t<ipxact:configurableElementValues>\n"
                            "\t\t\t\t\t<ipxact:configurableElementValue referenceId=\"testReferenceID\">10"
                                      "</ipxact:configurableElementValue>\n"
                        "\t\t\t\t</ipxact:configurableElementValues>\n"
                    "\t\t\t</ipxact:componentRef>\n"
                    "\t\t\t<ipxact:powerDomainLinks>\n"
                        "\t\t\t\t<ipxact:powerDomainLink>\n"
                            "\t\t\t\t\t<ipxact:externalPowerDomainReference>ext</ipxact:externalPowerDomainReference>\n"
                            "\t\t\t\t\t<ipxact:internalPowerDomainReference>int</ipxact:internalPowerDomainReference>\n"
                        "\t\t\t\t</ipxact:powerDomainLink>\n"
                        "\t\t\t\t<ipxact:powerDomainLink>\n"
                            "\t\t\t\t\t<ipxact:externalPowerDomainReference>outside</ipxact:externalPowerDomainReference>\n"
                            "\t\t\t\t\t<ipxact:internalPowerDomainReference>inside</ipxact:internalPowerDomainReference>\n"
                            "\t\t\t\t\t<ipxact:internalPowerDomainReference>in</ipxact:internalPowerDomainReference>\n"
                        "\t\t\t\t</ipxact:powerDomainLink>\n"
                    "\t\t\t</ipxact:powerDomainLinks>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<kactus2:uuid>testUUID</kactus2:uuid>\n"
                        "\t\t\t\t<kactus2:position x=\"10\" y=\"10\"/>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:componentInstance>\n"
            "\t</ipxact:componentInstances>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteComponentInstanceExtensions()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteComponentInstanceExtensions()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<ConfigurableVLNVReference> testComponentReference (
        new ConfigurableVLNVReference(VLNV::COMPONENT, "tuni.fi", "TestLibrary", "testComponent", "1.0"));

    QSharedPointer<ConfigurableElementValue> componentElement (
        new ConfigurableElementValue("10", "testReferenceID"));
    testComponentReference->getConfigurableElementValues()->append(componentElement);

    QSharedPointer<ComponentInstance> testComponentInstance(new ComponentInstance("testInstance",
        testComponentReference));
    testComponentInstance->setPosition(QPointF(10, 10));
    testComponentInstance->setUuid("testUUID");

    testComponentInstance->setComponentRef(testComponentReference);
    testComponentInstance->setImportRef("importSource");
    testComponentInstance->updateBusInterfacePosition("testInterface", QPointF(4,4));
    testComponentInstance->updateAdHocPortPosition("adHocPort", QPointF(3,3));
    testComponentInstance->updateApiInterfacePosition("apiInterface", QPointF(2,2));
    testComponentInstance->updateComInterfacePosition("comInterface", QPointF(1,1));
    testComponentInstance->setFileSetRef("filesetRef");
    testComponentInstance->setMapping("hwRef");

    QSharedPointer<ComponentInstance> otherComponentInstance(new ComponentInstance("otherInstance",
        testComponentReference));
    otherComponentInstance->setPosition(QPointF(1,1));
    otherComponentInstance->setUuid("otherUUID");
    otherComponentInstance->setImported(true);

    QMap<QString, QString> swProperties;
    swProperties.insert("testSWProperty", "8");
    testComponentInstance->setPropertyValues(swProperties);

    testDesign->getComponentInstances()->append(testComponentInstance);
    testDesign->getComponentInstances()->append(otherComponentInstance);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:componentInstances>\n"
                "\t\t<ipxact:componentInstance>\n"
                    "\t\t\t<ipxact:instanceName>testInstance</ipxact:instanceName>\n"
                    "\t\t\t<ipxact:componentRef vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"testComponent\""
                        " version=\"1.0\">\n"
                        "\t\t\t\t<ipxact:configurableElementValues>\n"
                            "\t\t\t\t\t<ipxact:configurableElementValue referenceId=\"testReferenceID\">10"
                                "</ipxact:configurableElementValue>\n"
                        "\t\t\t\t</ipxact:configurableElementValues>\n"
                    "\t\t\t</ipxact:componentRef>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<kactus2:uuid>testUUID</kactus2:uuid>\n"
                        "\t\t\t\t<kactus2:position x=\"10\" y=\"10\"/>\n"
                        "\t\t\t\t<kactus2:imported importRef=\"importSource\"/>\n"
                        "\t\t\t\t<kactus2:portPositions>\n"
                            "\t\t\t\t\t<kactus2:portPosition busRef=\"testInterface\" x=\"4\" y=\"4\"/>\n"
                        "\t\t\t\t</kactus2:portPositions>\n"
                        "\t\t\t\t<kactus2:adHocVisibilities>\n"
                            "\t\t\t\t\t<kactus2:adHocVisible portName=\"adHocPort\" x=\"3\" y=\"3\"/>\n"
                        "\t\t\t\t</kactus2:adHocVisibilities>\n"
                        "\t\t\t\t<kactus2:apiInterfacePositions>\n"
                            "\t\t\t\t\t<kactus2:apiInterfacePosition apiRef=\"apiInterface\" x=\"2\" y=\"2\"/>\n"
                        "\t\t\t\t</kactus2:apiInterfacePositions>\n"
                        "\t\t\t\t<kactus2:comInterfacePositions>\n"
                            "\t\t\t\t\t<kactus2:comInterfacePosition comRef=\"comInterface\" x=\"1\" y=\"1\"/>\n"
                        "\t\t\t\t</kactus2:comInterfacePositions>\n"
                        "\t\t\t\t<kactus2:fileSetRef>filesetRef</kactus2:fileSetRef>\n"
                        "\t\t\t\t<kactus2:mapping hwRef=\"hwRef\"/>\n"
                        "\t\t\t\t<kactus2:propertyValues>\n"
                            "\t\t\t\t\t<kactus2:propertyValue name=\"testSWProperty\" value=\"8\"/>\n"
                        "\t\t\t\t</kactus2:propertyValues>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:componentInstance>\n"
                "\t\t<ipxact:componentInstance>\n"
                    "\t\t\t<ipxact:instanceName>otherInstance</ipxact:instanceName>\n"
                    "\t\t\t<ipxact:componentRef vendor=\"tuni.fi\" library=\"TestLibrary\" name=\"testComponent\""
                        " version=\"1.0\">\n"
                        "\t\t\t\t<ipxact:configurableElementValues>\n"
                            "\t\t\t\t\t<ipxact:configurableElementValue referenceId=\"testReferenceID\">10"
                                "</ipxact:configurableElementValue>\n"
                        "\t\t\t\t</ipxact:configurableElementValues>\n"
                    "\t\t\t</ipxact:componentRef>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<kactus2:uuid>otherUUID</kactus2:uuid>\n"
                        "\t\t\t\t<kactus2:position x=\"1\" y=\"1\"/>\n"
                        "\t\t\t\t<kactus2:imported/>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:componentInstance>\n"
            "\t</ipxact:componentInstances>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteInterconnections()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteInterconnections()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<ActiveInterface> testStartInterface(new ActiveInterface("componentRef", "busRef"));
    testStartInterface->setIsPresent("1");
    testStartInterface->setDescription("interfaceDescription");
    testStartInterface->getExcludePorts()->append("testExcludePort");
    testStartInterface->getVendorExtensions()->append(testExtension);

    QSharedPointer<HierInterface> testHierInterface(new HierInterface("hierBusRef"));
    testHierInterface->setIsPresent("2-1");
    testHierInterface->setDescription("hierDescription");
    testHierInterface->getVendorExtensions()->append(testExtension);

    QSharedPointer<ActiveInterface> testActiveInterface(new ActiveInterface("otherComponent", "otherBus"));

    QSharedPointer<Interconnection> testInterconnection(new Interconnection(
        "testActiveHierActive", testStartInterface, "interconnectionDisplay", "interconnectionDescription"));
    testInterconnection->setIsPresent("4-3");
    testInterconnection->getActiveInterfaces()->append(testActiveInterface);
    testInterconnection->getHierInterfaces()->append(testHierInterface);
    testInterconnection->getVendorExtensions()->append(testExtension);

    testDesign->getInterconnections()->append(testInterconnection);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:interconnections>\n"
                "\t\t<ipxact:interconnection>\n"
                    "\t\t\t<ipxact:name>testActiveHierActive</ipxact:name>\n"
                    "\t\t\t<ipxact:displayName>interconnectionDisplay</ipxact:displayName>\n"
                    "\t\t\t<ipxact:description>interconnectionDescription</ipxact:description>\n"
                    "\t\t\t<ipxact:isPresent>4-3</ipxact:isPresent>\n"
                    "\t\t\t<ipxact:activeInterface componentRef=\"componentRef\" busRef=\"busRef\">\n"
                        "\t\t\t\t<ipxact:isPresent>1</ipxact:isPresent>\n"
                        "\t\t\t\t<ipxact:description>interfaceDescription</ipxact:description>\n"
                        "\t\t\t\t<ipxact:excludePorts>\n"
                            "\t\t\t\t\t<ipxact:excludePort>testExcludePort</ipxact:excludePort>\n"
                        "\t\t\t\t</ipxact:excludePorts>\n"
                        "\t\t\t\t<ipxact:vendorExtensions>\n"
                            "\t\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                        "\t\t\t\t</ipxact:vendorExtensions>\n"
                    "\t\t\t</ipxact:activeInterface>\n"
                    "\t\t\t<ipxact:activeInterface componentRef=\"otherComponent\" busRef=\"otherBus\"/>\n"
                    "\t\t\t<ipxact:hierInterface busRef=\"hierBusRef\">\n"
                        "\t\t\t\t<ipxact:isPresent>2-1</ipxact:isPresent>\n"
                        "\t\t\t\t<ipxact:description>hierDescription</ipxact:description>\n"
                        "\t\t\t\t<ipxact:vendorExtensions>\n"
                            "\t\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                        "\t\t\t\t</ipxact:vendorExtensions>\n"
                    "\t\t\t</ipxact:hierInterface>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:interconnection>\n"
            "\t</ipxact:interconnections>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWrite2022Interconnections()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWrite2022Interconnections()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std22));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<ActiveInterface> testStartInterface(new ActiveInterface("componentRef", "busRef"));
    testStartInterface->setDescription("interfaceDescription");
    testStartInterface->getExcludePorts()->append("testExcludePort");
    testStartInterface->getVendorExtensions()->append(testExtension);

    QSharedPointer<HierInterface> testHierInterface(new HierInterface("hierBusRef"));
    testHierInterface->setDescription("hierDescription");
    testHierInterface->getVendorExtensions()->append(testExtension);

    QSharedPointer<ActiveInterface> testActiveInterface(new ActiveInterface("otherComponent", "otherBus"));

    QSharedPointer<Interconnection> testInterconnection(new Interconnection());
    testInterconnection->setName("testActiveHierActive");
    testInterconnection->setDisplayName("connect");
    testInterconnection->setShortDescription("brief");
    testInterconnection->setDescription("interconnect description");
    testInterconnection->setStartInterface(testStartInterface);
    testInterconnection->getActiveInterfaces()->append(testActiveInterface);
    testInterconnection->getHierInterfaces()->append(testHierInterface);
    testInterconnection->getVendorExtensions()->append(testExtension);

    testDesign->getInterconnections()->append(testInterconnection);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:interconnections>\n"
                "\t\t<ipxact:interconnection>\n"
                    "\t\t\t<ipxact:name>testActiveHierActive</ipxact:name>\n"
                    "\t\t\t<ipxact:displayName>connect</ipxact:displayName>\n"
                    "\t\t\t<ipxact:shortDescription>brief</ipxact:shortDescription>\n"
                    "\t\t\t<ipxact:description>interconnect description</ipxact:description>\n"
                    "\t\t\t<ipxact:activeInterface componentInstanceRef=\"componentRef\" busRef=\"busRef\">\n"
                        "\t\t\t\t<ipxact:description>interfaceDescription</ipxact:description>\n"
                        "\t\t\t\t<ipxact:excludePorts>\n"
                            "\t\t\t\t\t<ipxact:excludePort>testExcludePort</ipxact:excludePort>\n"
                        "\t\t\t\t</ipxact:excludePorts>\n"
                        "\t\t\t\t<ipxact:vendorExtensions>\n"
                            "\t\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                        "\t\t\t\t</ipxact:vendorExtensions>\n"
                    "\t\t\t</ipxact:activeInterface>\n"
                    "\t\t\t<ipxact:activeInterface componentInstanceRef=\"otherComponent\" busRef=\"otherBus\"/>\n"
                    "\t\t\t<ipxact:hierInterface busRef=\"hierBusRef\">\n"
                        "\t\t\t\t<ipxact:description>hierDescription</ipxact:description>\n"
                        "\t\t\t\t<ipxact:vendorExtensions>\n"
                            "\t\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                        "\t\t\t\t</ipxact:vendorExtensions>\n"
                    "\t\t\t</ipxact:hierInterface>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:interconnection>\n"
            "\t</ipxact:interconnections>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteInterconnectionExtensions()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteInterconnectionExtensions()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<ActiveInterface> testStartInterface(new ActiveInterface("componentRef", "busRef"));

    QSharedPointer<HierInterface> testHierInterface(new HierInterface("hierBusRef"));

    QSharedPointer<Interconnection> testInterconnection(new Interconnection(
        "testActiveHierActive", testStartInterface, "", ""));
    testInterconnection->getHierInterfaces()->append(testHierInterface);
    testInterconnection->setOffPage(true);

    QList<QPointF> interconnectionRoute;
    interconnectionRoute.append(QPointF(1,1));
    interconnectionRoute.append(QPointF(1,2));
    testHierInterface->setRoute(interconnectionRoute);

    testDesign->getInterconnections()->append(testInterconnection);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:interconnections>\n"
                "\t\t<ipxact:interconnection>\n"
                    "\t\t\t<ipxact:name>testActiveHierActive</ipxact:name>\n"
                    "\t\t\t<ipxact:activeInterface componentRef=\"componentRef\" busRef=\"busRef\"/>\n"
                    "\t\t\t<ipxact:hierInterface busRef=\"hierBusRef\">\n"
                        "\t\t\t\t<ipxact:vendorExtensions>\n"
                            "\t\t\t\t\t<kactus2:route>\n"
                                "\t\t\t\t\t\t<kactus2:position x=\"1\" y=\"1\"/>\n"
                                "\t\t\t\t\t\t<kactus2:position x=\"1\" y=\"2\"/>\n"
                            "\t\t\t\t\t</kactus2:route>\n"
                        "\t\t\t\t</ipxact:vendorExtensions>\n"
                    "\t\t\t</ipxact:hierInterface>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<kactus2:offPage/>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:interconnection>\n"
            "\t</ipxact:interconnections>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteMonitorInterconnections()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteMonitorInterconnections()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<MonitorInterface> testActiveInterface (new MonitorInterface("componentRef", "busRef"));
    testActiveInterface->setPath("/path/to/test");
    testActiveInterface->setDescription("monitoredActiveDescription");
    testActiveInterface->getVendorExtensions()->append(testExtension);

    QSharedPointer<MonitorInterface> testMonitorInterface (new MonitorInterface("otherComponent", "otherBus"));
    testMonitorInterface->setPath("/path/to/other/test");
    testMonitorInterface->setDescription("monitorInterfaceDescription");
    testMonitorInterface->setIsPresent("1");
    testMonitorInterface->getVendorExtensions()->append(testExtension);

    QSharedPointer<MonitorInterconnection> testMonitorInterconnection (new MonitorInterconnection(
        "monitorInterconnection", testActiveInterface, "monitorDisplay", "monitorDescription"));
    testMonitorInterconnection->setIsPresent("2-1");
    testMonitorInterconnection->setMonitoredctiveInterface(testActiveInterface);
    testMonitorInterconnection->getMonitorInterfaces()->append(testMonitorInterface);

    testDesign->getMonitorInterconnecions()->append(testMonitorInterconnection);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:interconnections>\n"
                "\t\t<ipxact:monitorInterconnection>\n"
                    "\t\t\t<ipxact:name>monitorInterconnection</ipxact:name>\n"
                    "\t\t\t<ipxact:displayName>monitorDisplay</ipxact:displayName>\n"
                    "\t\t\t<ipxact:description>monitorDescription</ipxact:description>\n"
                    "\t\t\t<ipxact:isPresent>2-1</ipxact:isPresent>\n"
                    "\t\t\t<ipxact:monitoredActiveInterface componentRef=\"componentRef\" busRef=\"busRef\""
                        " path=\"/path/to/test\">\n"
                        "\t\t\t\t<ipxact:description>monitoredActiveDescription</ipxact:description>\n"
                        "\t\t\t\t<ipxact:vendorExtensions>\n"
                            "\t\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                        "\t\t\t\t</ipxact:vendorExtensions>\n"
                    "\t\t\t</ipxact:monitoredActiveInterface>\n"
                    "\t\t\t<ipxact:monitorInterface componentRef=\"otherComponent\" busRef=\"otherBus\" "
                        "path=\"/path/to/other/test\">\n"
                        "\t\t\t\t<ipxact:description>monitorInterfaceDescription</ipxact:description>\n"
                        "\t\t\t\t<ipxact:vendorExtensions>\n"
                            "\t\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                        "\t\t\t\t</ipxact:vendorExtensions>\n"
                        "\t\t\t\t<ipxact:isPresent>1</ipxact:isPresent>\n"
                    "\t\t\t</ipxact:monitorInterface>\n"
                "\t\t</ipxact:monitorInterconnection>\n"
            "\t</ipxact:interconnections>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}


//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWrite2022MonitorInterconnections()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWrite2022MonitorInterconnections()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std22));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<MonitorInterface> testActiveInterface (new MonitorInterface("componentRef", "busRef"));
    testActiveInterface->setPath("/path/to/test");
    testActiveInterface->setDescription("monitoredActiveDescription");
    testActiveInterface->getVendorExtensions()->append(testExtension);

    QSharedPointer<MonitorInterface> testMonitorInterface (new MonitorInterface("otherComponent", "otherBus"));
    testMonitorInterface->setPath("/path/to/other/test");
    testMonitorInterface->setDescription("monitorInterfaceDescription");
    testMonitorInterface->getVendorExtensions()->append(testExtension);

    QSharedPointer<MonitorInterconnection> testMonitorInterconnection (new MonitorInterconnection(
        "monitorInterconnection", testActiveInterface, "monitorDisplay", "monitorDescription"));
    testMonitorInterconnection->setMonitoredctiveInterface(testActiveInterface);
    testMonitorInterconnection->getMonitorInterfaces()->append(testMonitorInterface);

    testDesign->getMonitorInterconnecions()->append(testMonitorInterconnection);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:interconnections>\n"
                "\t\t<ipxact:monitorInterconnection>\n"
                    "\t\t\t<ipxact:name>monitorInterconnection</ipxact:name>\n"
                    "\t\t\t<ipxact:displayName>monitorDisplay</ipxact:displayName>\n"
                    "\t\t\t<ipxact:description>monitorDescription</ipxact:description>\n"
                    "\t\t\t<ipxact:monitoredActiveInterface componentInstanceRef=\"componentRef\" busRef=\"busRef\""
                        " path=\"/path/to/test\">\n"
                        "\t\t\t\t<ipxact:description>monitoredActiveDescription</ipxact:description>\n"
                        "\t\t\t\t<ipxact:vendorExtensions>\n"
                            "\t\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                        "\t\t\t\t</ipxact:vendorExtensions>\n"
                    "\t\t\t</ipxact:monitoredActiveInterface>\n"
                    "\t\t\t<ipxact:monitorInterface componentInstanceRef=\"otherComponent\" busRef=\"otherBus\" "
                        "path=\"/path/to/other/test\">\n"
                        "\t\t\t\t<ipxact:description>monitorInterfaceDescription</ipxact:description>\n"
                        "\t\t\t\t<ipxact:vendorExtensions>\n"
                            "\t\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                        "\t\t\t\t</ipxact:vendorExtensions>\n"
                    "\t\t\t</ipxact:monitorInterface>\n"
                "\t\t</ipxact:monitorInterconnection>\n"
            "\t</ipxact:interconnections>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteAdHocConnections()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteAdHocConnections()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<PartSelect> internalPartSelect (new PartSelect("1", "11"));
    internalPartSelect->getIndices()->append("8");
    internalPartSelect->getIndices()->append("4+4");

    QSharedPointer<PartSelect> externalPartSelect (new PartSelect("0", "4"));

    QSharedPointer<PortReference> testInternalPortRef (new PortReference("internalPort", "componentInstance"));
    testInternalPortRef->setIsPresent("1");
    testInternalPortRef->setPartSelect(internalPartSelect);

    QSharedPointer<PortReference> testExternalPortRef (new PortReference("externalPort"));
    testExternalPortRef->setIsPresent("0");
    testExternalPortRef->setPartSelect(externalPartSelect);

    QSharedPointer<AdHocConnection> testAdHocConnection (new AdHocConnection("adHoc"));
    testAdHocConnection->setDisplayName("displayAd");
    testAdHocConnection->setDescription("describeAd");
    testAdHocConnection->setTiedValue("default");
    testAdHocConnection->setIsPresent("4-3");
    testAdHocConnection->getInternalPortReferences()->append(testInternalPortRef);
    testAdHocConnection->getExternalPortReferences()->append(testExternalPortRef);
    testAdHocConnection->getVendorExtensions()->append(testExtension);

    testDesign->getAdHocConnections()->append(testAdHocConnection);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:adHocConnections>\n"
                "\t\t<ipxact:adHocConnection>\n"
                    "\t\t\t<ipxact:name>adHoc</ipxact:name>\n"
                    "\t\t\t<ipxact:displayName>displayAd</ipxact:displayName>\n"
                    "\t\t\t<ipxact:description>describeAd</ipxact:description>\n"
                    "\t\t\t<ipxact:isPresent>4-3</ipxact:isPresent>\n"
                    "\t\t\t<ipxact:tiedValue>default</ipxact:tiedValue>\n"
                    "\t\t\t<ipxact:portReferences>\n"
                        "\t\t\t\t<ipxact:internalPortReference componentRef=\"componentInstance\""
                                " portRef=\"internalPort\">\n"
                            "\t\t\t\t\t<ipxact:isPresent>1</ipxact:isPresent>\n"
                            "\t\t\t\t\t<ipxact:partSelect>\n"
                                "\t\t\t\t\t\t<ipxact:range>\n"
                                    "\t\t\t\t\t\t\t<ipxact:left>1</ipxact:left>\n"
                                    "\t\t\t\t\t\t\t<ipxact:right>11</ipxact:right>\n"
                                "\t\t\t\t\t\t</ipxact:range>\n"
                                "\t\t\t\t\t\t<ipxact:indices>\n"
                                    "\t\t\t\t\t\t\t<ipxact:index>8</ipxact:index>\n"
                                    "\t\t\t\t\t\t\t<ipxact:index>4+4</ipxact:index>\n"
                                "\t\t\t\t\t\t</ipxact:indices>\n"
                            "\t\t\t\t\t</ipxact:partSelect>\n"
                        "\t\t\t\t</ipxact:internalPortReference>\n"
                        "\t\t\t\t<ipxact:externalPortReference portRef=\"externalPort\">\n"
                            "\t\t\t\t\t<ipxact:isPresent>0</ipxact:isPresent>\n"
                            "\t\t\t\t\t<ipxact:partSelect>\n"
                                "\t\t\t\t\t\t<ipxact:range>\n"
                                    "\t\t\t\t\t\t\t<ipxact:left>0</ipxact:left>\n"
                                    "\t\t\t\t\t\t\t<ipxact:right>4</ipxact:right>\n"
                                "\t\t\t\t\t\t</ipxact:range>\n"
                            "\t\t\t\t\t</ipxact:partSelect>\n"
                        "\t\t\t\t</ipxact:externalPortReference>\n"
                    "\t\t\t</ipxact:portReferences>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:adHocConnection>\n"
            "\t</ipxact:adHocConnections>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteAdHocConnections()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWrite2022AdHocConnections()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std22));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<PartSelect> internalPartSelect (new PartSelect("1", "11"));
    internalPartSelect->getIndices()->append("8");
    internalPartSelect->getIndices()->append("4+4");

    QSharedPointer<PartSelect> externalPartSelect (new PartSelect("0", "4"));

    QSharedPointer<PortReference> testInternalPortRef (new PortReference("internalPort", "componentInstance"));
    testInternalPortRef->setPartSelect(internalPartSelect);

    QSharedPointer<PortReference> testExternalPortRef (new PortReference("externalPort"));
    testExternalPortRef->setPartSelect(externalPartSelect);

    QSharedPointer<AdHocConnection> testAdHocConnection(new AdHocConnection("adHoc"));
    testAdHocConnection->setDisplayName("displayAd");
    testAdHocConnection->setShortDescription("shortAd");
    testAdHocConnection->setDescription("describeAd");
    testAdHocConnection->setTiedValue("default");
    testAdHocConnection->getInternalPortReferences()->append(testInternalPortRef);
    testAdHocConnection->getExternalPortReferences()->append(testExternalPortRef);
    testAdHocConnection->getVendorExtensions()->append(testExtension);

    testDesign->getAdHocConnections()->append(testAdHocConnection);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:adHocConnections>\n"
                "\t\t<ipxact:adHocConnection>\n"
                    "\t\t\t<ipxact:name>adHoc</ipxact:name>\n"
                    "\t\t\t<ipxact:displayName>displayAd</ipxact:displayName>\n"
                    "\t\t\t<ipxact:shortDescription>shortAd</ipxact:shortDescription>\n"
                    "\t\t\t<ipxact:description>describeAd</ipxact:description>\n"
                    "\t\t\t<ipxact:tiedValue>default</ipxact:tiedValue>\n"
                    "\t\t\t<ipxact:portReferences>\n"
                        "\t\t\t\t<ipxact:internalPortReference componentInstanceRef=\"componentInstance\""
                                " portRef=\"internalPort\">\n"
                            "\t\t\t\t\t<ipxact:partSelect>\n"
                                "\t\t\t\t\t\t<ipxact:range>\n"
                                    "\t\t\t\t\t\t\t<ipxact:left>1</ipxact:left>\n"
                                    "\t\t\t\t\t\t\t<ipxact:right>11</ipxact:right>\n"
                                "\t\t\t\t\t\t</ipxact:range>\n"
                                "\t\t\t\t\t\t<ipxact:indices>\n"
                                    "\t\t\t\t\t\t\t<ipxact:index>8</ipxact:index>\n"
                                    "\t\t\t\t\t\t\t<ipxact:index>4+4</ipxact:index>\n"
                                "\t\t\t\t\t\t</ipxact:indices>\n"
                            "\t\t\t\t\t</ipxact:partSelect>\n"
                        "\t\t\t\t</ipxact:internalPortReference>\n"
                        "\t\t\t\t<ipxact:externalPortReference portRef=\"externalPort\">\n"
                            "\t\t\t\t\t<ipxact:partSelect>\n"
                                "\t\t\t\t\t\t<ipxact:range>\n"
                                    "\t\t\t\t\t\t\t<ipxact:left>0</ipxact:left>\n"
                                    "\t\t\t\t\t\t\t<ipxact:right>4</ipxact:right>\n"
                                "\t\t\t\t\t\t</ipxact:range>\n"
                            "\t\t\t\t\t</ipxact:partSelect>\n"
                        "\t\t\t\t</ipxact:externalPortReference>\n"
                    "\t\t\t</ipxact:portReferences>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:adHocConnection>\n"
            "\t</ipxact:adHocConnections>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWrite2022SubPortReferences()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWrite2022SubPortReferences()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std22));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<PartSelect> internalPartSelect (new PartSelect("1", "11"));
    internalPartSelect->getIndices()->append("8");
    internalPartSelect->getIndices()->append("4+4");

    QSharedPointer<PartSelect> externalPartSelect (new PartSelect("0", "4"));

    QSharedPointer<PortReference> testInternalPortRef (new PortReference("internalPort", "componentInstance"));
    testInternalPortRef->setPartSelect(internalPartSelect);

    QSharedPointer<PortReference> subPortRef(new PortReference("subPort"));
    testInternalPortRef->getSubPortReferences()->append(subPortRef);

    QSharedPointer<PortReference> complexSubPortRef(new PortReference("complexPort"));

    QSharedPointer<PartSelect> subPartSelect(new PartSelect("1", "0"));
    complexSubPortRef->setPartSelect(subPartSelect);
    testInternalPortRef->getSubPortReferences()->append(complexSubPortRef);

    QSharedPointer<PortReference> testExternalPortRef (new PortReference("externalPort"));
    testExternalPortRef->setPartSelect(externalPartSelect);

    QSharedPointer<AdHocConnection> testAdHocConnection(new AdHocConnection("adHoc"));

    testAdHocConnection->getInternalPortReferences()->append(testInternalPortRef);
    testAdHocConnection->getExternalPortReferences()->append(testExternalPortRef);
    testAdHocConnection->getVendorExtensions()->append(testExtension);

    testDesign->getAdHocConnections()->append(testAdHocConnection);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2022 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2022/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:adHocConnections>\n"
                "\t\t<ipxact:adHocConnection>\n"
                    "\t\t\t<ipxact:name>adHoc</ipxact:name>\n"
                    "\t\t\t<ipxact:portReferences>\n"
                        "\t\t\t\t<ipxact:internalPortReference componentInstanceRef=\"componentInstance\""
                                " portRef=\"internalPort\">\n"
                            "\t\t\t\t\t<ipxact:subPortReference subPortRef=\"subPort\"/>\n"
                            "\t\t\t\t\t<ipxact:subPortReference subPortRef=\"complexPort\">\n"
                                "\t\t\t\t\t\t<ipxact:partSelect>\n"
                                "\t\t\t\t\t\t\t<ipxact:range>\n"
                                    "\t\t\t\t\t\t\t\t<ipxact:left>1</ipxact:left>\n"
                                    "\t\t\t\t\t\t\t\t<ipxact:right>0</ipxact:right>\n"
                                "\t\t\t\t\t\t\t</ipxact:range>\n"
                            "\t\t\t\t\t\t</ipxact:partSelect>\n"
                            "\t\t\t\t\t</ipxact:subPortReference>\n"
                            "\t\t\t\t\t<ipxact:partSelect>\n"
                                "\t\t\t\t\t\t<ipxact:range>\n"
                                    "\t\t\t\t\t\t\t<ipxact:left>1</ipxact:left>\n"
                                    "\t\t\t\t\t\t\t<ipxact:right>11</ipxact:right>\n"
                                "\t\t\t\t\t\t</ipxact:range>\n"
                                "\t\t\t\t\t\t<ipxact:indices>\n"
                                    "\t\t\t\t\t\t\t<ipxact:index>8</ipxact:index>\n"
                                    "\t\t\t\t\t\t\t<ipxact:index>4+4</ipxact:index>\n"
                                "\t\t\t\t\t\t</ipxact:indices>\n"
                            "\t\t\t\t\t</ipxact:partSelect>\n"
                        "\t\t\t\t</ipxact:internalPortReference>\n"
                        "\t\t\t\t<ipxact:externalPortReference portRef=\"externalPort\">\n"
                            "\t\t\t\t\t<ipxact:partSelect>\n"
                                "\t\t\t\t\t\t<ipxact:range>\n"
                                    "\t\t\t\t\t\t\t<ipxact:left>0</ipxact:left>\n"
                                    "\t\t\t\t\t\t\t<ipxact:right>4</ipxact:right>\n"
                                "\t\t\t\t\t\t</ipxact:range>\n"
                            "\t\t\t\t\t</ipxact:partSelect>\n"
                        "\t\t\t\t</ipxact:externalPortReference>\n"
                    "\t\t\t</ipxact:portReferences>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:adHocConnection>\n"
            "\t</ipxact:adHocConnections>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteAdHocConnectionExtensions()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteAdHocConnectionExtensions()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    QSharedPointer<PortReference> testInternalPortRef (new PortReference("internalPort", "componentInstance"));

    QSharedPointer<PortReference> testExternalPortRef (new PortReference("externalPort"));

    QSharedPointer<AdHocConnection> testAdHocConnection (new AdHocConnection("adHoc"));
    testAdHocConnection->getInternalPortReferences()->append(testInternalPortRef);
    testAdHocConnection->getExternalPortReferences()->append(testExternalPortRef);
    testAdHocConnection->setOffPage(true);

    QList<QPointF> adHocRoute;
    adHocRoute.append(QPointF(1,1));
    adHocRoute.append(QPointF(4,1));
    adHocRoute.append(QPointF(4,2));
    testAdHocConnection->setRoute(adHocRoute);

    testDesign->getAdHocConnections()->append(testAdHocConnection);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:adHocConnections>\n"
                "\t\t<ipxact:adHocConnection>\n"
                    "\t\t\t<ipxact:name>adHoc</ipxact:name>\n"
                    "\t\t\t<ipxact:portReferences>\n"
                        "\t\t\t\t<ipxact:internalPortReference componentRef=\"componentInstance\""
                            " portRef=\"internalPort\"/>\n"
                        "\t\t\t\t<ipxact:externalPortReference portRef=\"externalPort\"/>\n"
                    "\t\t\t</ipxact:portReferences>\n"
                    "\t\t\t<ipxact:vendorExtensions>\n"
                        "\t\t\t\t<kactus2:offPage/>\n"
                        "\t\t\t\t<kactus2:route>\n"
                            "\t\t\t\t\t<kactus2:position x=\"1\" y=\"1\"/>\n"
                            "\t\t\t\t\t<kactus2:position x=\"4\" y=\"1\"/>\n"
                            "\t\t\t\t\t<kactus2:position x=\"4\" y=\"2\"/>\n"
                        "\t\t\t\t</kactus2:route>\n"
                    "\t\t\t</ipxact:vendorExtensions>\n"
                "\t\t</ipxact:adHocConnection>\n"
            "\t</ipxact:adHocConnections>\n"
        "</ipxact:design>\n"
        );

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteParameters()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteParameters()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    testDesign->getParameters()->append(testParameter);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
            "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
            "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
            "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
            "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:parameters>\n"
                "\t\t<ipxact:parameter parameterId=\"testID\">\n"
                "\t\t\t<ipxact:name>testParameter</ipxact:name>\n"
                "\t\t\t<ipxact:value>1</ipxact:value>\n"
                "\t\t</ipxact:parameter>\n"
            "\t</ipxact:parameters>\n"
        "</ipxact:design>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteAssertions()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteAssertions()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QSharedPointer<Assertion> testAssertion(new Assertion());
    testAssertion->setName("testAssertion");
    testAssertion->setDisplayName("assertionDisplay");
    testAssertion->setDescription("assertionDescription");
    testAssertion->setAssert("13");

    testDesign->getAssertions()->append(testAssertion);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
            "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
            "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
            "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
            "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:assertions>\n"
                "\t\t<ipxact:assertion>\n"
                    "\t\t\t<ipxact:name>testAssertion</ipxact:name>\n"
                    "\t\t\t<ipxact:displayName>assertionDisplay</ipxact:displayName>\n"
                    "\t\t\t<ipxact:description>assertionDescription</ipxact:description>\n"
                    "\t\t\t<ipxact:assert>13</ipxact:assert>\n"
                "\t\t</ipxact:assertion>\n"
            "\t</ipxact:assertions>\n"
        "</ipxact:design>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteVendorExtensions()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteVendorExtensions()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testDesign->getVendorExtensions()->append(testExtension);
    testDesign->setVersion("3.0.0");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
            "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
            "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
            "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
            "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
            "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>\n"
                "\t\t<kactus2:version>3.0.0</kactus2:version>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:design>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteColumns()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteColumns()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QSharedPointer<ColumnDesc> testColumn (new ColumnDesc("testColumn", ColumnTypes::COMPONENTS));
    QSharedPointer<ColumnDesc> otherColumn (new ColumnDesc("otherColumn", ColumnTypes::BUSES));

    QSharedPointer<Kactus2Group> columnLayout(new Kactus2Group("kactus2:columnLayout"));
    columnLayout->addToGroup(testColumn);
    columnLayout->addToGroup(otherColumn);

    testDesign->getVendorExtensions()->append(columnLayout);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:columnLayout>\n"
                    "\t\t\t<kactus2:column name=\"testColumn\" contentType=\"2\" allowedItems=\"0\" "
                        "minWidth=\"259\" width=\"259\" x=\"0\"/>\n"
                    "\t\t\t<kactus2:column name=\"otherColumn\" contentType=\"1\" allowedItems=\"0\" "
                        "minWidth=\"259\" width=\"259\" x=\"0\"/>\n"
                "\t\t</kactus2:columnLayout>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:design>\n"
        );

    compareOutputToExpected(output, expectedOutput);

    output.clear();

    testDesign->removeColumn(otherColumn);

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    expectedOutput =
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:columnLayout>\n"
                    "\t\t\t<kactus2:column name=\"testColumn\" contentType=\"2\" allowedItems=\"0\" "
                        "minWidth=\"259\" width=\"259\" x=\"0\"/>\n"
                "\t\t</kactus2:columnLayout>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:design>\n"
        ;

    compareOutputToExpected(output, expectedOutput);

    output.clear();

    QSharedPointer<Design> otherDesign (new Design(*testDesign));

    DesignWriter::writeDesign(xmlStreamWriter, otherDesign);
    compareOutputToExpected(output, expectedOutput);

    output.clear();
    QSharedPointer<Design> newDesign (new Design(VLNV(), Document::Revision::Std14));
    newDesign = otherDesign;

    DesignWriter::writeDesign(xmlStreamWriter, otherDesign);
    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWritePortAdHocVisibilitiesAndPositions()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWritePortAdHocVisibilitiesAndPositions()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QPointF portPosition;
    portPosition.setX(4);
    portPosition.setY(25);

    QMap<QString, QPointF> adHocPositions;
    adHocPositions.insert("testPort", portPosition);
    testDesign->setAdHocPortPositions(adHocPositions);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:adHocVisibilities>\n"
                    "\t\t\t<kactus2:adHocVisible portName=\"testPort\" x=\"4\" y=\"25\"/>\n"
                "\t\t</kactus2:adHocVisibilities>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:design>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteApiDependencies()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteApiDependencies()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QSharedPointer<ActiveInterface> testStartInterface(new ActiveInterface("applicationOne", "busOne"));
    QSharedPointer<ActiveInterface> testEndInterface(new ActiveInterface("applicationTwo", "busTwo"));

    QPointF pointOne;
    pointOne.setX(1);
    pointOne.setY(1);
    QList<QPointF> points;
    points.append(pointOne);

    QSharedPointer<ApiInterconnection> testApiConnection(new ApiInterconnection("apiConnect", "connection",
        "described", testStartInterface, testEndInterface));
    
    QList<QSharedPointer<ApiInterconnection> > apiConnections;
    apiConnections.append(testApiConnection);

    testDesign->setApiConnections(apiConnections);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:apiConnections>\n"
                    "\t\t\t<kactus2:apiConnection>\n"
                        "\t\t\t\t<ipxact:name>apiConnect</ipxact:name>\n"
                        "\t\t\t\t<ipxact:displayName>connection</ipxact:displayName>\n"
                        "\t\t\t\t<ipxact:description>described</ipxact:description>\n"
                        "\t\t\t\t<kactus2:activeApiInterface componentRef=\"applicationOne\" apiRef=\"busOne\"/>\n"
                        "\t\t\t\t<kactus2:activeApiInterface componentRef=\"applicationTwo\" apiRef=\"busTwo\"/>\n"
                        /*"\t\t\t\t<kactus2:route offPage=\"false\">\n"
                            "\t\t\t\t\t<kactus2:position x=\"1\" y=\"1\"/>\n"
                        "\t\t\t\t</kactus2:route>\n"*/
                    "\t\t\t</kactus2:apiConnection>\n"
                "\t\t</kactus2:apiConnections>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:design>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteHierApiDependencies()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteHierApiDependencies()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QSharedPointer<ActiveInterface> testInterface(new ActiveInterface("applicationOne", "busOne"));

    QSharedPointer<HierInterface> topInterface(new HierInterface("topInterface"));
    
    QSharedPointer<ApiInterconnection> testHierApi(new ApiInterconnection("hierApi", "display", "description", 
        topInterface, testInterface));
    
    QList<QSharedPointer<ApiInterconnection> > hierApiList;
    hierApiList.append(testHierApi);

    testDesign->setApiConnections(hierApiList);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:apiConnections>\n"
                    "\t\t\t<kactus2:apiConnection>\n"
                        "\t\t\t\t<ipxact:name>hierApi</ipxact:name>\n"
                        "\t\t\t\t<ipxact:displayName>display</ipxact:displayName>\n"
                        "\t\t\t\t<ipxact:description>description</ipxact:description>\n"
                        "\t\t\t\t<kactus2:activeApiInterface componentRef=\"applicationOne\" apiRef=\"busOne\"/>\n"
                        "\t\t\t\t<kactus2:hierApiInterface apiRef=\"topInterface\"/>\n"
                    "\t\t\t</kactus2:apiConnection>\n"
                "\t\t</kactus2:apiConnections>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:design>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteComConnections()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteComConnections()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QSharedPointer<ActiveInterface> testStartInterface(new ActiveInterface("applicationOne", "busOne"));
    QSharedPointer<ActiveInterface> testEndInterface(new ActiveInterface("applicationTwo", "busTwo"));

    QSharedPointer<ComInterconnection> testComConnection(new ComInterconnection("comConnect", "display",
        "description", testStartInterface, testEndInterface));

    QList<QSharedPointer<ComInterconnection> > comConnectionList;
    comConnectionList.append(testComConnection);

    testDesign->setComConnections(comConnectionList);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:comConnections>\n"
                    "\t\t\t<kactus2:comConnection>\n"
                        "\t\t\t\t<ipxact:name>comConnect</ipxact:name>\n"
                        "\t\t\t\t<ipxact:displayName>display</ipxact:displayName>\n"
                        "\t\t\t\t<ipxact:description>description</ipxact:description>\n"
                        "\t\t\t\t<kactus2:activeComInterface componentRef=\"applicationOne\" comRef=\"busOne\"/>\n"
                        "\t\t\t\t<kactus2:activeComInterface componentRef=\"applicationTwo\" comRef=\"busTwo\"/>\n"
                    "\t\t\t</kactus2:comConnection>\n"
                "\t\t</kactus2:comConnections>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:design>"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::testWriteHierComConnections()
//-----------------------------------------------------------------------------
void tst_DesignWriter::testWriteHierComConnections()
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    QSharedPointer<ActiveInterface> testInterface(new ActiveInterface("applicationOne", "busOne"));

    QSharedPointer<HierInterface> topInterface(new HierInterface("topInterface"));

    QSharedPointer<ComInterconnection> testHierComConnection(new ComInterconnection("hierComConnection",
        "display", "description", topInterface, testInterface));

    QList<QSharedPointer<ComInterconnection> > hierComList;
    hierComList.append(testHierComConnection);

    testDesign->setComConnections(hierComList);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    xmlStreamWriter.setAutoFormatting(true);
    xmlStreamWriter.setAutoFormattingIndent(-1);

    DesignWriter::writeDesign(xmlStreamWriter, testDesign);

    QString expectedOutput(
        "<?xml version=\"1.0\"?>\n"
        "<ipxact:design "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014 "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">\n"
            "\t<ipxact:vendor>tuni.fi</ipxact:vendor>\n"
            "\t<ipxact:library>TestLibrary</ipxact:library>\n"
            "\t<ipxact:name>TestDesign</ipxact:name>\n"
            "\t<ipxact:version>0.1</ipxact:version>\n"
            "\t<ipxact:vendorExtensions>\n"
                "\t\t<kactus2:comConnections>\n"
                    "\t\t\t<kactus2:comConnection>\n"
                        "\t\t\t\t<ipxact:name>hierComConnection</ipxact:name>\n"
                        "\t\t\t\t<ipxact:displayName>display</ipxact:displayName>\n"
                        "\t\t\t\t<ipxact:description>description</ipxact:description>\n"
                        "\t\t\t\t<kactus2:activeComInterface componentRef=\"applicationOne\" comRef=\"busOne\"/>\n"
                        "\t\t\t\t<kactus2:hierComInterface comRef=\"topInterface\"/>\n"
                    "\t\t\t</kactus2:comConnection>\n"
                "\t\t</kactus2:comConnections>\n"
            "\t</ipxact:vendorExtensions>\n"
        "</ipxact:design>\n"
        );

    compareOutputToExpected(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_DesignWriter::compareOutputToExpected()
//-----------------------------------------------------------------------------
void tst_DesignWriter::compareOutputToExpected(QString const& output, QString const& expectedOutput)
{
    VLNV designVLNV(VLNV::DESIGN, "tuni.fi", "TestLibrary", "TestDesign", "0.1");
    auto testDesign = QSharedPointer<Design>(new Design(designVLNV, Document::Revision::Std14));

    if (!output.contains(expectedOutput))
    {
        QStringList outputLines = output.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());
        if (lineOffset == -1)
        {
            QCOMPARE(output, expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; ++i)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if (output.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output.count(expectedOutput)) +
            " times in output.").toLocal8Bit());
    }
}

QTEST_APPLESS_MAIN(tst_DesignWriter)

#include "tst_DesignWriter.moc"

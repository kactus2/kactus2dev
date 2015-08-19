//-----------------------------------------------------------------------------
// File: tst_AbstractionDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Unit test for class AbstractionDefinitionWriter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/GenericVendorExtension.h>

#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Qualifier.h>
#include <IPXACTmodels/common/CellSpecification.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinitionWriter.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TimingConstraint.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>

class tst_AbstractionDefinition : public QObject
{
    Q_OBJECT

public:
    tst_AbstractionDefinition();

private slots:
    void testWriteAbstractionDefinitionWithoutPorts();
    void testTopCommentsAreWritten();
    void testWriteExtendingAbstractionDefinition();

    void testWriteSimpleWirePort();
    void testWriteWirePortForAllModes();
    void testWriteMultipleSystemWirePorts();

    void testWireConstraints();
    void testWireMirroredConstraints();

    void testWriteParameters();
    void testWriteAssertions();
    void testVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionWriter::tst_AbstractionDefinition()
//-----------------------------------------------------------------------------
tst_AbstractionDefinition::tst_AbstractionDefinition()
{
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionWriter::testWriteAbstractionDefinitionWithoutPorts()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testWriteAbstractionDefinitionWithoutPorts()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV vlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "MinimalAbsDef", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(vlnv);
    abstractionDefinition->setBusType(targetBus);
    abstractionDefinition->setDescription("This is a description");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter absDefWriter;

    absDefWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalAbsDef</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
            "<ipxact:description>This is a description</ipxact:description>"
        "</ipxact:abstractionDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinition::testTopCommentsAreWritten()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testTopCommentsAreWritten()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV vlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "MinimalAbsDef", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(vlnv);
    abstractionDefinition->setBusType(targetBus);
    abstractionDefinition->setTopComments("Commented section");

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter absDefWriter;

    absDefWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<!--Commented section-->"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalAbsDef</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
        "</ipxact:abstractionDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionWriter::testWriteExtendingAbstractionDefinition()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testWriteExtendingAbstractionDefinition()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV extendedVlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "extended", "1.0");

    VLNV extendingVlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "extending", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(extendingVlnv);
    abstractionDefinition->setBusType(targetBus);
    abstractionDefinition->setExtends(extendedVlnv);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter busWriter;

    busWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>extending</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
            "<ipxact:extends vendor=\"TUT\" library=\"TestLibrary\" name=\"extended\" version=\"1.0\"/>"
        "</ipxact:abstractionDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionWriter::testWriteSimpleWirePort()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testWriteSimpleWirePort()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV extendingVlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "TestAbsDef", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(extendingVlnv);
    abstractionDefinition->setBusType(targetBus);
    
    QSharedPointer<PortAbstraction> testPort(new PortAbstraction());
    testPort->setName("testPort");
    testPort->setDisplayName("test wire port");
    testPort->setDescription("This is a description for testPort");
    testPort->setIsPresent("1");
    
    testPort->setDefaultValue("0");
    testPort->setQualifier(Qualifier::Data);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("vendorAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > portExtensions = testPort->getVendorExtensions();
    portExtensions->append(testExtension);

    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports = abstractionDefinition->getLogicalPorts();
    ports->append(testPort);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter busWriter;

    busWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestAbsDef</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
            "<ipxact:ports>"
                "<ipxact:port>"
                    "<ipxact:isPresent>1</ipxact:isPresent>"
                    "<ipxact:logicalName>testPort</ipxact:logicalName>"
                    "<ipxact:displayName>test wire port</ipxact:displayName>"
                    "<ipxact:description>This is a description for testPort</ipxact:description>"
                    "<ipxact:wire>"
                        "<ipxact:qualifier>"
                            "<ipxact:isData>true</ipxact:isData>"
                        "</ipxact:qualifier>"
                        "<ipxact:defaultValue>0</ipxact:defaultValue>"
                    "</ipxact:wire>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinition::testWriteWirePortForAllModes()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testWriteWirePortForAllModes()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV extendingVlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "TestAbsDef", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(extendingVlnv);
    abstractionDefinition->setBusType(targetBus);

    QSharedPointer<PortAbstraction> testPort(new PortAbstraction());
    testPort->setName("reset");
    
    QSharedPointer<WireAbstraction> wire(new WireAbstraction());
    wire->setQualifier(Qualifier::Reset);
    wire->setRequiresDriver(true);
    wire->setDriverType(General::SINGLESHOT);
    testPort->setWire(wire);

    QSharedPointer<WirePort> systemPort(new WirePort);
    systemPort->setPresence(General::REQUIRED);
    systemPort->setWidth("1");
    systemPort->setDirection(General::INOUT);

    QSharedPointer<WirePort> masterPort(new WirePort);
    masterPort->setPresence(General::OPTIONAL);
    masterPort->setWidth("expression");
    masterPort->setDirection(General::IN);

    QSharedPointer<WirePort> slavePort(new WirePort);
    slavePort->setPresence(General::ILLEGAL);
    slavePort->setDirection(General::OUT);

    testPort->getWire()->addSystemPort(systemPort);
    testPort->getWire()->setMasterPort(masterPort);
    testPort->getWire()->setSlavePort(slavePort);

    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports = abstractionDefinition->getLogicalPorts();
    ports->append(testPort);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter busWriter;

    busWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestAbsDef</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
            "<ipxact:ports>"
                "<ipxact:port>"                
                    "<ipxact:logicalName>reset</ipxact:logicalName>"
                    "<ipxact:wire>"
                        "<ipxact:qualifier>"
                            "<ipxact:isReset>true</ipxact:isReset>"
                        "</ipxact:qualifier>"
                        "<ipxact:onSystem>"
                            "<ipxact:presence>required</ipxact:presence>"
                            "<ipxact:width>1</ipxact:width>"
                            "<ipxact:direction>inout</ipxact:direction>"
                        "</ipxact:onSystem>"
                        "<ipxact:onMaster>"
                            "<ipxact:presence>optional</ipxact:presence>"
                            "<ipxact:width>expression</ipxact:width>"
                            "<ipxact:direction>in</ipxact:direction>"
                        "</ipxact:onMaster>"
                        "<ipxact:onSlave>"
                            "<ipxact:presence>illegal</ipxact:presence>"
                            "<ipxact:direction>out</ipxact:direction>"
                        "</ipxact:onSlave>"
                        "<ipxact:requiresDriver driverType=\"singleshot\">true</ipxact:requiresDriver>"
                    "</ipxact:wire>"
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinition::testWriteMultipleSystemWirePorts()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testWriteMultipleSystemWirePorts()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV extendingVlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "TestAbsDef", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(extendingVlnv);
    abstractionDefinition->setBusType(targetBus);

    QSharedPointer<PortAbstraction> testPort(new PortAbstraction());
    testPort->setName("systemPorts");
    testPort->setRequiresDriver(false);

    QSharedPointer<WirePort> systemPort(new WirePort);
    systemPort->setPresence(General::REQUIRED);
    systemPort->setWidth("1");
    systemPort->setDirection(General::INOUT);

    QSharedPointer<WirePort> secondSystemPort(new WirePort);
    secondSystemPort->setPresence(General::OPTIONAL);
    secondSystemPort->setWidth("16");
    secondSystemPort->setDirection(General::INOUT);

    testPort->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));
    testPort->getWire()->addSystemPort(systemPort);
    testPort->getWire()->addSystemPort(secondSystemPort);

    QSharedPointer<QList<QSharedPointer<PortAbstraction> > > ports = abstractionDefinition->getLogicalPorts();
    ports->append(testPort);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter busWriter;

    busWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestAbsDef</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
            "<ipxact:ports>"
                "<ipxact:port>"                
                    "<ipxact:logicalName>systemPorts</ipxact:logicalName>"
                    "<ipxact:wire>"
                        "<ipxact:onSystem>"
                            "<ipxact:presence>required</ipxact:presence>"
                            "<ipxact:width>1</ipxact:width>"
                            "<ipxact:direction>inout</ipxact:direction>"
                        "</ipxact:onSystem>" 
                        "<ipxact:onSystem>"
                            "<ipxact:presence>optional</ipxact:presence>"
                            "<ipxact:width>16</ipxact:width>"
                            "<ipxact:direction>inout</ipxact:direction>"
                        "</ipxact:onSystem>" 
                        "<ipxact:requiresDriver>false</ipxact:requiresDriver>"
                    "</ipxact:wire>"
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinition::testWireConstraints()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testWireConstraints()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV extendingVlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "TestAbsDef", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(extendingVlnv);
    abstractionDefinition->setBusType(targetBus);

    QSharedPointer<PortAbstraction> testPort(new PortAbstraction());
    testPort->setName("constrainedPort");

    QSharedPointer<TimingConstraint> timeConstraint(new TimingConstraint());
    timeConstraint->setValue(80.0);
    timeConstraint->setClockEdge(TimingConstraint::FALL);
    timeConstraint->setDelayType(TimingConstraint::MINIMUM_DELAY);
    timeConstraint->setClockName("clk");

    QSharedPointer<CellSpecification> driveConstraint(new CellSpecification());
    driveConstraint->setCellClass(CellSpecification::COMBINATORIAL);

    QSharedPointer<CellSpecification> loadConstraint(new CellSpecification());
    loadConstraint->setCellStrength(CellSpecification::LOW);
    loadConstraint->setCellFunction("inv");

    QSharedPointer<WirePort> masterPort(new WirePort);
    masterPort->setTimingConstraint(timeConstraint);
    masterPort->setDriveConstraint(driveConstraint);
    masterPort->setLoadConstraint(loadConstraint);

    testPort->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));
    testPort->getWire()->setMasterPort(masterPort);

    abstractionDefinition->getLogicalPorts()->append(testPort);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter busWriter;

    busWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestAbsDef</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
            "<ipxact:ports>"
                "<ipxact:port>"                
                    "<ipxact:logicalName>constrainedPort</ipxact:logicalName>"
                    "<ipxact:wire>"
                        "<ipxact:onMaster>"
                            "<ipxact:modeConstraints>"
                                "<ipxact:timingConstraint clockEdge=\"fall\" delayType=\"min\" clockName=\"clk\">80</ipxact:timingConstraint>"
                                "<ipxact:driveConstraint>"
                                    "<ipxact:cellSpecification>"
                                        "<ipxact:cellClass>combinatorial</ipxact:cellClass>"
                                    "</ipxact:cellSpecification>"
                                "</ipxact:driveConstraint>"
                                "<ipxact:loadConstraint>"
                                    "<ipxact:cellSpecification cellStrength=\"low\">"
                                        "<ipxact:cellFunction>inv</ipxact:cellFunction>"
                                    "</ipxact:cellSpecification>"
                                "</ipxact:loadConstraint>"
                            "</ipxact:modeConstraints>"
                        "</ipxact:onMaster>"
                        "<ipxact:requiresDriver>false</ipxact:requiresDriver>"
                    "</ipxact:wire>"
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinition::testWireMirroredConstraints()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testWireMirroredConstraints()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV extendingVlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "TestAbsDef", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(extendingVlnv);
    abstractionDefinition->setBusType(targetBus);

    QSharedPointer<PortAbstraction> testPort(new PortAbstraction());
    testPort->setName("constrainedPort");

    QSharedPointer<TimingConstraint> timeConstraint(new TimingConstraint());
    timeConstraint->setValue(45.6);
    timeConstraint->setClockEdge(TimingConstraint::RISE);
    timeConstraint->setDelayType(TimingConstraint::MAXIMUM_DELAY);
    timeConstraint->setClockName("clk");

    QSharedPointer<CellSpecification> driveConstraint(new CellSpecification());
    driveConstraint->setCellClass(CellSpecification::SEQUENTIAL);

    QSharedPointer<CellSpecification> loadConstraint(new CellSpecification());
    loadConstraint->setCellStrength(CellSpecification::HIGH);
    loadConstraint->setCellFunction("customFunction");

    QSharedPointer<WirePort> masterPort(new WirePort);
    masterPort->setMirroredTimingConstraint(timeConstraint);
    masterPort->setMirroredDriveConstraint(driveConstraint);
    masterPort->setMirroredLoadConstraint(loadConstraint);

    testPort->setWire(QSharedPointer<WireAbstraction>(new WireAbstraction()));
    testPort->getWire()->setMasterPort(masterPort);

    abstractionDefinition->getLogicalPorts()->append(testPort);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter busWriter;

    busWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestAbsDef</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
            "<ipxact:ports>"
                "<ipxact:port>"                
                    "<ipxact:logicalName>constrainedPort</ipxact:logicalName>"
                    "<ipxact:wire>"
                        "<ipxact:onMaster>"
                        "<ipxact:mirroredModeConstraints>"
                            "<ipxact:timingConstraint clockEdge=\"rise\" delayType=\"max\" clockName=\"clk\">45.6</ipxact:timingConstraint>"
                            "<ipxact:driveConstraint>"
                                "<ipxact:cellSpecification>"
                                    "<ipxact:cellClass>sequential</ipxact:cellClass>"
                                "</ipxact:cellSpecification>"
                            "</ipxact:driveConstraint>"
                            "<ipxact:loadConstraint>"
                                "<ipxact:cellSpecification cellStrength=\"high\">"
                                    "<ipxact:cellFunction other=\"customFunction\">other</ipxact:cellFunction>"
                                "</ipxact:cellSpecification>"
                            "</ipxact:loadConstraint>"
                        "</ipxact:mirroredModeConstraints>"
                    "</ipxact:onMaster>"
                    "<ipxact:requiresDriver>false</ipxact:requiresDriver>"
                "</ipxact:wire>"
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinition::testWriteParameters()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testWriteParameters()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV vlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "TestBus", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(vlnv);
    abstractionDefinition->setBusType(targetBus);

    QSharedPointer<Parameter> firstParameter(new Parameter());
    firstParameter->setName("parameter1");
    firstParameter->setValueId("");
    firstParameter->setValue("1");

    QSharedPointer<Parameter> secondParameter(new Parameter());
    secondParameter->setName("parameter2");
    secondParameter->setValueId("");
    secondParameter->setValue("text");

    QSharedPointer<QList<QSharedPointer<Parameter> > > busParameters = abstractionDefinition->getParameters();
    busParameters->append(firstParameter);
    busParameters->append(secondParameter);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter busWriter;

    busWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
            "<ipxact:parameters>"
                "<ipxact:parameter>"
                    "<ipxact:name>parameter1</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"
                "<ipxact:parameter>"
                    "<ipxact:name>parameter2</ipxact:name>"
                    "<ipxact:value>text</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:abstractionDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinition::testWriteAssertions()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testWriteAssertions()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV vlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "TestBus", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(vlnv);
    abstractionDefinition->setBusType(targetBus);

    QSharedPointer<Assertion> firstTestAssertion(new Assertion());
    firstTestAssertion->setName("testAssertion1");
    firstTestAssertion->setDisplayName("Display name for assertion.");
    firstTestAssertion->setDescription("Description for assertion.");
    firstTestAssertion->setAssert("1");

    QSharedPointer<Assertion> secondTestAssertion(new Assertion());
    secondTestAssertion->setName("testAssertion2");
    secondTestAssertion->setAssert("0");

    abstractionDefinition->getAssertions()->append(firstTestAssertion);
    abstractionDefinition->getAssertions()->append(secondTestAssertion);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter busWriter;

    busWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<ipxact:vendor>TUT</ipxact:vendor>"
        "<ipxact:library>TestLibrary</ipxact:library>"
        "<ipxact:name>TestBus</ipxact:name>"
        "<ipxact:version>1.0</ipxact:version>"
        "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
            "<ipxact:assertions>"
                "<ipxact:assertion>"
                    "<ipxact:name>testAssertion1</ipxact:name>"
                    "<ipxact:displayName>Display name for assertion.</ipxact:displayName>"
                    "<ipxact:description>Description for assertion.</ipxact:description>"
                    "<ipxact:assert>1</ipxact:assert>"
                "</ipxact:assertion>"
                "<ipxact:assertion>"
                    "<ipxact:name>testAssertion2</ipxact:name>"
                    "<ipxact:assert>0</ipxact:assert>"
                "</ipxact:assertion>"
            "</ipxact:assertions>"
        "</ipxact:abstractionDefinition>\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinition::testVendorExtensions()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinition::testVendorExtensions()
{
    VLNV targetBus(VLNV::BUSDEFINITION, "TUT", "TestLibrary", "TargetBusDef", "1.0");

    VLNV vlnv(VLNV::ABSTRACTIONDEFINITION, "TUT", "TestLibrary", "TestBus", "1.0");
    QSharedPointer<AbstractionDefinition> abstractionDefinition(new AbstractionDefinition());
    abstractionDefinition->setVlnv(vlnv);
    abstractionDefinition->setBusType(targetBus);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("vendorAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > vendorExtensions = abstractionDefinition->getVendorExtensions();
    vendorExtensions->append(testExtension);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    AbstractionDefinitionWriter busWriter;

    busWriter.writeAbstractionDefinition(xmlStreamWriter, abstractionDefinition);

    QCOMPARE(output, QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestBus</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBusDef\" version=\"1.0\"/>"
            "<ipxact:vendorExtensions>"
                "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:abstractionDefinition>\n"));
}

QTEST_APPLESS_MAIN(tst_AbstractionDefinition)

#include "tst_AbstractionDefinitionWriter.moc"

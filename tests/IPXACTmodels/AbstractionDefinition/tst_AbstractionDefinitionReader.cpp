//-----------------------------------------------------------------------------
// File: tst_AbstractionDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.08.2015
//
// Description:
// Unit test for class AbstractionDefinition.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QDomNode>

#include <IPXACTmodels/common/CellSpecification.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/PresenceTypes.h>
#include <IPXACTmodels/common/Protocol.h>
#include <IPXACTmodels/common/TimingConstraint.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinitionReader.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
//! Unit test for class AbstractionDefinition.
//-----------------------------------------------------------------------------
class tst_AbstractionDefinitionReader : public QObject
{
    Q_OBJECT

public:
    tst_AbstractionDefinitionReader();

private slots:
    void testReadVLNAndMandatoryFields();
    void testReadTopComments();
    void testProcessingInstructionsAreParsed();

    void testReadExtends();
    void testReadDescription();

    void testReadWirePort();
    void testReadMultipleWireSystemPorts();
    void testReadWirePortConstraints();

    void testReadTransactionalPort();
    void testReadMultipleTransactionalSystemPorts();
    void testReadTransactionalWithProtocol();
    void testReadTransactionalWithCustomProtocol();
   
    void testReadParameters();
    void testReadAssertions();
    void testReadVendorExtension();

};

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::tst_AbstractionDefinitionReader()
//-----------------------------------------------------------------------------
tst_AbstractionDefinitionReader::tst_AbstractionDefinitionReader()
{
    
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadVLNAndMandatoryFields()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadVLNAndMandatoryFields()
{
    QDomDocument document;
    document.setContent(QString(        
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBus\" version=\"2.0\"/>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> definition = reader.createAbstractionDefinitionFrom(document);
   
    VLNV readVLNV = definition->getVlnv();
    QCOMPARE(readVLNV.getVendor(), QString("TUT"));
    QCOMPARE(readVLNV.getLibrary(), QString("TestLibrary"));
    QCOMPARE(readVLNV.getName(), QString("MinimalDefinition"));
    QCOMPARE(readVLNV.getVersion(), QString("1.0"));
    
    VLNV busType = definition->getBusType();
    QCOMPARE(busType.getVendor(), QString("TUT"));
    QCOMPARE(busType.getLibrary(), QString("TestLibrary"));
    QCOMPARE(busType.getName(), QString("TargetBus"));
    QCOMPARE(busType.getVersion(), QString("2.0"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadTopComments()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadTopComments()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<!--Commented line 1-->"
        "<!--Commented line 2-->"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBus\" version=\"2.0\"/>"
            "</ipxact:abstractionDefinition>"
        "<!--Comment not to include-->"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> definition = reader.createAbstractionDefinitionFrom(document);

    QCOMPARE(definition->getTopComments().size(), 2);
    QCOMPARE(definition->getTopComments().first(), QString("Commented line 1"));   
    QCOMPARE(definition->getTopComments().last(), QString("Commented line 2"));   
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testProcessingInstructionsAreWritten()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testProcessingInstructionsAreParsed()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<!--Header comment -->"
        "<?xml-stylesheet href=\"style.css\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBus\" version=\"2.0\"/>"
        "</ipxact:abstractionDefinition>"
        ));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> definition = reader.createAbstractionDefinitionFrom(document);

    QCOMPARE(definition->getXmlProcessingInstructions().count(), 1);

    QPair<QString, QString> styleInstruction = definition->getXmlProcessingInstructions().first();
    QCOMPARE(styleInstruction.first, QString("xml-stylesheet"));
    QCOMPARE(styleInstruction.second, QString("href=\"style.css\""));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadExtends()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadExtends()
{
    QDomDocument document;
    document.setContent(QString(        
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBus\" version=\"2.0\"/>"
            "<ipxact:extends vendor=\"TUT\" library=\"TestLibrary\" name=\"extended\" version=\"1.0\"/>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader busReader;
    QSharedPointer<AbstractionDefinition> testBus = busReader.createAbstractionDefinitionFrom(document);

    VLNV extendedBus = testBus->getExtends();
    QCOMPARE(extendedBus.getVendor(), QString("TUT"));
    QCOMPARE(extendedBus.getLibrary(), QString("TestLibrary"));
    QCOMPARE(extendedBus.getName(), QString("extended"));
    QCOMPARE(extendedBus.getVersion(), QString("1.0"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadDescription()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadDescription()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBus\" version=\"1.0\"/>"
            "<ipxact:description>Abstraction definition description.</ipxact:description>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader busReader;
    QSharedPointer<AbstractionDefinition> testBus = busReader.createAbstractionDefinitionFrom(document);

    QCOMPARE(testBus->getDescription(), QString("Abstraction definition description."));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadWirePort()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadWirePort()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBus\" version=\"1.0\"/>"
            "<ipxact:ports>"
                "<ipxact:port>"
                    "<ipxact:isPresent>1</ipxact:isPresent>"
                    "<ipxact:logicalName>wirePort</ipxact:logicalName>"
                    "<ipxact:displayName>simple wire</ipxact:displayName>"
                    "<ipxact:description>simple wire for testing</ipxact:description>"
                     "<ipxact:wire>"
                        "<ipxact:qualifier>"
                            "<ipxact:isData>true</ipxact:isData>"
                            "<ipxact:isAddress>true</ipxact:isAddress>"
                        "</ipxact:qualifier>"
                        "<ipxact:onMaster>"
                            "<ipxact:presence>required</ipxact:presence>"
                            "<ipxact:width>widthExpression</ipxact:width>"
                            "<ipxact:direction>inout</ipxact:direction>"
                        "</ipxact:onMaster>"
                         "<ipxact:defaultValue>1</ipxact:defaultValue>"
                    "</ipxact:wire>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> readAbsDef = reader.createAbstractionDefinitionFrom(document);

    QCOMPARE(readAbsDef->getLogicalPorts()->size(), 1);

    QSharedPointer<PortAbstraction> port = readAbsDef->getLogicalPorts()->first();

    QCOMPARE(port->isPresent(), QString("1"));
    QCOMPARE(port->getLogicalName(), QString("wirePort"));
    QCOMPARE(port->displayName(), QString("simple wire"));
    QCOMPARE(port->description(), QString("simple wire for testing"));
    QCOMPARE(port->getVendorExtensions()->size(), 1);

    QCOMPARE(port->hasWire(), true);
    QCOMPARE(port->getWire()->getDefaultValue(), QString("1"));

    Qualifier wireQualifier = port->getQualifier();
    QCOMPARE(wireQualifier.isData(), true);
    QCOMPARE(wireQualifier.isAddress(), true);
    QCOMPARE(wireQualifier.isClock(), false);
    QCOMPARE(wireQualifier.isReset(), false);

    QSharedPointer<WirePort> master = port->getWire()->getMasterPort();
    QCOMPARE(master->getPresence(), PresenceTypes::REQUIRED);
    QCOMPARE(master->getWidth(), QString("widthExpression"));
    QCOMPARE(master->getDirection(), DirectionTypes::INOUT);
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadMultipleWireSystemPorts()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadMultipleWireSystemPorts()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBus\" version=\"1.0\"/>"
            "<ipxact:ports>"
                "<ipxact:port>"
                    "<ipxact:isPresent>1</ipxact:isPresent>"
                    "<ipxact:logicalName>wirePort</ipxact:logicalName>"
                    "<ipxact:wire>"
                        "<ipxact:onSystem>"
                            "<ipxact:group>testInGroup</ipxact:group>"
                            "<ipxact:direction>in</ipxact:direction>"
                        "</ipxact:onSystem>"
                        "<ipxact:onSystem>"   
                            "<ipxact:group>testOutGroup</ipxact:group>"
                            "<ipxact:direction>out</ipxact:direction>"
                        "</ipxact:onSystem>"
                        "<ipxact:onSystem>"       
                            "<ipxact:group>testInOutGroup</ipxact:group>"
                            "<ipxact:direction>inout</ipxact:direction>"
                        "</ipxact:onSystem>"
                        "<ipxact:defaultValue>1</ipxact:defaultValue>"
                    "</ipxact:wire>"            
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> readAbsDef = reader.createAbstractionDefinitionFrom(document);

    QSharedPointer<PortAbstraction> port = readAbsDef->getLogicalPorts()->first();

    QSharedPointer<QList<QSharedPointer<WirePort> > > systemPorts = port->getWire()->getSystemPorts();
    QCOMPARE(systemPorts->size(), 3);

    QCOMPARE(systemPorts->at(0)->getDirection(), DirectionTypes::IN);
    QCOMPARE(systemPorts->at(0)->getSystemGroup(), QString("testInGroup"));

    QCOMPARE(systemPorts->at(1)->getDirection(), DirectionTypes::OUT);
    QCOMPARE(systemPorts->at(1)->getSystemGroup(), QString("testOutGroup"));

    QCOMPARE(systemPorts->at(2)->getDirection(), DirectionTypes::INOUT);
    QCOMPARE(systemPorts->at(2)->getSystemGroup(), QString("testInOutGroup"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadWirePortConstraints()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadWirePortConstraints()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>MinimalDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:busType vendor=\"TUT\" library=\"TestLibrary\" name=\"TargetBus\" version=\"1.0\"/>"
                "<ipxact:ports>"
                    "<ipxact:port>"
                        "<ipxact:isPresent>1</ipxact:isPresent>"
                        "<ipxact:logicalName>wirePort</ipxact:logicalName>"
                        "<ipxact:wire>"        
                            "<ipxact:onSlave>"
                                "<ipxact:modeConstraints>"
                                    "<ipxact:timingConstraint clockEdge=\"fall\" delayType=\"min\" clockName=\"clk_in\">2.25</ipxact:timingConstraint>"
                                    "<ipxact:loadConstraint>"
                                        "<ipxact:cellSpecification cellStrength=\"low\">"
                                            "<ipxact:cellFunction other=\"customFunction\">other</ipxact:cellFunction>"
                                        "</ipxact:cellSpecification>"
                                    "</ipxact:loadConstraint>"
                                    "<ipxact:driveConstraint>"
                                        "<ipxact:cellSpecification cellStrength=\"high\">"
                                            "<ipxact:cellClass>sequential</ipxact:cellClass>"
                                        "</ipxact:cellSpecification>"
                                    "</ipxact:driveConstraint>"
                                "</ipxact:modeConstraints>"
                            "</ipxact:onSlave>"
                        "<ipxact:requiresDriver driverType=\"singleShot\">true</ipxact:requiresDriver>"
                    "</ipxact:wire>"
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> readAbsDef = reader.createAbstractionDefinitionFrom(document);

    QCOMPARE(readAbsDef->getLogicalPorts()->size(), 1);

    QSharedPointer<PortAbstraction> port = readAbsDef->getLogicalPorts()->first();
    QCOMPARE(port->getLogicalName(), QString("wirePort"));

    QCOMPARE(port->hasWire(), true);
    QCOMPARE(port->getWire()->requiresDriver(), true);
    QCOMPARE(port->getWire()->getDriverType(), General::SINGLESHOT);

    QSharedPointer<WirePort> slave = port->getWire()->getSlavePort();
    QSharedPointer<TimingConstraint> timingConstraint = slave->getTimingConstraint();
    QCOMPARE(timingConstraint->getClockEdge(), TimingConstraint::FALL);
    QCOMPARE(timingConstraint->getDelayType(), TimingConstraint::MINIMUM_DELAY);
    QCOMPARE(timingConstraint->getClockName(), QString("clk_in"));
    QCOMPARE(timingConstraint->getValue(), 2.25);

    QSharedPointer<CellSpecification> driveConstraint = slave->getDriveConstraint();
    QCOMPARE(driveConstraint->getCellStrength(), CellSpecification::HIGH);
    QCOMPARE(driveConstraint->getCellFunction(), QString(""));
    QCOMPARE(driveConstraint->getCellClass(), CellSpecification::SEQUENTIAL);
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadTransactionalPort()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadTransactionalPort()
{
    QDomDocument document;
    document.setContent(QString(        
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
                    "<ipxact:logicalName>testPort</ipxact:logicalName>"
                    "<ipxact:transactional>"
                        "<ipxact:qualifier>"
                            "<ipxact:isData>true</ipxact:isData>"
                        "</ipxact:qualifier>"        
                        "<ipxact:onMaster>"
                            "<ipxact:presence>optional</ipxact:presence>"
                            "<ipxact:initiative>requires</ipxact:initiative>"
                            "<ipxact:kind>tlm_port</ipxact:kind>"
                            "<ipxact:busWidth>32</ipxact:busWidth>"
                        "</ipxact:onMaster>" 
                    "</ipxact:transactional>"        
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> testDefinition = reader.createAbstractionDefinitionFrom(document);

    QSharedPointer<PortAbstraction> port = testDefinition->getLogicalPorts()->first();

    QCOMPARE(port->hasTransactional(), true);

    Qualifier tranasctionalQualifier = port->getQualifier();
    QCOMPARE(tranasctionalQualifier.isData(), true);
    QCOMPARE(tranasctionalQualifier.isAddress(), false);
    QCOMPARE(tranasctionalQualifier.isClock(), false);
    QCOMPARE(tranasctionalQualifier.isReset(), false);

    QSharedPointer<TransactionalAbstraction> transactional = port->getTransactional();

    QCOMPARE(transactional->hasMasterPort(), true);

    QSharedPointer<TransactionalPort> masterPort = transactional->getMasterPort();
    QCOMPARE(masterPort->getPresence(), PresenceTypes::OPTIONAL);
    QCOMPARE(masterPort->getInitiative(), QString("requires"));
    QCOMPARE(masterPort->getKind(), QString("tlm_port"));
    QCOMPARE(masterPort->getBusWidth(), QString("32"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadMultipleTransactionalSystemPorts()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadMultipleTransactionalSystemPorts()
{
    QDomDocument document;
    document.setContent(QString(        
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
                        "<ipxact:logicalName>testPort</ipxact:logicalName>"
                        "<ipxact:transactional>"    
                        "<ipxact:onSystem>"
                            "<ipxact:group>illegalGroup</ipxact:group>"
                            "<ipxact:presence>illegal</ipxact:presence>"
                        "</ipxact:onSystem>" 
                        "<ipxact:onSystem>"
                            "<ipxact:group>requiredGroup</ipxact:group>"
                            "<ipxact:presence>required</ipxact:presence>"
                        "</ipxact:onSystem>" 
                    "</ipxact:transactional>"
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> testDefinition = reader.createAbstractionDefinitionFrom(document);

    QSharedPointer<PortAbstraction> port = testDefinition->getLogicalPorts()->first();

    QCOMPARE(port->hasTransactional(), true);

    QSharedPointer<TransactionalAbstraction> transactional = port->getTransactional();

    QCOMPARE(transactional->getSystemPorts()->count(), 2);
    QCOMPARE(transactional->getSystemPorts()->first()->getSystemGroup(), QString("illegalGroup"));
    QCOMPARE(transactional->getSystemPorts()->last()->getPresence(), PresenceTypes::REQUIRED);
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadTransactionalWithProtocol()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadTransactionalWithProtocol()
{
    QDomDocument document;
    document.setContent(QString(        
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
                    "<ipxact:logicalName>testPort</ipxact:logicalName>"
                    "<ipxact:transactional>"                    
                        "<ipxact:onMaster>"
                            "<ipxact:protocol>"
                                "<ipxact:protocolType>tlm</ipxact:protocolType>"
                                "<ipxact:payload>"
                                    "<ipxact:name>data</ipxact:name>"
                                    "<ipxact:type>generic</ipxact:type>"
                                    "<ipxact:extension mandatory=\"true\">extension</ipxact:extension>"
                                    "<ipxact:vendorExtensions>"
                                        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
                                    "</ipxact:vendorExtensions>"
                                "</ipxact:payload>"
                            "</ipxact:protocol>"
                        "</ipxact:onMaster>" 
                    "</ipxact:transactional>"
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> testDefinition = reader.createAbstractionDefinitionFrom(document);

    QSharedPointer<PortAbstraction> port = testDefinition->getLogicalPorts()->first();

    QCOMPARE(port->hasTransactional(), true);
    QSharedPointer<TransactionalAbstraction> transactional = port->getTransactional();

    QCOMPARE(transactional->hasMasterPort(), true);
    QSharedPointer<TransactionalPort> master = transactional->getMasterPort();

    QCOMPARE(master->hasProtocol(), true);
    QSharedPointer<Protocol> protocol = master->getProtocol();
   
    QCOMPARE(protocol->getProtocolType(), QString("tlm"));
    QCOMPARE(protocol->getPayloadName(), QString("data"));
    QCOMPARE(protocol->getPayloadType(), QString("generic"));
    QCOMPARE(protocol->getPayloadExtension(), QString("extension"));
    QCOMPARE(protocol->hasMandatoryPayloadExtension(), true);

    QCOMPARE(protocol->getVendorExtensions()->count(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadTransactionalWithCustomProtocol()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadTransactionalWithCustomProtocol()
{
    QDomDocument document;
    document.setContent(QString(        
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
                    "<ipxact:logicalName>testPort</ipxact:logicalName>"
                    "<ipxact:transactional>"                    
                        "<ipxact:onMaster>"
                            "<ipxact:protocol>"
                                "<ipxact:protocolType custom=\"customProtocol\">custom</ipxact:protocolType>"                        
                            "</ipxact:protocol>"
                        "</ipxact:onMaster>" 
                    "</ipxact:transactional>"
                "</ipxact:port>"
            "</ipxact:ports>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> testDefinition = reader.createAbstractionDefinitionFrom(document);

    QSharedPointer<PortAbstraction> port = testDefinition->getLogicalPorts()->first();

    QCOMPARE(port->hasTransactional(), true);
    QSharedPointer<TransactionalAbstraction> transactional = port->getTransactional();

    QCOMPARE(transactional->hasMasterPort(), true);
    QSharedPointer<TransactionalPort> master = transactional->getMasterPort();

    QCOMPARE(master->hasProtocol(), true);
    QSharedPointer<Protocol> protocol = master->getProtocol();

    QCOMPARE(protocol->getProtocolType(), QString("custom"));
    QCOMPARE(protocol->getCustomProtocolType(), QString("customProtocol"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadParameters()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadParameters()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
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
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> testDefinition = reader.createAbstractionDefinitionFrom(document);

    QCOMPARE(testDefinition->getParameters()->count(), 2);
    
    QSharedPointer<Parameter> testParameter = testDefinition->getParameters()->first();
    QCOMPARE(testParameter->name(), QString("parameter1"));
    QCOMPARE(testParameter->getValue(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadAssertions()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadAssertions()
{
    QDomDocument document;
    document.setContent(QString(        
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:assertions>"
                "<ipxact:assertion>"
                    "<ipxact:name>testAssert</ipxact:name>"
                    "<ipxact:displayName>testable assert</ipxact:displayName>"
                    "<ipxact:description>description for assertion</ipxact:description>"
                    "<ipxact:assert>1</ipxact:assert>"
                "</ipxact:assertion>"     
            "</ipxact:assertions>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> TestDefinition = reader.createAbstractionDefinitionFrom(document);

    QCOMPARE(TestDefinition->getAssertions()->count(), 1);

    QSharedPointer<Assertion> testAssertion = TestDefinition->getAssertions()->first();
    QCOMPARE(testAssertion->name(), QString("testAssert"));
    QCOMPARE(testAssertion->displayName(), QString("testable assert"));
    QCOMPARE(testAssertion->description(), QString("description for assertion"));
    QCOMPARE(testAssertion->getAssert(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionReader::testReadVendorExtension()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionReader::testReadVendorExtension()
{
    QDomDocument document;
    document.setContent(QString(
        "<?xml version=\"1.0\"?>"
        "<ipxact:abstractionDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " 
        "xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\" "
        "xmlns:kactus2=\"http://kactus2.cs.tut.fi\" "
        "xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/ "
        "http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
            "<ipxact:vendor>TUT</ipxact:vendor>"
            "<ipxact:library>TestLibrary</ipxact:library>"
            "<ipxact:name>TestDefinition</ipxact:name>"
            "<ipxact:version>1.0</ipxact:version>"
            "<ipxact:vendorExtensions>"
                "<kactus2:version>3.0.0</kactus2:version>"
                "<kactus2:definitionFile>fileName.sv</kactus2:definitionFile>"
                "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:abstractionDefinition>"));

    AbstractionDefinitionReader reader;
    QSharedPointer<AbstractionDefinition> TestDefinition = reader.createAbstractionDefinitionFrom(document);

    QCOMPARE(TestDefinition->getVendorExtensions()->count(), 3);
    QCOMPARE(TestDefinition->getVersion(), QString("3.0.0"));
    QCOMPARE(TestDefinition->getFileName(), QString("fileName.sv"));
}


QTEST_APPLESS_MAIN(tst_AbstractionDefinitionReader)

#include "tst_AbstractionDefinitionReader.moc"

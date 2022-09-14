//-----------------------------------------------------------------------------
// File: tst_VerilogWriterFactory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Unit test for class VerilogWriterFactory.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogGenerator/VerilogWriterFactory/VerilogWriterFactory.h>

#include <Plugins/PluginSystem/GeneratorPlugin/MessagePasser.h>

#include <tests/MockObjects/LibraryMock.h>

#include <Plugins/common/HDLParser/MetaDesign.h>
#include <Plugins/common/HDLParser/MetaComponent.h>
#include <Plugins/VerilogImport/VerilogSyntax.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>
#include <IPXACTmodels/BusDefinition/BusDefinition.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/TransactionalTypes.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

class tst_VerilogWriterFactory : public QObject
{
    Q_OBJECT

public:
    tst_VerilogWriterFactory();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Hierarchy test cases:
    void testTopLevelComponent();

    void testFileHeaderIsPrinted();

    void testHierarchicalConnections();
    void testInstanceSlicedHierarchicalConnections();
    void testTopSlicedHierarchicalConnections();

    void testMasterToSlaveInterconnection();
    void testOneBitSlicing();
    void testPhysicalSlicedMasterToSlaveInterconnection();
    void testLogicalSlicedMasterToSlaveInterconnection();
    void testLogicalSlicedMasterToMultiplePortsInSlave();
    void testHierarchicalInOutConnectionWithMultiplePorts();
    void testPortSlicedMasterToSlaveInterconnection();
    void testMasterToMultipleSlavesInterconnections();

    void testLogicalInvertBetweenInstances();
    void testLogicalInvertInHierarchicalConnection();

    void testAdhocConnectionBetweenComponentInstances();    
    void testPartSelectedAdhocConnectionBetweenComponentInstances();
    void testAdhocInOutConnectionBetweenComponentInstances();
    void testAdhocTieOffInComponentInstance();
    void testHierarchicalAdhocConnection();
    void testHierarchicalAdhocInOutConnection();
    void testHierarchicalAdHocTieOffValues();
    
    void testDescriptionAndVLNVIsPrintedAboveInstance();
    void testDescriptionAndVLNVIsPrintedAboveInstance_data();
    void testInstanceParametersAreWritten();

    void testAdhocConnectionToInterfacePorts();

    // Flat test cases:
    void testFlatComponent();
    void testFlatComponentWithTypedParameter();

    void testDoNotWriteTransactionalPorts();
    void testDoNotWriteTransactionalInterfaces();

    void testDoNotWriteTransactionalConnections();

	void testGenerationWithImplementation();
	void testGenerationWithImplementationWithTag();
	void testGenerationWithImplementationWithPostModule();

private slots:

    void gatherErrorMessage(QString const& errorMessage);

private:

    QSharedPointer<MetaInterconnection> addInterconnectToDesign(QSharedPointer<MetaInterface> first,
        QSharedPointer<MetaInterface> second,
        QString name);

    QSharedPointer<MetaWire> addWireToDesign(QString name, QString leftBound, QString rightBound, 
        QSharedPointer<MetaInterconnection> mInterconnect = QSharedPointer<MetaInterconnection>());

    QSharedPointer<MetaPort> addPort(QString const& portName, int portSize, DirectionTypes::Direction direction, 
        QSharedPointer<MetaComponent> component,
        QSharedPointer<MetaInterface> mInterface = QSharedPointer<MetaInterface>());

    QSharedPointer<MetaPort> addTransactional(QString const& portName, TransactionalTypes::Initiative initiative,
        QSharedPointer<MetaComponent> component,
        QSharedPointer<MetaInterface> containingInterface = QSharedPointer<MetaInterface>());

    QSharedPointer<Parameter> addParameter(QString const& name, QString const& value, 
        QSharedPointer<MetaComponent> mComponent);

    void runGenerator(bool useDesign);

    void createPortAssignment(QSharedPointer<MetaPort> mPort, QSharedPointer<MetaWire> wire, bool up,
        QString const& logicalLeft, QString const& logicalRight, QString const& physicalLeft, 
        QString const& physicalRight);

    QSharedPointer<Component> addTestComponentToLibrary(VLNV vlnv);

    QSharedPointer<MetaInstance> addInstanceToDesign(QString instanceName, QSharedPointer<Component> component);

    void addInterfacesToInstance(QSharedPointer<MetaInstance> mInstance);

    void createInterface(QSharedPointer<MetaPort> port, QString const& interfaceName,
        QSharedPointer<MetaInstance> component);
    
    QSharedPointer<MetaInterface> addInterfaceToComponent(QString const& interfaceName,
        QSharedPointer<MetaInstance> component);

    QSharedPointer<MetaInstance> addSender(QString const& instanceName);

    QSharedPointer<MetaInstance> addReceiver(QString const& instanceName);

    QSharedPointer<Component> createComponent(QString const& componentName);

    QSharedPointer<Design> createDesignForComponent(QString const& designName, QSharedPointer<Component> component);

    QSharedPointer<BusDefinition> createBusDefinition(QString const& definitionName);

    QSharedPointer<AbstractionDefinition> createAbstractionDefinition(QString const& definitionName);

    QSharedPointer<Port> addPortToComponent(QString const& portName, QSharedPointer<Component> containingComponent,
        TransactionalTypes::Initiative newInitiative = TransactionalTypes::INITIATIVE_INVALID,
        DirectionTypes::Direction newDirection = DirectionTypes::DIRECTION_INVALID);

    QSharedPointer<BusInterface> addBusInterfaceToComponent(QString const& busName,
        QSharedPointer<Component> containingComponent, General::InterfaceMode busMode,
        QSharedPointer<BusDefinition> busDefinition, QSharedPointer<AbstractionDefinition> abstraction,
        QString const& systemGroup = "");

    QSharedPointer<PortAbstraction> addLogicalPortToAbstraction(QString const& portName,
        QSharedPointer<AbstractionDefinition> containingAbstraction, General::InterfaceMode busMode,
        TransactionalTypes::Initiative logicalInitiative = TransactionalTypes::INITIATIVE_INVALID,
        DirectionTypes::Direction logicalDirection = DirectionTypes::DIRECTION_INVALID,
        QString const& systemGroup = "");

    void mapPortsToBusInterface(QSharedPointer<BusInterface> containingBus, QSharedPointer<Port> physicalPort,
        QSharedPointer<PortAbstraction> logicalPort);

    void mapMultiplePortsToBusInterface(QSharedPointer<BusInterface> containingBus,
        QList<QSharedPointer<Port> > physicalPorts, QSharedPointer<PortAbstraction> logicalPort);

    QSharedPointer<ComponentInstance> addInstanceToDesign(QSharedPointer<Design> containingDesign,
        QSharedPointer<Component> instancedComponent);

    QSharedPointer<Interconnection> addInterconnectionToDesign(QString const& interconnectionName,
        QSharedPointer<Design> containingDesign, QString const& startInterfaceName,
        QString const& startInstanceName, QString const& secondInterfaceName, QString const& secondInstanceName);

    QSharedPointer<Interconnection> addHierarchicalInterconnectionToDesign(QString const& interconnectionName,
        QSharedPointer<Design> containingDesign, QString const& topInterface, QString const& targetInterfaceName,
        QString const& targetInstanceName);

    QSharedPointer<MetaInterconnection> addConnectionToDesign();

    void addDefaultPortAssignmentToPort(QSharedPointer<MetaPort> port, QString tieOff, bool up);

    void verifyOutputContains(QString const& expectedOutput);

    void compareOutputTo(QString const& expectedOutput);

    QSharedPointer<MetaInstance> topComponent_;
    QSharedPointer<MetaDesign> design_;

    QSharedPointer<MetaComponent> flatComponent_;

    //! The generator output as a string.
    QString output_;
    
    //! The time of the generator run.
    QDateTime generationTime_;

    //! The test mock for library interface.
    LibraryMock library_;

    QStringList errorMessages_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::tst_VerilogWriterFactory()
//-----------------------------------------------------------------------------
tst_VerilogWriterFactory::tst_VerilogWriterFactory(): output_(), generationTime_(), library_(this),
errorMessages_()
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::initTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::initTestCase()
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::cleanupTestCase()
{
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::init()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    QSharedPointer<Component> component = QSharedPointer<Component>(new Component(vlnv));

    MessagePasser messages;

    topComponent_ =  QSharedPointer<MetaInstance>(new MetaInstance(QSharedPointer<ComponentInstance>(),
        &library_, &messages, component, QSharedPointer<View>()));

    design_ = QSharedPointer<MetaDesign>(new MetaDesign(&library_,&messages,
        QSharedPointer<Design>(), QSharedPointer<DesignInstantiation>(),
        QSharedPointer<DesignConfiguration>(),
        topComponent_));

    flatComponent_ = QSharedPointer<MetaComponent>(
        new MetaComponent(&messages, component, QSharedPointer<View>()));

    library_.clear();
    errorMessages_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::cleanup()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::cleanup()
{
    topComponent_.clear();

    output_.clear();

    QFile::remove("TestComponent.v");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testTopLevelComponent()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testTopLevelComponent()
{
    addPort("clk", 1, DirectionTypes::IN, topComponent_);
    addPort("rst_n", 1, DirectionTypes::IN, topComponent_);
    addPort("dataOut", 8, DirectionTypes::OUT, topComponent_);
    addPort("dataIn", 8, DirectionTypes::IN, topComponent_);
    addParameter("dataWidth", "8", topComponent_);
    addParameter("freq", "100000", topComponent_);

    runGenerator(true);

    verifyOutputContains(QString(
        "module TestComponent #(\n"
        "    parameter                              dataWidth        = 8,\n"
        "    parameter                              freq             = 100000\n"
        ") (\n"
        "    // These ports are not in any interface\n"         
        "    input                               clk,\n"
        "    input                [7:0]          dataIn,\n"
        "    input                               rst_n,\n"
        "    output               [7:0]          dataOut\n"
        ");\n"
        "\n"
        "    // Assignments for the ports of the encompassing component:\n"
        "\n"
        "\n"
        "\n"
        "endmodule\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addPort()
//-----------------------------------------------------------------------------
QSharedPointer<MetaPort> tst_VerilogWriterFactory::addPort(QString const& portName, int portSize, 
    DirectionTypes::Direction direction, QSharedPointer<MetaComponent> component,
    QSharedPointer<MetaInterface> mInterface /*= QSharedPointer<MetaInterface>()*/)
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName));
    port->setDirection(direction);
    port->setPortSize(portSize);

    QSharedPointer<MetaPort> gp(new MetaPort);
    gp->isWire_ = true;
    gp->isTransactional_ = false;
    gp->port_ = port;
    gp->vectorBounds_.first = QString::number(portSize-1);
    gp->vectorBounds_.second = "0";

    component->getPorts()->insert(portName,gp);
    component->getComponent()->getPorts()->append(port);

    if (mInterface)
    {
        mInterface->ports_.insert(portName,gp);

        QSharedPointer<PortMap> pm(new PortMap);
        QSharedPointer<PortMap::PhysicalPort> pp(new PortMap::PhysicalPort);
        pp->name_ = portName;
        pm->setPhysicalPort(pp);

        mInterface->interface_->getAbstractionTypes()->first()->getPortMaps()->append(pm);
    }

    return gp;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> tst_VerilogWriterFactory::addParameter(QString const& name, QString const& value,
    QSharedPointer<MetaComponent> mComponent)
{
    QSharedPointer<Parameter> parameter = QSharedPointer<Parameter>(new Parameter());
    parameter->setName(name);
    parameter->setValue(value);

    mComponent->getMetaParameters()->append(parameter);

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::runGenerator()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::runGenerator(bool useDesign)
{
    GenerationSettings settings;
    settings.generateInterfaces_ = false;

    MessagePasser messages;

    VerilogWriterFactory factory(&library_, &messages, &settings, "bogusToolVersion", "bogusGeneratorVersion");
    QString outputPath = ".";
    QSharedPointer<GenerationOutput> document;

	if (useDesign)
    {
        QList<QSharedPointer<MetaDesign> > ds;
        ds.append(design_);
		document = factory.prepareDesign(ds).first();
	}
	else
	{
		document = factory.prepareComponent(outputPath, flatComponent_);
	}

    generationTime_ =  QDateTime::currentDateTime();

    document->write(".");

    output_ = document->fileContent_;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testFileHeaderIsPrinted()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testFileHeaderIsPrinted()
{    
    topComponent_->getComponent()->setDescription("Component description\nspanning multiple\nlines.");
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_->getComponent());
    	
    QCoreApplication::setOrganizationName("TUT");
    QCoreApplication::setApplicationName("TestRunner");
    QSettings settings;
    settings.setValue("General/Username", "testUser");

    runGenerator(false);

    verifyOutputContains(QString(
        "//-----------------------------------------------------------------------------\n"
        "// File          : TestComponent.v\n"
        "// Creation date : " + generationTime_.date().toString("dd.MM.yyyy") + "\n"
        "// Creation time : " + generationTime_.time().toString("hh:mm:ss") + "\n"
        "// Description   : Component description\n"
        "//                 spanning multiple\n"
        "//                 lines.\n"
		"// Created by    : testUser\n"
		"// Tool : Kactus2 bogusToolVersion\n"
		"// Plugin : Verilog generator bogusGeneratorVersion\n"
		"// This file was generated based on IP-XACT component Test:TestLibrary:TestComponent:1.0\n"
        "// whose XML file is C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml\n"
        "//-----------------------------------------------------------------------------\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testHierarchicalConnections()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testHierarchicalConnections()
{
    QSharedPointer<MetaInterface> topClockIf = addInterfaceToComponent("clk_if", topComponent_);
    QSharedPointer<MetaInterface> topDataIf = addInterfaceToComponent("data_bus", topComponent_);

    QSharedPointer<MetaPort> topClock = addPort("top_clk", 1, DirectionTypes::IN, topComponent_, topClockIf);
    QSharedPointer<MetaPort> topData = addPort("data_to_instance", 8, DirectionTypes::IN, topComponent_, topDataIf);
    QSharedPointer<MetaPort> topEnable = addPort("enable_to_instance", 1, DirectionTypes::IN, topComponent_, topDataIf);
    QSharedPointer<MetaPort> topFull = addPort("full_from_instance", 1, DirectionTypes::OUT, topComponent_, topDataIf);

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");
    QSharedPointer<Component> comp = addTestComponentToLibrary(instanceVlnv);
    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign("instance1", comp);
    addInterfacesToInstance(mInstance);

    QSharedPointer<MetaInterface> instanceClockIf = mInstance->getInterfaces()->value("clk_if");
    QSharedPointer<MetaInterface> instanceDataIf = mInstance->getInterfaces()->value("data_if");

    QSharedPointer<MetaInterconnection> mI0 = addInterconnectToDesign(topClockIf, instanceClockIf, "clock_connection");
    mI0->hierIfs_.append(topClockIf);
    QSharedPointer<MetaInterconnection> mI1 = addInterconnectToDesign(topDataIf, instanceDataIf, "data_connection");
    mI1->hierIfs_.append(topDataIf);

    QSharedPointer<MetaWire> gw0 = addWireToDesign("clk_wire","0","0",mI0);
    gw0->hierPorts_.append(topClock);
    QSharedPointer<MetaWire> gw1 = addWireToDesign("data_wire","7","0",mI1);
    gw1->hierPorts_.append(topData);
    QSharedPointer<MetaWire> gw2 = addWireToDesign("enable_wire","0","0",mI1);
    gw2->hierPorts_.append(topEnable);
    QSharedPointer<MetaWire> gw3 = addWireToDesign("full_wire","0","0",mI1);
    gw3->hierPorts_.append(topFull);

    createPortAssignment(mInstance->getPorts()->value("clk"), gw0, true, "0", "0", "0", "0");
    createPortAssignment(mInstance->getPorts()->value("data_in"), gw1, true, "7", "0", "7", "0");
    createPortAssignment(mInstance->getPorts()->value("enable"), gw2, true, "0", "0", "0", "0");
    createPortAssignment(mInstance->getPorts()->value("full"), gw3, true, "0", "0", "0", "0");

    createPortAssignment(topClock, gw0, false, "0", "0", "0", "0");
    createPortAssignment(topData, gw1, false, "7", "0", "7", "0");
    createPortAssignment(topEnable, gw2, false, "0", "0", "0", "0");
    createPortAssignment(topFull, gw3, false, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains(QString(
        "module TestComponent(\n"
        "    // Interface: clk_if\n"
        "    input                               top_clk,\n"
        "\n"
        "    // Interface: data_bus\n"
        "    input                [7:0]          data_to_instance,\n"
        "    input                               enable_to_instance,\n"
        "    output                              full_from_instance\n"
        ");\n"
        "\n"
        "    // clock_connection wires:\n"
        "    wire       clk_wire;\n"
        "    // data_connection wires:\n"
        "    wire [7:0] data_wire;\n"
        "    wire       enable_wire;\n"
        "    wire       full_wire;\n"
        "\n"
        "    // instance1 port wires:\n"
        "    wire       instance1_clk;\n"
        "    wire [7:0] instance1_data_in;\n"
        "    wire       instance1_enable;\n"
        "    wire       instance1_full;\n"
        "\n"
        "    // Assignments for the ports of the encompassing component:\n"
        "    assign data_wire = data_to_instance;\n"
        "    assign enable_wire = enable_to_instance;\n"
        "    assign full_from_instance = full_wire;\n"
        "    assign clk_wire = top_clk;\n"
        "\n"
        "    // instance1 assignments:\n"
        "    assign instance1_clk = clk_wire;\n"
        "    assign instance1_data_in = data_wire;\n"
        "    assign instance1_enable = enable_wire;\n"
        "    assign full_wire = instance1_full;\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestInstance:1.0\n"
        "    TestInstance instance1(\n"
        "        // Interface: clk_if\n"
        "        .clk                 (instance1_clk),\n"
        "        // Interface: data_if\n"
        "        .data_in             (instance1_data_in),\n"
        "        .enable              (instance1_enable),\n"
        "        .full                (instance1_full),\n"
        "        // These ports are not in any interface\n"
        "        .data_out            ());\n"
        "\n"
        "\n"
        "endmodule"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testInstanceSlicedHierarchicalConnections()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testInstanceSlicedHierarchicalConnections()
{
    QSharedPointer<MetaInterface> topDataIf = addInterfaceToComponent("data_if", topComponent_);

    QSharedPointer<MetaPort> topiData = addPort("data_to_instance", 8, DirectionTypes::IN, topComponent_, topDataIf);
    QSharedPointer<MetaPort> topoData = addPort("data_from_instance", 8, DirectionTypes::OUT, topComponent_, topDataIf);

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");
    QSharedPointer<Component> comp = addTestComponentToLibrary(instanceVlnv);
    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign("instance1", comp);

    QSharedPointer<MetaInterface> instanceDataIf = addInterfaceToComponent("data_if", mInstance);
    QSharedPointer<MetaPort> idata1Port = addPort("idata1", 4, DirectionTypes::IN, mInstance, instanceDataIf);
    QSharedPointer<MetaPort> idata2Port = addPort("idata2", 4, DirectionTypes::IN, mInstance, instanceDataIf);
    QSharedPointer<MetaPort> odata1Port = addPort("odata1", 4, DirectionTypes::OUT, mInstance, instanceDataIf);
    QSharedPointer<MetaPort> odata2Port = addPort("odata2", 4, DirectionTypes::OUT, mInstance, instanceDataIf);

    QSharedPointer<MetaInterconnection> mI1 = addInterconnectToDesign(topDataIf, instanceDataIf, "data_connection");
    mI1->hierIfs_.append(topDataIf);

    QSharedPointer<MetaWire> imWire = addWireToDesign("idata_wire","7","0",mI1);
    imWire->hierPorts_.append(topiData);
    QSharedPointer<MetaWire> omWire = addWireToDesign("odata_wire","7","0",mI1);
    omWire->hierPorts_.append(topoData);

    createPortAssignment(idata1Port, imWire, true, "3", "0", "3", "0");
    createPortAssignment(idata2Port, imWire, true, "7", "4", "3", "0");
    createPortAssignment(odata1Port, omWire, true, "3", "0", "3", "0");
    createPortAssignment(odata2Port, omWire, true, "7", "4", "3", "0");

    createPortAssignment(topComponent_->getPorts()->value("data_to_instance"), imWire, false, "7", "0", "7", "0");
    createPortAssignment(topComponent_->getPorts()->value("data_from_instance"), omWire, false, "7", "0", "7", "0");

    runGenerator(true);

    verifyOutputContains("input                [7:0]          data_to_instance");
    verifyOutputContains("output               [7:0]          data_from_instance");

    verifyOutputContains("wire [7:0] idata_wire;");
    verifyOutputContains("wire [7:0] odata_wire;");

    verifyOutputContains("wire [3:0] instance1_idata1;");
    verifyOutputContains("wire [3:0] instance1_idata2");
    verifyOutputContains("wire [3:0] instance1_odata1;");
    verifyOutputContains("wire [3:0] instance1_odata2");

    verifyOutputContains("assign data_from_instance = odata_wire;");
    verifyOutputContains("assign idata_wire = data_to_instance;");

    verifyOutputContains("assign instance1_idata1 = idata_wire[3:0];");
    verifyOutputContains("assign instance1_idata2[3:0] = idata_wire[7:4];");
    verifyOutputContains("assign odata_wire[3:0] = instance1_odata1;");
    verifyOutputContains("assign odata_wire[7:4] = instance1_odata2[3:0];");

    verifyOutputContains(
        "    TestInstance instance1(\n"
        "        // Interface: data_if\n"
        "        .idata1              (instance1_idata1),\n"
        "        .idata2              (instance1_idata2),\n"
        "        .odata1              (instance1_odata1),\n"
        "        .odata2              (instance1_odata2)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testTopSlicedHierarchicalConnections()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testTopSlicedHierarchicalConnections()
{
    QSharedPointer<MetaInterface> topDataIf = addInterfaceToComponent("data_if", topComponent_);

    QSharedPointer<MetaPort> topiData1 = addPort("data_to_instance1", 4, DirectionTypes::IN, topComponent_, topDataIf);
    QSharedPointer<MetaPort> topiData2 = addPort("data_to_instance2", 4, DirectionTypes::IN, topComponent_, topDataIf);
    QSharedPointer<MetaPort> topoData1 = addPort("data_from_instance1", 4, DirectionTypes::OUT, topComponent_, topDataIf);
    QSharedPointer<MetaPort> topoData2 = addPort("data_from_instance2", 4, DirectionTypes::OUT, topComponent_, topDataIf);

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");
    QSharedPointer<Component> comp = addTestComponentToLibrary(instanceVlnv);
    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign("instance1", comp);

    QSharedPointer<MetaInterface> instanceDataIf = addInterfaceToComponent("data_if", mInstance);
    QSharedPointer<MetaPort> idataPort = addPort("idata", 8, DirectionTypes::IN, mInstance, instanceDataIf);
    QSharedPointer<MetaPort> odataPort = addPort("odata", 8, DirectionTypes::OUT, mInstance, instanceDataIf);

    QSharedPointer<MetaInterconnection> mI1 = addInterconnectToDesign(topDataIf, instanceDataIf, "data_connection");
    mI1->hierIfs_.append(topDataIf);

    QSharedPointer<MetaWire> imWire = addWireToDesign("idata_wire","7","0",mI1);
    imWire->hierPorts_.append(topiData1);
    imWire->hierPorts_.append(topiData2);

    QSharedPointer<MetaWire> omWire = addWireToDesign("odata_wire","7","0",mI1);
    omWire->hierPorts_.append(topoData1);
    omWire->hierPorts_.append(topoData2);

    createPortAssignment(idataPort, imWire, true, "7", "0", "7", "0");
    createPortAssignment(odataPort, omWire, true, "7", "0", "7", "0");

    createPortAssignment(topiData1, imWire, false, "3", "0", "3", "0");
    createPortAssignment(topiData2, imWire, false, "7", "4", "3", "0");
    createPortAssignment(topoData1, omWire, false, "3", "0", "3", "0");
    createPortAssignment(topoData2, omWire, false, "7", "4", "3", "0");

    runGenerator(true);

    verifyOutputContains("input                [3:0]          data_to_instance1");
    verifyOutputContains("input                [3:0]          data_to_instance2");
    verifyOutputContains("output               [3:0]          data_from_instance1");
    verifyOutputContains("output               [3:0]          data_from_instance2");

    verifyOutputContains("wire [7:0] idata_wire;");
    verifyOutputContains("wire [7:0] odata_wire;");

    verifyOutputContains("wire [7:0] instance1_idata;");
    verifyOutputContains("wire [7:0] instance1_odata;");

    verifyOutputContains("assign data_from_instance1 = odata_wire[3:0];");
    verifyOutputContains("assign data_from_instance2[3:0] = odata_wire[7:4];");
    verifyOutputContains("assign idata_wire[3:0] = data_to_instance1;");
    verifyOutputContains("assign idata_wire[7:4] = data_to_instance2[3:0];");

    verifyOutputContains("assign instance1_idata = idata_wire;");
    verifyOutputContains("assign odata_wire = instance1_odata;");

    verifyOutputContains(
        "    TestInstance instance1(\n"
        "        // Interface: data_if\n"
        "        .idata               (instance1_idata),\n"
        "        .odata               (instance1_odata)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addInterconnectToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInterconnection> tst_VerilogWriterFactory::addInterconnectToDesign(
    QSharedPointer<MetaInterface> first, QSharedPointer<MetaInterface> second, QString name)
{
    QSharedPointer<MetaInterconnection> mInterconnect(new MetaInterconnection);
    design_->getInterconnections()->append(mInterconnect);
    mInterconnect->name_ = name;

    first->upInterconnection_ = mInterconnect;
    second->upInterconnection_ = mInterconnect;

    return mInterconnect;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addWireToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<MetaWire> tst_VerilogWriterFactory::addWireToDesign(QString name,
    QString leftBound, QString rightBound, QSharedPointer<MetaInterconnection> mInterconnect)
{
    QSharedPointer<MetaWire> gw(new MetaWire);
    gw->name_ = name;
    gw->bounds_.first = leftBound;
    gw->bounds_.second = rightBound;

    if (mInterconnect)
    {
        mInterconnect->wires_.insert(gw->name_,gw);
    }
    else
    {
        design_->getAdHocWires()->append(gw);
    }

    return gw;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::createPortAssignment()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::createPortAssignment(QSharedPointer<MetaPort> mPort,
    QSharedPointer<MetaWire> wire,
    bool up,
    QString const& logicalLeft,
    QString const& logicalRight,
    QString const& physicalLeft,
    QString const& physicalRight)
{
    QSharedPointer<MetaPortAssignment> mpa(new MetaPortAssignment);
    mpa->logicalBounds_.first = logicalLeft;
    mpa->logicalBounds_.second = logicalRight;
    mpa->physicalBounds_.first = physicalLeft;
    mpa->physicalBounds_.second = physicalRight;
    mpa->wire_ = wire;
    mpa->invert_ = false;

    if (up)
    {
        mPort->upAssignments_.insert(wire->name_, mpa);
    }
    else
    {
        mPort->downAssignments_.insert(wire->name_, mpa);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addInterfaceToComponent()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInterface> tst_VerilogWriterFactory::addInterfaceToComponent(QString const& interfaceName,
    QSharedPointer<MetaInstance> component)
{
    QSharedPointer<MetaInterface> mInterface(new MetaInterface());
    component->getInterfaces()->insert(interfaceName,mInterface);

    QSharedPointer<BusInterface> bi = QSharedPointer<BusInterface>(new BusInterface);
    bi->setName(interfaceName);
    mInterface->interface_ = bi;
    component->getComponent()->getBusInterfaces()->append(bi);

    QSharedPointer<AbstractionType> at = QSharedPointer<AbstractionType>(new AbstractionType);
    bi->getAbstractionTypes()->append(at);

    return mInterface;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addTestComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_VerilogWriterFactory::addTestComponentToLibrary(VLNV vlnv)
{
    QSharedPointer<Component> instanceComponent(new Component(vlnv));

    library_.addComponent(instanceComponent);

	return instanceComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addInstanceToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInstance> tst_VerilogWriterFactory::addInstanceToDesign(QString instanceName,
    QSharedPointer<Component> component)
{
    QSharedPointer<ConfigurableVLNVReference> componentVLNV (new ConfigurableVLNVReference(component->getVlnv()));
    QSharedPointer<ComponentInstance> instance (new ComponentInstance(instanceName, componentVLNV));

    MessagePasser messages;

    QSharedPointer<MetaInstance> mInstance(new MetaInstance(instance,
        &library_, &messages, component, QSharedPointer<View>()));
    mInstance->parseInstance();

    design_->getInstances()->insert(instanceName, mInstance);

	return mInstance;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addInterfacesToInstance()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::addInterfacesToInstance(QSharedPointer<MetaInstance> mInstance)
{
    QSharedPointer<MetaInterface> mClkInterface = addInterfaceToComponent("clk_if", mInstance);

    QSharedPointer<MetaPort> mPort = addPort("clk", 1, DirectionTypes::IN, mInstance, mClkInterface);

    QSharedPointer<MetaInterface> mDataInterface = addInterfaceToComponent("data_if", mInstance);

    mPort = addPort("data_in", 8, DirectionTypes::IN, mInstance, mDataInterface);
    mPort = addPort("enable", 1, DirectionTypes::IN, mInstance, mDataInterface);
    mPort = addPort("full", 1, DirectionTypes::OUT, mInstance, mDataInterface);

    mPort = addPort("data_out", 8, DirectionTypes::OUT, mInstance);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testMasterToSlaveInterconnection()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance = addReceiver("receiver");

    QSharedPointer<MetaInterface> senderIf = senderInstance->getInterfaces()->value("data_if");
    QSharedPointer<MetaInterface> recvIf = receiverInstance->getInterfaces()->value("data_if");

    QSharedPointer<MetaInterconnection> mInterconnect = 
        addInterconnectToDesign(senderIf, recvIf, "data_connection");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",mInterconnect);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",mInterconnect);

    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(receiverInstance->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [7:0] sender_to_receiver_DATA;");
    verifyOutputContains("wire       sender_to_receiver_ENABLE;");

    verifyOutputContains("wire [7:0] receiver_data_in;");
    verifyOutputContains("wire       receiver_enable_in;");
    verifyOutputContains("wire [7:0] sender_data_out;");
    verifyOutputContains("wire       sender_enable_out;");

    verifyOutputContains("assign receiver_data_in = sender_to_receiver_DATA;");
    verifyOutputContains("assign receiver_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign sender_to_receiver_DATA = sender_data_out;");
    verifyOutputContains("assign sender_to_receiver_ENABLE = sender_enable_out;");

    verifyOutputContains(
        "    TestReceiver receiver(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver_data_in),\n"
        "        .enable_in           (receiver_enable_in)");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .enable_out          (sender_enable_out)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testOneBitSlicing()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testOneBitSlicing()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance = addReceiver("receiver");

    QSharedPointer<MetaInterface> senderIf = senderInstance->getInterfaces()->value("data_if");
    QSharedPointer<MetaInterface> recvIf = receiverInstance->getInterfaces()->value("data_if");

    QSharedPointer<MetaInterconnection> mInterconnect =
        addInterconnectToDesign(senderIf, recvIf, "data_connection");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",mInterconnect);

    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire, true, "5", "5", "7", "7");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "5", "5", "6", "6");

    runGenerator(true);

    verifyOutputContains("wire [7:0] sender_to_receiver_DATA;");

    verifyOutputContains("wire [7:0] receiver_data_in;");
    verifyOutputContains("wire [7:0] sender_data_out;");

    verifyOutputContains("assign receiver_data_in[7] = sender_to_receiver_DATA[5];");
    verifyOutputContains("assign sender_to_receiver_DATA[5] = sender_data_out[6];");

    verifyOutputContains(
        "    TestReceiver receiver(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver_data_in),\n"
        "        .enable_in           ()");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .enable_out          ()");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testPhysicalSlicedMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testPhysicalSlicedMasterToSlaveInterconnection()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance = addReceiver("receiver");

    QSharedPointer<MetaInterface> senderIf = senderInstance->getInterfaces()->value("data_if");
    QSharedPointer<MetaInterface> recvIf = receiverInstance->getInterfaces()->value("data_if");

    QSharedPointer<MetaInterconnection> mInterconnect =
        addInterconnectToDesign(senderIf, recvIf, "data_connection");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",mInterconnect);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",mInterconnect);

    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire, true, "7", "4", "7", "4");
    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire, true, "3", "0", "3", "0");
    createPortAssignment(receiverInstance->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "4", "7", "4");
    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "3", "0", "3", "0");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [7:0] sender_to_receiver_DATA;");
    verifyOutputContains("wire       sender_to_receiver_ENABLE;");

    verifyOutputContains("wire [7:0] receiver_data_in;");
    verifyOutputContains("wire       receiver_enable_in;");
    verifyOutputContains("wire [7:0] sender_data_out;");
    verifyOutputContains("wire       sender_enable_out;");

    verifyOutputContains("assign receiver_data_in[3:0] = sender_to_receiver_DATA[3:0];");
    verifyOutputContains("assign receiver_data_in[7:4] = sender_to_receiver_DATA[7:4];");
    verifyOutputContains("assign receiver_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign sender_to_receiver_DATA[3:0] = sender_data_out[3:0];");
    verifyOutputContains("assign sender_to_receiver_DATA[7:4] = sender_data_out[7:4];");
    verifyOutputContains("assign sender_to_receiver_ENABLE = sender_enable_out;");

    verifyOutputContains(
        "    TestReceiver receiver(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver_data_in),\n"
        "        .enable_in           (receiver_enable_in)");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .enable_out          (sender_enable_out)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testLogicalSlicedMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testLogicalSlicedMasterToSlaveInterconnection()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance = addReceiver("receiver");

    QSharedPointer<MetaInterface> senderIf = senderInstance->getInterfaces()->value("data_if");
    QSharedPointer<MetaInterface> recvIf = receiverInstance->getInterfaces()->value("data_if");

    QSharedPointer<MetaInterconnection> mInterconnect =
        addInterconnectToDesign(senderIf, recvIf, "data_connection");

    QSharedPointer<MetaWire> dataWire1 = addWireToDesign("sender_to_receiver_DATA1","3","0",mInterconnect);
    QSharedPointer<MetaWire> dataWire2 = addWireToDesign("sender_to_receiver_DATA2","3","0",mInterconnect);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",mInterconnect);

    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire1, true, "3", "0", "3", "0");
    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire2, true, "3", "0", "7", "4");
    createPortAssignment(receiverInstance->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire1, true, "3", "0", "3", "0");
    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire2, true, "3", "0", "7", "4");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [3:0] sender_to_receiver_DATA1;");
    verifyOutputContains("wire [3:0] sender_to_receiver_DATA2;");
    verifyOutputContains("wire       sender_to_receiver_ENABLE;");

    verifyOutputContains("wire [7:0] receiver_data_in;");
    verifyOutputContains("wire       receiver_enable_in;");
    verifyOutputContains("wire [7:0] sender_data_out;");
    verifyOutputContains("wire       sender_enable_out;");

    verifyOutputContains("assign receiver_data_in[3:0] = sender_to_receiver_DATA1;");
    verifyOutputContains("assign receiver_data_in[7:4] = sender_to_receiver_DATA2;");
    verifyOutputContains("assign receiver_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign sender_to_receiver_DATA1 = sender_data_out[3:0];");
    verifyOutputContains("assign sender_to_receiver_DATA2 = sender_data_out[7:4];");
    verifyOutputContains("assign sender_to_receiver_ENABLE = sender_enable_out;");

    verifyOutputContains(
        "    TestReceiver receiver(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver_data_in),\n"
        "        .enable_in           (receiver_enable_in)");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .enable_out          (sender_enable_out)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testLogicalSlicedMasterToMultiplePortsInSlave()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testLogicalSlicedMasterToMultiplePortsInSlave()
{
    QSharedPointer<Component> topComponent = createComponent("topComponent");
    QSharedPointer<Design> topDesign = createDesignForComponent("topDesign", topComponent);

    QSharedPointer<BusDefinition> busDefinition = createBusDefinition("testBus");
    QSharedPointer<AbstractionDefinition> abstractionDefinition = createAbstractionDefinition("testAbstraction");

    QSharedPointer<PortAbstraction> logicalData = addLogicalPortToAbstraction("DATA", abstractionDefinition,
        General::SYSTEM, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::IN);

    QSharedPointer<Component> instanceComponent = createComponent("Receiver");

    QSharedPointer<BusInterface> topBus = addBusInterfaceToComponent(
        "topBus", topComponent, General::SYSTEM, busDefinition, abstractionDefinition);
    QSharedPointer<BusInterface> instanceBus = addBusInterfaceToComponent(
        "instanceBus", instanceComponent, General::SYSTEM, busDefinition, abstractionDefinition);


    QSharedPointer<Port> topPort = addPortToComponent(
        "topPort", topComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::IN);
    QSharedPointer<Port> dataPort0 = addPortToComponent(
        "data0", instanceComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::IN);
    QSharedPointer<Port> dataPort1 = addPortToComponent(
        "data1", instanceComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::IN);
    QSharedPointer<Port> dataPort2 = addPortToComponent(
        "data2", instanceComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::IN);

    topPort->setLeftBound("2");
    topPort->setRightBound("0");

    dataPort0->setLeftBound("0");
    dataPort0->setRightBound("0");
    dataPort1->setLeftBound("0");
    dataPort1->setRightBound("0");
    dataPort2->setLeftBound("0");
    dataPort2->setRightBound("0");

    mapPortsToBusInterface(topBus, topPort, logicalData);

    QList<QSharedPointer<Port> > dataPorts;
    dataPorts.append(dataPort0);
    dataPorts.append(dataPort1);
    dataPorts.append(dataPort2);

    mapMultiplePortsToBusInterface(instanceBus, dataPorts, logicalData);

    QSharedPointer<ComponentInstance> targetInstance = addInstanceToDesign(topDesign, instanceComponent);

    QSharedPointer<Interconnection> wireConnection = addHierarchicalInterconnectionToDesign(
        "wireConnection", topDesign, topBus->name(), instanceBus->name(), targetInstance->getInstanceName());

    MessagePasser messages;

    connect(&messages, SIGNAL(errorMessage(const QString&)),
        this, SLOT(gatherErrorMessage(QString const&)), Qt::UniqueConnection);

    GenerationTuple input;
    input.component = topComponent;
    input.design = topDesign;
    input.messages = &messages;

    QList<QSharedPointer<MetaDesign> > designs =
        MetaDesign::parseHierarchy(&library_, input, topComponent->getViews()->first());

    QCOMPARE(designs.size(), 1);

    design_ = designs.first();
    runGenerator(true);

    output_;

    verifyOutputContains("input                [2:0]          topPort");

    verifyOutputContains("wire [2:0] wireConnection_DATA;");

    verifyOutputContains("wire       Receiver_instance_data0;");

    verifyOutputContains("assign wireConnection_DATA = topPort;");
    verifyOutputContains("assign Receiver_instance_data0 = wireConnection_DATA[0];");
    verifyOutputContains("assign Receiver_instance_data1 = wireConnection_DATA[1];");
    verifyOutputContains("assign Receiver_instance_data2 = wireConnection_DATA[2];");

    verifyOutputContains(
        "    Receiver Receiver_instance(\n"
        "        // Interface: instanceBus\n"
        "        .data0               (Receiver_instance_data0),\n"
        "        .data1               (Receiver_instance_data1),\n"
        "        .data2               (Receiver_instance_data2));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testHierarchicalInOutConnectionWithMultiplePorts()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testHierarchicalInOutConnectionWithMultiplePorts()
{
    QSharedPointer<Component> topComponent = createComponent("topComponent");
    QSharedPointer<Design> topDesign = createDesignForComponent("topDesign", topComponent);

    QString systemName("external_pad");
    QStringList systemGroups;
    systemGroups.append(systemName);

    QSharedPointer<BusDefinition> busDefinition = createBusDefinition("testBus");
    busDefinition->setSystemGroupNames(systemGroups);
    QSharedPointer<AbstractionDefinition> abstractionDefinition = createAbstractionDefinition("testAbstraction");

    QSharedPointer<PortAbstraction> logicalCSN = addLogicalPortToAbstraction("CSN", abstractionDefinition,
        General::SYSTEM, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT, systemName);
    QSharedPointer<PortAbstraction> logicalDATA = addLogicalPortToAbstraction("DATA", abstractionDefinition,
        General::SYSTEM, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT, systemName);
    QSharedPointer<PortAbstraction> logicalSCK = addLogicalPortToAbstraction("SCK", abstractionDefinition,
        General::SYSTEM, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT, systemName);

    logicalCSN->getWire()->getSystemPorts()->first()->setWidth("2");
    logicalCSN->getWire()->getSystemPorts()->first()->setPresence(PresenceTypes::REQUIRED);
    logicalDATA->getWire()->getSystemPorts()->first()->setWidth("4");
    logicalDATA->getWire()->getSystemPorts()->first()->setPresence(PresenceTypes::REQUIRED);
    logicalSCK->getWire()->getSystemPorts()->first()->setWidth("1");
    logicalSCK->getWire()->getSystemPorts()->first()->setPresence(PresenceTypes::REQUIRED);

    QSharedPointer<Port> topPortDATA0 = addPortToComponent(
        "topPortDATA0", topComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    topPortDATA0->setTypeName("wire");
    topPortDATA0->setPortSize(1);

    QSharedPointer<Port> topPortDATA1 = addPortToComponent(
        "topPortDATA1", topComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    topPortDATA1->setTypeName("wire");
    topPortDATA1->setPortSize(1);

    QSharedPointer<Port> topPortDATA2 = addPortToComponent(
        "topPortDATA2", topComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    topPortDATA2->setTypeName("wire");
    topPortDATA2->setPortSize(1);

    QSharedPointer<Port> topPortDATA3 = addPortToComponent(
        "topPortDATA3", topComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    topPortDATA3->setTypeName("wire");
    topPortDATA3->setPortSize(1);

    QSharedPointer<Port> topPortCSN1 = addPortToComponent(
        "topPortCSN1", topComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    topPortCSN1->setTypeName("wire");
    topPortCSN1->setPortSize(1);

    QSharedPointer<Port> topPortCSN2 = addPortToComponent(
        "topPortCSN2", topComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    topPortCSN2->setTypeName("wire");
    topPortCSN2->setPortSize(1);

    QSharedPointer<Port> topPortCLK = addPortToComponent(
        "topPortCLK", topComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    topPortCLK->setTypeName("wire");
    topPortCLK->setPortSize(1);

    QSharedPointer<BusInterface> topBus = addBusInterfaceToComponent(
        "topBus", topComponent, General::SYSTEM, busDefinition, abstractionDefinition, systemName);

    QList<QSharedPointer<Port> > csnPorts;
    csnPorts.append(topPortCSN1);
    csnPorts.append(topPortCSN2);

    QList<QSharedPointer<Port> > dataPorts;
    dataPorts.append(topPortDATA0);
    dataPorts.append(topPortDATA1);
    dataPorts.append(topPortDATA2);
    dataPorts.append(topPortDATA3);

    QList<QSharedPointer<Port> > sckPorts;
    sckPorts.append(topPortCLK);

    mapMultiplePortsToBusInterface(topBus, csnPorts, logicalCSN);
    mapMultiplePortsToBusInterface(topBus, dataPorts, logicalDATA);
    mapMultiplePortsToBusInterface(topBus, sckPorts, logicalSCK);

    QSharedPointer<Component> instanceComponent = createComponent("Receiver");

    QSharedPointer<Port> instanceCSN0 = addPortToComponent(
        "instanceCSN0", instanceComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    instanceCSN0->setTypeName("wire");
    instanceCSN0->setPortSize(1);

    QSharedPointer<Port> instanceCSN1 = addPortToComponent(
        "instanceCSN1", instanceComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    instanceCSN1->setTypeName("wire");
    instanceCSN1->setPortSize(1);

    QSharedPointer<Port> instanceDATA0 = addPortToComponent(
        "instanceDATA0", instanceComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    instanceDATA0->setTypeName("wire");
    instanceDATA0->setPortSize(1);

    QSharedPointer<Port> instanceDATA1 = addPortToComponent(
        "instanceDATA1", instanceComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    instanceDATA1->setTypeName("wire");
    instanceDATA1->setPortSize(1);

    QSharedPointer<Port> instanceDATA2 = addPortToComponent(
        "instanceDATA2", instanceComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    instanceDATA2->setTypeName("wire");
    instanceDATA2->setPortSize(1);

    QSharedPointer<Port> instanceDATA3 = addPortToComponent(
        "instanceDATA3", instanceComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    instanceDATA3->setTypeName("wire");
    instanceDATA3->setPortSize(1);

    QSharedPointer<Port> instanceSCK = addPortToComponent(
        "instanceSCK", instanceComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::INOUT);
    instanceSCK->setTypeName("wire");
    instanceSCK->setPortSize(1);

    QSharedPointer<BusInterface> instanceBus = addBusInterfaceToComponent(
        "pads", instanceComponent, General::SYSTEM, busDefinition, abstractionDefinition, systemName);

    QList<QSharedPointer<Port> > instanceCSNPorts;
    instanceCSNPorts.append(instanceCSN0);
    instanceCSNPorts.append(instanceCSN1);

    QList<QSharedPointer<Port> > instanceDATAPorts;
    instanceDATAPorts.append(instanceDATA0);
    instanceDATAPorts.append(instanceDATA1);
    instanceDATAPorts.append(instanceDATA2);
    instanceDATAPorts.append(instanceDATA3);

    QList<QSharedPointer<Port> > instanceSCKPorts;
    instanceSCKPorts.append(instanceSCK);

    mapMultiplePortsToBusInterface(instanceBus, instanceCSNPorts, logicalCSN);
    mapMultiplePortsToBusInterface(instanceBus, instanceDATAPorts, logicalDATA);
    mapMultiplePortsToBusInterface(instanceBus, instanceSCKPorts, logicalSCK);

    QSharedPointer<ComponentInstance> targetInstance = addInstanceToDesign(topDesign, instanceComponent);

    QSharedPointer<Interconnection> wireConnection = addHierarchicalInterconnectionToDesign(
        "wireConnection", topDesign, topBus->name(), instanceBus->name(), targetInstance->getInstanceName());

    MessagePasser messages;

    connect(&messages, SIGNAL(errorMessage(const QString&)),
        this, SLOT(gatherErrorMessage(QString const&)), Qt::UniqueConnection);

    GenerationTuple input;
    input.component = topComponent;
    input.design = topDesign;
    input.messages = &messages;

    QList<QSharedPointer<MetaDesign> > designs =
        MetaDesign::parseHierarchy(&library_, input, topComponent->getViews()->first());

    QCOMPARE(designs.size(), 1);

    design_ = designs.first();
    runGenerator(true);

    output_;

    verifyOutputContains(
        "    Receiver Receiver_instance(\n"
        "        // Interface: pads\n"
        "        .instanceCSN0        (topPortCSN1),\n"
        "        .instanceCSN1        (topPortCSN2),\n"
        "        .instanceDATA0       (topPortDATA0),\n"
        "        .instanceDATA1       (topPortDATA1),\n"
        "        .instanceDATA2       (topPortDATA2),\n"
        "        .instanceDATA3       (topPortDATA3),\n"
        "        .instanceSCK         (topPortCLK));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testPortSlicedMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testPortSlicedMasterToSlaveInterconnection()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance = addReceiver("receiver");

    QSharedPointer<MetaInterface> senderIf = senderInstance->getInterfaces()->value("data_if");
    addPort("data_out2", 4, DirectionTypes::OUT, senderInstance, senderIf);
    QSharedPointer<MetaInterface> recvIf = receiverInstance->getInterfaces()->value("data_if");
    addPort("data_in2", 4, DirectionTypes::IN, receiverInstance, recvIf);

    QSharedPointer<MetaInterconnection> mInterconnect =
        addInterconnectToDesign(senderIf, recvIf, "data_connection");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",mInterconnect);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",mInterconnect);

    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire, true, "7", "4", "3", "0");
    createPortAssignment(receiverInstance->getPorts()->value("data_in2"), dataWire, true, "3", "0", "3", "0");
    createPortAssignment(receiverInstance->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "4", "3", "0");
    createPortAssignment(senderInstance->getPorts()->value("data_out2"), dataWire, true, "3", "0", "3", "0");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [7:0] sender_to_receiver_DATA;");
    verifyOutputContains("wire       sender_to_receiver_ENABLE;");

    verifyOutputContains("wire [7:0] receiver_data_in;");
    verifyOutputContains("wire [3:0] receiver_data_in2;");
    verifyOutputContains("wire       receiver_enable_in;");
    verifyOutputContains("wire [7:0] sender_data_out;");
    verifyOutputContains("wire [3:0] sender_data_out2;");
    verifyOutputContains("wire       sender_enable_out;");

    verifyOutputContains("assign receiver_data_in[3:0] = sender_to_receiver_DATA[7:4];");
    verifyOutputContains("assign receiver_data_in2 = sender_to_receiver_DATA[3:0];");
    verifyOutputContains("assign receiver_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign sender_to_receiver_DATA[7:4] = sender_data_out[3:0];");
    verifyOutputContains("assign sender_to_receiver_DATA[3:0] = sender_data_out2;");
    verifyOutputContains("assign sender_to_receiver_ENABLE = sender_enable_out;");

    verifyOutputContains(
        "    TestReceiver receiver(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver_data_in),\n"
        "        .data_in2            (receiver_data_in2),\n"
        "        .enable_in           (receiver_enable_in)");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .data_out2           (sender_data_out2),\n"
        "        .enable_out          (sender_enable_out)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addSender()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInstance> tst_VerilogWriterFactory::addSender(QString const& instanceName)
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<Component> comp = addTestComponentToLibrary(senderVLNV);
    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign(instanceName, comp);

    QSharedPointer<MetaInterface> mInterface = addInterfaceToComponent("data_if", mInstance);
    mInterface->interface_->setInterfaceMode(General::MASTER);    
    QSharedPointer<MetaPort> mPort = addPort("enable_out", 1, DirectionTypes::OUT, mInstance, mInterface);
    mPort = addPort("data_out", 8, DirectionTypes::OUT, mInstance, mInterface);

	return mInstance;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addReceiver()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInstance> tst_VerilogWriterFactory::addReceiver(QString const& instanceName)
{
    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<Component> comp = addTestComponentToLibrary(receiverVLNV);
    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign(instanceName, comp);

    QSharedPointer<MetaInterface> mInterface = addInterfaceToComponent("data_if", mInstance);
    mInterface->interface_->setInterfaceMode(General::SLAVE);    
    QSharedPointer<MetaPort> gp = addPort("enable_in", 1, DirectionTypes::IN, mInstance, mInterface);
    gp = addPort("data_in", 8, DirectionTypes::IN, mInstance, mInterface);

    return mInstance;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testMasterToMultipleSlavesInterconnections()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testMasterToMultipleSlavesInterconnections()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance1 = addReceiver("receiver1");
    QSharedPointer<MetaInstance> receiverInstance2 = addReceiver("receiver2");

    QSharedPointer<MetaInterface> senderIf = senderInstance->getInterfaces()->value("data_if");
    QSharedPointer<MetaInterface> recvIf1 = receiverInstance1->getInterfaces()->value("data_if");
    QSharedPointer<MetaInterface> recvIf2 = receiverInstance2->getInterfaces()->value("data_if");

    QSharedPointer<MetaInterconnection> data_connection =
        addInterconnectToDesign(senderIf, recvIf1, "data_connection");
    QSharedPointer<MetaInterconnection> ena_connection =
        addInterconnectToDesign(senderIf, recvIf2, "ena_connection");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",data_connection);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",ena_connection);

    createPortAssignment(receiverInstance1->getPorts()->value("data_in"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(receiverInstance1->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");
    createPortAssignment(receiverInstance2->getPorts()->value("data_in"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(receiverInstance2->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [7:0] sender_to_receiver_DATA;");
    verifyOutputContains("wire       sender_to_receiver_ENABLE;");

    verifyOutputContains("wire [7:0] receiver1_data_in;");
    verifyOutputContains("wire       receiver1_enable_in;");
    verifyOutputContains("wire [7:0] receiver2_data_in;");
    verifyOutputContains("wire       receiver2_enable_in;");
    verifyOutputContains("wire [7:0] sender_data_out;");
    verifyOutputContains("wire       sender_enable_out;");

    verifyOutputContains("assign receiver1_data_in = sender_to_receiver_DATA;");
    verifyOutputContains("assign receiver1_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign receiver2_data_in = sender_to_receiver_DATA;");
    verifyOutputContains("assign receiver2_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign sender_to_receiver_DATA = sender_data_out;");
    verifyOutputContains("assign sender_to_receiver_ENABLE = sender_enable_out;");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver1_data_in),\n"
        "        .enable_in           (receiver1_enable_in)");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver2_data_in),\n"
        "        .enable_in           (receiver2_enable_in)");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .enable_out          (sender_enable_out)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testLogicalInvertBetweenInstances()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testLogicalInvertBetweenInstances()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance1 = addReceiver("receiver1");
   
    QSharedPointer<MetaInterface> senderIf = senderInstance->getInterfaces()->value("data_if");
    QSharedPointer<MetaInterface> recvIf1 = receiverInstance1->getInterfaces()->value("data_if");

    QSharedPointer<MetaInterconnection> enable_connection =
        addInterconnectToDesign(senderIf, recvIf1, "enable_connection");

    QSharedPointer<MetaWire> enableWire = addWireToDesign("sender_to_receiver_ENABLE", "0", "0", enable_connection);
    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA_LSB", "0", "0", enable_connection);

    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enableWire, true, "0", "0", "0", "0");
    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "0", "0", "0", "0");
    senderInstance->getPorts()->value("data_out")->upAssignments_.first()->invert_ = true;

    createPortAssignment(receiverInstance1->getPorts()->value("enable_in"), enableWire, true, "0", "0", "0", "0");
    receiverInstance1->getPorts()->value("enable_in")->upAssignments_.first()->invert_ = true;

    createPortAssignment(receiverInstance1->getPorts()->value("data_in"), dataWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire       sender_to_receiver_ENABLE;");
    verifyOutputContains("wire       receiver1_enable_in;");
    verifyOutputContains("wire       sender_enable_out;");

    verifyOutputContains("wire       sender_to_receiver_DATA_LSB;");
    verifyOutputContains("wire [7:0] receiver1_data_in;");
    verifyOutputContains("wire [7:0] sender_data_out;");

    verifyOutputContains("assign receiver1_enable_in = ~sender_to_receiver_ENABLE;");
    verifyOutputContains("assign sender_to_receiver_ENABLE = sender_enable_out;");

    verifyOutputContains("assign sender_to_receiver_DATA_LSB = ~sender_data_out[0];");
    verifyOutputContains("assign receiver1_data_in[0] = sender_to_receiver_DATA_LSB;");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver1_data_in),\n"
        "        .enable_in           (receiver1_enable_in)");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .enable_out          (sender_enable_out)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testLogicalInvertInHierarchicalConnection()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testLogicalInvertInHierarchicalConnection()
{
    QSharedPointer<MetaInterface> topDataIf = addInterfaceToComponent("data_bus", topComponent_);

    QSharedPointer<MetaPort> topEnable = addPort("enable_to_instance", 1, DirectionTypes::IN, topComponent_, topDataIf);
    QSharedPointer<MetaPort> topFull = addPort("full_from_instance", 1, DirectionTypes::OUT, topComponent_, topDataIf);

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");
    QSharedPointer<Component> comp = addTestComponentToLibrary(instanceVlnv);
    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign("instance1", comp);
    addInterfacesToInstance(mInstance);

    QSharedPointer<MetaInterface> instanceDataIf = mInstance->getInterfaces()->value("data_if");

    QSharedPointer<MetaInterconnection> mI1 = addInterconnectToDesign(topDataIf, instanceDataIf, "data_connection");
    mI1->hierIfs_.append(topDataIf);

    QSharedPointer<MetaWire> gw2 = addWireToDesign("enable_wire", "0", "0", mI1);
    gw2->hierPorts_.append(topEnable);
    QSharedPointer<MetaWire> gw3 = addWireToDesign("full_wire", "0", "0", mI1);
    gw3->hierPorts_.append(topFull);

    createPortAssignment(mInstance->getPorts()->value("enable"), gw2, true, "0", "0", "0", "0");
    mInstance->getPorts()->value("enable")->upAssignments_.first()->invert_ = true;

    createPortAssignment(mInstance->getPorts()->value("full"), gw3, true, "0", "0", "0", "0");
    mInstance->getPorts()->value("full")->upAssignments_.first()->invert_ = true;

    createPortAssignment(topEnable, gw2, false, "0", "0", "0", "0");
    createPortAssignment(topFull, gw3, false, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains(QString(
        "module TestComponent(\n"
        "    // Interface: data_bus\n"
        "    input                               enable_to_instance,\n"
        "    output                              full_from_instance\n"
        ");\n"
        "\n"
        "    // data_connection wires:\n"
        "    wire       enable_wire;\n"
        "    wire       full_wire;\n"
        "\n"
        "    // instance1 port wires:\n"        
        "    wire       instance1_enable;\n"
        "    wire       instance1_full;\n"
        "\n"
        "    // Assignments for the ports of the encompassing component:\n"
        "    assign enable_wire = enable_to_instance;\n"
        "    assign full_from_instance = full_wire;\n"
        "\n"
        "    // instance1 assignments:\n"
        "    assign instance1_enable = ~enable_wire;\n"
        "    assign full_wire = ~instance1_full;\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestInstance:1.0\n"
        "    TestInstance instance1(\n"
        "        // Interface: clk_if\n"
        "        .clk                 (),\n"
        "        // Interface: data_if\n"
        "        .data_in             (),\n"
        "        .enable              (instance1_enable),\n"
        "        .full                (instance1_full),\n"
        "        // These ports are not in any interface\n"
        "        .data_out            ());\n"
        "\n"
        "\n"
        "endmodule"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testAdhocConnectionBetweenComponentInstances()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testAdhocConnectionBetweenComponentInstances()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance1 = addReceiver("receiver1");
    QSharedPointer<MetaInstance> receiverInstance2 = addReceiver("receiver2");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("dataAdHoc","7","0");
    QSharedPointer<MetaWire> enaWire = addWireToDesign("enableAdHoc","0","0");

    createPortAssignment(receiverInstance1->getPorts()->value("data_in"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(receiverInstance1->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");
    createPortAssignment(receiverInstance2->getPorts()->value("data_in"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(receiverInstance2->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [7:0] dataAdHoc;");
    verifyOutputContains("wire       enableAdHoc;");

    verifyOutputContains("wire [7:0] receiver1_data_in;");
    verifyOutputContains("wire       receiver1_enable_in;");
    verifyOutputContains("wire [7:0] receiver2_data_in;");
    verifyOutputContains("wire       receiver2_enable_in;");
    verifyOutputContains("wire [7:0] sender_data_out;");
    verifyOutputContains("wire       sender_enable_out;");

    verifyOutputContains("assign receiver1_data_in = dataAdHoc;");
    verifyOutputContains("assign receiver1_enable_in = enableAdHoc;");
    verifyOutputContains("assign receiver2_data_in = dataAdHoc;");
    verifyOutputContains("assign receiver2_enable_in = enableAdHoc;");
    verifyOutputContains("assign dataAdHoc = sender_data_out;");
    verifyOutputContains("assign enableAdHoc = sender_enable_out;");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver1_data_in),\n"
        "        .enable_in           (receiver1_enable_in)");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver2_data_in),\n"
        "        .enable_in           (receiver2_enable_in)");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .enable_out          (sender_enable_out)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testPartSelectedAdhocConnectionBetweenComponentInstances()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testPartSelectedAdhocConnectionBetweenComponentInstances()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance1 = addReceiver("receiver1");
    QSharedPointer<MetaInstance> receiverInstance2 = addReceiver("receiver2");

    QSharedPointer<MetaWire> dataWire1 = addWireToDesign("dataAdHoc1","3","0");
    QSharedPointer<MetaWire> dataWire2 = addWireToDesign("dataAdHoc2","3","0");

    createPortAssignment(receiverInstance1->getPorts()->value("data_in"), dataWire1, true, "3", "0", "3", "0");
    createPortAssignment(receiverInstance2->getPorts()->value("data_in"), dataWire2, true, "3", "0", "3", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire1, true, "3", "0", "3", "0");
    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire2, true, "3", "0", "7", "4");

    runGenerator(true);

    verifyOutputContains("wire [3:0] dataAdHoc1;");
    verifyOutputContains("wire [3:0] dataAdHoc2;");

    verifyOutputContains("wire [7:0] receiver1_data_in;");
    verifyOutputContains("wire [7:0] receiver2_data_in;");
    verifyOutputContains("wire [7:0] sender_data_out;");

    verifyOutputContains("assign receiver1_data_in[3:0] = dataAdHoc1;");
    verifyOutputContains("assign receiver2_data_in[3:0] = dataAdHoc2;");
    verifyOutputContains("assign dataAdHoc1 = sender_data_out[3:0];");
    verifyOutputContains("assign dataAdHoc2 = sender_data_out[7:4];");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver1_data_in),\n"
        "        .enable_in           ()");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        // Interface: data_if\n"
        "        .data_in             (receiver2_data_in),\n"
        "        .enable_in           ()");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .enable_out          ()");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testAdhocInOutConnectionBetweenComponentInstances()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testAdhocInOutConnectionBetweenComponentInstances()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance1 = addReceiver("receiver1");
    QSharedPointer<MetaInstance> receiverInstance2 = addReceiver("receiver2");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("dataAdHoc","7","0");
    QSharedPointer<MetaWire> enaWire = addWireToDesign("enableAdHoc","0","0");

    createPortAssignment(receiverInstance1->getPorts()->value("data_in"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(receiverInstance2->getPorts()->value("data_in"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "0", "7", "0");

    receiverInstance1->getPorts()->first()->port_->setDirection(DirectionTypes::INOUT);
    receiverInstance2->getPorts()->first()->port_->setDirection(DirectionTypes::INOUT);
    senderInstance->getPorts()->first()->port_->setDirection(DirectionTypes::INOUT);

    runGenerator(true);

    verifyOutputContains("wire [7:0] dataAdHoc;");
    verifyOutputContains("wire       enableAdHoc;");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_if\n"
        "        .enable_in           (),\n"
        "        .data_in             (dataAdHoc)");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        // Interface: data_if\n"
        "        .enable_in           (),\n"
        "        .data_in             (dataAdHoc)");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .enable_out          (),\n"
        "        .data_out            (dataAdHoc)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testAdhocTieOffInComponentInstance()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testAdhocTieOffInComponentInstance()
{
    VLNV tieOffVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestTieOff", "1.0");
	QSharedPointer<Component> tieOffComponent(new Component(tieOffVLNV));

    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign("tieOffer", tieOffComponent);

    QString defaultName = "defaultValue";
    QString zeroName = "zeroTieOff";
    QString oneName = "oneTieOff";
    QString naName = "n/aTieOff";
    QString numberedName = "numberedTieOff";
    QString outName = "tieOffOut";
    QString inOutName = "tieOffInOut";
    QString unusedName = "unusedPort";
    QString slicedName = "slicedTieOff";

    QSharedPointer<MetaPort> defaultPort = addPort(defaultName, 4, DirectionTypes::IN, mInstance);
    defaultPort->defaultValue_ = "15";
    addPort(zeroName, 2, DirectionTypes::IN, mInstance);
    addPort(oneName, 4, DirectionTypes::IN, mInstance);
    addPort(naName, 0, DirectionTypes::IN, mInstance);
    addPort(numberedName, 10, DirectionTypes::IN, mInstance);
    addPort(outName, 2, DirectionTypes::OUT, mInstance);
    addPort(inOutName, 10, DirectionTypes::INOUT, mInstance);
    addPort(unusedName, 1, DirectionTypes::IN, mInstance);
    QSharedPointer<MetaPort> slicedPort = addPort(slicedName, 5, DirectionTypes::IN, mInstance);

    QSharedPointer<MetaPortAssignment> mpa1(new MetaPortAssignment);
    mpa1->defaultValue_ = "2";
    mpa1->physicalBounds_.first = "4";
    mpa1->physicalBounds_.second = "3";
    mpa1->invert_ = false;

    QSharedPointer<MetaPortAssignment> mpa0(new MetaPortAssignment);
    mpa0->defaultValue_ = "7";
    mpa0->physicalBounds_.first = "2";
    mpa0->physicalBounds_.second = "0";
    mpa0->invert_ = false;

    slicedPort->upAssignments_.insert("eka", mpa1);
    slicedPort->upAssignments_.insert("toka", mpa0);

    library_.addComponent(tieOffComponent);

    addDefaultPortAssignmentToPort(mInstance->getPorts()->value(zeroName),"0", true);
    addDefaultPortAssignmentToPort(mInstance->getPorts()->value(oneName),"1", true);
    addDefaultPortAssignmentToPort(mInstance->getPorts()->value(naName),"abc", true);
    addDefaultPortAssignmentToPort(mInstance->getPorts()->value(numberedName),"12", true);
    addDefaultPortAssignmentToPort(mInstance->getPorts()->value(outName),"0", true);
    addDefaultPortAssignmentToPort(mInstance->getPorts()->value(inOutName),"1", true);

    runGenerator(true);

    verifyOutputContains(
        "module TestComponent();\n"
        "\n"
        "    // tieOffer port wires:\n"
        "    wire [-1:0] tieOffer_n/aTieOff;\n"
        "    wire [9:0] tieOffer_numberedTieOff;\n"
        "    wire [3:0] tieOffer_oneTieOff;\n"
        "    wire [4:0] tieOffer_slicedTieOff;\n"
        "    wire [1:0] tieOffer_tieOffOut;\n"
        "    wire [1:0] tieOffer_zeroTieOff;\n"
        "\n"
        "    // tieOffer assignments:\n"
        "    assign tieOffer_n/aTieOff[-1:0] = abc;\n"
        "    assign tieOffer_numberedTieOff[9:0] = 12;\n"
        "    assign tieOffer_oneTieOff[3:0] = 1;\n"
        "    assign tieOffer_slicedTieOff[4:3] = 2;\n"
        "    assign tieOffer_slicedTieOff[2:0] = 7;\n"
        "    assign tieOffer_zeroTieOff[1:0] = 0;\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestTieOff:1.0\n"
        "    TestTieOff tieOffer(\n"
        "        // These ports are not in any interface\n"
        "        .defaultValue        (15),\n"
        "        .n/aTieOff           (tieOffer_n/aTieOff),\n"
        "        .numberedTieOff      (tieOffer_numberedTieOff),\n"
        "        .oneTieOff           (tieOffer_oneTieOff),\n"
        "        .slicedTieOff        (tieOffer_slicedTieOff),\n"
        "        .unusedPort          (),\n"
        "        .zeroTieOff          (tieOffer_zeroTieOff),\n"
        "        .tieOffOut           (tieOffer_tieOffOut),\n"
        "        .tieOffInOut         ());\n"
        "\n"
        "\n"
        "endmodule");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addDefaultPortAssignmentToPort()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::addDefaultPortAssignmentToPort(QSharedPointer<MetaPort> port,
    QString tieOff, bool up)
{
    QSharedPointer<MetaPortAssignment> mpa(new MetaPortAssignment);
    mpa->defaultValue_ = tieOff;
    mpa->physicalBounds_ = port->vectorBounds_;
    mpa->invert_ = false;

    if (up)
    {
        port->upAssignments_.insert("eka", mpa);
    }
    else
    {
        port->downAssignments_.insert("eka", mpa);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testHierarchicalAdhocConnection()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testHierarchicalAdhocConnection()
{
    QSharedPointer<MetaPort> topEna = addPort("enable_from_sender", 1, DirectionTypes::OUT, topComponent_);
    QSharedPointer<MetaPort> topData = addPort("data_from_sender", 8, DirectionTypes::OUT, topComponent_);

    QSharedPointer<MetaInstance> senderInstance = addSender("sender");;

    QSharedPointer<MetaWire> dataWire = addWireToDesign("dataAdHoc","7","0");
    dataWire->hierPorts_.append(topData);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("enableAdHoc","0","0");
    enaWire->hierPorts_.append(topEna);

    createPortAssignment(topData, dataWire, false, "7", "0", "7", "0");
    createPortAssignment(topEna, enaWire, false, "0", "0", "0", "0");
    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("output               [7:0]          data_from_sender");
    verifyOutputContains("output                              enable_from_sender");

    verifyOutputContains("wire [7:0] dataAdHoc;");
    verifyOutputContains("wire       enableAdHoc;");

    verifyOutputContains("wire [7:0] sender_data_out;");
    verifyOutputContains("wire       sender_enable_out;");

    verifyOutputContains("assign data_from_sender = dataAdHoc;");
    verifyOutputContains("assign enable_from_sender = enableAdHoc;");

    verifyOutputContains("assign dataAdHoc = sender_data_out;");
    verifyOutputContains("assign enableAdHoc = sender_enable_out;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .enable_out          (sender_enable_out)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testHierarchicalAdhocInOutConnection()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testHierarchicalAdhocInOutConnection()
{
    QSharedPointer<MetaPort> topData = addPort("data_from_sender", 8, DirectionTypes::INOUT, topComponent_);

    QSharedPointer<MetaInstance> senderInstance = addSender("sender");

    senderInstance->getPorts()->first()->port_->setDirection(DirectionTypes::INOUT);

    QSharedPointer<MetaWire> dataWire = addWireToDesign("dataAdHoc","7","0");
    dataWire->hierPorts_.append(topData);

    createPortAssignment(topData, dataWire, false, "7", "0", "7", "0");
    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "0", "7", "0");

    runGenerator(true);

    verifyOutputContains("inout                [7:0]          data_from_sender");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .enable_out          (),\n"
        "        .data_out            (data_from_sender[7:0])");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testHierarchicalAdHocTieOffValues()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testHierarchicalAdHocTieOffValues()
{
    QString defaultName = "defaultTieOff";
    QString zeroName = "zeroTieOff";
    QString oneName = "oneTieOff";
    QString naName = "n/aTieOff";
    QString numberedName = "numberedTieOff";
    QString inName = "tieOffIn";
    QString inOutName = "inOutTieOff";
    QString slicedName = "slicedTieOff";

    QSharedPointer<MetaPort> defaultPort = addPort(defaultName, 5, DirectionTypes::OUT, topComponent_);
    defaultPort->defaultValue_ = "7";
    addPort(zeroName, 2, DirectionTypes::OUT, topComponent_);
    addPort(oneName, 4, DirectionTypes::OUT, topComponent_);
    addPort(naName, 0, DirectionTypes::OUT, topComponent_);
    addPort(numberedName, 10, DirectionTypes::OUT, topComponent_);
    addPort(inName, 2, DirectionTypes::IN, topComponent_);
    addPort(inOutName, 10, DirectionTypes::INOUT, topComponent_);
    QSharedPointer<MetaPort> slicedPort = addPort(slicedName, 5, DirectionTypes::OUT, topComponent_);

    QSharedPointer<MetaPortAssignment> mpa1(new MetaPortAssignment);
    mpa1->defaultValue_ = "2";
    mpa1->physicalBounds_.first = "4";
    mpa1->physicalBounds_.second = "3";
    mpa1->invert_ = false;

    QSharedPointer<MetaPortAssignment> mpa0(new MetaPortAssignment);
    mpa0->defaultValue_ = "7";
    mpa0->physicalBounds_.first = "2";
    mpa0->physicalBounds_.second = "0";
    mpa0->invert_ = false;

    slicedPort->downAssignments_.insert("eka", mpa1);
    slicedPort->downAssignments_.insert("toka", mpa0);

    addDefaultPortAssignmentToPort(topComponent_->getPorts()->value(zeroName),"0", false);
    addDefaultPortAssignmentToPort(topComponent_->getPorts()->value(oneName),"1", false);
    addDefaultPortAssignmentToPort(topComponent_->getPorts()->value(naName),"abc", false);
    addDefaultPortAssignmentToPort(topComponent_->getPorts()->value(numberedName),"12", false);
    addDefaultPortAssignmentToPort(topComponent_->getPorts()->value(inName),"0", false);
    addDefaultPortAssignmentToPort(topComponent_->getPorts()->value(inOutName),"1", false);

    runGenerator(true);

    verifyOutputContains(
        "module TestComponent(\n"
        "    // These ports are not in any interface\n"
        "    input                [1:0]          tieOffIn,\n"
        "    output               [4:0]          defaultTieOff,\n"
        "    output               [-1:0]         n/aTieOff,\n"
        "    output               [9:0]          numberedTieOff,\n"
        "    output               [3:0]          oneTieOff,\n"
        "    output               [4:0]          slicedTieOff,\n"
        "    output               [1:0]          zeroTieOff,\n"
        "    inout                [9:0]          inOutTieOff\n"
        ");\n"
        "\n"
        "    // Assignments for the ports of the encompassing component:\n"
        "    assign inOutTieOff[9:0] = 1;\n"
        "    assign n/aTieOff[-1:0] = abc;\n"
        "    assign numberedTieOff[9:0] = 12;\n"
        "    assign oneTieOff[3:0] = 1;\n"
        "    assign slicedTieOff[4:3] = 2;\n"
        "    assign slicedTieOff[2:0] = 7;\n"
        "    assign zeroTieOff[1:0] = 0;\n"
        "\n"
        "    assign defaultTieOff = 7;\n"
        "\n"
        "\n"
        "endmodule");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testDescriptionAndVLNVIsPrintedAboveInstance()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testDescriptionAndVLNVIsPrintedAboveInstance()
{
    QFETCH(QString, description);
    QFETCH(QString, expectedOutput);
    
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(
		new ConfigurableVLNVReference(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0"));

	QSharedPointer<Component> comp = addTestComponentToLibrary(*instanceVLNV);

    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign("instance1", comp);
    mInstance->getComponentInstance()->setDescription(description);

    runGenerator(true);

    verifyOutputContains(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testDescriptionAndVLNVIsPrintedAboveInstance_data()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testDescriptionAndVLNVIsPrintedAboveInstance_data()
{
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("empty description") << "" <<
        "    // IP-XACT VLNV: Test:TestLibrary:TestComponent:1.0\n"
        "    TestComponent instance1(";
    QTest::newRow("one line description") << "Instance description." << 
        "    // Instance description.\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestComponent:1.0\n"
        "    TestComponent instance1(";

    QTest::newRow("multiline description") << 
        "Description on\n" 
        "multiple\n" 
        "lines." 
        << 
        "    // Description on\n"
        "    // multiple\n"
        "    // lines.\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestComponent:1.0\n"
        "    TestComponent instance1(";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testInstanceParametersAreWritten()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testInstanceParametersAreWritten()
{
    QSharedPointer<View> activeView(new View("rtl"));
    activeView->setComponentInstantiationRef("instance1");

    QSharedPointer<MetaInstance> sender = addSender("sender");

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setValueId("parameterId");
    parameter->setName("testParameter");
    parameter->setValue("2");
    parameter->setValueResolve("user");

    sender->getParameters()->append(parameter);

    runGenerator(true);

    verifyOutputContains(
        "    TestSender #(\n"
        "        .testParameter       (2))\n"
        "    sender(");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testAdhocConnectionToInterfacePorts()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testAdhocConnectionToInterfacePorts()
{
    //QSharedPointer<MetaInterface> topDataIf = addInterfaceToComponent("data_bus", topComponent_);

    QSharedPointer<MetaPort> topData = addPort("data_to_instance", 8, DirectionTypes::IN, topComponent_);
    QSharedPointer<MetaPort> topEnable = addPort("enable_to_instance", 1, DirectionTypes::IN, topComponent_);
    QSharedPointer<MetaPort> topFull = addPort("full_from_instance", 1, DirectionTypes::OUT, topComponent_);

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");
    QSharedPointer<Component> comp = addTestComponentToLibrary(instanceVlnv);
    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign("instance1", comp);

    QSharedPointer<MetaInterface> dataIf = addInterfaceToComponent("data_bus", mInstance);
    QSharedPointer<MetaPort> instanceData = addPort("data_in", 8, DirectionTypes::IN, mInstance, dataIf);
    QSharedPointer<MetaPort> instanceEnable = addPort("enable_in", 1, DirectionTypes::IN, mInstance, dataIf);
    QSharedPointer<MetaPort> instanceFull = addPort("full_out", 1, DirectionTypes::OUT, mInstance, dataIf);

    QSharedPointer<MetaWire> gw1 = addWireToDesign("data_wire", "7", "0");
    gw1->hierPorts_.append(topData);
    QSharedPointer<MetaWire> gw2 = addWireToDesign("enable_wire", "0", "0");
    gw2->hierPorts_.append(topEnable);
    QSharedPointer<MetaWire> gw3 = addWireToDesign("full_wire", "0", "0");
    gw3->hierPorts_.append(topFull);

    createPortAssignment(mInstance->getPorts()->value("data_in"), gw1, true, "7", "0", "7", "0");
    createPortAssignment(mInstance->getPorts()->value("enable_in"), gw2, true, "0", "0", "0", "0");
    createPortAssignment(mInstance->getPorts()->value("full_out"), gw3, true, "0", "0", "0", "0");

    createPortAssignment(topData, gw1, false, "7", "0", "7", "0");
    createPortAssignment(topEnable, gw2, false, "0", "0", "0", "0");
    createPortAssignment(topFull, gw3, false, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains(QString(
        "module TestComponent(\n"
        "    // These ports are not in any interface\n"
        "    input                [7:0]          data_to_instance,\n"
        "    input                               enable_to_instance,\n"
        "    output                              full_from_instance\n"
        ");\n"
        "\n"
        "    // Ad-hoc wires:\n"
        "    wire [7:0] data_wire;\n"
        "    wire       enable_wire;\n"
        "    wire       full_wire;\n"
        "\n"
        "    // instance1 port wires:\n"
        "    wire [7:0] instance1_data_in;\n"
        "    wire       instance1_enable_in;\n"
        "    wire       instance1_full_out;\n"
        "\n"
        "    // Assignments for the ports of the encompassing component:\n"
        "    assign data_wire = data_to_instance;\n"
        "    assign enable_wire = enable_to_instance;\n"
        "    assign full_from_instance = full_wire;\n"
        "\n"
        "    // instance1 assignments:\n"
        "    assign instance1_data_in = data_wire;\n"
        "    assign instance1_enable_in = enable_wire;\n"
        "    assign full_wire = instance1_full_out;\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestInstance:1.0\n"
        "    TestInstance instance1(\n"
        "        // Interface: data_bus\n"        
        "        .data_in             (instance1_data_in),\n"
        "        .enable_in           (instance1_enable_in),\n"
        "        .full_out            (instance1_full_out));\n"        
        "\n"
        "\n"
        "endmodule"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testFlatComponent()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testFlatComponent()
{
    addPort("clk", 1, DirectionTypes::IN, flatComponent_);
    addPort("rst_n", 1, DirectionTypes::IN, flatComponent_);
    addPort("dataOut", 8, DirectionTypes::OUT, flatComponent_);
    addPort("dataIn", 8, DirectionTypes::IN, flatComponent_);
    addParameter("dataWidth", "8", flatComponent_);
    addParameter("freq", "100000", flatComponent_);

    runGenerator(false);

    verifyOutputContains(QString(
        "module TestComponent #(\n"
        "    parameter                              dataWidth        = 8,\n"
        "    parameter                              freq             = 100000\n"
        ") (\n"
        "    // These ports are not in any interface\n"         
        "    input                               clk,\n"
        "    input                [7:0]          dataIn,\n"
        "    input                               rst_n,\n"
        "    output               [7:0]          dataOut\n"
        ");\n"
        "\n"
        "// " + VerilogSyntax::TAG_OVERRIDE + "\n"
        "endmodule\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testFlatComponentWithTypedParameter()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testFlatComponentWithTypedParameter()
{
    QSharedPointer<Parameter> testParameter = addParameter("dataWidth", "8", flatComponent_);
    testParameter->setAttribute(QStringLiteral("dataType"), "integer");
    addParameter("freq", "100000", flatComponent_);

    runGenerator(false);

    verifyOutputContains(QString(
        "module TestComponent #(\n"
        "    parameter integer                      dataWidth        = 8,\n"
        "    parameter                              freq             = 100000\n"
        ") ();\n"
        "\n"
        "// " + VerilogSyntax::TAG_OVERRIDE + "\n"
        "endmodule\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testDoNotWriteTransactionalPorts()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testDoNotWriteTransactionalPorts()
{
    addTransactional("provide", TransactionalTypes::PROVIDES, flatComponent_);
    addTransactional("require", TransactionalTypes::REQUIRES, flatComponent_);
    addTransactional("both", TransactionalTypes::BOTH, flatComponent_);
    addTransactional("phantom", TransactionalTypes::PHANTOM, flatComponent_);
    addPort("dataIn", 8, DirectionTypes::IN, flatComponent_);

    runGenerator(false);

    verifyOutputContains(QString(
        "module TestComponent(\n"
        "    // These ports are not in any interface\n"
        "    input                [7:0]          dataIn\n"
        ");\n"
        "\n"
        "// " + VerilogSyntax::TAG_OVERRIDE + "\n"
        "endmodule\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testDoNotWriteTransactionalInterfaces()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testDoNotWriteTransactionalInterfaces()
{
    QSharedPointer<MetaInterface> transactionalInterface =
        addInterfaceToComponent("transactionalInterface", topComponent_);
    QSharedPointer<MetaInterface> wireInterface = addInterfaceToComponent("wireInterface", topComponent_);

    addTransactional("provide", TransactionalTypes::PROVIDES, topComponent_, transactionalInterface);
    addTransactional("require", TransactionalTypes::REQUIRES, topComponent_, transactionalInterface);
    addTransactional("both", TransactionalTypes::BOTH, topComponent_, transactionalInterface);
    addTransactional("phantom", TransactionalTypes::PHANTOM, topComponent_, transactionalInterface);
    addPort("clk", 1, DirectionTypes::IN, flatComponent_, wireInterface);
    addPort("rst_n", 1, DirectionTypes::IN, flatComponent_, wireInterface);
    addTransactional("otherBoat", TransactionalTypes::BOTH, topComponent_, wireInterface);
    addPort("dataIn", 8, DirectionTypes::IN, flatComponent_);

    runGenerator(false);

    verifyOutputContains(QString(
        "module TestComponent(\n"
        "    // Interface: wireInterface\n"
        "    input                               clk,\n"
        "    input                               rst_n,\n"
        "\n"
        "    // These ports are not in any interface\n"
        "    input                [7:0]          dataIn\n"
        ");\n"
        "\n"
        "// " + VerilogSyntax::TAG_OVERRIDE + "\n"
        "endmodule\n"
    ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addTransactional()
//-----------------------------------------------------------------------------
QSharedPointer<MetaPort> tst_VerilogWriterFactory::addTransactional(QString const& portName,
    TransactionalTypes::Initiative initiative, QSharedPointer<MetaComponent> component,
    QSharedPointer<MetaInterface> containingInterface /* = QSharedPointer<MetaInterface>() */)
{
    QSharedPointer<Transactional> portTransactional(new Transactional());
    portTransactional->setInitiative(TransactionalTypes::initiativeToString(initiative));

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName));
    port->setTransactional(portTransactional);

    QSharedPointer<MetaPort> newMetaPort(new MetaPort);
    newMetaPort->isWire_ = false;
    newMetaPort->isTransactional_ = true;
    newMetaPort->port_ = port;
    
    component->getPorts()->insert(portName, newMetaPort);
    component->getComponent()->getPorts()->append(port);

    if (containingInterface)
    {
        containingInterface->ports_.insert(portName, newMetaPort);

        QSharedPointer<PortMap> newPortMap(new PortMap);
        QSharedPointer<PortMap::PhysicalPort> portMapPhysicalPort(new PortMap::PhysicalPort);
        portMapPhysicalPort->name_ = portName;
        newPortMap->setPhysicalPort(portMapPhysicalPort);

        containingInterface->interface_->getAbstractionTypes()->first()->getPortMaps()->append(newPortMap);
    }

    return newMetaPort;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testDoNotWriteTransactionalConnections()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testDoNotWriteTransactionalConnections()
{
    QSharedPointer<Component> topComponent = createComponent("topComponent");
    QSharedPointer<Design> topDesign = createDesignForComponent("topDesign", topComponent);

    QSharedPointer<BusDefinition> busDefinition = createBusDefinition("testBus");
    QSharedPointer<AbstractionDefinition> abstractionDefinition = createAbstractionDefinition("testAbstraction");

    QSharedPointer<PortAbstraction> logicalProvider = addLogicalPortToAbstraction(
        "logicalTransactional", abstractionDefinition, General::MASTER, TransactionalTypes::PROVIDES);
    QSharedPointer<PortAbstraction> logicalRequirer = addLogicalPortToAbstraction(
        "logicalTransactional", abstractionDefinition, General::SLAVE, TransactionalTypes::REQUIRES);

    QSharedPointer<PortAbstraction> logicalIn = addLogicalPortToAbstraction("logicalWire", abstractionDefinition,
        General::MASTER, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::IN);
    QSharedPointer<PortAbstraction> logicalOut = addLogicalPortToAbstraction("logicalWire", abstractionDefinition,
        General::SLAVE, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::OUT);

    QSharedPointer<Component> transactionProviderComponent = createComponent("TestProvider");
    QSharedPointer<Component> transactionRequirerComponent = createComponent("TestRequirer");

    QSharedPointer<BusInterface> providerBus = addBusInterfaceToComponent(
        "providerBus", transactionProviderComponent, General::MASTER, busDefinition, abstractionDefinition);
    QSharedPointer<BusInterface> requirerBus = addBusInterfaceToComponent(
        "requirerBus", transactionRequirerComponent, General::SLAVE, busDefinition, abstractionDefinition);

    QSharedPointer<BusInterface> wireInBus = addBusInterfaceToComponent(
        "wireInBus", transactionProviderComponent, General::MASTER, busDefinition, abstractionDefinition);
    QSharedPointer<BusInterface> wireOutBus = addBusInterfaceToComponent(
        "wireOutBus", transactionRequirerComponent, General::SLAVE, busDefinition, abstractionDefinition);

    QSharedPointer<Port> providerPort =
        addPortToComponent("providerPort", transactionProviderComponent, TransactionalTypes::PROVIDES);
    QSharedPointer<Port> requirerPort =
        addPortToComponent("requirerPort", transactionRequirerComponent, TransactionalTypes::REQUIRES);

    QSharedPointer<Port> wireInPort = addPortToComponent(
        "wireInPort", transactionProviderComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::IN);
    QSharedPointer<Port> wireOutPort = addPortToComponent(
        "wireOutPort", transactionRequirerComponent, TransactionalTypes::INITIATIVE_INVALID, DirectionTypes::OUT);

    mapPortsToBusInterface(providerBus, providerPort, logicalProvider);
    mapPortsToBusInterface(requirerBus, requirerPort, logicalRequirer);

    mapPortsToBusInterface(wireInBus, wireInPort, logicalIn);
    mapPortsToBusInterface(wireOutBus, wireOutPort, logicalOut);

    QSharedPointer<ComponentInstance> providerInstance =
        addInstanceToDesign(topDesign, transactionProviderComponent);
    QSharedPointer<ComponentInstance> requirerInstance =
        addInstanceToDesign(topDesign, transactionRequirerComponent);

    QSharedPointer<Interconnection> transactionalConnection = addInterconnectionToDesign("transactionalConnection",
        topDesign, providerBus->name(), providerInstance->getInstanceName(), requirerBus->name(),
        requirerInstance->getInstanceName());

    QSharedPointer<Interconnection> wireConnection = addInterconnectionToDesign("wireConnection", topDesign,
        wireInBus->name(), providerInstance->getInstanceName(), wireOutBus->name(),
        requirerInstance->getInstanceName());

    MessagePasser messages;

    connect(&messages, SIGNAL(errorMessage(const QString&)),
        this, SLOT(gatherErrorMessage(QString const&)), Qt::UniqueConnection);

    GenerationTuple input;
    input.component = topComponent;
    input.design = topDesign;
    input.messages = &messages;

    QList<QSharedPointer<MetaDesign> > designs =
        MetaDesign::parseHierarchy(&library_, input, topComponent->getViews()->first());

    QCOMPARE(designs.size(), 1);

    design_ = designs.first();
    runGenerator(true);

    output_;

    QCOMPARE(errorMessages_.isEmpty(), true);

    verifyOutputContains("module topComponent();\n\n"
        "    // wireConnection wires:\n"
        "    wire       wireConnection_logicalWire;");

    verifyOutputContains("    // TestProvider_instance port wires:\n"
        "    wire       TestProvider_instance_wireInPort;\n"
        "    // TestRequirer_instance port wires:\n"
        "    wire       TestRequirer_instance_wireOutPort;");

    verifyOutputContains("    // TestProvider_instance assignments:\n"
        "    assign TestProvider_instance_wireInPort = wireConnection_logicalWire;\n"
        "    // TestRequirer_instance assignments:\n"
        "    assign wireConnection_logicalWire = TestRequirer_instance_wireOutPort;");

    verifyOutputContains("    // IP-XACT VLNV: Test:TestLibrary:TestProvider:1.0\n"
        "    TestProvider TestProvider_instance(\n"
        "        // Interface: wireInBus\n"
        "        .wireInPort          (TestProvider_instance_wireInPort));");

    verifyOutputContains("    // IP-XACT VLNV: Test:TestLibrary:TestRequirer:1.0\n"
        "    TestRequirer TestRequirer_instance(\n"
        "        // Interface: wireOutBus\n"
        "        .wireOutPort         (TestRequirer_instance_wireOutPort));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::createComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_VerilogWriterFactory::createComponent(QString const& componentName)
{
    VLNV providerVLNV(VLNV::COMPONENT, "Test", "TestLibrary", componentName, "1.0");
    QSharedPointer<Component> transactionProviderComponent = addTestComponentToLibrary(providerVLNV);

    return transactionProviderComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::createDesignForComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Design> tst_VerilogWriterFactory::createDesignForComponent(QString const& designName,
    QSharedPointer<Component> component)
{
    VLNV designVLNV(VLNV::DESIGN, "Test", "TestLibrary", designName, "1.0");
    QSharedPointer<Design> newDesign(new Design(designVLNV));
    library_.addComponent(newDesign);

    QSharedPointer<DesignInstantiation> newDesignInstantiation(new DesignInstantiation("designInstantiation"));
    component->getDesignInstantiations()->append(newDesignInstantiation);

    QSharedPointer<View> newView(new View("hierarchical"));
    newView->setDesignInstantiationRef(newDesignInstantiation->name());
    component->getViews()->append(newView);

    return newDesign;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::createBusDefinition()
//-----------------------------------------------------------------------------
QSharedPointer<BusDefinition> tst_VerilogWriterFactory::createBusDefinition(QString const& definitionName)
{
    VLNV busVLNV(VLNV::BUSDEFINITION, "Test", "TestLibrary", definitionName, "1.0");
    QSharedPointer<BusDefinition> newBus(new BusDefinition());
    newBus->setVlnv(busVLNV);
    library_.addComponent(newBus);

    return newBus;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::createAbstractionDefinition()
//-----------------------------------------------------------------------------
QSharedPointer<AbstractionDefinition> tst_VerilogWriterFactory::createAbstractionDefinition(
    QString const& definitionName)
{
    VLNV abstractionVLNV(VLNV::BUSDEFINITION, "Test", "TestLibrary", definitionName, "1.0");
    QSharedPointer<AbstractionDefinition> newAbstraction(new AbstractionDefinition());
    newAbstraction->setVlnv(abstractionVLNV);
    library_.addComponent(newAbstraction);

    return newAbstraction;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addPortToComponent()
//-----------------------------------------------------------------------------
QSharedPointer<Port> tst_VerilogWriterFactory::addPortToComponent(QString const& portName,
    QSharedPointer<Component> containingComponent, TransactionalTypes::Initiative newInitiative,
    DirectionTypes::Direction newDirection)
{
    QSharedPointer<Port> newPort = QSharedPointer<Port>(new Port(portName));

    if (newDirection != DirectionTypes::DIRECTION_INVALID)
    {
        QSharedPointer<Wire> newPortWire(new Wire());
        newPortWire->setDirection(newDirection);

        newPort->setWire(newPortWire);
    }
    else if (newInitiative != TransactionalTypes::INITIATIVE_INVALID)
    {
        QSharedPointer<Transactional> newPortTransactional(new Transactional());
        newPortTransactional->setInitiative(TransactionalTypes::initiativeToString(newInitiative));

        newPort->setTransactional(newPortTransactional);
    }

    containingComponent->getPorts()->append(newPort);
    return newPort;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addBusInterfaceToComponent()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> tst_VerilogWriterFactory::addBusInterfaceToComponent(QString const& busName,
    QSharedPointer<Component> containingComponent, General::InterfaceMode busMode,
    QSharedPointer<BusDefinition> busDefinition, QSharedPointer<AbstractionDefinition> abstraction,
    QString const& systemGroup /* = "" */)
{
    const ConfigurableVLNVReference busVLNV(busDefinition->getVlnv());
    QSharedPointer<ConfigurableVLNVReference> abstractionVLNV(
        new ConfigurableVLNVReference(abstraction->getVlnv()));

    QSharedPointer<BusInterface> newInterface(new BusInterface());
    newInterface->setName(busName);
    newInterface->setInterfaceMode(busMode);
    newInterface->setBusType(busVLNV);

    if (!systemGroup.isEmpty())
    {
        newInterface->setSystem(systemGroup);
    }

    QSharedPointer<AbstractionType> newAbstractionType(new AbstractionType());
    newAbstractionType->setAbstractionRef(abstractionVLNV);

    newInterface->getAbstractionTypes()->append(newAbstractionType);

    containingComponent->getBusInterfaces()->append(newInterface);
    return newInterface;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addLogicalPortToAbstraction()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> tst_VerilogWriterFactory::addLogicalPortToAbstraction(QString const& portName,
    QSharedPointer<AbstractionDefinition> containingAbstraction, General::InterfaceMode busMode,
    TransactionalTypes::Initiative logicalInitiative /* = TransactionalTypes::INITIATIVE_INVALID */,
    DirectionTypes::Direction logicalDirection /* = DirectionTypes::DIRECTION_INVALID */,
    QString const& systemGroup /* = "" */)
{
    QSharedPointer<PortAbstraction> newLogical = containingAbstraction->getPort(portName);
    if (!newLogical)
    {
        newLogical = QSharedPointer<PortAbstraction>(new PortAbstraction());
        newLogical->setName(portName);

        containingAbstraction->getLogicalPorts()->append(newLogical);
    }

    if (logicalDirection != DirectionTypes::DIRECTION_INVALID)
    {
        QSharedPointer<WireAbstraction> newWireAbstraction = newLogical->getWire();
        if (!newWireAbstraction)
        {
            newWireAbstraction = QSharedPointer<WireAbstraction>(new WireAbstraction());
            newLogical->setWire(newWireAbstraction);
        }

        QSharedPointer<WirePort> newWirePort(new WirePort());
        if (busMode == General::MASTER)
        {
            newWireAbstraction->setMasterPort(newWirePort);
        }
        else if (busMode == General::SLAVE)
        {
            newWireAbstraction->setSlavePort(newWirePort);
        }
        else if (busMode == General::SYSTEM)
        {
            newWirePort->setSystemGroup(systemGroup);
            newWireAbstraction->addSystemPort(newWirePort);
        }
    }
    else if (logicalInitiative != TransactionalTypes::INITIATIVE_INVALID)
    {
        QSharedPointer<TransactionalAbstraction> newTransactionalAbstraction = newLogical->getTransactional();
        if (!newTransactionalAbstraction)
        {
            newTransactionalAbstraction = QSharedPointer<TransactionalAbstraction>(new TransactionalAbstraction());
            newLogical->setTransactional(newTransactionalAbstraction);
        }

        QSharedPointer<TransactionalPort> newTransactionalPort(new TransactionalPort());
        if (busMode == General::MASTER)
        {
            newTransactionalAbstraction->setMasterPort(newTransactionalPort);
        }
        else if (busMode == General::SLAVE)
        {
            newTransactionalAbstraction->setSlavePort(newTransactionalPort);
        }
        else if (busMode == General::SYSTEM)
        {
            newTransactionalPort->setSystemGroup(systemGroup);
            newTransactionalAbstraction->addSystemPort(newTransactionalPort);
        }
    }

    return newLogical;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::mapPortsToBusInterface()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::mapPortsToBusInterface(QSharedPointer<BusInterface> containingBus,
    QSharedPointer<Port> physicalPort, QSharedPointer<PortAbstraction> logicalPort)
{
    QSharedPointer<PortMap> newPortMap(new PortMap());

    QSharedPointer<PortMap::PhysicalPort> newPhysical(new PortMap::PhysicalPort(physicalPort->name()));
    QSharedPointer<PortMap::LogicalPort> newLogical(new PortMap::LogicalPort(logicalPort->name()));

    newPortMap->setPhysicalPort(newPhysical);
    newPortMap->setLogicalPort(newLogical);

    containingBus->getAbstractionTypes()->first()->getPortMaps()->append(newPortMap);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::mapMultiplePortsToBusInterface()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::mapMultiplePortsToBusInterface(QSharedPointer<BusInterface> containingBus,
    QList<QSharedPointer<Port> > physicalPorts, QSharedPointer<PortAbstraction> logicalPort)
{
    int logicalIndex = 0;

    for (auto port : physicalPorts)
    {
        QSharedPointer<PortMap> newPortMap(new PortMap());

        QSharedPointer<PortMap::PhysicalPort> newPhysical(new PortMap::PhysicalPort(port->name()));
        QSharedPointer<PortMap::LogicalPort> newLogical(new PortMap::LogicalPort(logicalPort->name()));

        QString logicalLeft = QString::number(logicalIndex);

        int portLeft = port->getLeftBound().toInt();
        int portRight = port->getRightBound().toInt();

        int portWidth = abs(portLeft - portRight) + 1;

        QString rangeLeft(QString::number(logicalIndex));
        QString rangeRight(QString::number(logicalIndex + portWidth - 1));

        logicalIndex = logicalIndex + portWidth;

        QSharedPointer<Range> newRange(new Range(rangeLeft, rangeRight));
        newLogical->range_ = newRange;

        newPortMap->setPhysicalPort(newPhysical);
        newPortMap->setLogicalPort(newLogical);

        containingBus->getAbstractionTypes()->first()->getPortMaps()->append(newPortMap);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addInstanceToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> tst_VerilogWriterFactory::addInstanceToDesign(
    QSharedPointer<Design> containingDesign, QSharedPointer<Component> instancedComponent)
{
    QSharedPointer<ConfigurableVLNVReference> newConfigurableVLNV(
        new ConfigurableVLNVReference(instancedComponent->getVlnv()));

    QString instanceName = instancedComponent->getVlnv().getName() + "_instance";
    QSharedPointer<ComponentInstance> newComponentInstance(
        new ComponentInstance(instanceName, newConfigurableVLNV));

    containingDesign->getComponentInstances()->append(newComponentInstance);
    return newComponentInstance;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addInterconnectionToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Interconnection> tst_VerilogWriterFactory::addInterconnectionToDesign(
    QString const& interconnectionName, QSharedPointer<Design> containingDesign, QString const& startInterfaceName,
    QString const& startInstanceName, QString const& secondInterfaceName, QString const& secondInstanceName)
{
    QSharedPointer<ActiveInterface> startInterface(new ActiveInterface(startInstanceName, startInterfaceName));
    QSharedPointer<ActiveInterface> secondInterface(new ActiveInterface(secondInstanceName, secondInterfaceName));

    QSharedPointer<Interconnection> newInterconnection(new Interconnection(interconnectionName, startInterface));
    newInterconnection->getActiveInterfaces()->append(secondInterface);

    containingDesign->getInterconnections()->append(newInterconnection);
    return newInterconnection;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addHierarchicalInterconnectionToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Interconnection> tst_VerilogWriterFactory::addHierarchicalInterconnectionToDesign(
    QString const& interconnectionName, QSharedPointer<Design> containingDesign, QString const& topInterface,
    QString const& targetInterfaceName, QString const& targetInstanceName)
{
    QSharedPointer<HierInterface> startInterface(new HierInterface(topInterface));
    QSharedPointer<ActiveInterface> secondInterface(new ActiveInterface(targetInstanceName, targetInterfaceName));

    QSharedPointer<Interconnection> newInterconnection(new Interconnection(interconnectionName, secondInterface));
    newInterconnection->getHierInterfaces()->append(startInterface);

    containingDesign->getInterconnections()->append(newInterconnection);
    return newInterconnection;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::gatherErrorMessage()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::gatherErrorMessage(QString const& errorMessage)
{
    errorMessages_.append(errorMessage);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testGenerationWithImplementation()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testGenerationWithImplementation()
{
	QString contentBefore(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n"
		"assign adr_slave_1 = adr_master;\n"
		"assign cyc_slave_1 = cyc_master;\n"
		"endmodule\n"
		);

	QFile existingFile("TestComponent.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	outputStream << contentBefore;

	existingFile.close();

	addPort("clk", 1, DirectionTypes::IN, flatComponent_);
	addPort("rst_n", 1, DirectionTypes::IN, flatComponent_);
	addPort("dataOut", 8, DirectionTypes::OUT, flatComponent_);
	addPort("dataIn", 8, DirectionTypes::IN, flatComponent_);
	addParameter("dataWidth", "8", flatComponent_);
	addParameter("freq", "100000", flatComponent_);

	runGenerator(false);

	QString contentAfter(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input                [7:0]          dataIn,\n"
		"    input                               rst_n,\n"
		"    output               [7:0]          dataOut\n"
		");\n"
		"\n"
		"// " + VerilogSyntax::TAG_OVERRIDE + "\n"
		"assign adr_slave_1 = adr_master;\n"
		"assign cyc_slave_1 = cyc_master;\n"
		"endmodule\n"
		);

	verifyOutputContains(contentAfter);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testGenerationWithImplementationWithTag()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testGenerationWithImplementationWithTag()
{
	QString content(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input                [7:0]          dataIn,\n"
		"    input                               rst_n,\n"
		"    output               [7:0]          dataOut\n"
		");\n"
		"\n"
		"// " + VerilogSyntax::TAG_OVERRIDE + "\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		);

	QFile existingFile("TestComponent.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	outputStream << content;

	existingFile.close();

	addPort("clk", 1, DirectionTypes::IN, flatComponent_);
	addPort("rst_n", 1, DirectionTypes::IN, flatComponent_);
	addPort("dataOut", 8, DirectionTypes::OUT, flatComponent_);
	addPort("dataIn", 8, DirectionTypes::IN, flatComponent_);
	addParameter("dataWidth", "8", flatComponent_);
	addParameter("freq", "100000", flatComponent_);

	runGenerator(false);

	verifyOutputContains(content);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::testGenerationWithImplementationWithPostModule()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testGenerationWithImplementationWithPostModule()
{
	QString content(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input                [7:0]          dataIn,\n"
		"    input                               rst_n,\n"
		"    output               [7:0]          dataOut\n"
		");\n"
		"\n"
		"// " + VerilogSyntax::TAG_OVERRIDE + "\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		"additional stuff\n"
		"more stuff"
		);

	QFile existingFile("TestComponent.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	outputStream << content;

	existingFile.close();

	addPort("clk", 1, DirectionTypes::IN, flatComponent_);
	addPort("rst_n", 1, DirectionTypes::IN, flatComponent_);
	addPort("dataOut", 8, DirectionTypes::OUT, flatComponent_);
	addPort("dataIn", 8, DirectionTypes::IN, flatComponent_);
	addParameter("dataWidth", "8", flatComponent_);
	addParameter("freq", "100000", flatComponent_);

	runGenerator(false);

	verifyOutputContains(content);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::verifyOutputContains()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::verifyOutputContains(QString const& expectedOutput)
{
    if (!output_.contains(expectedOutput))
    {
        QStringList outputLines = output_.split("\n");
        QStringList expectedLines = expectedOutput.split("\n");

        QVERIFY(outputLines.count() >= expectedLines.count());

        int lineOffset = outputLines.indexOf(expectedLines.first());

        if (lineOffset == -1)
        {
            compareOutputTo(expectedOutput);
        }
        else
        {
            int lineCount = expectedLines.count();
            for (int i = 0; i < lineCount; i++)
            {
                QCOMPARE(outputLines.at(i + lineOffset), expectedLines.at(i));
            }
        }
    }
    else if(output_.count(expectedOutput) != 1)
    {
        QVERIFY2(false, QString(expectedOutput + " was found " + QString::number(output_.count(expectedOutput))
            + " times in output.").toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::compareLineByLine()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::compareOutputTo(QString const& expectedOutput)
{
    QCOMPARE(output_, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_VerilogWriterFactory)

#include "tst_VerilogWriterFactory.moc"

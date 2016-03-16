//-----------------------------------------------------------------------------
// File: tst_VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Unit test for class VerilogGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/common/ModuleParameter.h>

#include <Plugins/VerilogGenerator/VerilogGenerator/VerilogGenerator.h>

#include <tests/MockObjects/LibraryMock.h>
#include "IPXACTmodels/Component/ComponentInstantiation.h"

class tst_VerilogGenerator : public QObject
{
    Q_OBJECT

public:
    tst_VerilogGenerator();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test cases:
    void testTopLevelComponent();
    void testTopLevelComponentExpressions();

    void testConsecutiveParseCalls();

    void testFileHeaderIsPrinted();

    void testHierarchicalConnections();
    void testHierarchicalConnectionsWithExpressions();
    void testSlicedHierarchicalConnection();
    void testUnknownInstanceIsNotWritten();

    void testMasterToSlaveInterconnection();
    void testMasterToSlaveInterconnectionWithExpressions();
    void testMasterToMultipleSlavesInterconnections();
    void testInterconnectionToVaryingSizeLogicalMaps();
    void testSlicedInterconnection();
    void testMasterInterconnectionToMirroredMaster();
    void testMirroredSlaveInterconnectionToSlaves();  

    void testAdhocConnectionBetweenComponentInstances();    
    void testAdHocConnectionBetweenComponentInstancesWithExpressions();
    void testHierarchicalAdhocConnection();
    void testAdHocConnectionToUnknownInstanceIsNotWritten();

    void testDescriptionAndVLNVIsPrintedAboveInstance();
    void testDescriptionAndVLNVIsPrintedAboveInstance_data();

    void testTopLevelModuleParametersAreWritten();

private:

    void addPort( QString const& portName, int portSize, DirectionTypes::Direction direction, 
        QSharedPointer<Component> component);

    void addModuleParameter( QString const& name, QString const& value );

    void runGenerator(QString const& activeView = QString());

    void createHierarchicalConnection(QString const& topInterfaceRef, QString const& instanceInterfaceRef);

    void addTestComponentToLibrary(VLNV vlnv);

    void addInstanceToDesign(QString instanceName, VLNV instanceVlnv);

    void mapPortToInterface(QString const& portName, QString const& logicalName, 
        QString const& interfaceName, QSharedPointer<Component> component);

    void mapPortToInterface(QString const& portName, int left, int right, 
        QString const& logicalName, QString const& interfaceName, QSharedPointer<Component> component);
    
    void addInterfaceToComponent(QString const& interfaceName, QSharedPointer<Component> component);

    void addSenderComponentToLibrary(VLNV senderVLNV, General::InterfaceMode mode);

    void addReceiverComponentToLibrary(VLNV receiverVLNV, General::InterfaceMode mode);    

    void setReceiverComponentDataWidth(VLNV receiverVLNV, int dataWidth);

    void addConnectionToDesign(QString fromInstance, QString fromInterface, QString toInstance, QString toInterface);

    void addAdhocConnection(QString const& connectionName, QString const& sourceInstance, QString const& sourcePort,
        QString const& targetInstance, QString const& targetPort);

    void addHierAdhocConnection(QString const& topPort, QString const& targetInstance, QString const& targetPort);

    void verifyOutputContains(QString const& expectedOutput);

    void compareOutputTo(QString const& expectedOutput);

    void readOutputFile();   
   

    //! The top level component for which the generator is run.
    QSharedPointer<Component> topComponent_;
    
    //! The design for which the generator is run.
    QSharedPointer<Design> design_;

    //! The generator output as a string.
    QString output_;
    
    //! The time of the generator run.
    QDateTime generationTime_;

    //! The test mock for library interface.
    LibraryMock library_;
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::tst_VerilogGenerator()
//-----------------------------------------------------------------------------
tst_VerilogGenerator::tst_VerilogGenerator(): topComponent_(), design_(), output_(), generationTime_(), library_(this)
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::initTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::init()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    topComponent_ = QSharedPointer<Component>(new Component(vlnv));

    VLNV designVlnv(VLNV::DESIGN, "Test", "TestLibrary", "TestDesign", "1.0");
    design_ = QSharedPointer<Design>(new Design(designVlnv));

    library_.clear();

    QVERIFY(!QFile::exists("./generatorOutput.v"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::cleanup()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::cleanup()
{
    topComponent_.clear();

    output_.clear();

    QFile::remove("./generatorOutput.v");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testTopLevelComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testTopLevelComponent()
{
    addPort("clk", 1, DirectionTypes::IN, topComponent_);
    addPort("rst_n", 1, DirectionTypes::IN, topComponent_);
    addPort("dataOut", 8, DirectionTypes::OUT, topComponent_);
    addPort("dataIn", 8, DirectionTypes::IN, topComponent_);
    addModuleParameter("dataWidth", "8");
    addModuleParameter("freq", "100000");

    runGenerator(topComponent_->getViews()->first()->name());

    verifyOutputContains(QString(
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
        "\n"
		"\n"
        "endmodule\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testTopLevelComponentExpressions()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testTopLevelComponentExpressions()
{
    addModuleParameter("module", "10");
    topComponent_->getComponentInstantiations()->first()->getModuleParameters()->first()->setValueId("MODEL-ID");

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port("clk", DirectionTypes::IN));
    port->setLeftBound("MODEL-ID*2");
    port->setRightBound("2+5");
    topComponent_->getPorts()->append(port);

    runGenerator(topComponent_->getViews()->first()->name());

    verifyOutputContains(QString(
        "module TestComponent #(\n"
        "    parameter                              module           = 10\n"
        ") (\n"
        "    // These ports are not in any interface\n"
        "    input          [module*2:2+5]       clk\n"
        ");\n"
        "\n"
        "\n"
        "endmodule\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addPort(QString const& portName, int portSize, 
    DirectionTypes::Direction direction, QSharedPointer<Component> component)
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, direction));
    port->setPortSize(portSize);
    component->getPorts()->append(port);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addModuleParameter()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addModuleParameter( QString const& name, QString const& value )
{
    QSharedPointer<ModuleParameter> parameter = QSharedPointer<ModuleParameter>(new ModuleParameter());
    parameter->setName(name);
    parameter->setValue(value);

    if (topComponent_->getComponentInstantiations()->isEmpty())
    {
        QSharedPointer<ComponentInstantiation> newInstantiation (new ComponentInstantiation("testInstantiation"));
        newInstantiation->getModuleParameters()->append(parameter);
        topComponent_->getComponentInstantiations()->append(newInstantiation);

        if (topComponent_->getViews()->isEmpty())
        {
            QSharedPointer<View> newView (new View("testView"));
            newView->setComponentInstantiationRef(newInstantiation->name());
            topComponent_->getViews()->append(newView);
        }
    }
    else
    {
        topComponent_->getComponentInstantiations()->first()->getModuleParameters()->append(parameter);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::runGenerator()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::runGenerator(QString const& activeView /*= QString()*/)
{
    VerilogGenerator generator(&library_);

    generator.parse(topComponent_, activeView, design_);

    generationTime_ =  QDateTime::currentDateTime();

    generator.generate("./generatorOutput.v");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFileHeaderIsPrinted()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFileHeaderIsPrinted()
{    
    topComponent_->setDescription("Component description\nspanning multiple\nlines.");
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_);
    	
    QCoreApplication::setOrganizationName("TUT");
    QCoreApplication::setApplicationName("TestRunner");
    QSettings settings;
    settings.setValue("General/Username", "testUser");

    runGenerator();
    
    verifyOutputContains(QString(
        "//-----------------------------------------------------------------------------\n"
        "// File          : generatorOutput.v\n"
        "// Creation date : " + generationTime_.date().toString("dd.MM.yyyy") + "\n"
        "// Creation time : " + generationTime_.time().toString("hh:mm:ss") + "\n"
        "// Description   : Component description\n"
        "//                 spanning multiple\n"
        "//                 lines.\n"
        "// Created by    : testUser\n"
        "// This file was generated with Kactus2 verilog generator\n"
        "// based on IP-XACT component Test:TestLibrary:TestComponent:1.0\n"
        "// whose XML file is C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml\n"
        "//-----------------------------------------------------------------------------\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testConsecutiveParseCalls()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testConsecutiveParseCalls()
{
    VLNV secondVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "Override", "1.0");
    QSharedPointer<Component> secondComponent = QSharedPointer<Component>(new Component());
    secondComponent->setVlnv(secondVlnv);

    VerilogGenerator generator(&library_);

    generator.parse(topComponent_, QString());
    generator.parse(secondComponent, QString());

    generator.generate("./generatorOutput.v");
    
    verifyOutputContains(QString("module Override();\n"
        "\n"
		"\n"
        "endmodule\n"));
    QVERIFY(!output_.contains("module TestComponent"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalConnections()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalConnections()
{
    addPort("top_clk", 1, DirectionTypes::IN, topComponent_);
    addPort("data_to_instance", 8, DirectionTypes::IN, topComponent_);
    addPort("enable_to_instance", 1, DirectionTypes::IN, topComponent_);
    addPort("full_from_instance", 1, DirectionTypes::OUT, topComponent_);

    addInterfaceToComponent("clk_if", topComponent_);
    addInterfaceToComponent("data_bus", topComponent_);
    
    mapPortToInterface("top_clk", "CLK", "clk_if", topComponent_);
    mapPortToInterface("data_to_instance", "DATA", "data_bus", topComponent_);
    mapPortToInterface("enable_to_instance", "ENABLE", "data_bus", topComponent_);
    mapPortToInterface("full_from_instance", "FULL", "data_bus", topComponent_);

    createHierarchicalConnection("clk_if", "clk");
    createHierarchicalConnection("data_bus", "data");

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");
    addTestComponentToLibrary(instanceVlnv);
    addInstanceToDesign("instance1", instanceVlnv);

    runGenerator();

    verifyOutputContains(QString(
        "module TestComponent(\n"
        "    // Interface: clk_if\n"
        "    input                               top_clk,\n"
        "\n"
        "    // Interface: data_bus\n"     
        "    input          [7:0]                data_to_instance,\n"
        "    input                               enable_to_instance,\n"
        "    output                              full_from_instance\n"
        ");\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestInstance:1.0\n"
        "    TestInstance instance1(\n"
        "        // Interface: clk\n"
        "        .clk                 (top_clk),\n"
        "        // Interface: data\n"
        "        .data_in             (data_to_instance),\n"
        "        .enable              (enable_to_instance),\n"
        "        .full                (full_from_instance),\n"
        "        // These ports are not in any interface\n"
        "        .data_out            ( ));\n"
        "\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createHierarchicalConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::createHierarchicalConnection(QString const& topInterfaceRef, 
    QString const& instanceInterfaceRef)
{
    QSharedPointer<Interconnection> hierachicalConnection(new Interconnection());
    QSharedPointer<ActiveInterface> instanceInterface(new ActiveInterface("instance1", instanceInterfaceRef));
    hierachicalConnection->setStartInterface(instanceInterface);
    
    QSharedPointer<HierInterface> topInterface(new HierInterface(topInterfaceRef));
    hierachicalConnection->getHierInterfaces()->append(topInterface);
   
    design_->getInterconnections()->append(hierachicalConnection);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalConnectionsWithExpressions()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalConnectionsWithExpressions()
{
    QSharedPointer<Port> clkPort (new Port("top_clk", DirectionTypes::IN));
    clkPort->setLeftBound("2+2");
    clkPort->setRightBound("0");
    topComponent_->getPorts()->append(clkPort);

    addInterfaceToComponent("clk_if", topComponent_);

    mapPortToInterface("top_clk", "CLK", "clk_if", topComponent_);

    createHierarchicalConnection("clk_if", "instanceInterface");

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");

    QSharedPointer<Component> instanceComponent(new Component(instanceVlnv));

    QSharedPointer<ModuleParameter> instanceParameter (new ModuleParameter());
    instanceParameter->setName("instanceParameter");
    instanceParameter->setValue("1");
    instanceParameter->setValueId("instant_ID");

    QSharedPointer<ComponentInstantiation> instanceInstantiation (new ComponentInstantiation("instanceInstant"));
    instanceInstantiation->getModuleParameters()->append(instanceParameter);
    instanceComponent->getComponentInstantiations()->append(instanceInstantiation);

    QSharedPointer<View> instanceView (new View("instanceView"));
    instanceView->setComponentInstantiationRef(instanceInstantiation->name());
    instanceComponent->getViews()->append(instanceView);

    QSharedPointer<Port> instanceClkPort (new Port("instance_clk", DirectionTypes::IN));
    instanceClkPort->setLeftBound(instanceParameter->getValueId() + "*2");
    instanceClkPort->setRightBound("4-2*2");
    instanceComponent->getPorts()->append(instanceClkPort);

    addInterfaceToComponent("instanceInterface", instanceComponent);
    mapPortToInterface("instance_clk", "CLK", "instanceInterface", instanceComponent);

    library_.addComponent(instanceComponent);

    addInstanceToDesign("instance1", instanceVlnv);

    runGenerator();

    verifyOutputContains(QString(
        "module TestComponent(\n"
        "    // Interface: clk_if\n"
        "    input          [2+2:0]              top_clk\n"
        ");\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestInstance:1.0\n"
        "    TestInstance instance1(\n"
        "        // Interface: instanceInterface\n"
        "        .instance_clk        (top_clk[2:0]));\n"
        "\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testSlicedHierarchicalConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testSlicedHierarchicalConnection()
{    
    addInterfaceToComponent("data_bus", topComponent_);
    addPort("enable_to_instance", 2, DirectionTypes::IN, topComponent_);
    addPort("data_to_instance", 16, DirectionTypes::IN, topComponent_);
    addPort("full_from_instance", 3, DirectionTypes::OUT, topComponent_);   
    mapPortToInterface("enable_to_instance", 1, 1, "ENABLE", "data_bus", topComponent_);
    mapPortToInterface("data_to_instance", 7, 0, "DATA", "data_bus", topComponent_);
    mapPortToInterface("full_from_instance", 1, 1, "FULL", "data_bus", topComponent_);

    createHierarchicalConnection("data_bus", "data");
    createHierarchicalConnection("clk_if", "clk");

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");
    addTestComponentToLibrary(instanceVlnv);
    addInstanceToDesign("instance1", instanceVlnv);

    runGenerator();

    verifyOutputContains(QString(
        "    TestInstance instance1(\n"
        "        // Interface: clk\n"
        "        .clk                 ( ),\n"
        "        // Interface: data\n"
        "        .data_in             (data_to_instance[7:0]),\n"
        "        .enable              (enable_to_instance[1]),\n"
        "        .full                (full_from_instance[1]),\n"
        "        // These ports are not in any interface\n"
        "        .data_out            ( ));"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testUnknownInstanceIsNotWritten()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testUnknownInstanceIsNotWritten()
{
    VLNV nonLibraryComponent(VLNV::COMPONENT, "Test", "TestLibrary", "Unknown", "1.0");
    addInstanceToDesign("unknown", nonLibraryComponent);

    runGenerator();

    verifyOutputContains(
        "module TestComponent();\n"
        "\n"
		"\n"
        "endmodule");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addInterfaceToComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addInterfaceToComponent(QString const& interfaceName,
    QSharedPointer<Component> component)
{
    QSharedPointer<BusInterface> busInterface(new BusInterface());
    busInterface->setName(interfaceName);
    component->getBusInterfaces()->append(busInterface);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addPortMap()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::mapPortToInterface(QString const& portName, QString const& logicalName, 
    QString const& interfaceName, QSharedPointer<Component> component)
{
	QSharedPointer<Port> port = component->getPort(portName);

    QSharedPointer<PortMap> portMap(new PortMap());
	QSharedPointer<PortMap::LogicalPort> logPort( new PortMap::LogicalPort );
	QSharedPointer<PortMap::PhysicalPort> physPort( new PortMap::PhysicalPort );
	logPort->name_ = logicalName;
	physPort->name_ = portName;

    QSharedPointer<PartSelect> physicalPart (new PartSelect(port->getLeftBound(), port->getRightBound()));
    physPort->partSelect_ = physicalPart;

    portMap->setLogicalPort(logPort);
    portMap->setPhysicalPort(physPort);

    QSharedPointer<BusInterface> containingBus = component->getBusInterface(interfaceName);
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps = containingBus->getPortMaps();

    if (!portMaps)
    {
        QSharedPointer<QList<QSharedPointer<PortMap> > > newPortMapList (new QList<QSharedPointer<PortMap> > ());

        if (containingBus->getAbstractionTypes()->isEmpty())
        {
            QSharedPointer<AbstractionType> testAbstraction (new AbstractionType());

            QSharedPointer<ConfigurableVLNVReference> abstractionVLNV(new ConfigurableVLNVReference(
                VLNV::ABSTRACTIONDEFINITION, "Test", "TestLibrary", "absDef", "1.0"));
            testAbstraction->setAbstractionRef(abstractionVLNV);

            QSharedPointer<AbstractionDefinition> testAbstractionDefinition (new AbstractionDefinition());
            testAbstractionDefinition->setVlnv(*abstractionVLNV.data());

            QSharedPointer<PortAbstraction> logicalPort (new PortAbstraction());
            logicalPort->setName(logicalName);

            containingBus->getAbstractionTypes()->append(testAbstraction);
        }

        containingBus->setPortMaps(newPortMapList);
        portMaps = containingBus->getPortMaps();
    }

    portMaps->append(portMap);
}
//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::mapPortToInterface()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::mapPortToInterface(QString const& portName, int left, int right, QString const& logicalName, 
    QString const& interfaceName, QSharedPointer<Component> component)
{
	QSharedPointer<PortMap> portMap(new PortMap());
	QSharedPointer<PortMap::LogicalPort> logPort( new PortMap::LogicalPort );
	QSharedPointer<PortMap::PhysicalPort> physPort( new PortMap::PhysicalPort );
	logPort->name_ = logicalName;
	physPort->name_ = portName;

    QSharedPointer<PartSelect> physicalPart (new PartSelect(QString::number(left), QString::number(right)));
    physPort->partSelect_ = physicalPart;

	portMap->setLogicalPort(logPort);
	portMap->setPhysicalPort(physPort);

    QSharedPointer<BusInterface> containingBus = component->getBusInterface(interfaceName);
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps = containingBus->getPortMaps();

    if (!portMaps)
    {
        QSharedPointer<QList<QSharedPointer<PortMap> > > newPortMapList (new QList<QSharedPointer<PortMap> > ());

        if (containingBus->getAbstractionTypes()->isEmpty())
        {
            QSharedPointer<AbstractionType> testAbstraction (new AbstractionType());
            containingBus->getAbstractionTypes()->append(testAbstraction);
        }

        containingBus->setPortMaps(newPortMapList);
        portMaps = containingBus->getPortMaps();
    }
    
    portMaps->append(portMap);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addTestComponentToLibrary()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addTestComponentToLibrary(VLNV vlnv)
{
    QSharedPointer<Component> instanceComponent(new Component(vlnv));
    addPort("clk", 1, DirectionTypes::IN, instanceComponent);
    addPort("data_in", 8, DirectionTypes::IN, instanceComponent);
    addPort("data_out", 8, DirectionTypes::OUT, instanceComponent);
    addPort("enable", 1, DirectionTypes::IN, instanceComponent);
    addPort("full", 1, DirectionTypes::OUT, instanceComponent);

    addInterfaceToComponent("clk", instanceComponent);

    mapPortToInterface("clk", "CLK", "clk", instanceComponent);

    addInterfaceToComponent("data", instanceComponent);

    mapPortToInterface("data_in", "DATA", "data", instanceComponent);
    mapPortToInterface("enable", "ENABLE", "data", instanceComponent);
    mapPortToInterface("full", "FULL", "data", instanceComponent);

    library_.addComponent(instanceComponent);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addInstanceToDesign()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addInstanceToDesign(QString instanceName, VLNV instanceVlnv)
{
    QSharedPointer<ConfigurableVLNVReference> componentVLNV (new ConfigurableVLNVReference(instanceVlnv));
    QSharedPointer<ComponentInstance> instance (new ComponentInstance(instanceName, componentVLNV));

    design_->getComponentInstances()->append(instance);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterToSlaveInterconnection()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("receiver", receiverVLNV);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    runGenerator();

    verifyOutputContains(
    "    wire [7:0]  sender_to_receiver_DATA;\n"
    "    wire        sender_to_receiver_ENABLE;\n"
    "\n"
    "    // IP-XACT VLNV: Test:TestLibrary:TestReceiver:1.0\n"
    "    TestReceiver receiver(\n"
    "        // Interface: data_bus\n"
    "        .data_in             (sender_to_receiver_DATA),\n"
    "        .enable_in           (sender_to_receiver_ENABLE));\n"   
    "\n"
    "    // IP-XACT VLNV: Test:TestLibrary:TestSender:1.0\n"
    "    TestSender sender(\n"
    "        // Interface: data_bus\n"
    "        .data_out            (sender_to_receiver_DATA),\n"
    "        .enable_out          (sender_to_receiver_ENABLE));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMasterToSlaveInterconnectionWithExpressions()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterToSlaveInterconnectionWithExpressions()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));

    QSharedPointer<Port> senderPort = QSharedPointer<Port>(new Port("data_out", DirectionTypes::OUT));
    senderPort->setLeftBound("20-2");
    senderPort->setRightBound("0");
    senderComponent->getPorts()->append(senderPort);

    addInterfaceToComponent("data_bus", senderComponent);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(General::MASTER);

    mapPortToInterface("data_out", "DATA", "data_bus", senderComponent);
    QSharedPointer<PortMap> dataMap = senderComponent->getBusInterface("data_bus")->getPortMaps()->first();
    QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
    logPort->range_ = QSharedPointer<Range>( new Range("7*2","0") );

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));

    QSharedPointer<Port> receiverPort = QSharedPointer<Port>(new Port("data_in", DirectionTypes::IN));
    receiverPort->setLeftBound("7+1");
    receiverPort->setRightBound("0");
    receiverComponent->getPorts()->append(receiverPort);

    addInterfaceToComponent("data_bus", receiverComponent);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(General::SLAVE);
    mapPortToInterface("data_in", "DATA", "data_bus", receiverComponent);

    library_.addComponent(receiverComponent);
    addInstanceToDesign("receiver", receiverVLNV);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    runGenerator();

    verifyOutputContains(
        "    wire [14:0] sender_to_receiver_DATA;\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestReceiver:1.0\n"
        "    TestReceiver receiver(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_to_receiver_DATA[8:0]));\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestSender:1.0\n"
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (sender_to_receiver_DATA));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addSenderComponentToLibrary()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addSenderComponentToLibrary(VLNV senderVLNV, General::InterfaceMode mode)
{
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    addPort("enable_out", 1, DirectionTypes::OUT, senderComponent);
    addPort("data_out", 8, DirectionTypes::OUT, senderComponent);

    addInterfaceToComponent("data_bus", senderComponent);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(mode);    
    mapPortToInterface("data_out", "DATA", "data_bus", senderComponent);
    QSharedPointer<PortMap> dataMap = senderComponent->getBusInterface("data_bus")->getPortMaps()->first();
    QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
	logPort->range_ = QSharedPointer<Range>( new Range("7","0") );

    mapPortToInterface("enable_out", "ENABLE", "data_bus", senderComponent);

    library_.addComponent(senderComponent);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addReceiverComponentToLibrary()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addReceiverComponentToLibrary(VLNV receiverVLNV, General::InterfaceMode mode)
{
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));
    addPort("enable_in", 1, DirectionTypes::IN, receiverComponent);
    addPort("data_in", 8, DirectionTypes::IN, receiverComponent);

    addInterfaceToComponent("data_bus", receiverComponent);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(mode);    
    mapPortToInterface("data_in", "DATA", "data_bus", receiverComponent);

    mapPortToInterface("enable_in", "ENABLE", "data_bus", receiverComponent);

    library_.addComponent(receiverComponent);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addConnectionToDesign()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addConnectionToDesign(QString fromInstance, QString fromInterface, 
    QString toInstance, QString toInterface)
{
    QSharedPointer<ActiveInterface> firstInterface(new ActiveInterface(fromInstance, fromInterface));
    QSharedPointer<ActiveInterface> secondInterface(new ActiveInterface(toInstance, toInterface));

    QSharedPointer<Interconnection> connection(new Interconnection(fromInstance + "_to_" + toInstance, firstInterface));
    connection->getActiveInterfaces()->append(secondInterface);

    design_->getInterconnections()->append(connection);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMasterToMultipleSlavesInterconnections()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterToMultipleSlavesInterconnections()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("receiver1", receiverVLNV);
    addInstanceToDesign("receiver2", receiverVLNV);

    addConnectionToDesign("sender", "data_bus", "receiver1", "data_bus");
    addConnectionToDesign("sender", "data_bus", "receiver2", "data_bus");    

    runGenerator();

    verifyOutputContains("wire        sender_data_bus_ENABLE;");
    verifyOutputContains("wire [7:0]  sender_data_bus_DATA;");

    verifyOutputContains("TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (sender_data_bus_DATA),\n"
        "        .enable_out          (sender_data_bus_ENABLE)");

    verifyOutputContains("TestReceiver receiver1(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_data_bus_DATA),\n"
        "        .enable_in           (sender_data_bus_ENABLE)");

    verifyOutputContains("TestReceiver receiver2(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_data_bus_DATA),\n"
        "        .enable_in           (sender_data_bus_ENABLE)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testInterconnectionToVaryingSizeLogicalMaps()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testInterconnectionToVaryingSizeLogicalMaps()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV);

    VLNV oneBitReceiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    addReceiverComponentToLibrary(oneBitReceiverVLNV, General::SLAVE);
    setReceiverComponentDataWidth(oneBitReceiverVLNV, 1);
    addInstanceToDesign("oneBitReceiver", oneBitReceiverVLNV);
    
    VLNV fourBitReceiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver4bit", "1.0");
    addReceiverComponentToLibrary(fourBitReceiverVLNV, General::SLAVE);
    setReceiverComponentDataWidth(fourBitReceiverVLNV, 4);
    addInstanceToDesign("fourBitReceiver", fourBitReceiverVLNV);

    VLNV sixteenBitReceiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver16bit", "1.0");
    addReceiverComponentToLibrary(sixteenBitReceiverVLNV, General::SLAVE);
    setReceiverComponentDataWidth(sixteenBitReceiverVLNV, 16);
    addInstanceToDesign("sixteenBitReceiver", sixteenBitReceiverVLNV);

    addConnectionToDesign("sender", "data_bus", "oneBitReceiver", "data_bus");
    addConnectionToDesign("sender", "data_bus", "fourBitReceiver", "data_bus");
    addConnectionToDesign("sender", "data_bus", "sixteenBitReceiver", "data_bus");

    runGenerator();

    verifyOutputContains("wire [15:0] sender_data_bus_DATA;");
    verifyOutputContains("wire        sender_data_bus_ENABLE;");    

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (sender_data_bus_DATA[7:0]),\n"
        "        .enable_out          (sender_data_bus_ENABLE)");

    verifyOutputContains(
        "    TestReceiver oneBitReceiver(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_data_bus_DATA[0]),\n"
        "        .enable_in           (sender_data_bus_ENABLE)");

    verifyOutputContains(
        "    TestReceiver4bit fourBitReceiver(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_data_bus_DATA[3:0]),\n"
        "        .enable_in           (sender_data_bus_ENABLE)");

    verifyOutputContains(
        "    TestReceiver16bit sixteenBitReceiver(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_data_bus_DATA),\n"
        "        .enable_in           (sender_data_bus_ENABLE)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::setComponentDataWidth()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::setReceiverComponentDataWidth(VLNV receiverVLNV, int dataWidth)
{
    QSharedPointer<Component> component = library_.getModel(receiverVLNV).dynamicCast<Component>();
    QSharedPointer<Port> componentPort = component->getPort("data_in");
    componentPort->setLeftBound(QString::number(dataWidth - 1));
    componentPort->setRightBound(QString::number(0));

	QSharedPointer<PortMap> dataMap = component->getBusInterface("data_bus")->getPortMaps()->first();

    QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
    if (!logPort)
    {
        QSharedPointer<PortMap::LogicalPort> newLogicalPort (new PortMap::LogicalPort());
        dataMap->setLogicalPort(newLogicalPort);
        logPort = dataMap->getLogicalPort();
    }

    logPort->range_ = QSharedPointer<Range>(new Range(QString::number(dataWidth-1), "0"));

	dataMap->setLogicalPort(logPort);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testSlicedInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testSlicedInterconnection()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");

    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    addPort("enable_out_low", 1, DirectionTypes::OUT, senderComponent);
    addPort("enable_out_high", 1, DirectionTypes::OUT, senderComponent);

    addInterfaceToComponent("data_bus", senderComponent);
    QSharedPointer<BusInterface> enableIf = senderComponent->getBusInterface("data_bus");
    enableIf->setInterfaceMode(General::MASTER);    

	QSharedPointer<PortMap> enableLowPortMap(new PortMap());
	QSharedPointer<PortMap::LogicalPort> logPortLow( new PortMap::LogicalPort );
	QSharedPointer<PortMap::PhysicalPort> physPortLow( new PortMap::PhysicalPort );
    enableLowPortMap->setLogicalPort(logPortLow);
	enableLowPortMap->setPhysicalPort(physPortLow);
	logPortLow->name_ = "ENABLE";
	physPortLow->name_ = "enable_out_low";

    QSharedPointer<PartSelect> lowPart (new PartSelect("0", "0"));
    physPortLow->partSelect_ = lowPart;

    QSharedPointer<Range> lowRange (new Range("0", "0"));
    logPortLow->range_ = lowRange;

	QSharedPointer<PortMap> enableHighPortMap(new PortMap());
	QSharedPointer<PortMap::LogicalPort> logPortHigh( new PortMap::LogicalPort );
	QSharedPointer<PortMap::PhysicalPort> physPortHigh( new PortMap::PhysicalPort );
    enableHighPortMap->setLogicalPort(logPortHigh);
    enableHighPortMap->setPhysicalPort(physPortHigh);
    logPortHigh->name_ = "ENABLE";
	physPortHigh->name_ = "enable_out_high";

    QSharedPointer<PartSelect> highPart (new PartSelect("0", "0"));
    physPortHigh->partSelect_ = highPart;

    QSharedPointer<Range> highRange (new Range("1", "1"));
    logPortHigh->range_ = highRange;

    QSharedPointer<AbstractionType> senderEnableAbstraction (new AbstractionType());
    enableIf->getAbstractionTypes()->append(senderEnableAbstraction);

    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps = enableIf->getPortMaps();
    portMaps->append(enableLowPortMap);
    portMaps->append(enableHighPortMap);

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV);

    VLNV receiver(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    addReceiverComponentToLibrary(receiver, General::SLAVE);
    QSharedPointer<Component> receiverComponent = library_.getModel(receiver).dynamicCast<Component>();
    QSharedPointer<PortMap> enableMap = receiverComponent->getBusInterface("data_bus")->getPortMaps()->last();
    enableMap->getLogicalPort()->range_ = QSharedPointer<Range>(new Range("0", "0"));

    addInstanceToDesign("receiver", receiver);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    runGenerator();

    verifyOutputContains("wire [1:0]  sender_to_receiver_ENABLE;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .enable_out_high     (sender_to_receiver_ENABLE[1]),\n"
        "        .enable_out_low      (sender_to_receiver_ENABLE[0]));");

    verifyOutputContains(
        "    TestReceiver receiver(\n"
        "        // Interface: data_bus\n"
        "        .data_in             ( ),\n"
        "        .enable_in           (sender_to_receiver_ENABLE[0]));");

}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMasterInterconnectionToMirroredMaster()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterInterconnectionToMirroredMaster()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "BusComponent", "1.0");
    addReceiverComponentToLibrary(receiverVLNV, General::MIRROREDMASTER);
    addInstanceToDesign("receiver", receiverVLNV);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    runGenerator();

    verifyOutputContains("wire [7:0]  sender_to_receiver_DATA;");
    verifyOutputContains("wire        sender_to_receiver_ENABLE;");    

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (sender_to_receiver_DATA),\n"
        "        .enable_out          (sender_to_receiver_ENABLE)");

    verifyOutputContains(
        "    BusComponent receiver(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_to_receiver_DATA),\n"
        "        .enable_in           (sender_to_receiver_ENABLE)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMirroredSlaveInterconnectionToSlaves()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMirroredSlaveInterconnectionToSlaves()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    addSenderComponentToLibrary(senderVLNV, General::MIRROREDSLAVE);
    addInstanceToDesign("sender", senderVLNV);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "BusComponent", "1.0");
    addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("bus1", receiverVLNV);
    addInstanceToDesign("bus2", receiverVLNV);

    addConnectionToDesign("sender", "data_bus", "bus1", "data_bus");
    addConnectionToDesign("sender", "data_bus", "bus2", "data_bus");

    runGenerator();

    verifyOutputContains("wire [7:0]  sender_data_bus_DATA;");
    verifyOutputContains("wire        sender_data_bus_ENABLE;");    

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (sender_data_bus_DATA),\n"
        "        .enable_out          (sender_data_bus_ENABLE)");

    verifyOutputContains(
        "    BusComponent bus1(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_data_bus_DATA),\n"
        "        .enable_in           (sender_data_bus_ENABLE)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testAdhocConnectionBetweenTwoComponentInstances()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testAdhocConnectionBetweenComponentInstances()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("receiver1", receiverVLNV);
    addInstanceToDesign("receiver2", receiverVLNV);

    addAdhocConnection("enableAdHoc", "sender", "enable_out", "receiver1", "enable_in");
    addAdhocConnection("enableAdHoc", "sender", "enable_out", "receiver2", "enable_in");
    addAdhocConnection("dataAdHoc", "sender", "data_out", "receiver1", "data_in");

    runGenerator();

    verifyOutputContains("wire        enableAdHoc;");
    verifyOutputContains("wire [7:0]  dataAdHoc;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (dataAdHoc),\n"
        "        .enable_out          (enableAdHoc)");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (dataAdHoc),\n"
        "        .enable_in           (enableAdHoc)");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        // Interface: data_bus\n"
        "        .data_in             ( ),\n"
        "        .enable_in           (enableAdHoc)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testAdHocConnectionBetweenComponentInstancesWithExpressions()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testAdHocConnectionBetweenComponentInstancesWithExpressions()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));

    QSharedPointer<Port> senderPort = QSharedPointer<Port>(new Port("enable_out", DirectionTypes::OUT));
    senderPort->setLeftBound("10*2-8");
    senderPort->setRightBound("2");
    senderComponent->getPorts()->append(senderPort);

    addInterfaceToComponent("data_bus", senderComponent);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(General::MASTER);

    mapPortToInterface("enable_out", "ENABLE", "data_bus", senderComponent);
    QSharedPointer<PortMap> dataMap = senderComponent->getBusInterface("data_bus")->getPortMaps()->first();
    QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
    logPort->range_ = QSharedPointer<Range>( new Range("10/2","1") );

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));

    QSharedPointer<Port> receiverPort = QSharedPointer<Port>(new Port("enable_in", DirectionTypes::IN));
    receiverPort->setLeftBound("2+2");
    receiverPort->setRightBound("0");
    receiverComponent->getPorts()->append(receiverPort);

    addInterfaceToComponent("data_bus", receiverComponent);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(General::SLAVE);
    mapPortToInterface("enable_in", "ENABLE", "data_bus", receiverComponent);

    library_.addComponent(receiverComponent);
    addInstanceToDesign("receiver1", receiverVLNV);
    addInstanceToDesign("receiver2", receiverVLNV);

    addAdhocConnection("enableAdHoc", "sender", "enable_out", "receiver1", "enable_in");
    addAdhocConnection("enableAdHoc", "sender", "enable_out", "receiver2", "enable_in");

    runGenerator();

    verifyOutputContains("wire [10:0] enableAdHoc;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .enable_out          (enableAdHoc)");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_bus\n"
        "        .enable_in           (enableAdHoc[4:0]));\n");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        // Interface: data_bus\n"
        "        .enable_in           (enableAdHoc[4:0]));\n");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addAdhocConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addAdhocConnection(QString const& connectionName, 
    QString const& sourceInstance, QString const& sourcePort, 
    QString const& targetInstance, QString const& targetPort)
{
    QSharedPointer<QList<QSharedPointer<PortReference> > > internalRefs(
        new QList<QSharedPointer<PortReference> >());

    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > adHocConnections = design_->getAdHocConnections();
    for (int i = 0; i < adHocConnections->size(); i++)
    {
        if (adHocConnections->at(i)->name() == connectionName)
        {
            internalRefs  = adHocConnections->at(i)->getInternalPortReferences();
            adHocConnections->removeAt(i);
            break;
        }
    }

    QSharedPointer<PortReference> fromRef(new PortReference(sourcePort, sourceInstance));
    QSharedPointer<PortReference> toRef(new PortReference(targetPort, targetInstance));
    
    if (internalRefs->isEmpty())
    {
        internalRefs->append(fromRef);        
    }
    internalRefs->append(toRef);

    QSharedPointer<AdHocConnection> connection(new AdHocConnection(connectionName));
    connection->setInternalPortReferences(internalRefs);

    design_->getAdHocConnections()->append(connection);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalAdhocConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalAdhocConnection()
{
    addPort("enable_from_sender", 1, DirectionTypes::OUT, topComponent_);
    addPort("data_from_sender", 8, DirectionTypes::OUT, topComponent_);

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV);

    addHierAdhocConnection("enable_from_sender", "sender", "enable_out");
    addHierAdhocConnection("data_from_sender", "sender", "data_out");

    runGenerator();

    readOutputFile();

    QStringList outputList = output_.split("\n");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (data_from_sender),\n"
        "        .enable_out          (enable_from_sender)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addHierAdhocConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addHierAdhocConnection(QString const& topPort, 
    QString const& targetInstance, QString const& targetPort)
{    
    QSharedPointer<PortReference> topComponentReference(new PortReference(topPort));

    QSharedPointer<PortReference> instancePortReference(new PortReference(targetPort, targetInstance));

    QSharedPointer<AdHocConnection> connection(new AdHocConnection(topPort + "_to_" + targetPort));
    connection->getExternalPortReferences()->append(topComponentReference);
    connection->getInternalPortReferences()->append(instancePortReference);

    design_->getAdHocConnections()->append(connection);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testAdHocConnectionToUnknownInstanceIsNotWritten()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testAdHocConnectionToUnknownInstanceIsNotWritten()
{
    addAdhocConnection("notConnected", "unknownInstance", "emptyPort", "unknownInstance", "emptyPort" );
    addHierAdhocConnection("top_clk", "unknownInstance", "clk");

    runGenerator();

    verifyOutputContains(
        "module TestComponent();\n"
        "\n"
		"\n"
        "endmodule");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testDescriptionAndVLNVIsPrintedAboveInstance()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testDescriptionAndVLNVIsPrintedAboveInstance()
{
    QFETCH(QString, description);
    QFETCH(QString, expectedOutput);
    
    QSharedPointer<ConfigurableVLNVReference> instanceVLNV(
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0"));
    QSharedPointer<Component> refComponent(new Component(*instanceVLNV.data()));
    library_.addComponent(refComponent);

    QSharedPointer<ComponentInstance> instance (new ComponentInstance("instance1", instanceVLNV));
    instance->setDescription(description);

    design_->getComponentInstances()->append(instance);

    runGenerator();

    verifyOutputContains(expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testDescriptionAndVLNVIsPrintedAboveInstance_data()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testDescriptionAndVLNVIsPrintedAboveInstance_data()
{
    QTest::addColumn<QString>("description");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("empty description") << "" <<
        "    // IP-XACT VLNV: Test:TestLibrary:TestComponent:1.0\n"
        "    TestComponent instance1();\n";
    QTest::newRow("one line description") << "Instance description." << 
        "    // Instance description.\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestComponent:1.0\n"
        "    TestComponent instance1();\n";

    QTest::newRow("multiline description") << 
        "Description on\n" 
        "multiple\n" 
        "lines." 
        << 
        "    // Description on\n"
        "    // multiple\n"
        "    // lines.\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestComponent:1.0\n"
        "    TestComponent instance1();\n";
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testTopLevelModuleParametersAreWritten()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testTopLevelModuleParametersAreWritten()
{
    QSharedPointer<View> activeView(new View());
    activeView->setName("rtl");
    activeView->setComponentInstantiationRef("instance1");

    QSharedPointer<ModuleParameter> moduleParameter(new ModuleParameter());
    moduleParameter->setName("moduleParameter");
    moduleParameter->setValue("1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	instantiation->getModuleParameters()->append(moduleParameter);

	topComponent_->getComponentInstantiations()->append(instantiation);
    topComponent_->getViews()->append(activeView);

    runGenerator("rtl");

    verifyOutputContains(QString(
        "module TestComponent #(\n"
        "    parameter                              moduleParameter  = 1\n"
        ") ();\n"
        "\n"
        "\n"
        "endmodule\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::verifyOutputContains()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::verifyOutputContains(QString const& expectedOutput)
{
    readOutputFile();

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
// Function: tst_VerilogGenerator::compareLineByLine()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::compareOutputTo(QString const& expectedOutput)
{
    readOutputFile();

    QCOMPARE(output_, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::readOutputFile()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::readOutputFile()
{
    QFile outputFile("./generatorOutput.v");

    QVERIFY(outputFile.open(QIODevice::ReadOnly));

    output_ = outputFile.readAll();
    outputFile.close();
}

QTEST_APPLESS_MAIN(tst_VerilogGenerator)

#include "tst_VerilogGenerator.moc"

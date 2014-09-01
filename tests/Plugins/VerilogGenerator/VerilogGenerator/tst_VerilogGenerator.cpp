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

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/ComponentInstance.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/PortMap.h>
#include <IPXACTmodels/vlnv.h>

#include <Plugins/VerilogGenerator/VerilogGenerator/VerilogGenerator.h>
#include "LibraryMock.h"

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
    void testConsecutiveParseCalls();
    void testFileHeaderIsPrinted();
    void testHierarchicalConnections();
    void testSlicedHierarchicalConnection();
    void testUnknownInstanceIsNotWritten();
    void testMasterToSlaveInterconnection();
    void testMasterToMultipleSlavesInterconnections();
    void testInterconnectionToVaryingSizeLogicalMaps();
    void testMasterInterconnectionToMirroredMaster();
    void testMirroredSlaveInterconnectionToSlaves();  
    void testAdhocConnectionBetweenComponentInstances();    
    void testHierarchicalAdhocConnection();
    void testAdHocConnectionToUnknownInstanceIsNotWritten();
    void testDescriptionAndVLNVIsPrintedAboveInstance();
    void testDescriptionAndVLNVIsPrintedAboveInstance_data();

private:

    void addPort( QString const& portName, int portSize, General::Direction direction, 
        QSharedPointer<Component> component);

    void addModelParameter( QString const& name, QString const& value );

    void runGenerator();

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
tst_VerilogGenerator::tst_VerilogGenerator(): topComponent_(), design_(), output_(), library_(this)
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
    addPort("clk", 1, General::IN, topComponent_);
    addPort("rst_n", 1, General::IN, topComponent_);
    addPort("dataOut", 8, General::OUT, topComponent_);
    addPort("dataIn", 8, General::IN, topComponent_);
    addModelParameter("dataWidth", "8");
    addModelParameter("freq", "100000");

    runGenerator();

    verifyOutputContains(QString(
        "module TestComponent #(\n"
        "    parameter         dataWidth        = 8,\n"
        "    parameter         freq             = 100000\n"
        ") (\n"
        "    // These ports are not in any interface\n"         
        "    input                 clk,\n"
        "    input          [7:0]  dataIn,\n"
        "    input                 rst_n,\n"
        "    output         [7:0]  dataOut\n"
        ");\n"
        "\n"
        "endmodule\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addPort(QString const& portName, int portSize, 
    General::Direction direction, QSharedPointer<Component> component)
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, direction, portSize - 1, 0, "", true));
    component->addPort(port);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addModelParameter()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addModelParameter( QString const& name, QString const& value )
{
    QSharedPointer<ModelParameter> parameter = QSharedPointer<ModelParameter>(new ModelParameter());
    parameter->setName(name);
    parameter->setValue(value);
    topComponent_->getModel()->addModelParameter(parameter);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::runGenerator()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::runGenerator()
{
    VerilogGenerator generator(&library_);

    generator.parse(topComponent_, design_);

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
        "// Creation date : " + generationTime_.date().toString(Qt::LocalDate) + "\n"
        "// Creation time : " + generationTime_.time().toString(Qt::LocalDate) + "\n"
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

    generator.parse(topComponent_);
    generator.parse(secondComponent);

    generator.generate("./generatorOutput.v");
    
    verifyOutputContains(QString("module Override();\n"
        "\n"
        "endmodule\n"));
    QVERIFY(!output_.contains("module TestComponent"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalConnections()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalConnections()
{
    addPort("top_clk", 1, General::IN, topComponent_);
    addPort("data_to_instance", 8, General::IN, topComponent_);
    addPort("enable_to_instance", 1, General::IN, topComponent_);
    addPort("full_from_instance", 1, General::OUT, topComponent_);

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
        "    input                 top_clk,\n"
        "\n"
        "    // Interface: data_bus\n"     
        "    input          [7:0]  data_to_instance,\n"
        "    input                 enable_to_instance,\n"
        "    output                full_from_instance\n"
        ");\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestInstance:1.0\n"
        "    TestInstance instance1(\n"
        "        .clk                 (top_clk),\n"
        "        .data_in             (data_to_instance),\n"
        "        .enable              (enable_to_instance),\n"
        "        .full                (full_from_instance),\n"
        "        .data_out            ( ));\n"
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
    addPort("enable_to_instance", 2, General::IN, topComponent_);
    addPort("data_to_instance", 16, General::IN, topComponent_);
    addPort("full_from_instance", 3, General::OUT, topComponent_);   
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
        "        .clk                 ( ),\n"
        "        .data_in             (data_to_instance[7:0]),\n"
        "        .enable              (enable_to_instance[1]),\n"
        "        .full                (full_from_instance[1]),\n"
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
    component->addBusInterface(busInterface);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addPortMap()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::mapPortToInterface(QString const& portName, QString const& logicalName, 
    QString const& interfaceName, QSharedPointer<Component> component)
{
    QSharedPointer<PortMap> portMap(new PortMap());
    portMap->setLogicalPort(logicalName);
    portMap->setPhysicalPort(portName);
    portMap->setPhysicalLeft(component->getPortLeftBound(portName));
    portMap->setPhysicalRight(component->getPortRightBound(portName));
    QList<QSharedPointer<PortMap> > portMaps = component->getBusInterface(interfaceName)->getPortMaps();
    portMaps.append(portMap);
    component->getBusInterface(interfaceName)->setPortMaps(portMaps);
}
//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::mapPortToInterface()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::mapPortToInterface(QString const& portName, int left, int right, QString const& logicalName, 
    QString const& interfaceName, QSharedPointer<Component> component)
{
    QSharedPointer<PortMap> portMap(new PortMap());
    portMap->setLogicalPort(logicalName);
    portMap->setPhysicalPort(portName);
    portMap->setPhysicalLeft(left);
    portMap->setPhysicalRight(right);
    QList<QSharedPointer<PortMap> > portMaps = component->getBusInterface(interfaceName)->getPortMaps();
    portMaps.append(portMap);
    component->getBusInterface(interfaceName)->setPortMaps(portMaps);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addTestComponentToLibrary()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addTestComponentToLibrary(VLNV vlnv)
{
    QSharedPointer<Component> instanceComponent(new Component(vlnv));
    addPort("clk", 1, General::IN, instanceComponent);
    addPort("data_in", 8, General::IN, instanceComponent);
    addPort("data_out", 8, General::OUT, instanceComponent);
    addPort("enable", 1, General::IN, instanceComponent);
    addPort("full", 1, General::OUT, instanceComponent);

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
    ComponentInstance instance(instanceName, "", "", instanceVlnv, QPointF(), "");

    QList<ComponentInstance> componentInstances = design_->getComponentInstances();
    componentInstances.append(instance);
    design_->setComponentInstances(componentInstances);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createHierarchicalConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::createHierarchicalConnection(QString const& topInterfaceRef, 
    QString const& instanceInterfaceRef)
{
    HierConnection clkConnection(topInterfaceRef, Interface("instance1", instanceInterfaceRef));    
    QList<HierConnection> hierConnections = design_->getHierarchicalConnections();
    hierConnections.append(clkConnection);
    design_->setHierarchicalConnections(hierConnections);
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
    "        .data_in             (sender_to_receiver_DATA),\n"
    "        .enable_in           (sender_to_receiver_ENABLE));\n"   
    "\n"
    "    // IP-XACT VLNV: Test:TestLibrary:TestSender:1.0\n"
    "    TestSender sender(\n"
    "        .data_out            (sender_to_receiver_DATA),\n"
    "        .enable_out          (sender_to_receiver_ENABLE));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addSenderComponentToLibrary()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addSenderComponentToLibrary(VLNV senderVLNV, General::InterfaceMode mode)
{
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    addPort("enable_out", 1, General::OUT, senderComponent);
    addPort("data_out", 8, General::OUT, senderComponent);

    addInterfaceToComponent("data_bus", senderComponent);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(mode);    
    mapPortToInterface("data_out", "DATA", "data_bus", senderComponent);
    QSharedPointer<PortMap> dataMap = senderComponent->getBusInterface("data_bus")->getPortMaps().first();
    dataMap->setLogicalLeft(7);
    dataMap->setLogicalRight(0);

    mapPortToInterface("enable_out", "ENABLE", "data_bus", senderComponent);

    library_.addComponent(senderComponent);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addReceiverComponentToLibrary()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addReceiverComponentToLibrary(VLNV receiverVLNV, General::InterfaceMode mode)
{
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));
    addPort("enable_in", 1, General::IN, receiverComponent);
    addPort("data_in", 8, General::IN, receiverComponent);

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
    Interface firstInterface(fromInstance, fromInterface);
    Interface secondInterface(toInstance, toInterface);

    Interconnection connection(fromInstance + "_to_" + toInstance, firstInterface, secondInterface);

    QList<Interconnection> connections = design_->getInterconnections();
    connections.append(connection);
    design_->setInterconnections(connections);
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
        "        .data_out            (sender_data_bus_DATA),\n"
        "        .enable_out          (sender_data_bus_ENABLE)");

    verifyOutputContains("TestReceiver receiver1(\n"
        "        .data_in             (sender_data_bus_DATA),\n"
        "        .enable_in           (sender_data_bus_ENABLE)");

    verifyOutputContains("TestReceiver receiver2(\n"
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
        "        .data_out            (sender_data_bus_DATA[7:0]),\n"
        "        .enable_out          (sender_data_bus_ENABLE)");

    verifyOutputContains(
        "    TestReceiver oneBitReceiver(\n"
        "        .data_in             (sender_data_bus_DATA[0]),\n"
        "        .enable_in           (sender_data_bus_ENABLE)");

    verifyOutputContains(
        "    TestReceiver4bit fourBitReceiver(\n"
        "        .data_in             (sender_data_bus_DATA[3:0]),\n"
        "        .enable_in           (sender_data_bus_ENABLE)");

    verifyOutputContains(
        "    TestReceiver16bit sixteenBitReceiver(\n"
        "        .data_in             (sender_data_bus_DATA),\n"
        "        .enable_in           (sender_data_bus_ENABLE)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::setComponentDataWidth()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::setReceiverComponentDataWidth(VLNV fourBitReceiverVLNV, int dataWidth)
{
    QSharedPointer<Component> component = library_.getModel(fourBitReceiverVLNV).dynamicCast<Component>();
    component->getPort("data_in")->setLeftBound(dataWidth - 1);
    component->getPort("data_in")->setRightBound(0);

    QSharedPointer<PortMap> dataMap = component->getBusInterface("data_bus")->getPortMaps().first();
    dataMap->setLogicalLeft(dataWidth - 1);
    dataMap->setLogicalRight(0);
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
        "        .data_out            (sender_to_receiver_DATA),\n"
        "        .enable_out          (sender_to_receiver_ENABLE)");

    verifyOutputContains(
        "    BusComponent receiver(\n"
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
        "        .data_out            (sender_data_bus_DATA),\n"
        "        .enable_out          (sender_data_bus_ENABLE)");

    verifyOutputContains(
        "    BusComponent bus1(\n"
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
        "        .data_out            (dataAdHoc),\n"
        "        .enable_out          (enableAdHoc)");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        .data_in             (dataAdHoc),\n"
        "        .enable_in           (enableAdHoc)");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        .data_in             ( ),\n"
        "        .enable_in           (enableAdHoc)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addAdhocConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addAdhocConnection(QString const& connectionName, 
    QString const& sourceInstance, QString const& sourcePort, 
    QString const& targetInstance, QString const& targetPort)
{
    QList<PortRef> internalRefs;

    QList<AdHocConnection> adHocConnections = design_->getAdHocConnections();
    for (int i = 0; i < adHocConnections.size(); i++)
    {
        if (adHocConnections.at(i).name() == connectionName)
        {
            internalRefs  = adHocConnections.at(i).internalPortReferences();
            adHocConnections.removeAt(i);
            break;
        }
    }

    PortRef fromRef(sourcePort, sourceInstance);
    PortRef toRef(targetPort, targetInstance);
    
    if (internalRefs.isEmpty())
    {
        internalRefs.append(fromRef);        
    }
    internalRefs.append(toRef);

    AdHocConnection connection(connectionName, "", "", "", internalRefs);
    adHocConnections.append(connection);
    design_->setAdHocConnections(adHocConnections);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalAdhocConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalAdhocConnection()
{
    addPort("enable_from_sender", 1, General::OUT, topComponent_);
    addPort("data_from_sender", 8, General::OUT, topComponent_);

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV);

    addHierAdhocConnection("enable_from_sender", "sender", "enable_out");
    addHierAdhocConnection("data_from_sender", "sender", "data_out");

    runGenerator();

    verifyOutputContains(
        "    TestSender sender(\n"
        "        .data_out            (data_from_sender),\n"
        "        .enable_out          (enable_from_sender)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addHierAdhocConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addHierAdhocConnection(QString const& topPort, 
    QString const& targetInstance, QString const& targetPort)
{    
    PortRef topRef(topPort);
    QList<PortRef> externalRefs;
    externalRefs.append(topRef);

    PortRef toRef(targetPort, targetInstance);
    QList<PortRef> internalRefs;
    internalRefs.append(toRef);

    AdHocConnection connection("", "", "", "", internalRefs, externalRefs);

    QList<AdHocConnection> adHocConnections = design_->getAdHocConnections();
    adHocConnections.append(connection);
    design_->setAdHocConnections(adHocConnections);
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
        "endmodule");
}


//-----------------------------------------------------------------------------
// Function: tst_ComponentInstanceVerilogWriter::testDescriptionAndVLNVIsPrintedAboveInstance()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testDescriptionAndVLNVIsPrintedAboveInstance()
{
    QFETCH(QString, description);
    QFETCH(QString, expectedOutput);
    
    VLNV instanceVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    QSharedPointer<Component> refComponent(new Component(instanceVLNV));
    library_.addComponent(refComponent);

    ComponentInstance instance("instance1", "", description, instanceVLNV, QPointF(), "");

    QList<ComponentInstance> componentInstances = design_->getComponentInstances();
    componentInstances.append(instance);
    design_->setComponentInstances(componentInstances);

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

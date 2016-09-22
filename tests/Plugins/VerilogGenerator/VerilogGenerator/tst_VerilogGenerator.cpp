//-----------------------------------------------------------------------------
// File: tst_VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Unit test for class VerilogGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogGenerator/VerilogGenerator/VerilogGenerator.h>

#include <tests/MockObjects/LibraryMock.h>

#include <Plugins/common/HDLParser/HDLParserCommon.h>
#include <Plugins/VerilogImport/VerilogSyntax.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/Component.h>

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

    void testFileHeaderIsPrinted();

    /*void testHierarchicalConnections();
    void testHierarchicalConnectionsWithExpressions();
    void testSlicedHierarchicalConnection();
    void testUnknownInstanceIsNotWritten();

    void testMasterToSlaveInterconnection();
    void testMasterToSlaveInterconnectionWithExpressions();
    void testMasterToMultipleSlavesInterconnections();
    //void testInterconnectionToVaryingSizeLogicalMaps();
    void testSlicedInterconnection();
    void testMasterInterconnectionToMirroredMaster();
    void testMirroredSlaveInterconnectionToSlaves();  

    void testPortMapsWithoutBoundsInInterconnection();
    void testAdhocConnectionBetweenComponentInstances();    
    void testAdhocTieOffInComponentInstance();
    void testMultipleAdhocConnectionsBetweenComponentInstances();
    void testAdHocConnectionBetweenComponentInstancesWithExpressions();
    void testHierarchicalAdhocConnection();
    void testHierarchicalAdHocTieOffValues();
    void testAdHocConnectionToUnknownInstanceIsNotWritten();
    void testAdHocConnectionBetweenMultipleComponentInstances();
    
    void testDescriptionAndVLNVIsPrintedAboveInstance();
    void testDescriptionAndVLNVIsPrintedAboveInstance_data();

    void testTopLevelModuleParametersAreWritten();
    void testInstanceModuleParametersAreWritten();
	void testTopComponentParametersAreUtilized();
	void testInstanceComponentParametersAreUtilized();

	void testParameterPropagationFromTop();
    void testParameterPropagationFromTop2();
    void testParameterPropagationFromTopWire();

	void testParametersAreInOrder();
    void testParametersAreInOrder2();
    void testParametersAreInOrder3();*/

	void testImplementationSelection();
	void testImplementationSelectionWithTag();
	void testImplementationSelectionWithoutParameters();
	void testImplementationSelectionWithoutPorts();
	void testImplementationSelectionWithInstantiation();
	void testImplementationSelectionWithPostModule();
	void testImplementationSelectionWithTooManyModules();
	void testImplementationSelectionWithNoModuleHeaderStart();
	void testImplementationSelectionWithNoModuleHeaderEnd();
	void testImplementationSelectionWithNoModuleEnd();

	void testGenerationWithImplementation();
	void testGenerationWithImplementationWithTag();
	void testGenerationWithImplementationWithPostModule();

private:

    void addPort( QString const& portName, int portSize, DirectionTypes::Direction direction, 
        QSharedPointer<GenerationComponent> component);

    void addParameter(QString const& name, QString const& value, QString const& formattedValue="");

    void runGenerator(bool useDesign);

    void createHierarchicalConnection(QString const& topInterfaceRef, QString const& instanceInterfaceRef);

    QSharedPointer<View> addTestComponentToLibrary(VLNV vlnv);

    QSharedPointer<ComponentInstance> addInstanceToDesign(QString instanceName, VLNV instanceVlnv, QSharedPointer<View> activeView);

    void mapPortToInterface(QString const& portName, QString const& logicalName, 
        QString const& interfaceName, QSharedPointer<Component> component);

    void mapPortToInterface(QString const& portName, int left, int right, 
        QString const& logicalName, QString const& interfaceName, QSharedPointer<Component> component);
    
    void addInterfaceToComponent(QString const& interfaceName, QSharedPointer<Component> component);

    QSharedPointer<View> addSenderComponentToLibrary(VLNV senderVLNV, General::InterfaceMode mode);

    QSharedPointer<View> addReceiverComponentToLibrary(VLNV receiverVLNV, General::InterfaceMode mode);    

    void setReceiverComponentDataWidth(VLNV receiverVLNV, int dataWidth);

    void addConnectionToDesign(QString fromInstance, QString fromInterface, QString toInstance, QString toInterface);

    void addAdhocConnection(QString const& connectionName, QString const& sourceInstance, QString const& sourcePort,
        QString const& targetInstance, QString const& targetPort);

    void addTieOffAdhocConnectionToInstancePort(QString const& tieOffValue, QString const& instanceName,
        QString const& portName, QString const& connectionName);

    void addTieOffConnectionToTopComponentPort(QString const& tieOffValue, QString const& portName,
        QString const& connectionName);

    void addHierAdhocConnection(QString const& topPort, QString const& targetInstance, QString const& targetPort);

    void verifyOutputContains(QString const& expectedOutput);

    void compareOutputTo(QString const& expectedOutput);

    void readOutputFile();   
   
    QSharedPointer<GenerationComponent> topComponent_;
    QList<QSharedPointer<GenerationDesign> > parsedDesigns_;

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
tst_VerilogGenerator::tst_VerilogGenerator(): output_(), generationTime_(), library_(this)
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
    library_.clear();

    topComponent_ = QSharedPointer<GenerationComponent>(new GenerationComponent);
    QSharedPointer<Component> ipxactComponent(new Component);
    topComponent_->component = ipxactComponent;
    topComponent_->fileName_ = "generatorOutput.v";
    topComponent_->moduleName_ = "TestComponent";

    //QVERIFY(!QFile::exists("./generatorOutput.v"));
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
    addParameter("dataWidth", "8");
    addParameter("freq", "100000");

    runGenerator(false);

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
		"// " + VerilogSyntax::TAG_OVERRIDE + "\n"
        "endmodule\n"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addPort(QString const& portName, int portSize, 
    DirectionTypes::Direction direction, QSharedPointer<GenerationComponent> component)
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, direction));
    port->setPortSize(portSize);
    QSharedPointer<GenerationPort> gp(new GenerationPort);
    gp->port = port;
    gp->vectorBounds.first = QString::number(portSize-1);
    gp->vectorBounds.second = "0";
    component->ports.insert(portName,gp);
    component->component->getPorts()->append(port);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addParameter()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addParameter(QString const& name, QString const& value, QString const& formattedValue/*=""*/)
{
    QSharedPointer<Parameter> parameter = QSharedPointer<Parameter>(new Parameter());
    parameter->setName(name);
    parameter->setValue(value);

    topComponent_->originalParameters.append(parameter);

    parameter = QSharedPointer<Parameter>(new Parameter());
    parameter->setName(name);

    if (formattedValue.isEmpty())
    {
        parameter->setValue(value);
    }
    else
    {
        parameter->setValue(formattedValue);
    }

    topComponent_->formattedParameters.append(parameter);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::runGenerator()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::runGenerator(bool useDesign)
{
    VerilogGenerator generator(&library_, false);
	QString outputPath = ".";

	if (useDesign)
	{
		generator.parseDesign(outputPath, parsedDesigns_);
	}
	else
	{
		generator.parseComponent(outputPath, topComponent_);
	}

    generationTime_ =  QDateTime::currentDateTime();

    generator.generate(outputPath);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFileHeaderIsPrinted()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFileHeaderIsPrinted()
{    
    topComponent_->component->setDescription("Component description\nspanning multiple\nlines.");
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_->component);
    	
    QCoreApplication::setOrganizationName("TUT");
    QCoreApplication::setApplicationName("TestRunner");
    QSettings settings;
    settings.setValue("General/Username", "testUser");

    runGenerator(false);
    
    verifyOutputContains(QString(
        "//-----------------------------------------------------------------------------\n"
        "// File          : generatorOutput.v\n"
        "// Creation date : " + generationTime_.date().toString("dd.MM.yyyy") + "\n"
        "// Creation time : " + generationTime_.time().toString("hh:mm:ss") + "\n"
        "// Description   : Component description\n"
        "//                 spanning multiple\n"
        "//                 lines.\n"
		"// Created by    : testUser\n"
		"// Tool : Kactus2 \n"
		"// Plugin : Verilog generator \n"
		"// This file was generated based on IP-XACT component Test:TestLibrary:TestComponent:1.0\n"
        "// whose XML file is C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml\n"
        "//-----------------------------------------------------------------------------\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalConnections()
//-----------------------------------------------------------------------------
/*void tst_VerilogGenerator::testHierarchicalConnections()
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
    QSharedPointer<View> activeView = addTestComponentToLibrary(instanceVlnv);
    addInstanceToDesign("instance1", instanceVlnv, activeView);

    runGenerator(true);

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
        "        .clk                 (top_clk[0:0]),\n"
        "        // Interface: data\n"
        "        .data_in             (data_to_instance[7:0]),\n"
        "        .enable              (enable_to_instance[0:0]),\n"
        "        .full                (full_from_instance[0:0]),\n"
        "        // These ports are not in any interface\n"
        "        .data_out            ());\n"
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
    QSharedPointer<View> activeView = addTestComponentToLibrary(instanceVlnv);
    addInstanceToDesign("instance1", instanceVlnv, activeView);

    runGenerator(true);

    verifyOutputContains(QString(
        "    TestInstance instance1(\n"
        "        // Interface: clk\n"
        "        .clk                 (),\n"
        "        // Interface: data\n"
        "        .data_in             (data_to_instance[7:0]),\n"
        "        .enable              (enable_to_instance[0:0]),\n"
        "        .full                (full_from_instance[0:0]),\n"
        "        // These ports are not in any interface\n"
        "        .data_out            ());"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testUnknownInstanceIsNotWritten()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testUnknownInstanceIsNotWritten()
{
    VLNV nonLibraryComponent(VLNV::COMPONENT, "Test", "TestLibrary", "Unknown", "1.0");
    addInstanceToDesign("unknown", nonLibraryComponent, QSharedPointer<View>());

    runGenerator(true);

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
	QSharedPointer<PortMap::LogicalPort> logPort(new PortMap::LogicalPort(logicalName));
	QSharedPointer<PortMap::PhysicalPort> physPort(new PortMap::PhysicalPort(portName));

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
	QSharedPointer<PortMap::LogicalPort> logPort(new PortMap::LogicalPort(logicalName));
	QSharedPointer<PortMap::PhysicalPort> physPort(new PortMap::PhysicalPort(portName) );
	
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
QSharedPointer<View> tst_VerilogGenerator::addTestComponentToLibrary(VLNV vlnv)
{
    QSharedPointer<Component> instanceComponent(new Component(vlnv));
    addPort("clk", 1, DirectionTypes::IN, instanceComponent);
    addPort("data_in", 8, DirectionTypes::IN, instanceComponent);
    addPort("data_out", 8, DirectionTypes::OUT, instanceComponent);
    addPort("enable", 1, DirectionTypes::IN, instanceComponent);
	addPort("full", 1, DirectionTypes::OUT, instanceComponent);

	QSharedPointer<View> activeView(new View());
	activeView->setName("rtl");
	activeView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	instanceComponent->getComponentInstantiations()->append(instantiation);
	instanceComponent->getViews()->append(activeView);

    addInterfaceToComponent("clk", instanceComponent);

    mapPortToInterface("clk", "CLK", "clk", instanceComponent);

    addInterfaceToComponent("data", instanceComponent);

    mapPortToInterface("data_in", "DATA", "data", instanceComponent);
    mapPortToInterface("enable", "ENABLE", "data", instanceComponent);
    mapPortToInterface("full", "FULL", "data", instanceComponent);

    library_.addComponent(instanceComponent);

	return activeView;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addInstanceToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> tst_VerilogGenerator::addInstanceToDesign(QString instanceName, VLNV instanceVlnv, QSharedPointer<View> activeView)
{
    QSharedPointer<ConfigurableVLNVReference> componentVLNV (new ConfigurableVLNVReference(instanceVlnv));
    QSharedPointer<ComponentInstance> instance (new ComponentInstance(instanceName, componentVLNV));

    design_->getComponentInstances()->append(instance);

	if (activeView)
	{
		designConf_->addViewConfiguration(instanceName, activeView->name());
	}

	return instance;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterToSlaveInterconnection()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> activeView = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV, activeView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    activeView = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("receiver", receiverVLNV, activeView);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    runGenerator(true);

    verifyOutputContains(
    "    wire [7:0]  sender_to_receiver_DATA;\n"
    "    wire [0:0]  sender_to_receiver_ENABLE;\n"
    "\n"
    "    // IP-XACT VLNV: Test:TestLibrary:TestReceiver:1.0\n"
    "    TestReceiver receiver(\n"
    "        // Interface: data_bus\n"
    "        .data_in             (sender_to_receiver_DATA[7:0]),\n"
    "        .enable_in           (sender_to_receiver_ENABLE[0:0]));\n"   
    "\n"
    "    // IP-XACT VLNV: Test:TestLibrary:TestSender:1.0\n"
    "    TestSender sender(\n"
    "        // Interface: data_bus\n"
    "        .data_out            (sender_to_receiver_DATA[7:0]),\n"
    "        .enable_out          (sender_to_receiver_ENABLE[0:0]));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addSenderComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<View> tst_VerilogGenerator::addSenderComponentToLibrary(VLNV senderVLNV, General::InterfaceMode mode)
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

	QSharedPointer<View> activeView(new View());
	activeView->setName("rtl");
	activeView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	senderComponent->getComponentInstantiations()->append(instantiation);
	senderComponent->getViews()->append(activeView);

    library_.addComponent(senderComponent);

	return activeView;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addReceiverComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<View> tst_VerilogGenerator::addReceiverComponentToLibrary(VLNV receiverVLNV, General::InterfaceMode mode)
{
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));
    addPort("enable_in", 1, DirectionTypes::IN, receiverComponent);
    addPort("data_in", 8, DirectionTypes::IN, receiverComponent);

    addInterfaceToComponent("data_bus", receiverComponent);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(mode);    
    mapPortToInterface("data_in", "DATA", "data_bus", receiverComponent);

	mapPortToInterface("enable_in", "ENABLE", "data_bus", receiverComponent);

	QSharedPointer<View> activeView(new View());
	activeView->setName("rtl");
	activeView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	receiverComponent->getComponentInstantiations()->append(instantiation);
	receiverComponent->getViews()->append(activeView);

	library_.addComponent(receiverComponent);

	return activeView;
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
    connection->setStartInterface(firstInterface);
    connection->getActiveInterfaces()->append(secondInterface);

    design_->getInterconnections()->append(connection);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMasterToMultipleSlavesInterconnections()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterToMultipleSlavesInterconnections()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> sendView = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV, sendView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<View> recvView = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("receiver1", receiverVLNV, recvView);
    addInstanceToDesign("receiver2", receiverVLNV, recvView);

    addConnectionToDesign("sender", "data_bus", "receiver1", "data_bus");
    addConnectionToDesign("sender", "data_bus", "receiver2", "data_bus");    

    runGenerator(true);

    verifyOutputContains("wire [0:0]  sender_to_receiver1_ENABLE;");
    verifyOutputContains("wire [7:0]  sender_to_receiver1_DATA;");

    verifyOutputContains("TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (sender_to_receiver1_DATA[7:0]),\n"
        "        .enable_out          (sender_to_receiver1_ENABLE[0:0])");

    verifyOutputContains("TestReceiver receiver1(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_to_receiver1_DATA[7:0]),\n"
        "        .enable_in           (sender_to_receiver1_ENABLE[0:0])");

    verifyOutputContains("TestReceiver receiver2(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_to_receiver1_DATA[7:0]),\n"
        "        .enable_in           (sender_to_receiver1_ENABLE[0:0])");
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
        logPort = newLogicalPort;
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
	QSharedPointer<PortMap::LogicalPort> logPortLow(new PortMap::LogicalPort("ENABLE"));
	QSharedPointer<PortMap::PhysicalPort> physPortLow(new PortMap::PhysicalPort("enable_out_low"));
    enableLowPortMap->setLogicalPort(logPortLow);
	enableLowPortMap->setPhysicalPort(physPortLow);

    QSharedPointer<PartSelect> lowPart (new PartSelect("0", "0"));
    physPortLow->partSelect_ = lowPart;

    QSharedPointer<Range> lowRange (new Range("0", "0"));
    logPortLow->range_ = lowRange;

	QSharedPointer<PortMap> enableHighPortMap(new PortMap());
	QSharedPointer<PortMap::LogicalPort> logPortHigh(new PortMap::LogicalPort("ENABLE"));
	QSharedPointer<PortMap::PhysicalPort> physPortHigh(new PortMap::PhysicalPort("enable_out_high") );
    enableHighPortMap->setLogicalPort(logPortHigh);
    enableHighPortMap->setPhysicalPort(physPortHigh);

    QSharedPointer<PartSelect> highPart (new PartSelect("0", "0"));
    physPortHigh->partSelect_ = highPart;

    QSharedPointer<Range> highRange (new Range("1", "1"));
    logPortHigh->range_ = highRange;

    QSharedPointer<AbstractionType> senderEnableAbstraction (new AbstractionType());
    enableIf->getAbstractionTypes()->append(senderEnableAbstraction);

    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps = enableIf->getPortMaps();
    portMaps->append(enableLowPortMap);
	portMaps->append(enableHighPortMap);

	QSharedPointer<View> sendView(new View("rtl"));
	sendView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	senderComponent->getComponentInstantiations()->append(instantiation);
	senderComponent->getViews()->append(sendView);

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, sendView);

    VLNV receiver(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<View> recvView = addReceiverComponentToLibrary(receiver, General::SLAVE);
    QSharedPointer<Component> receiverComponent = library_.getModel(receiver).dynamicCast<Component>();
    QSharedPointer<PortMap> enableMap = receiverComponent->getBusInterface("data_bus")->getPortMaps()->last();
    enableMap->getLogicalPort()->range_ = QSharedPointer<Range>(new Range("0", "0"));

	addInstanceToDesign("receiver", receiver, recvView);

	QSharedPointer<ConfigurableVLNVReference> abstractionVLNV(new ConfigurableVLNVReference(
		VLNV::ABSTRACTIONDEFINITION, "Test", "TestLibrary", "absDef", "1.0"));

	QSharedPointer<AbstractionDefinition> testAbstractionDefinition(new AbstractionDefinition());
	testAbstractionDefinition->setVlnv(*abstractionVLNV.data());
	library_.addComponent(testAbstractionDefinition);

	QSharedPointer<PortAbstraction> logicalPort (new PortAbstraction());
	logicalPort->setName("ENABLE");
	QSharedPointer<WireAbstraction> wire(new WireAbstraction);
	logicalPort->setWire(wire);
	QSharedPointer<WirePort> wp(new WirePort);
	wp->setWidth("2");
	wire->setSlavePort(wp);
	wire->setMasterPort(wp);

	testAbstractionDefinition->getLogicalPorts()->append(logicalPort);

	enableIf->getAbstractionTypes()->first()->setAbstractionRef(abstractionVLNV);
	receiverComponent->getBusInterface("data_bus")->getAbstractionTypes()->first()->setAbstractionRef(abstractionVLNV);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    runGenerator(true);

    verifyOutputContains("wire [2-1:0] sender_to_receiver_ENABLE;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
		"        .enable_out_high     (sender_to_receiver_ENABLE[1:1]),\n"
        "        .enable_out_low      (sender_to_receiver_ENABLE[0:0]));");

    verifyOutputContains(
        "    TestReceiver receiver(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (),\n"
        "        .enable_in           (sender_to_receiver_ENABLE[0:0]));");


//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMirroredSlaveInterconnectionToSlaves()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMirroredSlaveInterconnectionToSlaves()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> sendView = addSenderComponentToLibrary(senderVLNV, General::MIRROREDSLAVE);
    addInstanceToDesign("sender", senderVLNV, sendView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "BusComponent", "1.0");
    QSharedPointer<View> recvView = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("bus1", receiverVLNV, recvView);
    addInstanceToDesign("bus2", receiverVLNV, recvView);

    addConnectionToDesign("sender", "data_bus", "bus1", "data_bus");
    addConnectionToDesign("sender", "data_bus", "bus2", "data_bus");

    runGenerator(true);

    verifyOutputContains("wire [7:0]  sender_to_bus1_DATA;");
    verifyOutputContains("wire [0:0]  sender_to_bus1_ENABLE;");    

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (sender_to_bus1_DATA[7:0]),\n"
        "        .enable_out          (sender_to_bus1_ENABLE[0:0])");

    verifyOutputContains(
        "    BusComponent bus1(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_to_bus1_DATA[7:0]),\n"
        "        .enable_in           (sender_to_bus1_ENABLE[0:0])");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testPortMapsWithoutBoundsInInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testPortMapsWithoutBoundsInInterconnection()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");

    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    addPort("data_out", 8, DirectionTypes::OUT, senderComponent);
    
    QSharedPointer<BusInterface> senderInterface(new BusInterface());
    senderInterface->setName("data_bus");
    senderInterface->setInterfaceMode(General::MASTER);
    senderComponent->getBusInterfaces()->append(senderInterface);
    
    QSharedPointer<PortMap::LogicalPort> senderLogicalPort(new PortMap::LogicalPort("DATA"));
    QSharedPointer<PortMap::PhysicalPort> senderPhysicalPort(new PortMap::PhysicalPort("data_out"));

    QSharedPointer<PortMap> senderPortMap(new PortMap());
    senderPortMap->setLogicalPort(senderLogicalPort);
    senderPortMap->setPhysicalPort(senderPhysicalPort);

    QSharedPointer<ConfigurableVLNVReference> abstractionVLNV(new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "Test", "TestLibrary", "absDef", "1.0"));
        
    QSharedPointer<AbstractionDefinition> testAbstractionDefinition (new AbstractionDefinition());
    testAbstractionDefinition->setVlnv(*abstractionVLNV.data());

    QSharedPointer<AbstractionType> senderAbstraction(new AbstractionType());
    senderAbstraction->setAbstractionRef(abstractionVLNV);
    senderAbstraction->getPortMaps()->append(senderPortMap);
	senderInterface->getAbstractionTypes()->append(senderAbstraction);

	QSharedPointer<View> sendView(new View("rtl"));
	sendView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	senderComponent->getComponentInstantiations()->append(instantiation);
	senderComponent->getViews()->append(sendView);

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, sendView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "BusComponent", "1.0");
 
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));
    addPort("data_in", 8, DirectionTypes::IN, receiverComponent);

    QSharedPointer<BusInterface> receiverInterface(new BusInterface());
    receiverInterface->setName("data_bus");
    receiverInterface->setInterfaceMode(General::SLAVE);
    receiverComponent->getBusInterfaces()->append(receiverInterface);

    QSharedPointer<PortMap::LogicalPort> receiverLogicalPort(new PortMap::LogicalPort("DATA"));
    QSharedPointer<PortMap::PhysicalPort> receiverPhysicalPort(new PortMap::PhysicalPort("data_in"));

    QSharedPointer<PortMap> receiverPortMap(new PortMap());
    receiverPortMap->setLogicalPort(receiverLogicalPort);
    receiverPortMap->setPhysicalPort(receiverPhysicalPort);

    QSharedPointer<AbstractionType> receiverAbstraction(new AbstractionType());
    receiverAbstraction->setAbstractionRef(abstractionVLNV);
    receiverAbstraction->getPortMaps()->append(receiverPortMap);
	receiverInterface->getAbstractionTypes()->append(receiverAbstraction);

	QSharedPointer<View> recvView(new View("rtl"));
	recvView->setComponentInstantiationRef("instance2");

	QSharedPointer<ComponentInstantiation> instantiation2(new ComponentInstantiation("instance2"));
	receiverComponent->getComponentInstantiations()->append(instantiation2);
	receiverComponent->getViews()->append(recvView);
    
    library_.addComponent(receiverComponent);
    addInstanceToDesign("receiver", receiverVLNV, recvView);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    runGenerator(true);

    verifyOutputContains("wire [7:0]  sender_to_receiver_DATA;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (sender_to_receiver_DATA[7:0]));\n");

    verifyOutputContains(
        "    BusComponent receiver(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_to_receiver_DATA[7:0]));\n");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testAdhocConnectionBetweenTwoComponentInstances()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testAdhocConnectionBetweenComponentInstances()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> senderView = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV, senderView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<View> receiverView = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("receiver1", receiverVLNV, receiverView);
    addInstanceToDesign("receiver2", receiverVLNV, receiverView);

    addAdhocConnection("enableAdHoc", "sender", "enable_out", "receiver1", "enable_in");
    addAdhocConnection("enableAdHoc", "sender", "enable_out", "receiver2", "enable_in");
    addAdhocConnection("dataAdHoc", "sender", "data_out", "receiver1", "data_in");

    runGenerator(true);

    verifyOutputContains("wire [0:0]  enableAdHoc;");
    verifyOutputContains("wire [7:0]  dataAdHoc;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (dataAdHoc[7:0]),\n"
        "        .enable_out          (enableAdHoc[0:0])");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (dataAdHoc[7:0]),\n"
        "        .enable_in           (enableAdHoc[0:0])");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (),\n"
        "        .enable_in           (enableAdHoc[0:0])");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testAdhocTieOffInComponentInstance()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testAdhocTieOffInComponentInstance()
{
    VLNV tieOffVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestTieOff", "1.0");
	QSharedPointer<Component> tieOffComponent(new Component(tieOffVLNV));

	QSharedPointer<View> activeView(new View("rtl"));
	activeView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	tieOffComponent->getComponentInstantiations()->append(instantiation);
	tieOffComponent->getViews()->append(activeView);

    QString instanceName = "tieOffer";

    QString zeroName = "zeroTieOff";
    QString oneName = "oneTieOff";
    QString naName = "n/aTieOff";
    QString numberedName = "numberedTieOff";
    QString outName = "tieOffOut";
    QString inOutName = "tieOffInOut";
    QString defaultName = "defaultTieOff";
    QString openName = "openTieOff";
    QString expressionName = "expressionTieOff";

    addPort(zeroName, 2, DirectionTypes::IN, tieOffComponent);
    addPort(oneName, 4, DirectionTypes::IN, tieOffComponent);
    addPort(naName, 0, DirectionTypes::IN, tieOffComponent);
    addPort(numberedName, 10, DirectionTypes::IN, tieOffComponent);
    addPort(outName, 2, DirectionTypes::OUT, tieOffComponent);
    addPort(inOutName, 10, DirectionTypes::INOUT, tieOffComponent);
    addPort(defaultName, 1, DirectionTypes::IN, tieOffComponent);
    addPort(openName, 1, DirectionTypes::IN, tieOffComponent);
    addPort(expressionName, 1, DirectionTypes::IN, tieOffComponent);

    library_.addComponent(tieOffComponent);

    addInstanceToDesign(instanceName, tieOffVLNV, activeView);

    QSharedPointer<Port> defaultPort = tieOffComponent->getPort("defaultTieOff");
    defaultPort->setDefaultValue("20");

    QSharedPointer<Parameter> expressionParameter (new Parameter());
    expressionParameter->setName("expName");
    expressionParameter->setValueId("expID");
    expressionParameter->setValue("5");
    tieOffComponent->getParameters()->append(expressionParameter);

    addTieOffAdhocConnectionToInstancePort("0", instanceName, zeroName, "zero_connection");
    addTieOffAdhocConnectionToInstancePort("1", instanceName, oneName, "one_connection");
    addTieOffAdhocConnectionToInstancePort("abc", instanceName, naName, "n/a_connection");
    addTieOffAdhocConnectionToInstancePort("12", instanceName, numberedName, "number_connection");
    addTieOffAdhocConnectionToInstancePort("0", instanceName, outName, "out_connection");
    addTieOffAdhocConnectionToInstancePort("1", instanceName, inOutName, "inOut_connection");
    addTieOffAdhocConnectionToInstancePort("default", instanceName, defaultName, "default_connection");
    addTieOffAdhocConnectionToInstancePort("open", instanceName, openName, "open_connection");
    addTieOffAdhocConnectionToInstancePort("expID - 4", instanceName, expressionName, "expression_connection");

    runGenerator(true);

    verifyOutputContains(
        "        // These ports are not in any interface\n"
        "        .defaultTieOff       (20),\n"
        "        .expressionTieOff    (expName - 4),\n"
        "        .n/aTieOff           (abc),\n"
        "        .numberedTieOff      (12),\n"
        "        .oneTieOff           (1),\n"
        "        .openTieOff          (),\n"
        "        .zeroTieOff          (0),\n"
        "        .tieOffOut           (),\n"
        "        .tieOffInOut         (1));");
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

	QSharedPointer<View> sendView(new View("rtl"));
	sendView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	senderComponent->getComponentInstantiations()->append(instantiation);
	senderComponent->getViews()->append(sendView);

    addInterfaceToComponent("data_bus", senderComponent);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(General::MASTER);

    mapPortToInterface("enable_out", "ENABLE", "data_bus", senderComponent);
    QSharedPointer<PortMap> dataMap = senderComponent->getBusInterface("data_bus")->getPortMaps()->first();
    QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
    logPort->range_ = QSharedPointer<Range>(new Range("10/2","1"));

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, sendView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));

    QSharedPointer<Port> receiverPort = QSharedPointer<Port>(new Port("enable_in", DirectionTypes::IN));
    receiverPort->setLeftBound("2+2");
    receiverPort->setRightBound("0");
    receiverComponent->getPorts()->append(receiverPort);

    addInterfaceToComponent("data_bus", receiverComponent);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(General::SLAVE);
	mapPortToInterface("enable_in", "ENABLE", "data_bus", receiverComponent);

	QSharedPointer<View> recvView(new View("rtl"));
	recvView->setComponentInstantiationRef("instance2");

	QSharedPointer<ComponentInstantiation> instantiation2(new ComponentInstantiation("instance2"));
	receiverComponent->getComponentInstantiations()->append(instantiation2);
	receiverComponent->getViews()->append(recvView);

    library_.addComponent(receiverComponent);
    addInstanceToDesign("receiver1", receiverVLNV, recvView);
    addInstanceToDesign("receiver2", receiverVLNV, recvView);

    addAdhocConnection("enableAdHoc", "sender", "enable_out", "receiver1", "enable_in");
    addAdhocConnection("enableAdHoc", "sender", "enable_out", "receiver2", "enable_in");

    runGenerator(true);

    verifyOutputContains("wire [10*2-8:2] enableAdHoc;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .enable_out          (enableAdHoc[10*2-8:2])");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_bus\n"
        "        .enable_in           (enableAdHoc[2+2:0]));\n");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        // Interface: data_bus\n"
        "        .enable_in           (enableAdHoc[2+2:0]));\n");
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
// Function: tst_VerilogGenerator::addTieOffAdhocConnectionToInstancePort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addTieOffAdhocConnectionToInstancePort(QString const& tieOffValue,
    QString const& instanceName, QString const& portName, QString const& connectionName)
{
    QSharedPointer<PortReference> referencedPort(new PortReference(portName, instanceName));

    QSharedPointer<AdHocConnection> connection(new AdHocConnection(connectionName));
    connection->setTiedValue(tieOffValue);
    connection->getInternalPortReferences()->append(referencedPort);

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
    QSharedPointer<View> activeView = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV, activeView);

    addHierAdhocConnection("enable_from_sender", "sender", "enable_out");
    addHierAdhocConnection("data_from_sender", "sender", "data_out");

    runGenerator(true);

    readOutputFile();

    QStringList outputList = output_.split("\n");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (data_from_sender[7:0]),\n"
        "        .enable_out          (enable_from_sender[0:0])");
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
// Function: tst_VerilogGenerator::testHierarchicalAdHocTieOffValues()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalAdHocTieOffValues()
{
    QString zeroName = "zeroTieOff";
    QString oneName = "oneTieOff";
    QString naName = "n/aTieOff";
    QString numberedName = "numberedTieOff";
    QString inName = "tieOffIn";
    QString inOutName = "inOutTieOff";
    QString defaultName = "defaultTieOff";
    QString openName = "openTieOff";
    QString expressionName = "expressionTieOff";

    addPort(zeroName, 2, DirectionTypes::OUT, topComponent_);
    addPort(oneName, 4, DirectionTypes::OUT, topComponent_);
    addPort(naName, 0, DirectionTypes::OUT, topComponent_);
    addPort(numberedName, 10, DirectionTypes::OUT, topComponent_);
    addPort(inName, 2, DirectionTypes::IN, topComponent_);
    addPort(inOutName, 10, DirectionTypes::INOUT, topComponent_);
    addPort(defaultName, 1, DirectionTypes::OUT, topComponent_);
    addPort(openName, 1, DirectionTypes::OUT, topComponent_);
    addPort(expressionName, 1, DirectionTypes::OUT, topComponent_);

    QSharedPointer<Port> defaultPort = topComponent_->getPort("defaultTieOff");
    defaultPort->setDefaultValue("20");

    QSharedPointer<Parameter> expressionParameter (new Parameter());
    expressionParameter->setName("expName");
    expressionParameter->setValueId("expID");
    expressionParameter->setValue("6");
    topComponent_->getParameters()->append(expressionParameter);

    addTieOffConnectionToTopComponentPort("0", zeroName, "zero_connection");
    addTieOffConnectionToTopComponentPort("1", oneName, "one_connection");
    addTieOffConnectionToTopComponentPort("abc", naName, "n/a_connection");
    addTieOffConnectionToTopComponentPort("12", numberedName, "number_connection");
    addTieOffConnectionToTopComponentPort("0", inName, "in_connection");
    addTieOffConnectionToTopComponentPort("1", inOutName, "inOut_connection");
    addTieOffConnectionToTopComponentPort("default", defaultName, "default_connection");
    addTieOffConnectionToTopComponentPort("open", openName, "open_connection");
    addTieOffConnectionToTopComponentPort("expID - 4", expressionName, "expression_connection");

    runGenerator(true);

    verifyOutputContains(
        "    // Tie off values for the ports of the encompassing component\n"
        "    assign defaultTieOff = 20;\n"
        "    assign expressionTieOff = expName - 4;\n"
        "    assign inOutTieOff = 1;\n"
        "    assign n/aTieOff = abc;\n"
        "    assign numberedTieOff = 12;\n"
        "    assign oneTieOff = 1;\n"
        "    assign zeroTieOff = 0;\n"
        );
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addTieOffConnectionToTopComponentPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addTieOffConnectionToTopComponentPort(QString const& tieOffValue,
    QString const& portName, QString const& connectionName)
{
    QSharedPointer<PortReference> newPortReference (new PortReference(portName));

    QSharedPointer<AdHocConnection> newConnection(new AdHocConnection(connectionName));
    newConnection->setTiedValue(tieOffValue);
    newConnection->getExternalPortReferences()->append(newPortReference);

    design_->getAdHocConnections()->append(newConnection);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testAdHocConnectionToUnknownInstanceIsNotWritten()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testAdHocConnectionToUnknownInstanceIsNotWritten()
{
    addAdhocConnection("notConnected", "unknownInstance", "emptyPort", "unknownInstance", "emptyPort" );
    addHierAdhocConnection("top_clk", "unknownInstance", "clk");

    runGenerator(true);

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

	QSharedPointer<View> activeView = addTestComponentToLibrary(*instanceVLNV);

    QSharedPointer<ComponentInstance> instance = addInstanceToDesign("instance1", *instanceVLNV, activeView);
    instance->setDescription(description);

    design_->getComponentInstances()->append(instance);

    runGenerator(true);

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
// Function: tst_VerilogGenerator::testTopLevelModuleParametersAreWritten()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testTopLevelModuleParametersAreWritten()
{
    QSharedPointer<ModuleParameter> moduleParameter(new ModuleParameter());
    moduleParameter->setName("moduleParameter");
    moduleParameter->setValue("1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	instantiation->getModuleParameters()->append(moduleParameter);

	topComponent_->getComponentInstantiations()->append(instantiation);
    topView_->setComponentInstantiationRef(instantiation->name());

    runGenerator(false);

    verifyOutputContains(QString(
        "module TestComponent #(\n"
        "    parameter                              moduleParameter  = 1\n"
        ") ();"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testInstanceModuleParametersAreWritten()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testInstanceModuleParametersAreWritten()
{
	QSharedPointer<View> activeView(new View("rtl"));
	activeView->setComponentInstantiationRef("instance1");

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");    
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, activeView);

    QSharedPointer<ModuleParameter> moduleParameter(new ModuleParameter());
    moduleParameter->setValueId("parameterId");
    moduleParameter->setName("moduleParameter");
    moduleParameter->setValue("1");
	moduleParameter->setValueResolve("user");

    QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
    instantiation->getModuleParameters()->append(moduleParameter);

    senderComponent->getComponentInstantiations()->append(instantiation);
    senderComponent->getViews()->append(activeView);

    QSharedPointer<ComponentInstance> senderInstance = design_->getComponentInstances()->first();
    
    QSharedPointer<ConfigurableElementValue> parameterOverride(new ConfigurableElementValue());
    parameterOverride->setReferenceId("parameterId");
    parameterOverride->setConfigurableValue("2");
    senderInstance->getConfigurableElementValues()->append(parameterOverride);

    runGenerator(true);

    verifyOutputContains(
        "    TestSender #(\n"
        "        .moduleParameter     (2))\n"
        "    sender();");
}*/

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelection()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
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
		"foo\n"
		"bar\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	VerilogGenerator generator(&library_,false);

	QString implementation;
	QString postModule;

	generator.selectImplementation("./generatorOutput.v", implementation, postModule);

	implementation = implementation.trimmed();

	QCOMPARE(implementation,QString("foo\nbar"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelectionWithTag()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelectionWithTag()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
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
		"// " + VerilogSyntax::TAG_OVERRIDE + "\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	VerilogGenerator generator(&library_,false);

	QString implementation;
	QString postModule;

	generator.selectImplementation("./generatorOutput.v", implementation, postModule);

	implementation = implementation.trimmed();

	QCOMPARE(implementation,QString("foo\nbar"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelectionWithoutParameters()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelectionWithoutParameters()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent(\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n"
		"dataOut <= rst_n & clk\n"
		"bar\n"
		"baz\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	VerilogGenerator generator(&library_,false);

	QString implementation;
	QString postModule;

	generator.selectImplementation("./generatorOutput.v", implementation, postModule);

	implementation = implementation.trimmed();

	QCOMPARE(implementation,QString("dataOut <= rst_n & clk\nbar\nbaz"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelectionWithoutPorts()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelectionWithoutPorts()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent();\n"
		"dataOut <= rst_n & clk\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	VerilogGenerator generator(&library_,false);

	QString implementation;
	QString postModule;

	generator.selectImplementation("./generatorOutput.v", implementation, postModule);

	implementation = implementation.trimmed();

	QCOMPARE(implementation,QString("dataOut <= rst_n & clk"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelectionWithInstantiation()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelectionWithInstantiation()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n");

	QString expectedImplementation = QString(
		"foo\n"
		"bar\n"
		"\n"
		"// IP-XACT VLNV: tut.fi:ip.hw:TestIntitializer:1.0\n"
		"TestIntitializer #(\n"
		".WAIT_TIME           (2100))\n"
		"TestIntitializer_0(\n"
		"// These ports are not in any interface\n"
		".done                (SampleHW_0_done_to_TestIntitializer_0_done),\n"
		".clk                 (TestIntitializer_0_clk_to_SampleHW_0_clk),\n"
		".rst                 (SampleHW_0_rst_to_TestIntitializer_0_rst),\n"
		".start\n"
		");");

	outputStream << content << expectedImplementation << "endmodule";

	existingFile.close();

	VerilogGenerator generator(&library_,false);

	QString actualImplementation;
	QString postModule;

	generator.selectImplementation("./generatorOutput.v", actualImplementation, postModule);

	actualImplementation = actualImplementation.trimmed();

	QCOMPARE(actualImplementation,expectedImplementation);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelectionWithPostModule()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelectionWithPostModule()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
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
		"foo\n"
		"bar\n"
		"endmodule"
		); 

	QString expectedPostModule = QString(
		"lrem ipsum\n"
		"    parameter                              bbb        = 64,\n"
		"    parameter                              two        = bbb * 2 \n"
		"// Bogus copy paste stuff\n"
		"foo\n"
		"bar"
		); 

	outputStream << content << endl << expectedPostModule;

	existingFile.close();

	VerilogGenerator generator(&library_,false);

	QString implementation;
	QString actualPostModule;

	generator.selectImplementation("./generatorOutput.v", implementation, actualPostModule);

	implementation = implementation.trimmed();
	actualPostModule = actualPostModule.trimmed();

	QCOMPARE(implementation,QString("foo\nbar"));
	QCOMPARE(actualPostModule,expectedPostModule);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelectionWithTooManyModules()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelectionWithTooManyModules()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
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
		"foo\n"
		"bar\n"
		"endmodule"
		); 

	QString extraModule = QString(
		"module MasterComponent #(\n"
		"    parameter                              bbb        = 64,\n"
		"    parameter                              two        = bbb * 2 \n"
		") (\n"        
		"    input          [7:0]                iii,\n"
		"    output         [7:0]                aaa\n"
		");\n"
		"foo\n"
		"bar\n"
		"endmodule"
		); 

	outputStream << content << endl << extraModule;

	existingFile.close();

	VerilogGenerator generator(&library_,false);

	QString implementation;
	QString postModule;

	bool success = generator.selectImplementation("./generatorOutput.v", implementation, postModule);

	QVERIFY(!success);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelectionWithNoModuleHeaderStart()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelectionWithNoModuleHeaderStart()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		");\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	VerilogGenerator generator(&library_,false);

	QString implementation;
	QString postModule;

	bool success = generator.selectImplementation("./generatorOutput.v", implementation, postModule);

	QVERIFY(!success);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelectionWithNoModuleHeaderEnd()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelectionWithNoModuleHeaderEnd()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
		"module TestComponent #(\n"
		"    parameter                              dataWidth        = 8,\n"
		"    parameter                              freq             = 100000\n"
		") (\n"
		"    // These ports are not in any interface\n"         
		"    input                               clk,\n"
		"    input          [7:0]                dataIn,\n"
		"    input                               rst_n,\n"
		"    output         [7:0]                dataOut\n"
		"\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		); 

	outputStream << content;

	existingFile.close();

	VerilogGenerator generator(&library_,false);

	QString implementation;
	QString postModule;

	bool success = generator.selectImplementation("./generatorOutput.v", implementation, postModule);

	QVERIFY(!success);
}


//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelectionWithNoModuleEnd()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelectionWithNoModuleEnd()
{
	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	QString content = QString(
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
		"foo\n"
		"bar\n"
		); 

	outputStream << content;

	existingFile.close();

	VerilogGenerator generator(&library_,false);

	QString implementation;
	QString postModule;

	bool success = generator.selectImplementation("./generatorOutput.v", implementation, postModule);

	QVERIFY(!success);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testGenerationWithImplementation()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testGenerationWithImplementation()
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

	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	outputStream << contentBefore;

	existingFile.close();

	addPort("clk", 1, DirectionTypes::IN, topComponent_);
	addPort("rst_n", 1, DirectionTypes::IN, topComponent_);
	addPort("dataOut", 8, DirectionTypes::OUT, topComponent_);
	addPort("dataIn", 8, DirectionTypes::IN, topComponent_);
	addParameter("dataWidth", "8");
	addParameter("freq", "100000");

	runGenerator(false);

	QString contentAfter(
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
		"// " + VerilogSyntax::TAG_OVERRIDE + "\n"
		"assign adr_slave_1 = adr_master;\n"
		"assign cyc_slave_1 = cyc_master;\n"
		"endmodule\n"
		);

	verifyOutputContains(contentAfter);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testGenerationWithImplementationWithTag()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testGenerationWithImplementationWithTag()
{
	QString content(
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
		"// " + VerilogSyntax::TAG_OVERRIDE + "\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		);

	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	outputStream << content;

	existingFile.close();

	addPort("clk", 1, DirectionTypes::IN, topComponent_);
	addPort("rst_n", 1, DirectionTypes::IN, topComponent_);
	addPort("dataOut", 8, DirectionTypes::OUT, topComponent_);
	addPort("dataIn", 8, DirectionTypes::IN, topComponent_);
	addParameter("dataWidth", "8");
	addParameter("freq", "100000");

	runGenerator(false);

	verifyOutputContains(content);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testGenerationWithImplementationWithPostModule()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testGenerationWithImplementationWithPostModule()
{
	QString content(
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
		"// " + VerilogSyntax::TAG_OVERRIDE + "\n"
		"foo\n"
		"bar\n"
		"endmodule\n"
		"additional stuff\n"
		"more stuff"
		);

	QFile existingFile("./generatorOutput.v");

	existingFile.open(QIODevice::WriteOnly);
	QTextStream outputStream(&existingFile);

	outputStream << content;

	existingFile.close();

	addPort("clk", 1, DirectionTypes::IN, topComponent_);
	addPort("rst_n", 1, DirectionTypes::IN, topComponent_);
	addPort("dataOut", 8, DirectionTypes::OUT, topComponent_);
	addPort("dataIn", 8, DirectionTypes::IN, topComponent_);
	addParameter("dataWidth", "8");
	addParameter("freq", "100000");

	runGenerator(false);

	verifyOutputContains(content);
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

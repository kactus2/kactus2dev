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
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

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

    void testHierarchicalConnections();

    void testMasterToSlaveInterconnection();
    void testMasterToMultipleSlavesInterconnections();

    void testAdhocConnectionBetweenComponentInstances();    
    void testAdhocTieOffInComponentInstance();
    void testHierarchicalAdhocConnection();
    void testHierarchicalAdHocTieOffValues();
    
    void testDescriptionAndVLNVIsPrintedAboveInstance();
    void testDescriptionAndVLNVIsPrintedAboveInstance_data();

    void testTopLevelParametersAreWritten();
    void testInstanceParametersAreWritten();

	void testImplementationSelection();
    void testImplementationSelectionWithEvilComments();
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

    QSharedPointer<GenerationPort> addPort( QString const& portName, int portSize, DirectionTypes::Direction direction, 
        QSharedPointer<GenerationComponent> component);

    void addParameter(QString const& name, QString const& value, QString const& formattedValue="");

    void runGenerator(bool useDesign);

    void createHierarchicalPortAssignment(QSharedPointer<GenerationPort> port, QString const& topPort, QSharedPointer<GenerationInstance> instance, QString const& leftBound, QString const& rightBound);

    void createWirePortAssignment(QSharedPointer<GenerationPort> port, QSharedPointer<GenerationWire> wire, QSharedPointer<GenerationInstance> instance, QString const& leftBound, QString const& rightBound);

    QSharedPointer<GenerationComponent> addTestComponentToLibrary(VLNV vlnv);

    QSharedPointer<GenerationInstance> addInstanceToDesign(QString instanceName, QSharedPointer<GenerationComponent> component);

    void mapPortToInterface(QSharedPointer<GenerationPort> port, QString const& interfaceName, QSharedPointer<GenerationComponent> component);
    
    void addInterfaceToComponent(QString const& interfaceName, QSharedPointer<GenerationComponent> component);

    QSharedPointer<GenerationComponent> addSenderComponentToLibrary(VLNV senderVLNV, General::InterfaceMode mode);

    QSharedPointer<GenerationComponent> addReceiverComponentToLibrary(VLNV receiverVLNV, General::InterfaceMode mode);

    QSharedPointer<GenerationInterconnection> addConnectionToDesign();

    QSharedPointer<GenerationWire> addWireToDesign(QSharedPointer<GenerationInterconnection> gic, QString name, QString leftBound, QString rightBound);

    void addTieOffAdhocConnectionToInstancePort(QSharedPointer<GenerationPort> port, QString tieOff,
        QSharedPointer<GenerationInstance> instance);

    void addTieOffConnectionToTopComponentPort(QString const& tieOffValue, QString const& portName);

    void verifyOutputContains(QString const& expectedOutput);

    void compareOutputTo(QString const& expectedOutput);

    void readOutputFile();   
   
    QSharedPointer<GenerationComponent> topComponent_;
    QList<QSharedPointer<GenerationDesign> > parsedDesigns_;
    QSharedPointer<GenerationDesign> design_;

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
    topComponent_ = QSharedPointer<GenerationComponent>(new GenerationComponent);
    topComponent_->fileName_ = "generatorOutput.v";
    topComponent_->moduleName_ = "TestComponent";

    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    topComponent_->component = QSharedPointer<Component>(new Component(vlnv));

    design_ = QSharedPointer<GenerationDesign>(new GenerationDesign);
    design_->topComponent_ = topComponent_;

    parsedDesigns_.clear();
    parsedDesigns_.append(design_);

    library_.clear();
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
QSharedPointer<GenerationPort> tst_VerilogGenerator::addPort(QString const& portName, int portSize, 
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

    return gp;
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
    VerilogGenerator generator(&library_,false,false);
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

    generator.generate("bogusVersion", "bogusVersion");
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
        "// File          : ./generatorOutput.v\n"
        "// Creation date : " + generationTime_.date().toString("dd.MM.yyyy") + "\n"
        "// Creation time : " + generationTime_.time().toString("hh:mm:ss") + "\n"
        "// Description   : Component description\n"
        "//                 spanning multiple\n"
        "//                 lines.\n"
		"// Created by    : testUser\n"
		"// Tool : Kactus2 bogusVersion\n"
		"// Plugin : Verilog generator bogusVersion\n"
		"// This file was generated based on IP-XACT component Test:TestLibrary:TestComponent:1.0\n"
        "// whose XML file is C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml\n"
        "//-----------------------------------------------------------------------------\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalConnections()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalConnections()
{
    addInterfaceToComponent("clk_if", topComponent_);
    addInterfaceToComponent("data_bus", topComponent_);

    QSharedPointer<GenerationPort> gp = addPort("top_clk", 1, DirectionTypes::IN, topComponent_);
    mapPortToInterface(gp,"clk_if",topComponent_);
    gp = addPort("data_to_instance", 8, DirectionTypes::IN, topComponent_);
    mapPortToInterface(gp,"data_bus",topComponent_);
    gp = addPort("enable_to_instance", 1, DirectionTypes::IN, topComponent_);
    mapPortToInterface(gp,"data_bus",topComponent_);
    gp = addPort("full_from_instance", 1, DirectionTypes::OUT, topComponent_);
    mapPortToInterface(gp,"data_bus",topComponent_);

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");
    QSharedPointer<GenerationComponent> gc = addTestComponentToLibrary(instanceVlnv);
    QSharedPointer<GenerationInstance> gi = addInstanceToDesign("instance1", gc);

    createHierarchicalPortAssignment(gc->ports["clk"], "top_clk", gi, "0", "0");
    createHierarchicalPortAssignment(gc->ports["data_in"], "data_to_instance", gi, "7", "0");
    createHierarchicalPortAssignment(gc->ports["enable"], "enable_to_instance", gi, "0", "0");
    createHierarchicalPortAssignment(gc->ports["full"], "full_from_instance", gi, "0", "0");
    createHierarchicalPortAssignment(gc->ports["data_out"], "", gi, "", "");

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
        "        .clk                 (top_clk),\n"
        "        // Interface: data\n"
        "        .data_in             (data_to_instance[7:0]),\n"
        "        .enable              (enable_to_instance),\n"
        "        .full                (full_from_instance),\n"
        "        // These ports are not in any interface\n"
        "        .data_out            ());\n"
        "\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createHierarchicalPortAssignment()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::createHierarchicalPortAssignment(QSharedPointer<GenerationPort> port,
    QString const& topPort,
    QSharedPointer<GenerationInstance> instance,
    QString const& leftBound,
    QString const& rightBound)
{
    QSharedPointer<GenerationPortAssignMent> gpa(new GenerationPortAssignMent);
    gpa->bounds.first = leftBound;
    gpa->bounds.second = rightBound;
    gpa->port = port;
    gpa->topPort = topPort;

    instance->portAssignments_.insert(port->port->name(), gpa);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createWirePortAssignment()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::createWirePortAssignment(QSharedPointer<GenerationPort> port,
    QSharedPointer<GenerationWire> wire,
    QSharedPointer<GenerationInstance> instance,
    QString const& leftBound,
    QString const& rightBound)
{
    QSharedPointer<GenerationPortAssignMent> gpa(new GenerationPortAssignMent);
    gpa->bounds.first = leftBound;
    gpa->bounds.second = rightBound;
    gpa->port = port;
    gpa->wire = wire;

    instance->portAssignments_.insert(port->port->name(), gpa);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addInterfaceToComponent()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addInterfaceToComponent(QString const& interfaceName,
    QSharedPointer<GenerationComponent> component)
{
    QSharedPointer<GenerationInterface> gi(new GenerationInterface());
    gi->name = interfaceName;
    component->interfaces.insert(interfaceName,gi);

    QSharedPointer<BusInterface> bi = QSharedPointer<BusInterface>(new BusInterface);
    bi->setName(interfaceName);
    gi->interface = bi;
    component->component->getBusInterfaces()->append(bi);

    QSharedPointer<AbstractionType> at = QSharedPointer<AbstractionType>(new AbstractionType);
    bi->getAbstractionTypes()->append(at);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::mapPortToInterface()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::mapPortToInterface(QSharedPointer<GenerationPort> port, QString const& interfaceName,
    QSharedPointer<GenerationComponent> component)
{
    QSharedPointer<GenerationInterface> gi = component->interfaces[interfaceName];
    QSharedPointer<BusInterface> bi = gi->interface;
    QSharedPointer<AbstractionType> at = bi->getAbstractionTypes()->first();
       
    QSharedPointer<PortMap> pm(new PortMap);
    QSharedPointer<PortMap::PhysicalPort> pp(new PortMap::PhysicalPort);
    pp->name_ = port->port->name();
    pm->setPhysicalPort(pp);
    at->getPortMaps()->append(pm);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addTestComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationComponent> tst_VerilogGenerator::addTestComponentToLibrary(VLNV vlnv)
{
    QSharedPointer<Component> instanceComponent(new Component(vlnv));

	QSharedPointer<GenerationComponent> gc(new GenerationComponent);
    gc->component = instanceComponent;
    gc->moduleName_ = vlnv.getName();

    addInterfaceToComponent("clk", gc);

    QSharedPointer<GenerationPort> gp = addPort("clk", 1, DirectionTypes::IN, gc);
    mapPortToInterface(gp,"clk",gc);

    addInterfaceToComponent("data", gc);

    gp = addPort("data_in", 8, DirectionTypes::IN, gc);
    mapPortToInterface(gp,"data",gc);
    gp = addPort("enable", 1, DirectionTypes::IN, gc);
    mapPortToInterface(gp,"data",gc);
    gp = addPort("full", 1, DirectionTypes::OUT, gc);
    mapPortToInterface(gp,"data",gc);

    gp = addPort("data_out", 8, DirectionTypes::OUT, gc);

    library_.addComponent(instanceComponent);

	return gc;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addInstanceToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationInstance> tst_VerilogGenerator::addInstanceToDesign(QString instanceName, QSharedPointer<GenerationComponent> component)
{
    QSharedPointer<ConfigurableVLNVReference> componentVLNV (new ConfigurableVLNVReference(component->component->getVlnv()));
    QSharedPointer<ComponentInstance> instance (new ComponentInstance(instanceName, componentVLNV));

    QSharedPointer<GenerationInstance> gi(new GenerationInstance);
    gi->componentInstance_ = instance;
    gi->component = component;

    design_->instances_.insert(instanceName, gi);

	return gi;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterToSlaveInterconnection()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<GenerationComponent> senderComponent = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    QSharedPointer<GenerationInstance> senderInstance = addInstanceToDesign("sender", senderComponent);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<GenerationComponent> receiverComponent = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    QSharedPointer<GenerationInstance> receiverInstance = addInstanceToDesign("receiver", receiverComponent);

    QSharedPointer<GenerationInterconnection> gic = addConnectionToDesign();
    QSharedPointer<GenerationWire> gw = addWireToDesign(gic,"sender_to_receiver_DATA","7","0");
    createWirePortAssignment(receiverComponent->ports["data_in"], gw, receiverInstance, "7", "0");
    createWirePortAssignment(senderComponent->ports["data_out"], gw, senderInstance, "7", "0");

    gw = addWireToDesign(gic,"sender_to_receiver_ENABLE","0","0");
    createWirePortAssignment(receiverComponent->ports["enable_in"], gw, receiverInstance, "0", "0");
    createWirePortAssignment(senderComponent->ports["enable_out"], gw, senderInstance, "0", "0");

    runGenerator(true);

    verifyOutputContains(
    "    wire [7:0]  sender_to_receiver_DATA;\n"
    "    wire        sender_to_receiver_ENABLE;\n"
    "\n"
    "    // IP-XACT VLNV: Test:TestLibrary:TestReceiver:1.0\n"
    "    TestReceiver receiver(\n"
    "        // Interface: data_bus\n"
    "        .data_in             (sender_to_receiver_DATA[7:0]),\n"
    "        .enable_in           (sender_to_receiver_ENABLE));\n"
    "\n"
    "    // IP-XACT VLNV: Test:TestLibrary:TestSender:1.0\n"
    "    TestSender sender(\n"
    "        // Interface: data_bus\n"
    "        .data_out            (sender_to_receiver_DATA[7:0]),\n"
    "        .enable_out          (sender_to_receiver_ENABLE));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addSenderComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationComponent> tst_VerilogGenerator::addSenderComponentToLibrary(VLNV senderVLNV, General::InterfaceMode mode)
{
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));

    QSharedPointer<GenerationComponent> gc(new GenerationComponent);
    gc->component = senderComponent;
    gc->moduleName_ = senderVLNV.getName();

    addInterfaceToComponent("data_bus", gc);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(mode);    

    QSharedPointer<GenerationPort> gp = addPort("enable_out", 1, DirectionTypes::OUT, gc);
    mapPortToInterface(gp, "data_bus", gc);

    gp = addPort("data_out", 8, DirectionTypes::OUT, gc);
    mapPortToInterface(gp, "data_bus", gc);

    library_.addComponent(senderComponent);

	return gc;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addReceiverComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationComponent> tst_VerilogGenerator::addReceiverComponentToLibrary(VLNV receiverVLNV, General::InterfaceMode mode)
{
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));

    QSharedPointer<GenerationComponent> gc(new GenerationComponent);
    gc->component = receiverComponent;
    gc->moduleName_ = receiverVLNV.getName();

    addInterfaceToComponent("data_bus", gc);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(mode);    

    QSharedPointer<GenerationPort> gp = addPort("enable_in", 1, DirectionTypes::OUT, gc);
    mapPortToInterface(gp, "data_bus", gc);

    gp = addPort("data_in", 8, DirectionTypes::OUT, gc);
    mapPortToInterface(gp, "data_bus", gc);

    library_.addComponent(receiverComponent);

    return gc;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addConnectionToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationInterconnection> tst_VerilogGenerator::addConnectionToDesign()
{
    QSharedPointer<GenerationInterconnection> gic(new GenerationInterconnection);

    design_->interConnections_.append(gic);

    return gic;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addWireToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationWire> tst_VerilogGenerator::addWireToDesign(QSharedPointer<GenerationInterconnection> gic, QString name,
    QString leftBound, QString rightBound)
{
    QSharedPointer<GenerationWire> gw(new GenerationWire);
    gw->name = name;
    gw->bounds.first = leftBound;
    gw->bounds.second = rightBound;

    if (gic)
    {
        gic->wires_.insert(gw->name,gw);
    }
    else
    {
        QSharedPointer<GenerationAdHoc> gah(new GenerationAdHoc);
        gah->wire = gw;
        design_->adHocs_.append(gah);
    }

    return gw;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMasterToMultipleSlavesInterconnections()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterToMultipleSlavesInterconnections()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<GenerationComponent> senderComponent = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    QSharedPointer<GenerationInstance> senderInstance = addInstanceToDesign("sender", senderComponent);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<GenerationComponent> receiverComponent = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    QSharedPointer<GenerationInstance> receiverInstance1 = addInstanceToDesign("receiver1", receiverComponent);
    QSharedPointer<GenerationInstance> receiverInstance2 = addInstanceToDesign("receiver2", receiverComponent);

    QSharedPointer<GenerationInterconnection> gic1 = addConnectionToDesign();
    QSharedPointer<GenerationWire> gw = addWireToDesign(gic1,"sender_to_receiver_DATA","7","0");
    createWirePortAssignment(receiverComponent->ports["data_in"], gw, receiverInstance1, "7", "0");
    createWirePortAssignment(receiverComponent->ports["data_in"], gw, receiverInstance2, "7", "0");
    createWirePortAssignment(senderComponent->ports["data_out"], gw, senderInstance, "7", "0");

    gw = addWireToDesign(gic1,"sender_to_receiver_ENABLE","0","0");
    createWirePortAssignment(receiverComponent->ports["enable_in"], gw, receiverInstance1, "0", "0");
    createWirePortAssignment(receiverComponent->ports["enable_in"], gw, receiverInstance2, "0", "0");
    createWirePortAssignment(senderComponent->ports["enable_out"], gw, senderInstance, "0", "0");

    runGenerator(true);

    verifyOutputContains("wire        sender_to_receiver_ENABLE;");
    verifyOutputContains("wire [7:0]  sender_to_receiver_DATA;");

    verifyOutputContains("TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (sender_to_receiver_DATA[7:0]),\n"
        "        .enable_out          (sender_to_receiver_ENABLE)");

    verifyOutputContains("TestReceiver receiver1(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_to_receiver_DATA[7:0]),\n"
        "        .enable_in           (sender_to_receiver_ENABLE)");

    verifyOutputContains("TestReceiver receiver2(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (sender_to_receiver_DATA[7:0]),\n"
        "        .enable_in           (sender_to_receiver_ENABLE)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testAdhocConnectionBetweenComponentInstances()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testAdhocConnectionBetweenComponentInstances()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<GenerationComponent> senderComponent = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    QSharedPointer<GenerationInstance> senderInstance = addInstanceToDesign("sender", senderComponent);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<GenerationComponent> receiverComponent = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    QSharedPointer<GenerationInstance> receiverInstance1 = addInstanceToDesign("receiver1", receiverComponent);
    QSharedPointer<GenerationInstance> receiverInstance2 = addInstanceToDesign("receiver2", receiverComponent);

    QSharedPointer<GenerationInterconnection> gic1 = addConnectionToDesign();
    QSharedPointer<GenerationWire> gw = addWireToDesign(QSharedPointer<GenerationInterconnection>(),
        "dataAdHoc","7","0");
    createWirePortAssignment(receiverComponent->ports["data_in"], gw, receiverInstance1, "7", "0");
    createWirePortAssignment(senderComponent->ports["data_out"], gw, senderInstance, "7", "0");

    gw = addWireToDesign(QSharedPointer<GenerationInterconnection>(),"enableAdHoc","0","0");
    createWirePortAssignment(receiverComponent->ports["enable_in"], gw, receiverInstance1, "0", "0");
    createWirePortAssignment(receiverComponent->ports["enable_in"], gw, receiverInstance2, "0", "0");
    createWirePortAssignment(senderComponent->ports["enable_out"], gw, senderInstance, "0", "0");

    runGenerator(true);

    verifyOutputContains("wire        enableAdHoc;");
    verifyOutputContains("wire [7:0]  dataAdHoc;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (dataAdHoc[7:0]),\n"
        "        .enable_out          (enableAdHoc)");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_bus\n"
        "        .data_in             (dataAdHoc[7:0]),\n"
        "        .enable_in           (enableAdHoc)");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        // Interface: data_bus\n"
        "        .enable_in           (enableAdHoc)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testAdhocTieOffInComponentInstance()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testAdhocTieOffInComponentInstance()
{
    VLNV tieOffVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestTieOff", "1.0");
	QSharedPointer<Component> tieOffComponent(new Component(tieOffVLNV));

    QSharedPointer<GenerationComponent> gc(new GenerationComponent);
    gc->component = tieOffComponent;

    QString instanceName = "tieOffer";

    QString zeroName = "zeroTieOff";
    QString oneName = "oneTieOff";
    QString naName = "n/aTieOff";
    QString numberedName = "numberedTieOff";
    QString outName = "tieOffOut";
    QString inOutName = "tieOffInOut";
    QString openName = "openTieOff";

    addPort(zeroName, 2, DirectionTypes::IN, gc);
    addPort(oneName, 4, DirectionTypes::IN, gc);
    addPort(naName, 0, DirectionTypes::IN, gc);
    addPort(numberedName, 10, DirectionTypes::IN, gc);
    addPort(outName, 2, DirectionTypes::OUT, gc);
    addPort(inOutName, 10, DirectionTypes::INOUT, gc);
    addPort(openName, 1, DirectionTypes::IN, gc);

    library_.addComponent(tieOffComponent);

    QSharedPointer<GenerationInstance> gi = addInstanceToDesign(instanceName, gc);

    addTieOffAdhocConnectionToInstancePort(gc->ports[zeroName],"0", gi);
    addTieOffAdhocConnectionToInstancePort(gc->ports[oneName],"1", gi);
    addTieOffAdhocConnectionToInstancePort(gc->ports[naName],"abc", gi);
    addTieOffAdhocConnectionToInstancePort(gc->ports[numberedName],"12", gi);
    addTieOffAdhocConnectionToInstancePort(gc->ports[outName],"0", gi);
    addTieOffAdhocConnectionToInstancePort(gc->ports[inOutName],"1", gi);;

    runGenerator(true);

    verifyOutputContains(
        "        // These ports are not in any interface\n"
        "        .n/aTieOff           (abc),\n"
        "        .numberedTieOff      (12),\n"
        "        .oneTieOff           (1),\n"
        "        .zeroTieOff          (0),\n"
        "        .tieOffOut           (0),\n"
        "        .tieOffInOut         (1));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addTieOffAdhocConnectionToInstancePort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addTieOffAdhocConnectionToInstancePort(QSharedPointer<GenerationPort> port,
    QString tieOff,
    QSharedPointer<GenerationInstance> instance)
{
    QSharedPointer<GenerationPortAssignMent> gpa(new GenerationPortAssignMent);
    gpa->port = port;
    gpa->tieOff = tieOff;

    instance->portAssignments_.insert(port->port->name(), gpa);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalAdhocConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalAdhocConnection()
{
    addPort("enable_from_sender", 1, DirectionTypes::OUT, topComponent_);
    addPort("data_from_sender", 8, DirectionTypes::OUT, topComponent_);

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<GenerationComponent> senderComponent = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    QSharedPointer<GenerationInstance> senderInstance = addInstanceToDesign("sender", senderComponent);

    createHierarchicalPortAssignment(senderComponent->ports["data_out"], "data_from_sender", senderInstance, "7", "0");
    createHierarchicalPortAssignment(senderComponent->ports["enable_out"], "enable_from_sender", senderInstance, "0", "0");

    runGenerator(true);

    readOutputFile();

    QStringList outputList = output_.split("\n");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_bus\n"
        "        .data_out            (data_from_sender[7:0]),\n"
        "        .enable_out          (enable_from_sender)");
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
    QString inOutName = "inOutTieOff";
    QString defaultName = "defaultTieOff";

    addPort(zeroName, 2, DirectionTypes::OUT, topComponent_);
    addPort(oneName, 4, DirectionTypes::OUT, topComponent_);
    addPort(naName, 0, DirectionTypes::OUT, topComponent_);
    addPort(numberedName, 10, DirectionTypes::OUT, topComponent_);
    addPort(inOutName, 10, DirectionTypes::INOUT, topComponent_);

    addTieOffConnectionToTopComponentPort("0", zeroName);
    addTieOffConnectionToTopComponentPort("1", oneName);
    addTieOffConnectionToTopComponentPort("abc", naName);
    addTieOffConnectionToTopComponentPort("12", numberedName);
    addTieOffConnectionToTopComponentPort("1", inOutName);

    runGenerator(true);

    verifyOutputContains(
        "    // Tie off values for the ports of the encompassing component\n"
        "    assign inOutTieOff = 1;\n"
        "    assign n/aTieOff = abc;\n"
        "    assign numberedTieOff = 12;\n"
        "    assign oneTieOff = 1;\n"
        "    assign zeroTieOff = 0;"
        );
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addTieOffConnectionToTopComponentPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addTieOffConnectionToTopComponentPort(QString const& tieOffValue,
    QString const& portName)
{
    design_->portTiedValues_.insert(portName,tieOffValue);
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

	QSharedPointer<GenerationComponent> gc = addTestComponentToLibrary(*instanceVLNV);

    QSharedPointer<GenerationInstance> gi = addInstanceToDesign("instance1", gc);
    gi->componentInstance_->setDescription(description);

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
// Function: tst_VerilogGenerator::testTopLevelParametersAreWritten()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testTopLevelParametersAreWritten()
{
    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setName("testParameter");
    parameter->setValue("1");
    topComponent_->formattedParameters.append(parameter);

    runGenerator(false);

    verifyOutputContains(QString(
        "module TestComponent #(\n"
        "    parameter                              testParameter    = 1\n"
        ") ();"
        ));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testInstanceParametersAreWritten()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testInstanceParametersAreWritten()
{
	QSharedPointer<View> activeView(new View("rtl"));
	activeView->setComponentInstantiationRef("instance1");

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");    
    QSharedPointer<GenerationComponent> senderComponent = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    QSharedPointer<GenerationInstance> senderInstance = addInstanceToDesign("sender", senderComponent);

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setValueId("parameterId");
    parameter->setName("testParameter");
    parameter->setValue("2");
	parameter->setValueResolve("user");

    senderInstance->parameters.append(parameter);

    runGenerator(true);

    verifyOutputContains(
        "    TestSender #(\n"
        "        .testParameter       (2))\n"
        "sender();");
}

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

	VerilogGenerator generator(&library_,false,false);

	QString implementation;
	QString postModule;

	generator.selectImplementation("./generatorOutput.v", implementation, postModule);

	implementation = implementation.trimmed();

	QCOMPARE(implementation,QString("foo\nbar"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testImplementationSelectionWithEvilComments()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testImplementationSelectionWithEvilComments()
{
    QFile existingFile("./generatorOutput.v");

    existingFile.open(QIODevice::WriteOnly);
    QTextStream outputStream(&existingFile);

    QString content = QString(
        "module TestComponent #(\n"
        "    parameter                              dataWidth        = 8, // module ending );\n"
        "    parameter                              freq             = 100000 /* ending module header );\n"
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

    VerilogGenerator generator(&library_,false,false);

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

	VerilogGenerator generator(&library_,false,false);

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

	VerilogGenerator generator(&library_,false,false);

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

	VerilogGenerator generator(&library_,false,false);

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

	VerilogGenerator generator(&library_,false,false);

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

	VerilogGenerator generator(&library_,false,false);

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

	VerilogGenerator generator(&library_,false,false);

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

	VerilogGenerator generator(&library_,false,false);

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

	VerilogGenerator generator(&library_,false,false);

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

	VerilogGenerator generator(&library_,false,false);

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

//-----------------------------------------------------------------------------
// File: tst_VerilogGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.07.2014
//
// Description:
// Unit test for class VerilogGenerator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogGenerator/VerilogGenerator/VerilogGenerator.h>

#include <tests/MockObjects/LibraryMock.h>

#include <Plugins/common/HDLParser/MetaDesign.h>
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
    /*void testMasterToMultipleSlavesInterconnections();

    void testAdhocConnectionBetweenComponentInstances();    
    void testAdhocTieOffInComponentInstance();
    void testHierarchicalAdhocConnection();
    void testHierarchicalAdHocTieOffValues();
    
    void testDescriptionAndVLNVIsPrintedAboveInstance();
    void testDescriptionAndVLNVIsPrintedAboveInstance_data();

    void testTopLevelParametersAreWritten();
    void testInstanceParametersAreWritten();*/

	void testGenerationWithImplementation();
	void testGenerationWithImplementationWithTag();
	void testGenerationWithImplementationWithPostModule();

private:

    QSharedPointer<MetaInterconnection> addInterconnectToDesign(QSharedPointer<MetaInterface> first,
        QSharedPointer<MetaInterface> second);

    QSharedPointer<MetaWire> addWireToDesign(QSharedPointer<MetaInterconnection> gic, QString name, QString leftBound, QString rightBound);

    QSharedPointer<MetaPort> addPort(QString const& portName, int portSize, DirectionTypes::Direction direction, 
        QSharedPointer<MetaInstance> component, QSharedPointer<MetaInterface> mInterface =  QSharedPointer<MetaInterface>::QSharedPointer());

    void addParameter(QString const& name, QString const& value);

    void runGenerator(bool useDesign);

    void createPortAssignments(QSharedPointer<MetaPort> instancePort, QSharedPointer<MetaPort> hierPort, QSharedPointer<MetaWire> wire, QString const& leftBound, QString const& rightBound);

    void createWirePortAssignment(QSharedPointer<MetaPort> port, QSharedPointer<MetaWire> wire, QSharedPointer<MetaInstance> instance, QString const& leftBound, QString const& rightBound);

    QSharedPointer<Component> addTestComponentToLibrary(VLNV vlnv);

    QSharedPointer<MetaInstance> addInstanceToDesign(QString instanceName, QSharedPointer<Component> component);

    void createInterface(QSharedPointer<MetaPort> port, QString const& interfaceName, QSharedPointer<MetaInstance> component);
    
    QSharedPointer<MetaInterface> addInterfaceToComponent(QString const& interfaceName, QSharedPointer<MetaInstance> component);

    QSharedPointer<MetaInstance> addSenderComponentToLibrary(QString const& senderVLNV, General::InterfaceMode mode);

    QSharedPointer<MetaInstance> addReceiverComponentToLibrary(QString const& receiverVLNV, General::InterfaceMode mode);

    QSharedPointer<MetaInterconnection> addConnectionToDesign();

    void addTieOffAdhocConnectionToInstancePort(QSharedPointer<MetaPort> port, QString tieOff,
        QSharedPointer<MetaInstance> instance);

    void addTieOffConnectionToTopComponentPort(QString const& tieOffValue, QString const& portName);

    void verifyOutputContains(QString const& expectedOutput);

    void compareOutputTo(QString const& expectedOutput);

    void readOutputFile();   
   
    QSharedPointer<MetaInstance> topComponent_;
    QList<QSharedPointer<MetaDesign> > parsedDesigns_;
    QSharedPointer<MetaDesign> design_;

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
    topComponent_ = QSharedPointer<MetaInstance>(new MetaInstance);
    topComponent_->fileName_ = "generatorOutput.v";
    topComponent_->moduleName_ = "TestComponent";

    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    topComponent_->component_ = QSharedPointer<Component>(new Component(vlnv));

    design_ = QSharedPointer<MetaDesign>(new MetaDesign);
    design_->topInstance_ = topComponent_;

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
QSharedPointer<MetaPort> tst_VerilogGenerator::addPort(QString const& portName, int portSize, 
    DirectionTypes::Direction direction, QSharedPointer<MetaInstance> component,
    QSharedPointer<MetaInterface> mInterface /*= QSharedPointer<MetaInterface>::QSharedPointer()*/)
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, direction));
    port->setPortSize(portSize);
    QSharedPointer<MetaPort> gp(new MetaPort);
    gp->port_ = port;
    gp->vectorBounds_.first = QString::number(portSize-1);
    gp->vectorBounds_.second = "0";
    component->ports_.insert(portName,gp);
    component->component_->getPorts()->append(port);

    if (mInterface)
    {
        mInterface->ports_.insert(portName,gp);
    }

    return gp;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addParameter()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addParameter(QString const& name, QString const& value)
{
    QSharedPointer<Parameter> parameter = QSharedPointer<Parameter>(new Parameter());
    parameter->setName(name);
    parameter->setValue(value);

    topComponent_->parameters_.append(parameter);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::runGenerator()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::runGenerator(bool useDesign)
{
    VerilogGenerator generator(&library_,false,false,"bogusToolVersion","bogusGeneratorVersion");
	QString outputPath = ".";

	if (useDesign)
    {
        foreach (QSharedPointer<MetaDesign> design, parsedDesigns_)
        {
		    generator.prepareDesign(outputPath, design);
        }
	}
	else
	{
		generator.prepareComponent(outputPath, topComponent_);
	}

    generationTime_ =  QDateTime::currentDateTime();

    generator.write();
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testFileHeaderIsPrinted()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testFileHeaderIsPrinted()
{    
    topComponent_->component_->setDescription("Component description\nspanning multiple\nlines.");
    library_.writeModelToFile("C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml", topComponent_->component_);
    	
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
		"// Tool : Kactus2 bogusToolVersion\n"
		"// Plugin : Verilog generator bogusGeneratorVersion\n"
		"// This file was generated based on IP-XACT component Test:TestLibrary:TestComponent:1.0\n"
        "// whose XML file is C:/Test/TestLibrary/TestComponent/1.0/TestComponent.1.0.xml\n"
        "//-----------------------------------------------------------------------------\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalConnections()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalConnections()
{
    QSharedPointer<MetaInterface> topClockIf = addInterfaceToComponent("clk_if", topComponent_);
    QSharedPointer<MetaInterface> topDataIf = addInterfaceToComponent("data_bus", topComponent_);

    QSharedPointer<MetaPort> topClock = addPort("top_clk", 1, DirectionTypes::IN, topComponent_, topClockIf);
    QSharedPointer<MetaPort> topData = addPort("data_to_instance", 8, DirectionTypes::IN, topComponent_, topDataIf);
    QSharedPointer<MetaPort> topEnable = addPort("enable_to_instance", 1, DirectionTypes::IN, topComponent_, topDataIf);
    QSharedPointer<MetaPort> topFull = addPort("full_from_instance", 1, DirectionTypes::OUT, topComponent_, topDataIf);

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");
    QSharedPointer<Component> comp = addTestComponentToLibrary(instanceVlnv);
    QSharedPointer<MetaInstance> instance = addInstanceToDesign("instance1", comp);

    QSharedPointer<MetaInterface> instanceClockIf = instance->interfaces_["clk"];
    QSharedPointer<MetaInterface> instanceDataIf = instance->interfaces_["data"];

    QSharedPointer<MetaInterconnection> mI0 = addInterconnectToDesign(topClockIf, instanceClockIf);
    mI0->hierIfs_.append(topClockIf);
    QSharedPointer<MetaInterconnection> mI1 = addInterconnectToDesign(topDataIf, instanceDataIf);
    mI1->hierIfs_.append(topDataIf);

    QSharedPointer<MetaWire> gw0 = addWireToDesign(mI0,"clk_wire","0","0");
    gw0->hierPorts_.append(topClock);
    QSharedPointer<MetaWire> gw1 = addWireToDesign(mI1,"data_wire","7","0");
    gw1->hierPorts_.append(topData);
    QSharedPointer<MetaWire> gw2 = addWireToDesign(mI1,"enable_wire","0","0");
    gw2->hierPorts_.append(topEnable);
    QSharedPointer<MetaWire> gw3 = addWireToDesign(mI1,"full_wire","0","0");
    gw3->hierPorts_.append(topFull);

    createPortAssignments(instance->ports_["clk"], topComponent_->ports_["top_clk"], gw0, "0", "0");
    createPortAssignments(instance->ports_["data_in"], topComponent_->ports_["data_to_instance"], gw1, "7", "0");
    createPortAssignments(instance->ports_["enable"], topComponent_->ports_["enable_to_instance"], gw2, "0", "0");
    createPortAssignments(instance->ports_["full"], topComponent_->ports_["full_from_instance"], gw3, "0", "0");

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
// Function: tst_VerilogGenerator::addInterconnectToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInterconnection> tst_VerilogGenerator::addInterconnectToDesign(QSharedPointer<MetaInterface> first,
    QSharedPointer<MetaInterface> second)
{
    QSharedPointer<MetaInterconnection> mInterconnect(new MetaInterconnection);
    design_->interconnections_.append(mInterconnect);

    first->interconnection_ = mInterconnect;
    second->interconnection_ = mInterconnect;

    return mInterconnect;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addWireToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<MetaWire> tst_VerilogGenerator::addWireToDesign(QSharedPointer<MetaInterconnection> mInterconnect, QString name,
    QString leftBound, QString rightBound)
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
        design_->adHocWires_.append(gw);
    }

    return gw;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createPortAssignments()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::createPortAssignments(QSharedPointer<MetaPort> firstPort,
    QSharedPointer<MetaPort> secondPort,
    QSharedPointer<MetaWire> wire,
    QString const& leftBound,
    QString const& rightBound)
{
    QSharedPointer<MetaPortAssignMent> instanceAssignment(new MetaPortAssignMent);
    instanceAssignment->bounds_.first = leftBound;
    instanceAssignment->bounds_.second = rightBound;
    instanceAssignment->wire_ = wire;
    firstPort->assignments_.insert(wire->name_, instanceAssignment);

    QSharedPointer<MetaPortAssignMent> hierAssignment(new MetaPortAssignMent);
    hierAssignment->bounds_.first = leftBound;
    hierAssignment->bounds_.second = rightBound;
    hierAssignment->wire_ = wire;
    secondPort->assignments_.insert(wire->name_, hierAssignment);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addInterfaceToComponent()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInterface> tst_VerilogGenerator::addInterfaceToComponent(QString const& interfaceName,
    QSharedPointer<MetaInstance> component)
{
    QSharedPointer<MetaInterface> mInterface(new MetaInterface());
    component->interfaces_.insert(interfaceName,mInterface);

    QSharedPointer<BusInterface> bi = QSharedPointer<BusInterface>(new BusInterface);
    bi->setName(interfaceName);
    mInterface->interface_ = bi;
    component->component_->getBusInterfaces()->append(bi);

    QSharedPointer<AbstractionType> at = QSharedPointer<AbstractionType>(new AbstractionType);
    bi->getAbstractionTypes()->append(at);

    return mInterface;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addTestComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<Component> tst_VerilogGenerator::addTestComponentToLibrary(VLNV vlnv)
{
    QSharedPointer<Component> instanceComponent(new Component(vlnv));

    library_.addComponent(instanceComponent);

	return instanceComponent;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addInstanceToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInstance> tst_VerilogGenerator::addInstanceToDesign(QString instanceName, QSharedPointer<Component> component)
{
    QSharedPointer<ConfigurableVLNVReference> componentVLNV (new ConfigurableVLNVReference(component->getVlnv()));
    QSharedPointer<ComponentInstance> instance (new ComponentInstance(instanceName, componentVLNV));

    QSharedPointer<MetaInstance> gi(new MetaInstance);
    gi->componentInstance_ = instance;
    gi->component_ = component;
    gi->moduleName_ = component->getVlnv().getName();
    gi->fileName_ = gi->moduleName_;

    QSharedPointer<MetaInterface> mClkInterface = addInterfaceToComponent("clk", gi);

    QSharedPointer<MetaPort> mPort = addPort("clk", 1, DirectionTypes::IN, gi, mClkInterface);

    QSharedPointer<MetaInterface> mDataInterface = addInterfaceToComponent("data", gi);

    mPort = addPort("data_in", 8, DirectionTypes::IN, gi, mDataInterface);
    mPort = addPort("enable", 1, DirectionTypes::IN, gi, mDataInterface);
    mPort = addPort("full", 1, DirectionTypes::OUT, gi, mDataInterface);

    mPort = addPort("data_out", 8, DirectionTypes::OUT, gi, mDataInterface);

    design_->instances_.insert(instanceName, gi);

	return gi;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterToSlaveInterconnection()
{
    QSharedPointer<MetaInstance> senderInstance = addSenderComponentToLibrary("sender", General::MASTER);
    QSharedPointer<MetaInstance> receiverInstance = addReceiverComponentToLibrary("receiver", General::SLAVE);

    QSharedPointer<MetaInterface> senderIf = senderInstance->interfaces_["data_bus"];
    QSharedPointer<MetaInterface> recvIf = receiverInstance->interfaces_["data_bus"];

    QSharedPointer<MetaInterconnection> mInterconnect = addInterconnectToDesign(senderIf, recvIf);

    QSharedPointer<MetaWire> dataWire = addWireToDesign(mInterconnect,"sender_to_receiver_DATA","7","0");
    QSharedPointer<MetaWire> enaWire = addWireToDesign(mInterconnect,"sender_to_receiver_ENABLE","0","0");

    createPortAssignments(receiverInstance->ports_["data_in"], senderInstance->ports_["data_out"], dataWire, "7", "0");
    createPortAssignments(receiverInstance->ports_["enable_in"], senderInstance->ports_["enable_out"], enaWire, "0", "0");

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
QSharedPointer<MetaInstance> tst_VerilogGenerator::addSenderComponentToLibrary(QString const& instanceName, General::InterfaceMode mode)
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<Component> comp = addTestComponentToLibrary(senderVLNV);
    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign(instanceName, comp);

    mInstance->interfaces_.clear();
    mInstance->ports_.clear();

    addInterfaceToComponent("data_bus", mInstance);
    comp->getBusInterface("data_bus")->setInterfaceMode(mode);    

    QSharedPointer<MetaInterface> mInterface = addInterfaceToComponent("data", mInstance);
    QSharedPointer<MetaPort> mPort = addPort("enable_out", 1, DirectionTypes::OUT, mInstance, mInterface);
    mPort = addPort("data_out", 8, DirectionTypes::OUT, mInstance, mInterface);

	return mInstance;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addReceiverComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInstance> tst_VerilogGenerator::addReceiverComponentToLibrary(QString const& instanceName, General::InterfaceMode mode)
{
    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<Component> comp = addTestComponentToLibrary(receiverVLNV);
    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign(instanceName, comp);
    mInstance->interfaces_.clear();
    mInstance->ports_.clear();

    addInterfaceToComponent("data_bus", mInstance);
    comp->getBusInterface("data_bus")->setInterfaceMode(mode);    

    QSharedPointer<MetaInterface> mInterface = addInterfaceToComponent("data", mInstance);
    QSharedPointer<MetaPort> gp = addPort("enable_in", 1, DirectionTypes::IN, mInstance, mInterface);
    gp = addPort("data_in", 8, DirectionTypes::IN, mInstance, mInterface);

    return mInstance;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testMasterToMultipleSlavesInterconnections()
//-----------------------------------------------------------------------------
/*void tst_VerilogGenerator::testMasterToMultipleSlavesInterconnections()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<MetaInstance> senderComponent = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    QSharedPointer<MetaInstance> senderInstance = addInstanceToDesign("sender", senderComponent);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<MetaInstance> receiverComponent = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    QSharedPointer<MetaInstance> receiverInstance1 = addInstanceToDesign("receiver1", receiverComponent);
    QSharedPointer<MetaInstance> receiverInstance2 = addInstanceToDesign("receiver2", receiverComponent);

    QSharedPointer<MetaInterconnection> gic1 = addConnectionToDesign();
    QSharedPointer<MetaWire> gw = addWireToDesign(gic1,"sender_to_receiver_DATA","7","0");
    createWirePortAssignment(receiverComponent->ports_["data_in"], gw, receiverInstance1, "7", "0");
    createWirePortAssignment(receiverComponent->ports_["data_in"], gw, receiverInstance2, "7", "0");
    createWirePortAssignment(senderComponent->ports_["data_out"], gw, senderInstance, "7", "0");

    gw = addWireToDesign(gic1,"sender_to_receiver_ENABLE","0","0");
    createWirePortAssignment(receiverComponent->ports_["enable_in"], gw, receiverInstance1, "0", "0");
    createWirePortAssignment(receiverComponent->ports_["enable_in"], gw, receiverInstance2, "0", "0");
    createWirePortAssignment(senderComponent->ports_["enable_out"], gw, senderInstance, "0", "0");

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
    QSharedPointer<MetaInstance> senderComponent = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    QSharedPointer<MetaInstance> senderInstance = addInstanceToDesign("sender", senderComponent);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<MetaInstance> receiverComponent = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    QSharedPointer<MetaInstance> receiverInstance1 = addInstanceToDesign("receiver1", receiverComponent);
    QSharedPointer<MetaInstance> receiverInstance2 = addInstanceToDesign("receiver2", receiverComponent);

    QSharedPointer<MetaInterconnection> gic1 = addConnectionToDesign();
    QSharedPointer<MetaWire> gw = addWireToDesign(QSharedPointer<MetaInterconnection>(),
        "dataAdHoc","7","0");
    createWirePortAssignment(receiverComponent->ports_["data_in"], gw, receiverInstance1, "7", "0");
    createWirePortAssignment(senderComponent->ports_["data_out"], gw, senderInstance, "7", "0");

    gw = addWireToDesign(QSharedPointer<MetaInterconnection>(),"enableAdHoc","0","0");
    createWirePortAssignment(receiverComponent->ports_["enable_in"], gw, receiverInstance1, "0", "0");
    createWirePortAssignment(receiverComponent->ports_["enable_in"], gw, receiverInstance2, "0", "0");
    createWirePortAssignment(senderComponent->ports_["enable_out"], gw, senderInstance, "0", "0");

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

    QSharedPointer<MetaInstance> gc(new MetaInstance);
    gc->component_ = tieOffComponent;

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

    QSharedPointer<MetaInstance> gi = addInstanceToDesign(instanceName, gc);

    addTieOffAdhocConnectionToInstancePort(gc->ports_[zeroName],"0", gi);
    addTieOffAdhocConnectionToInstancePort(gc->ports_[oneName],"1", gi);
    addTieOffAdhocConnectionToInstancePort(gc->ports_[naName],"abc", gi);
    addTieOffAdhocConnectionToInstancePort(gc->ports_[numberedName],"12", gi);
    addTieOffAdhocConnectionToInstancePort(gc->ports_[outName],"0", gi);
    addTieOffAdhocConnectionToInstancePort(gc->ports_[inOutName],"1", gi);;

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
void tst_VerilogGenerator::addTieOffAdhocConnectionToInstancePort(QSharedPointer<MetaPort> port,
    QString tieOff,
    QSharedPointer<MetaInstance> instance)
{
    QSharedPointer<MetaPortAssignMent> gpa(new MetaPortAssignMent);
    gpa->port_ = port;
    gpa->tieOff_ = tieOff;

    instance->portAssignments_.insert(port->port_->name(), gpa);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalAdhocConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalAdhocConnection()
{
    addPort("enable_from_sender", 1, DirectionTypes::OUT, topComponent_);
    addPort("data_from_sender", 8, DirectionTypes::OUT, topComponent_);

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<MetaInstance> senderComponent = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    QSharedPointer<MetaInstance> senderInstance = addInstanceToDesign("sender", senderComponent);

    createHierarchicalPortAssignment(senderComponent->ports_["data_out"], "data_from_sender", senderInstance, "7", "0");
    createHierarchicalPortAssignment(senderComponent->ports_["enable_out"], "enable_from_sender", senderInstance, "0", "0");

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

	QSharedPointer<MetaInstance> gc = addTestComponentToLibrary(*instanceVLNV);

    QSharedPointer<MetaInstance> gi = addInstanceToDesign("instance1", gc);
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
    topComponent_->formattedParameters_.append(parameter);

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
    QSharedPointer<MetaInstance> senderComponent = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    QSharedPointer<MetaInstance> senderInstance = addInstanceToDesign("sender", senderComponent);

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setValueId("parameterId");
    parameter->setName("testParameter");
    parameter->setValue("2");
	parameter->setValueResolve("user");

    senderInstance->parameters_.append(parameter);

    runGenerator(true);

    verifyOutputContains(
        "    TestSender #(\n"
        "        .testParameter       (2))\n"
        "sender();");
}*/

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

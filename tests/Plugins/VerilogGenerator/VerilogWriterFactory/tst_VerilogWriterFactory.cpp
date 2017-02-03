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

#include <tests/MockObjects/LibraryMock.h>

#include <Plugins/common/HDLParser/MetaDesign.h>
#include <Plugins/common/HDLParser/MetaComponent.h>
#include <Plugins/VerilogImport/VerilogSyntax.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>

#include <IPXACTmodels/common/Parameter.h>
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
    void testPhysicalSlicedMasterToSlaveInterconnection();
    void testLogicalSlicedMasterToSlaveInterconnection();
    void testPortSlicedMasterToSlaveInterconnection();
    void testMasterToMultipleSlavesInterconnections();

    void testAdhocConnectionBetweenComponentInstances();    
    void testAdhocTieOffInComponentInstance();
    void testHierarchicalAdhocConnection();
    void testHierarchicalAdHocTieOffValues();
    
    void testDescriptionAndVLNVIsPrintedAboveInstance();
    void testDescriptionAndVLNVIsPrintedAboveInstance_data();
    void testInstanceParametersAreWritten();

    // Flat test cases:
    void testFlatComponent();

	void testGenerationWithImplementation();
	void testGenerationWithImplementationWithTag();
	void testGenerationWithImplementationWithPostModule();

private:

    QSharedPointer<MetaInterconnection> addInterconnectToDesign(QSharedPointer<MetaInterface> first,
        QSharedPointer<MetaInterface> second,
        QString name);

    QSharedPointer<MetaWire> addWireToDesign(QString name, QString leftBound, QString rightBound, QSharedPointer<MetaInterconnection> mInterconnect = QSharedPointer<MetaInterconnection>::QSharedPointer());

    QSharedPointer<MetaPort> addPort(QString const& portName, int portSize, DirectionTypes::Direction direction, 
        QSharedPointer<MetaComponent> component, QSharedPointer<MetaInterface> mInterface = QSharedPointer<MetaInterface>::QSharedPointer());

    void addParameter(QString const& name, QString const& value, QSharedPointer<MetaComponent> mComponent);

    void runGenerator(bool useDesign);

    void createPortAssignment(QSharedPointer<MetaPort> mPort, QSharedPointer<MetaWire> wire, bool up,
        QString const& logicalLeft, QString const& logicalRight, QString const& physicalLeft, QString const& physicalRight);

    void createWirePortAssignment(QSharedPointer<MetaPort> port, QSharedPointer<MetaWire> wire, QSharedPointer<MetaInstance> instance, QString const& leftBound, QString const& rightBound);

    QSharedPointer<Component> addTestComponentToLibrary(VLNV vlnv);

    QSharedPointer<MetaInstance> addInstanceToDesign(QString instanceName, QSharedPointer<Component> component);

    void addInterfacesToInstance(QSharedPointer<MetaInstance> mInstance);

    void createInterface(QSharedPointer<MetaPort> port, QString const& interfaceName, QSharedPointer<MetaInstance> component);
    
    QSharedPointer<MetaInterface> addInterfaceToComponent(QString const& interfaceName, QSharedPointer<MetaInstance> component);

    QSharedPointer<MetaInstance> addSender(QString const& instanceName);

    QSharedPointer<MetaInstance> addReceiver(QString const& instanceName);

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
};

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::tst_VerilogWriterFactory()
//-----------------------------------------------------------------------------
tst_VerilogWriterFactory::tst_VerilogWriterFactory(): output_(), generationTime_(), library_(this)
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

    topComponent_ =  QSharedPointer<MetaInstance>(new MetaInstance(
        &library_, &messages, component, QSharedPointer<View>::QSharedPointer()));

    design_ = QSharedPointer<MetaDesign>(new MetaDesign(&library_,&messages,
        QSharedPointer<Design>::QSharedPointer(),QSharedPointer<DesignConfiguration>::QSharedPointer(),
        topComponent_));

    flatComponent_ = QSharedPointer<MetaComponent>(
        new MetaComponent(&messages, component, QSharedPointer<View>::QSharedPointer()));

    library_.clear();
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
        "    input          [7:0]                dataIn,\n"
        "    input                               rst_n,\n"
        "    output         [7:0]                dataOut\n"
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
    QSharedPointer<MetaInterface> mInterface /*= QSharedPointer<MetaInterface>::QSharedPointer()*/)
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, direction));
    port->setPortSize(portSize);
    QSharedPointer<MetaPort> gp(new MetaPort);
    gp->port_ = port;
    gp->vectorBounds_.first = QString::number(portSize-1);
    gp->vectorBounds_.second = "0";
    component->getPorts()->insert(portName,gp);
    component->getComponent()->getPorts()->append(port);

    if (mInterface)
    {
        mInterface->getPorts()->insert(portName,gp);

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
void tst_VerilogWriterFactory::addParameter(QString const& name, QString const& value,
    QSharedPointer<MetaComponent> mComponent)
{
    QSharedPointer<Parameter> parameter = QSharedPointer<Parameter>(new Parameter());
    parameter->setName(name);
    parameter->setValue(value);

    mComponent->getParameters()->append(parameter);
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
    QSharedPointer<GenerationFile> document;

	if (useDesign)
    {
		document = factory.prepareDesign(design_);
	}
	else
	{
		document = factory.prepareComponent(outputPath, flatComponent_);
	}

    generationTime_ =  QDateTime::currentDateTime();

    document->write();

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
    createPortAssignment(mInstance->getPorts()->value("data_in"), gw1, true, "7", "0", "0", "0");
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
        "    input          [7:0]                data_to_instance,\n"
        "    input                               enable_to_instance,\n"
        "    output                              full_from_instance\n"
        ");\n"
        "\n"
        "    // clock_connection wires:\n"
        "    wire        clk_wire;\n"
        "    // data_connection wires:\n"
        "    wire [7:0]  data_wire;\n"
        "    wire        enable_wire;\n"
        "    wire        full_wire;\n"
        "\n"
        "    // instance1 port wires:\n"
        "    wire        instance1_clk;\n"
        "    wire [7:0]  instance1_data_in;\n"
        "    wire        instance1_enable;\n"
        "    wire        instance1_full;\n"
        "\n"
        "    // Assignments for the ports of the encompassing component:\n"
        "    assign data_wire[7:0] = data_to_instance[7:0];\n"
        "    assign enable_wire = enable_to_instance;\n"
        "    assign full_from_instance = full_wire;\n"
        "    assign clk_wire = top_clk;\n"
        "\n"
        "    // instance1 assignments:\n"
        "    assign instance1_clk = clk_wire;\n"
        "    assign instance1_data_in = data_wire[7:0];\n"
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

    verifyOutputContains("input          [7:0]                data_to_instance");
    verifyOutputContains("output         [7:0]                data_from_instance");

    verifyOutputContains("wire [7:0]  idata_wire;");
    verifyOutputContains("wire [7:0]  odata_wire;");

    verifyOutputContains("wire [3:0]  instance1_idata1;");
    verifyOutputContains("wire [3:0]  instance1_idata2");
    verifyOutputContains("wire [3:0]  instance1_odata1;");
    verifyOutputContains("wire [3:0]  instance1_odata2");

    verifyOutputContains("assign data_from_instance[7:0] = odata_wire[7:0];");
    verifyOutputContains("assign idata_wire[7:0] = data_to_instance[7:0];");

    verifyOutputContains("assign instance1_idata1[3:0] = idata_wire[3:0];");
    verifyOutputContains("assign instance1_idata2[3:0] = idata_wire[7:4];");
    verifyOutputContains("assign odata_wire[3:0] = instance1_odata1[3:0];");
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

    verifyOutputContains("input          [3:0]                data_to_instance1");
    verifyOutputContains("input          [3:0]                data_to_instance2");
    verifyOutputContains("output         [3:0]                data_from_instance1");
    verifyOutputContains("output         [3:0]                data_from_instance2");

    verifyOutputContains("wire [7:0]  idata_wire;");
    verifyOutputContains("wire [7:0]  odata_wire;");

    verifyOutputContains("wire [7:0]  instance1_idata;");
    verifyOutputContains("wire [7:0]  instance1_odata;");

    verifyOutputContains("assign data_from_instance1[3:0] = odata_wire[3:0];");
    verifyOutputContains("assign data_from_instance2[3:0] = odata_wire[7:4];");
    verifyOutputContains("assign idata_wire[3:0] = data_to_instance1[3:0];");
    verifyOutputContains("assign idata_wire[7:4] = data_to_instance2[3:0];");

    verifyOutputContains("assign instance1_idata[7:0] = idata_wire[7:0];");
    verifyOutputContains("assign odata_wire[7:0] = instance1_odata[7:0];");

    verifyOutputContains(
        "    TestInstance instance1(\n"
        "        // Interface: data_if\n"
        "        .idata               (instance1_idata),\n"
        "        .odata               (instance1_odata)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogWriterFactory::addInterconnectToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInterconnection> tst_VerilogWriterFactory::addInterconnectToDesign(QSharedPointer<MetaInterface> first,
    QSharedPointer<MetaInterface> second, QString name)
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
    QString leftBound, QString rightBound, QSharedPointer<MetaInterconnection> mInterconnect /*= QSharedPointer<MetaInterconnection>::QSharedPointer()*/)
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
QSharedPointer<MetaInstance> tst_VerilogWriterFactory::addInstanceToDesign(QString instanceName, QSharedPointer<Component> component)
{
    QSharedPointer<ConfigurableVLNVReference> componentVLNV (new ConfigurableVLNVReference(component->getVlnv()));
    QSharedPointer<ComponentInstance> instance (new ComponentInstance(instanceName, componentVLNV));
    QSharedPointer<ListParameterFinder> topFinder;
    QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs;

    MessagePasser messages;

    QSharedPointer<MetaInstance> mInstance(new MetaInstance(
        &library_, &messages, component, QSharedPointer<View>::QSharedPointer()));
    mInstance->parseInstance(instance, topFinder, cevs);

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

    QSharedPointer<MetaInterconnection> mInterconnect = addInterconnectToDesign(senderIf, recvIf, "data_connection");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",mInterconnect);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",mInterconnect);

    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(receiverInstance->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [7:0]  sender_to_receiver_DATA;");
    verifyOutputContains("wire        sender_to_receiver_ENABLE;");

    verifyOutputContains("wire [7:0]  receiver_data_in;");
    verifyOutputContains("wire        receiver_enable_in;");
    verifyOutputContains("wire [7:0]  sender_data_out;");
    verifyOutputContains("wire        sender_enable_out;");

    verifyOutputContains("assign receiver_data_in[7:0] = sender_to_receiver_DATA[7:0];");
    verifyOutputContains("assign receiver_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign sender_to_receiver_DATA[7:0] = sender_data_out[7:0];");
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
// Function: tst_VerilogWriterFactory::testPhysicalSlicedMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogWriterFactory::testPhysicalSlicedMasterToSlaveInterconnection()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance = addReceiver("receiver");

    QSharedPointer<MetaInterface> senderIf = senderInstance->getInterfaces()->value("data_if");
    QSharedPointer<MetaInterface> recvIf = receiverInstance->getInterfaces()->value("data_if");

    QSharedPointer<MetaInterconnection> mInterconnect = addInterconnectToDesign(senderIf, recvIf, "data_connection");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",mInterconnect);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",mInterconnect);

    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire, true, "7", "4", "7", "4");
    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire, true, "3", "0", "3", "0");
    createPortAssignment(receiverInstance->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "4", "7", "4");
    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "3", "0", "3", "0");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [7:0]  sender_to_receiver_DATA;");
    verifyOutputContains("wire        sender_to_receiver_ENABLE;");

    verifyOutputContains("wire [7:0]  receiver_data_in;");
    verifyOutputContains("wire        receiver_enable_in;");
    verifyOutputContains("wire [7:0]  sender_data_out;");
    verifyOutputContains("wire        sender_enable_out;");

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

    QSharedPointer<MetaInterconnection> mInterconnect = addInterconnectToDesign(senderIf, recvIf, "data_connection");

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

    verifyOutputContains("wire [3:0]  sender_to_receiver_DATA1;");
    verifyOutputContains("wire [3:0]  sender_to_receiver_DATA2;");
    verifyOutputContains("wire        sender_to_receiver_ENABLE;");

    verifyOutputContains("wire [7:0]  receiver_data_in;");
    verifyOutputContains("wire        receiver_enable_in;");
    verifyOutputContains("wire [7:0]  sender_data_out;");
    verifyOutputContains("wire        sender_enable_out;");

    verifyOutputContains("assign receiver_data_in[3:0] = sender_to_receiver_DATA1[3:0];");
    verifyOutputContains("assign receiver_data_in[7:4] = sender_to_receiver_DATA2[3:0];");
    verifyOutputContains("assign receiver_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign sender_to_receiver_DATA1[3:0] = sender_data_out[3:0];");
    verifyOutputContains("assign sender_to_receiver_DATA2[3:0] = sender_data_out[7:4];");
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

    QSharedPointer<MetaInterconnection> mInterconnect = addInterconnectToDesign(senderIf, recvIf, "data_connection");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",mInterconnect);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",mInterconnect);

    createPortAssignment(receiverInstance->getPorts()->value("data_in"), dataWire, true, "7", "4", "3", "0");
    createPortAssignment(receiverInstance->getPorts()->value("data_in2"), dataWire, true, "3", "0", "3", "0");
    createPortAssignment(receiverInstance->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "4", "3", "0");
    createPortAssignment(senderInstance->getPorts()->value("data_out2"), dataWire, true, "3", "0", "3", "0");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [7:0]  sender_to_receiver_DATA;");
    verifyOutputContains("wire        sender_to_receiver_ENABLE;");

    verifyOutputContains("wire [7:0]  receiver_data_in;");
    verifyOutputContains("wire [3:0]  receiver_data_in2;");
    verifyOutputContains("wire        receiver_enable_in;");
    verifyOutputContains("wire [7:0]  sender_data_out;");
    verifyOutputContains("wire [3:0]  sender_data_out2;");
    verifyOutputContains("wire        sender_enable_out;");

    verifyOutputContains("assign receiver_data_in[3:0] = sender_to_receiver_DATA[7:4];");
    verifyOutputContains("assign receiver_data_in2[3:0] = sender_to_receiver_DATA[3:0];");
    verifyOutputContains("assign receiver_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign sender_to_receiver_DATA[7:4] = sender_data_out[3:0];");
    verifyOutputContains("assign sender_to_receiver_DATA[3:0] = sender_data_out2[3:0];");
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

    QSharedPointer<MetaInterconnection> data_connection = addInterconnectToDesign(senderIf, recvIf1, "data_connection");
    QSharedPointer<MetaInterconnection> ena_connection = addInterconnectToDesign(senderIf, recvIf2, "ena_connection");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",data_connection);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",ena_connection);

    createPortAssignment(receiverInstance1->getPorts()->value("data_in"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(receiverInstance1->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");
    createPortAssignment(receiverInstance2->getPorts()->value("data_in"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(receiverInstance2->getPorts()->value("enable_in"), enaWire, true, "0", "0", "0", "0");

    createPortAssignment(senderInstance->getPorts()->value("data_out"), dataWire, true, "7", "0", "7", "0");
    createPortAssignment(senderInstance->getPorts()->value("enable_out"), enaWire, true, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [7:0]  sender_to_receiver_DATA;");
    verifyOutputContains("wire        sender_to_receiver_ENABLE;");

    verifyOutputContains("wire [7:0]  receiver1_data_in;");
    verifyOutputContains("wire        receiver1_enable_in;");
    verifyOutputContains("wire [7:0]  receiver2_data_in;");
    verifyOutputContains("wire        receiver2_enable_in;");
    verifyOutputContains("wire [7:0]  sender_data_out;");
    verifyOutputContains("wire        sender_enable_out;");

    verifyOutputContains("assign receiver1_data_in[7:0] = sender_to_receiver_DATA[7:0];");
    verifyOutputContains("assign receiver1_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign receiver2_data_in[7:0] = sender_to_receiver_DATA[7:0];");
    verifyOutputContains("assign receiver2_enable_in = sender_to_receiver_ENABLE;");
    verifyOutputContains("assign sender_to_receiver_DATA[7:0] = sender_data_out[7:0];");
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

    verifyOutputContains("wire [7:0]  dataAdHoc;");
    verifyOutputContains("wire        enableAdHoc;");

    verifyOutputContains("wire [7:0]  receiver1_data_in;");
    verifyOutputContains("wire        receiver1_enable_in;");
    verifyOutputContains("wire [7:0]  receiver2_data_in;");
    verifyOutputContains("wire        receiver2_enable_in;");
    verifyOutputContains("wire [7:0]  sender_data_out;");
    verifyOutputContains("wire        sender_enable_out;");

    verifyOutputContains("assign receiver1_data_in[7:0] = dataAdHoc[7:0];");
    verifyOutputContains("assign receiver1_enable_in = enableAdHoc;");
    verifyOutputContains("assign receiver2_data_in[7:0] = dataAdHoc[7:0];");
    verifyOutputContains("assign receiver2_enable_in = enableAdHoc;");
    verifyOutputContains("assign dataAdHoc[7:0] = sender_data_out[7:0];");
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
    QString openName = "openTieOff";
    QString slicedName = "slicedTieOff";

    QSharedPointer<MetaPort> defaultPort = addPort(defaultName, 4, DirectionTypes::IN, mInstance);
    defaultPort->defaultValue_ = "15";
    addPort(zeroName, 2, DirectionTypes::IN, mInstance);
    addPort(oneName, 4, DirectionTypes::IN, mInstance);
    addPort(naName, 0, DirectionTypes::IN, mInstance);
    addPort(numberedName, 10, DirectionTypes::IN, mInstance);
    addPort(outName, 2, DirectionTypes::OUT, mInstance);
    addPort(inOutName, 10, DirectionTypes::INOUT, mInstance);
    addPort(openName, 1, DirectionTypes::IN, mInstance);
    QSharedPointer<MetaPort> slicedPort = addPort(slicedName, 5, DirectionTypes::IN, mInstance);

    QSharedPointer<MetaPortAssignment> mpa1(new MetaPortAssignment);
    mpa1->defaultValue_ = "2";
    mpa1->physicalBounds_.first = "4";
    mpa1->physicalBounds_.second = "3";

    QSharedPointer<MetaPortAssignment> mpa0(new MetaPortAssignment);
    mpa0->defaultValue_ = "7";
    mpa0->physicalBounds_.first = "2";
    mpa0->physicalBounds_.second = "0";

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
        "    wire [9:0]  tieOffer_numberedTieOff;\n"
        "    wire [3:0]  tieOffer_oneTieOff;\n"
        "    wire [4:0]  tieOffer_slicedTieOff;\n"
        "    wire [9:0]  tieOffer_tieOffInOut;\n"
        "    wire [1:0]  tieOffer_tieOffOut;\n"
        "    wire [1:0]  tieOffer_zeroTieOff;\n"
        "\n"
        "    // tieOffer assignments:\n"
        "    assign tieOffer_n/aTieOff[-1:0] = abc;\n"
        "    assign tieOffer_numberedTieOff[9:0] = 12;\n"
        "    assign tieOffer_oneTieOff[3:0] = 1;\n"
        "    assign tieOffer_slicedTieOff[4:3] = 2;\n"
        "    assign tieOffer_slicedTieOff[2:0] = 7;\n"
        "    assign tieOffer_tieOffInOut[9:0] = 1;\n"
        "    assign tieOffer_zeroTieOff[1:0] = 0;\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestTieOff:1.0\n"
        "    TestTieOff tieOffer(\n"
        "        // These ports are not in any interface\n"
        "        .defaultValue        (15),\n"
        "        .n/aTieOff           (tieOffer_n/aTieOff),\n"
        "        .numberedTieOff      (tieOffer_numberedTieOff),\n"
        "        .oneTieOff           (tieOffer_oneTieOff),\n"
        "        .openTieOff          (),\n"
        "        .slicedTieOff        (tieOffer_slicedTieOff),\n"
        "        .zeroTieOff          (tieOffer_zeroTieOff),\n"
        "        .tieOffOut           (tieOffer_tieOffOut),\n"
        "        .tieOffInOut         (tieOffer_tieOffInOut));\n"
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

    verifyOutputContains("output         [7:0]                data_from_sender");
    verifyOutputContains("output                              enable_from_sender");

    verifyOutputContains("wire [7:0]  dataAdHoc;");
    verifyOutputContains("wire        enableAdHoc;");

    verifyOutputContains("wire [7:0]  sender_data_out;");
    verifyOutputContains("wire        sender_enable_out;");

    verifyOutputContains("assign data_from_sender[7:0] = dataAdHoc[7:0];");
    verifyOutputContains("assign enable_from_sender = enableAdHoc;");

    verifyOutputContains("assign dataAdHoc[7:0] = sender_data_out[7:0];");
    verifyOutputContains("assign enableAdHoc = sender_enable_out;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_data_out),\n"
        "        .enable_out          (sender_enable_out)");
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

    QSharedPointer<MetaPortAssignment> mpa0(new MetaPortAssignment);
    mpa0->defaultValue_ = "7";
    mpa0->physicalBounds_.first = "2";
    mpa0->physicalBounds_.second = "0";

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
        "    input          [1:0]                tieOffIn,\n"
        "    output         [4:0]                defaultTieOff,\n"
        "    output         [-1:0]               n/aTieOff,\n"
        "    output         [9:0]                numberedTieOff,\n"
        "    output         [3:0]                oneTieOff,\n"
        "    output         [4:0]                slicedTieOff,\n"
        "    output         [1:0]                zeroTieOff,\n"
        "    inout          [9:0]                inOutTieOff\n"
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
        "sender(");
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

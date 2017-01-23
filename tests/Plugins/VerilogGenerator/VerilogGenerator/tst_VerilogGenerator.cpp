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
    void testInstanceSlicedHierarchicalConnections();
    void testTopSlicedHierarchicalConnections();

    void testMasterToSlaveInterconnection();
    void testSlicedMasterToSlaveInterconnection();
    void testMasterToMultipleSlavesInterconnections();

    void testAdhocConnectionBetweenComponentInstances();    
    void testAdhocTieOffInComponentInstance();
    void testHierarchicalAdhocConnection();
    void testHierarchicalAdHocTieOffValues();
    
    void testDescriptionAndVLNVIsPrintedAboveInstance();
    void testDescriptionAndVLNVIsPrintedAboveInstance_data();

    void testTopLevelParametersAreWritten();
    void testInstanceParametersAreWritten();

	void testGenerationWithImplementation();
	void testGenerationWithImplementationWithTag();
	void testGenerationWithImplementationWithPostModule();

private:

    QSharedPointer<MetaInterconnection> addInterconnectToDesign(QSharedPointer<MetaInterface> first,
        QSharedPointer<MetaInterface> second);

    QSharedPointer<MetaWire> addWireToDesign(QString name, QString leftBound, QString rightBound, QSharedPointer<MetaInterconnection> mInterconnect = QSharedPointer<MetaInterconnection>::QSharedPointer());

    QSharedPointer<MetaPort> addPort(QString const& portName, int portSize, DirectionTypes::Direction direction, 
        QSharedPointer<MetaInstance> component, QSharedPointer<MetaInterface> mInterface =  QSharedPointer<MetaInterface>::QSharedPointer());

    void addParameter(QString const& name, QString const& value);

    void runGenerator(bool useDesign);

    void createPortAssignment(QSharedPointer<MetaPort> mPort, QSharedPointer<MetaWire> wire,
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

    void addDefaultPortAssignmentToPort(QSharedPointer<MetaPort> port, QString tieOff);

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

        QSharedPointer<PortMap> pm(new PortMap);
        QSharedPointer<PortMap::PhysicalPort> pp(new PortMap::PhysicalPort);
        pp->name_ = portName;
        pm->setPhysicalPort(pp);

        mInterface->interface_->getAbstractionTypes()->first()->getPortMaps()->append(pm);
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
    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign("instance1", comp);
    addInterfacesToInstance(mInstance);

    QSharedPointer<MetaInterface> instanceClockIf = mInstance->interfaces_["clk_if"];
    QSharedPointer<MetaInterface> instanceDataIf = mInstance->interfaces_["data_if"];

    QSharedPointer<MetaInterconnection> mI0 = addInterconnectToDesign(topClockIf, instanceClockIf);
    mI0->hierIfs_.append(topClockIf);
    QSharedPointer<MetaInterconnection> mI1 = addInterconnectToDesign(topDataIf, instanceDataIf);
    mI1->hierIfs_.append(topDataIf);

    QSharedPointer<MetaWire> gw0 = addWireToDesign("clk_wire","0","0",mI0);
    gw0->hierPorts_.append(topClock);
    QSharedPointer<MetaWire> gw1 = addWireToDesign("data_wire","7","0",mI1);
    gw1->hierPorts_.append(topData);
    QSharedPointer<MetaWire> gw2 = addWireToDesign("enable_wire","0","0",mI1);
    gw2->hierPorts_.append(topEnable);
    QSharedPointer<MetaWire> gw3 = addWireToDesign("full_wire","0","0",mI1);
    gw3->hierPorts_.append(topFull);

    createPortAssignment(mInstance->ports_["clk"], gw0, "0", "0", "0", "0");
    createPortAssignment(mInstance->ports_["data_in"], gw1, "7", "0", "0", "0");
    createPortAssignment(mInstance->ports_["enable"], gw2, "0", "0", "0", "0");
    createPortAssignment(mInstance->ports_["full"], gw3, "0", "0", "0", "0");

    createPortAssignment(topComponent_->ports_["top_clk"], gw0, "0", "0", "0", "0");
    createPortAssignment(topComponent_->ports_["data_to_instance"], gw1, "7", "0", "7", "0");
    createPortAssignment(topComponent_->ports_["enable_to_instance"], gw2, "0", "0", "0", "0");
    createPortAssignment(topComponent_->ports_["full_from_instance"], gw3, "0", "0", "0", "0");

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
        "    wire        clk_wire;\n"
        "    wire [7:0]  data_wire;\n"
        "    wire        enable_wire;\n"
        "    wire        full_wire;\n"
        "\n"
        "    // Assignments for the ports of the encompassing component\n"
        "    assign data_wire[7:0] = data_to_instance[7:0];\n"
        "    assign enable_wire = enable_to_instance;\n"
        "    assign full_from_instance = full_wire;\n"
        "    assign clk_wire = top_clk;\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestInstance:1.0\n"
        "    TestInstance instance1(\n"
        "        // Interface: clk_if\n"
        "        .clk                 (clk_wire),\n"
        "        // Interface: data_if\n"
        "        .data_in             (data_wire[7:0]),\n"
        "        .enable              (enable_wire),\n"
        "        .full                (full_wire),\n"
        "        // These ports are not in any interface\n"
        "        .data_out            ());\n"
        "\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testInstanceSlicedHierarchicalConnections()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testInstanceSlicedHierarchicalConnections()
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

    QSharedPointer<MetaInterconnection> mI1 = addInterconnectToDesign(topDataIf, instanceDataIf);
    mI1->hierIfs_.append(topDataIf);

    QSharedPointer<MetaWire> imWire = addWireToDesign("idata_wire","7","0",mI1);
    imWire->hierPorts_.append(topiData);
    QSharedPointer<MetaWire> omWire = addWireToDesign("odata_wire","7","0",mI1);
    omWire->hierPorts_.append(topoData);

    createPortAssignment(idata1Port, imWire, "3", "0", "0", "0");
    createPortAssignment(idata2Port, imWire, "7", "4", "0", "0");
    createPortAssignment(odata1Port, omWire, "3", "0", "0", "0");
    createPortAssignment(odata2Port, omWire, "7", "4", "0", "0");

    createPortAssignment(topComponent_->ports_["data_to_instance"], imWire, "7", "0", "7", "0");
    createPortAssignment(topComponent_->ports_["data_from_instance"], omWire, "7", "0", "7", "0");

    runGenerator(true);

    verifyOutputContains(QString(
        "module TestComponent(\n"
        "    // Interface: data_if\n"     
        "    input          [7:0]                data_to_instance,\n"
        "    output         [7:0]                data_from_instance\n"
        ");\n"
        "\n"
        "    wire [7:0]  idata_wire;\n"
        "    wire [7:0]  odata_wire;\n"
        "\n"
        "    // Assignments for the ports of the encompassing component\n"
        "    assign data_from_instance[7:0] = odata_wire[7:0];\n"
        "    assign idata_wire[7:0] = data_to_instance[7:0];\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestInstance:1.0\n"
        "    TestInstance instance1(\n"
        "        // Interface: data_if\n"
        "        .idata1              (idata_wire[3:0]),\n"
        "        .idata2              (idata_wire[7:4]),\n"
        "        .odata1              (odata_wire[3:0]),\n"
        "        .odata2              (odata_wire[7:4]));\n"
        "\n"
        "\n"
        "endmodule\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testTopSlicedHierarchicalConnections()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testTopSlicedHierarchicalConnections()
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

    QSharedPointer<MetaInterconnection> mI1 = addInterconnectToDesign(topDataIf, instanceDataIf);
    mI1->hierIfs_.append(topDataIf);

    QSharedPointer<MetaWire> imWire = addWireToDesign("idata_wire","7","0",mI1);
    imWire->hierPorts_.append(topiData1);
    imWire->hierPorts_.append(topiData2);

    QSharedPointer<MetaWire> omWire = addWireToDesign("odata_wire","7","0",mI1);
    omWire->hierPorts_.append(topoData1);
    omWire->hierPorts_.append(topoData2);

    createPortAssignment(idataPort, imWire, "7", "0", "7", "0");
    createPortAssignment(odataPort, omWire, "7", "0", "7", "0");

    createPortAssignment(topiData1, imWire, "3", "0", "3", "0");
    createPortAssignment(topiData2, imWire, "7", "4", "3", "0");
    createPortAssignment(topoData1, omWire, "3", "0", "3", "0");
    createPortAssignment(topoData2, omWire, "7", "4", "3", "0");

    runGenerator(true);

    verifyOutputContains(QString(
        "module TestComponent(\n"
        "    // Interface: data_if\n"     
        "    input          [3:0]                data_to_instance1,\n"
        "    input          [3:0]                data_to_instance2,\n"
        "    output         [3:0]                data_from_instance1,\n"
        "    output         [3:0]                data_from_instance2\n"
        ");\n"
        "\n"
        "    wire [7:0]  idata_wire;\n"
        "    wire [7:0]  odata_wire;\n"
        "\n"
        "    // Assignments for the ports of the encompassing component\n"
        "    assign data_from_instance1[3:0] = odata_wire[3:0];\n"
        "    assign data_from_instance2[3:0] = odata_wire[7:4];\n"
        "    assign idata_wire[3:0] = data_to_instance1[3:0];\n"
        "    assign idata_wire[7:4] = data_to_instance2[3:0];\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestInstance:1.0\n"
        "    TestInstance instance1(\n"
        "        // Interface: data_if\n"
        "        .idata               (idata_wire[7:0]),\n"
        "        .odata               (odata_wire[7:0]));\n"
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
QSharedPointer<MetaWire> tst_VerilogGenerator::addWireToDesign(QString name,
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
        design_->adHocWires_.append(gw);
    }

    return gw;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::createPortAssignment()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::createPortAssignment(QSharedPointer<MetaPort> mPort,
    QSharedPointer<MetaWire> wire,
    QString const& logicalLeft,
    QString const& logicalRight,
    QString const& physicalLeft,
    QString const& physicalRight)
{
    QSharedPointer<MetaPortAssignMent> mpa(new MetaPortAssignMent);
    mpa->logicalBounds_.first = logicalLeft;
    mpa->logicalBounds_.second = logicalRight;
    mpa->physicalBounds_.first = physicalLeft;
    mpa->physicalBounds_.second = physicalRight;
    mpa->wire_ = wire;

    mPort->assignments_.insert(wire->name_, mpa);
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

    QSharedPointer<MetaInstance> mInstance(new MetaInstance);
    mInstance->componentInstance_ = instance;
    mInstance->component_ = component;
    mInstance->moduleName_ = component->getVlnv().getName();
    mInstance->fileName_ = mInstance->moduleName_;

    design_->instances_.insert(instanceName, mInstance);

	return mInstance;
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addInterfacesToInstance()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addInterfacesToInstance(QSharedPointer<MetaInstance> mInstance)
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
// Function: tst_VerilogGenerator::testMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterToSlaveInterconnection()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance = addReceiver("receiver");

    QSharedPointer<MetaInterface> senderIf = senderInstance->interfaces_["data_if"];
    QSharedPointer<MetaInterface> recvIf = receiverInstance->interfaces_["data_if"];

    QSharedPointer<MetaInterconnection> mInterconnect = addInterconnectToDesign(senderIf, recvIf);

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",mInterconnect);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",mInterconnect);

    createPortAssignment(receiverInstance->ports_["data_in"], dataWire, "7", "0", "7", "0");
    createPortAssignment(receiverInstance->ports_["enable_in"], enaWire, "0", "0", "0", "0");

    createPortAssignment(senderInstance->ports_["data_out"], dataWire, "7", "0", "7", "0");
    createPortAssignment(senderInstance->ports_["enable_out"], enaWire, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains(
    "    wire [7:0]  sender_to_receiver_DATA;\n"
    "    wire        sender_to_receiver_ENABLE;\n"
    "\n"
    "    // IP-XACT VLNV: Test:TestLibrary:TestReceiver:1.0\n"
    "    TestReceiver receiver(\n"
    "        // Interface: data_if\n"
    "        .data_in             (sender_to_receiver_DATA[7:0]),\n"
    "        .enable_in           (sender_to_receiver_ENABLE));\n"
    "\n"
    "    // IP-XACT VLNV: Test:TestLibrary:TestSender:1.0\n"
    "    TestSender sender(\n"
    "        // Interface: data_if\n"
    "        .data_out            (sender_to_receiver_DATA[7:0]),\n"
    "        .enable_out          (sender_to_receiver_ENABLE));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testSlicedMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testSlicedMasterToSlaveInterconnection()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance = addReceiver("receiver");

    QSharedPointer<MetaInterface> senderIf = senderInstance->interfaces_["data_if"];
    QSharedPointer<MetaInterface> recvIf = receiverInstance->interfaces_["data_if"];

    QSharedPointer<MetaInterconnection> mInterconnect = addInterconnectToDesign(senderIf, recvIf);

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",mInterconnect);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",mInterconnect);

    createPortAssignment(receiverInstance->ports_["data_in"], dataWire, "7", "4", "7", "4");
    createPortAssignment(receiverInstance->ports_["data_in"], dataWire, "3", "0", "3", "0");
    createPortAssignment(receiverInstance->ports_["enable_in"], enaWire, "0", "0", "0", "0");

    createPortAssignment(senderInstance->ports_["data_out"], dataWire, "7", "4", "7", "4");
    createPortAssignment(senderInstance->ports_["data_out"], dataWire, "3", "0", "3", "0");
    createPortAssignment(senderInstance->ports_["enable_out"], enaWire, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains(
        "    wire [7:0]  sender_to_receiver_DATA;\n"
        "    wire        sender_to_receiver_ENABLE;\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestReceiver:1.0\n"
        "    TestReceiver receiver(\n"
        "        // Interface: data_if\n"
        "        .data_in             ({sender_to_receiver_DATA[3:0], sender_to_receiver_DATA[7:4]}),\n"
        "        .enable_in           (sender_to_receiver_ENABLE));\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestSender:1.0\n"
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            ({sender_to_receiver_DATA[3:0], sender_to_receiver_DATA[7:4]}),\n"
        "        .enable_out          (sender_to_receiver_ENABLE));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addSender()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInstance> tst_VerilogGenerator::addSender(QString const& instanceName)
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
// Function: tst_VerilogGenerator::addReceiver()
//-----------------------------------------------------------------------------
QSharedPointer<MetaInstance> tst_VerilogGenerator::addReceiver(QString const& instanceName)
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
// Function: tst_VerilogGenerator::testMasterToMultipleSlavesInterconnections()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testMasterToMultipleSlavesInterconnections()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance1 = addReceiver("receiver1");
    QSharedPointer<MetaInstance> receiverInstance2 = addReceiver("receiver2");

    QSharedPointer<MetaInterface> senderIf = senderInstance->interfaces_["data_if"];
    QSharedPointer<MetaInterface> recvIf1 = receiverInstance1->interfaces_["data_if"];
    QSharedPointer<MetaInterface> recvIf2 = receiverInstance2->interfaces_["data_if"];

    QSharedPointer<MetaInterconnection> mInterconnect1 = addInterconnectToDesign(senderIf, recvIf1);
    QSharedPointer<MetaInterconnection> mInterconnect2 = addInterconnectToDesign(senderIf, recvIf2);

    QSharedPointer<MetaWire> dataWire = addWireToDesign("sender_to_receiver_DATA","7","0",mInterconnect1);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("sender_to_receiver_ENABLE","0","0",mInterconnect2);

    createPortAssignment(receiverInstance1->ports_["data_in"], dataWire, "7", "0", "0", "0");
    createPortAssignment(receiverInstance1->ports_["enable_in"], enaWire, "0", "0", "0", "0");
    createPortAssignment(receiverInstance2->ports_["data_in"], dataWire, "7", "0", "0", "0");
    createPortAssignment(receiverInstance2->ports_["enable_in"], enaWire, "0", "0", "0", "0");

    createPortAssignment(senderInstance->ports_["data_out"], dataWire, "7", "0", "0", "0");
    createPortAssignment(senderInstance->ports_["enable_out"], enaWire, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire [7:0]  sender_to_receiver_DATA;");
    verifyOutputContains("wire        sender_to_receiver_ENABLE;");

    verifyOutputContains("TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (sender_to_receiver_DATA[7:0]),\n"
        "        .enable_out          (sender_to_receiver_ENABLE)");

    verifyOutputContains("TestReceiver receiver1(\n"
        "        // Interface: data_if\n"
        "        .data_in             (sender_to_receiver_DATA[7:0]),\n"
        "        .enable_in           (sender_to_receiver_ENABLE)");

    verifyOutputContains("TestReceiver receiver2(\n"
        "        // Interface: data_if\n"
        "        .data_in             (sender_to_receiver_DATA[7:0]),\n"
        "        .enable_in           (sender_to_receiver_ENABLE)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testAdhocConnectionBetweenComponentInstances()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testAdhocConnectionBetweenComponentInstances()
{
    QSharedPointer<MetaInstance> senderInstance = addSender("sender");
    QSharedPointer<MetaInstance> receiverInstance1 = addReceiver("receiver1");
    QSharedPointer<MetaInstance> receiverInstance2 = addReceiver("receiver2");

    QSharedPointer<MetaWire> dataWire = addWireToDesign("dataAdHoc","7","0");
    QSharedPointer<MetaWire> enaWire = addWireToDesign("enableAdHoc","0","0");

    createPortAssignment(receiverInstance1->ports_["data_in"], dataWire, "7", "0", "0", "0");
    createPortAssignment(receiverInstance1->ports_["enable_in"], enaWire, "0", "0", "0", "0");
    createPortAssignment(receiverInstance2->ports_["data_in"], dataWire, "7", "0", "0", "0");
    createPortAssignment(receiverInstance2->ports_["enable_in"], enaWire, "0", "0", "0", "0");

    createPortAssignment(senderInstance->ports_["data_out"], dataWire, "7", "0", "0", "0");
    createPortAssignment(senderInstance->ports_["enable_out"], enaWire, "0", "0", "0", "0");

    runGenerator(true);

    verifyOutputContains("wire        enableAdHoc;");
    verifyOutputContains("wire [7:0]  dataAdHoc;");

    verifyOutputContains(
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (dataAdHoc[7:0]),\n"
        "        .enable_out          (enableAdHoc)");

    verifyOutputContains(
        "    TestReceiver receiver1(\n"
        "        // Interface: data_if\n"
        "        .data_in             (dataAdHoc[7:0]),\n"
        "        .enable_in           (enableAdHoc)");

    verifyOutputContains(
        "    TestReceiver receiver2(\n"
        "        // Interface: data_if\n"
        "        .data_in             (dataAdHoc[7:0]),\n"
        "        .enable_in           (enableAdHoc)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testAdhocTieOffInComponentInstance()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testAdhocTieOffInComponentInstance()
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

    QSharedPointer<MetaPort> defaultPort = addPort(defaultName, 4, DirectionTypes::IN, mInstance);
    defaultPort->defaultValue_ = "15";
    addPort(zeroName, 2, DirectionTypes::IN, mInstance);
    addPort(oneName, 4, DirectionTypes::IN, mInstance);
    addPort(naName, 0, DirectionTypes::IN, mInstance);
    addPort(numberedName, 10, DirectionTypes::IN, mInstance);
    addPort(outName, 2, DirectionTypes::OUT, mInstance);
    addPort(inOutName, 10, DirectionTypes::INOUT, mInstance);
    addPort(openName, 1, DirectionTypes::IN, mInstance);

    library_.addComponent(tieOffComponent);

    addDefaultPortAssignmentToPort(mInstance->ports_[zeroName],"0");
    addDefaultPortAssignmentToPort(mInstance->ports_[oneName],"1");
    addDefaultPortAssignmentToPort(mInstance->ports_[naName],"abc");
    addDefaultPortAssignmentToPort(mInstance->ports_[numberedName],"12");
    addDefaultPortAssignmentToPort(mInstance->ports_[outName],"0");
    addDefaultPortAssignmentToPort(mInstance->ports_[inOutName],"1");;

    runGenerator(true);

    verifyOutputContains(
        "        // These ports are not in any interface\n"
        "        .defaultValue        (15),\n"
        "        .n/aTieOff           (abc),\n"
        "        .numberedTieOff      (12),\n"
        "        .oneTieOff           (1),\n"
        "        .openTieOff          (),\n"
        "        .zeroTieOff          (0),\n"
        "        .tieOffOut           (),\n"
        "        .tieOffInOut         (1));");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::addDefaultPortAssignmentToPort()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::addDefaultPortAssignmentToPort(QSharedPointer<MetaPort> port,
    QString tieOff)
{
    QSharedPointer<MetaPortAssignMent> mpa(new MetaPortAssignMent);
    mpa->defaultValue_ = tieOff;
    port->assignments_.insert("eka", mpa);
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalAdhocConnection()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalAdhocConnection()
{
    QSharedPointer<MetaPort> topEna = addPort("enable_from_sender", 1, DirectionTypes::OUT, topComponent_);
    QSharedPointer<MetaPort> topData = addPort("data_from_sender", 8, DirectionTypes::OUT, topComponent_);

    QSharedPointer<MetaInstance> senderInstance = addSender("sender");;

    QSharedPointer<MetaWire> dataWire = addWireToDesign("dataAdHoc","7","0");
    dataWire->hierPorts_.append(topData);
    QSharedPointer<MetaWire> enaWire = addWireToDesign("enableAdHoc","0","0");
    enaWire->hierPorts_.append(topEna);

    createPortAssignment(topData, dataWire, "7", "0", "0", "0");
    createPortAssignment(topEna, enaWire, "0", "0", "0", "0");
    createPortAssignment(senderInstance->ports_["data_out"], dataWire, "7", "0", "0", "0");
    createPortAssignment(senderInstance->ports_["enable_out"], enaWire, "0", "0", "0", "0");

    runGenerator(true);

    readOutputFile();

    QStringList outputList = output_.split("\n");

    verifyOutputContains(
        "    wire [7:0]  dataAdHoc;\n"
        "    wire        enableAdHoc;\n"
        "\n"
        "    // Assignments for the ports of the encompassing component\n"
        "    assign data_from_sender = dataAdHoc[7:0];\n"
        "    assign enable_from_sender = enableAdHoc;\n"
        "\n"
        "    // IP-XACT VLNV: Test:TestLibrary:TestSender:1.0\n"
        "    TestSender sender(\n"
        "        // Interface: data_if\n"
        "        .data_out            (dataAdHoc[7:0]),\n"
        "        .enable_out          (enableAdHoc)");
}

//-----------------------------------------------------------------------------
// Function: tst_VerilogGenerator::testHierarchicalAdHocTieOffValues()
//-----------------------------------------------------------------------------
void tst_VerilogGenerator::testHierarchicalAdHocTieOffValues()
{
    QString defaultName = "defaultTieOff";
    QString zeroName = "zeroTieOff";
    QString oneName = "oneTieOff";
    QString naName = "n/aTieOff";
    QString numberedName = "numberedTieOff";
    QString inName = "tieOffIn";
    QString inOutName = "inOutTieOff";

    QSharedPointer<MetaPort> defaultPort = addPort(defaultName, 5, DirectionTypes::OUT, topComponent_);
    defaultPort->defaultValue_ = "7";
    addPort(zeroName, 2, DirectionTypes::OUT, topComponent_);
    addPort(oneName, 4, DirectionTypes::OUT, topComponent_);
    addPort(naName, 0, DirectionTypes::OUT, topComponent_);
    addPort(numberedName, 10, DirectionTypes::OUT, topComponent_);
    addPort(inName, 2, DirectionTypes::IN, topComponent_);
    addPort(inOutName, 10, DirectionTypes::INOUT, topComponent_);

    addDefaultPortAssignmentToPort(topComponent_->ports_[zeroName],"0");
    addDefaultPortAssignmentToPort(topComponent_->ports_[oneName],"1");
    addDefaultPortAssignmentToPort(topComponent_->ports_[naName],"abc");
    addDefaultPortAssignmentToPort(topComponent_->ports_[numberedName],"12");
    addDefaultPortAssignmentToPort(topComponent_->ports_[inName],"0");
    addDefaultPortAssignmentToPort(topComponent_->ports_[inOutName],"1");

    runGenerator(true);

    verifyOutputContains(
        "    // Assignments for the ports of the encompassing component\n"
        "    assign defaultTieOff = 7;\n"
        "    assign inOutTieOff = 1;\n"
        "    assign n/aTieOff = abc;\n"
        "    assign numberedTieOff = 12;\n"
        "    assign oneTieOff = 1;\n"
        "    assign zeroTieOff = 0;\n"
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

	QSharedPointer<Component> comp = addTestComponentToLibrary(*instanceVLNV);

    QSharedPointer<MetaInstance> mInstance = addInstanceToDesign("instance1", comp);
    mInstance->componentInstance_->setDescription(description);

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
    topComponent_->parameters_.append(parameter);

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
  
    QSharedPointer<MetaInstance> sender = addSender("sender");

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setValueId("parameterId");
    parameter->setName("testParameter");
    parameter->setValue("2");
	parameter->setValueResolve("user");

    sender->parameters_.append(parameter);

    runGenerator(true);

    verifyOutputContains(
        "    TestSender #(\n"
        "        .testParameter       (2))\n"
        "sender(");
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

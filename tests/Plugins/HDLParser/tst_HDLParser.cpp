//-----------------------------------------------------------------------------
// File: tst_HDLParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 31.8.2016
//
// Description:
// Unit test suite for HDL parsing.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

#include <IPXACTmodels/common/ModuleParameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <tests/MockObjects/LibraryMock.h>

#include <Plugins/common/HDLParser/MetaComponent.h>
#include <Plugins/common/HDLParser/MetaDesign.h>

#include <Plugins/PluginSystem/GeneratorPlugin/MessagePasser.h>

class tst_HDLParser : public QObject
{
    Q_OBJECT

public:
    tst_HDLParser();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Hierarchy test cases:
    void testTopLevelComponent();
    void testTopLevelComponentExpressions();

    void testHierarchicalConnections();
    void testHierarchicalConnectionsWithExpressions();

    //void testPortMapping();

    void testMasterToSlaveInterconnection();
    void testEmptyBounds();
    void testMasterToSlaveInterconnectionWithExpressions();
    void testMasterToMultipleSlavesInterconnections();
    void testInterconnectionToVaryingSizeLogicalMaps();
    void testSlicedInterconnection();
    //void testAbsDefDefault();
    void testPortDefaultValueInComponentInstance();
    void testHierarchicalPortDefaultValue();

    void testAdhocConnectionBetweenComponentInstances();    
    void testAdhocConnectionToVaryingSizePorts();    
    void testAdhocConnectionWithPartSelect();
    void testAdhocTieOffInComponentInstance();
    void testHierarchicalAdhocConnection();
    void testAdHocConnectionBetweenMultipleComponentInstances();

    void testInstanceParametersAreCulled();
    void testDesignParametersAreUtilized();
	void testTopComponentParametersAreUtilized();

    // Flat test cases:
    void testFlatComponent();
    void testFlatComponentExpressions();

    void testParameterSorting();
    void testParameterSorting2();
    void testParameterSorting3();

private:

    QSharedPointer<Port> addPort(QString const& portName, int portSize, DirectionTypes::Direction direction, 
        QSharedPointer<Component> component);

    void addParameter(QString const& name, QString const& value, QString const& valueID, QSharedPointer<Component> component);
    void addParameter(QString const& name, QString const& value, QString const& valueID, QSharedPointer<Design> design);

    void addModuleParameter(QString const& name, QString const& value, QString const& valueID, QSharedPointer<Component> component);

    void createHierarchicalConnection(QString const& topInterfaceRef, QString const& instanceInterfaceRef);

    QSharedPointer<View> addTestComponentToLibrary(VLNV vlnv);

    QSharedPointer<ComponentInstance> addInstanceToDesign(QString instanceName, VLNV instanceVlnv, QSharedPointer<View> activeView);

    QSharedPointer<PortMap> mapPortToInterface(QString const& portName, QString const& logicalName, 
        QString const& interfaceName, QSharedPointer<Component> component);

    QSharedPointer<PortMap> mapPortToInterface(QString const& portName, int left, int right, 
        QString const& logicalName, QString const& interfaceName, QSharedPointer<Component> component);
    
    QSharedPointer<BusInterface> addInterfaceToComponent(QString const& interfaceName, QSharedPointer<Component> component, QSharedPointer<ConfigurableVLNVReference> absRef);

    QSharedPointer<View> addSenderComponentToLibrary(VLNV senderVLNV, General::InterfaceMode mode, bool createInterfaces = true);

    QSharedPointer<View> addReceiverComponentToLibrary(VLNV receiverVLNV, General::InterfaceMode mode, bool createInterfaces = true);    

    void setReceiverComponentDataWidth(VLNV receiverVLNV, int dataWidth);

    void addConnectionToDesign(QString fromInstance, QString fromInterface, QString toInstance, QString toInterface);

    QSharedPointer<AdHocConnection> addAdhocConnection(QString const& connectionName, QString const& sourceInstance, QString const& sourcePort,
        QString const& targetInstance, QString const& targetPort);

    void addTieOffAdhocConnectionToInstancePort(QString const& tieOffValue, QString const& instanceName,
        QString const& portName, QString const& connectionName);

    void addTieOffConnectionToTopComponentPort(QString const& tieOffValue, QString const& portName,
        QString const& connectionName);

    void addHierAdhocConnection(QString const& topPort, QString const& targetInstance, QString const& targetPort);
   
    //! The top level component used in the tests.
    QSharedPointer<Component> topComponent_;
    
    //! The design used in the tests.
    QSharedPointer<Design> design_;

	//! The design configuration used in the tests.
    QSharedPointer<DesignConfiguration> designConf_;

    //! Channel for generation messages.
    MessagePasser messageChannel_;

    //! The above three bundled together.
    GenerationTuple input_;

    //! The abstraction definitions used in the tests.
    QSharedPointer<ConfigurableVLNVReference> clkAbstractionVLNV_;
    QSharedPointer<AbstractionDefinition> clkAbstractionDefinition_;
    QSharedPointer<ConfigurableVLNVReference> dataAbstractionVLNV_;
    QSharedPointer<AbstractionDefinition> dataAbstractionDefinition_;

    //! The view of the top component chosen as the active view.
	QSharedPointer<View> topView_;

    //! The test mock for library interface.
    LibraryMock library_;
};

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::tst_HDLParser()
//-----------------------------------------------------------------------------
tst_HDLParser::tst_HDLParser(): 
topComponent_(), 
    design_(), 
    designConf_(),
    messageChannel_(),
    input_(), 
    clkAbstractionVLNV_(),
    clkAbstractionDefinition_(),
    dataAbstractionVLNV_(),
    dataAbstractionDefinition_(),
    topView_(), 
    library_(this)
{

}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::initTestCase()
//-----------------------------------------------------------------------------
void tst_HDLParser::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_HDLParser::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::init()
//-----------------------------------------------------------------------------
void tst_HDLParser::init()
{
    VLNV vlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestComponent", "1.0");
    topComponent_ = QSharedPointer<Component>(new Component(vlnv));

    topView_ = QSharedPointer<View>(new View("topView"));
    topComponent_->getViews()->append(topView_);

    QSharedPointer<ConfigurableVLNVReference> designVlnv(
        new ConfigurableVLNVReference(VLNV::DESIGN, "Test", "TestLibrary", "TestDesign", "1.0"));
    design_ = QSharedPointer<Design>(new Design(*designVlnv));

    QSharedPointer<DesignInstantiation> di(new DesignInstantiation);
    di->setName("TestDesignInstantiation");
    topView_->setDesignInstantiationRef(di->name());
    di->setDesignReference(designVlnv);
    topComponent_->getDesignInstantiations()->append(di);

	VLNV designConfVlnv(VLNV::DESIGNCONFIGURATION, "Test", "TestLibrary", "TestDesignConfiguration", "1.0");
	designConf_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(designConfVlnv));
    designConf_->setDesignRef(*designVlnv);

    input_.component = topComponent_;
    input_.design = design_;
    input_.designConfiguration = designConf_;
    input_.messages = &messageChannel_;

    clkAbstractionVLNV_ = QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "Test", "TestLibrary", "clkAbsDef", "1.0"));

    clkAbstractionDefinition_ =  QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
    clkAbstractionDefinition_->setVlnv(*clkAbstractionVLNV_.data());

    dataAbstractionVLNV_ = QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "Test", "TestLibrary", "dataAbsDef", "1.0"));

    dataAbstractionDefinition_ =  QSharedPointer<AbstractionDefinition>(new AbstractionDefinition());
    dataAbstractionDefinition_->setVlnv(*dataAbstractionVLNV_.data());

    QSharedPointer<PortAbstraction> logicalPort (new PortAbstraction());
    logicalPort->setName("CLK");
    clkAbstractionDefinition_->getLogicalPorts()->append(logicalPort);

    logicalPort = QSharedPointer<PortAbstraction>(new PortAbstraction());
    logicalPort->setName("DATA");
    dataAbstractionDefinition_->getLogicalPorts()->append(logicalPort);
    logicalPort = QSharedPointer<PortAbstraction>(new PortAbstraction());
    logicalPort->setName("ENABLE");
    dataAbstractionDefinition_->getLogicalPorts()->append(logicalPort);
    logicalPort = QSharedPointer<PortAbstraction>(new PortAbstraction());
    logicalPort->setName("FULL");
    dataAbstractionDefinition_->getLogicalPorts()->append(logicalPort);

    library_.clear();
    library_.addComponent(dataAbstractionDefinition_);
    library_.addComponent(clkAbstractionDefinition_);
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::cleanup()
//-----------------------------------------------------------------------------
void tst_HDLParser::cleanup()
{
    topComponent_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testTopLevelComponent()
//-----------------------------------------------------------------------------
void tst_HDLParser::testTopLevelComponent()
{
    addPort("clk", 1, DirectionTypes::IN, topComponent_);
    addPort("rst_n", 1, DirectionTypes::IN, topComponent_);
    addPort("dataOut", 8, DirectionTypes::OUT, topComponent_);
    addPort("dataIn", 8, DirectionTypes::IN, topComponent_);
    addModuleParameter("dataWidth", "8", "firstParameter", topComponent_);
    addModuleParameter("freq", "100000", "secondParameter", topComponent_);

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QVERIFY(design->getTopInstance());
    QSharedPointer<MetaInstance> mInstance = design->getTopInstance();

    QCOMPARE(mInstance->getModuleParameters()->size(), 2);
    QCOMPARE(mInstance->getModuleParameters()->value(0)->name(), QString("dataWidth"));
    QCOMPARE(mInstance->getModuleParameters()->value(0)->getValue(), QString("8"));
    QCOMPARE(mInstance->getModuleParameters()->value(1)->name(), QString("freq"));
    QCOMPARE(mInstance->getModuleParameters()->value(1)->getValue(), QString("100000"));

    QCOMPARE(mInstance->getPorts()->size(), 4);
    QCOMPARE(mInstance->getPorts()->value("clk")->port_->getDirection(), DirectionTypes::IN);

    QCOMPARE(mInstance->getPorts()->value("dataIn")->port_->getDirection(), DirectionTypes::IN);
    QCOMPARE(mInstance->getPorts()->value("dataIn")->vectorBounds_.first, QString("7"));
    QCOMPARE(mInstance->getPorts()->value("dataIn")->vectorBounds_.second, QString("0"));

    QCOMPARE(mInstance->getPorts()->value("rst_n")->port_->getDirection(), DirectionTypes::IN);

    QCOMPARE(mInstance->getPorts()->value("dataOut")->port_->getDirection(), DirectionTypes::OUT);
    QCOMPARE(mInstance->getPorts()->value("dataOut")->vectorBounds_.first, QString("7"));
    QCOMPARE(mInstance->getPorts()->value("dataOut")->vectorBounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testTopLevelComponentExpressions()
//-----------------------------------------------------------------------------
void tst_HDLParser::testTopLevelComponentExpressions()
{
    addParameter("module", "10", "firstParameter", topComponent_);
    addParameter("freq", "firstParameter*3.14", "secondParameter", topComponent_);

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port("clk", DirectionTypes::IN));
    port->setLeftBound("secondParameter*2");
    port->setRightBound("2+5");
    topComponent_->getPorts()->append(port);

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QVERIFY(design->getTopInstance());
    QSharedPointer<MetaInstance> mInstance = design->getTopInstance();

    QCOMPARE(mInstance->getParameters()->size(), 2);
    QCOMPARE(mInstance->getParameters()->value(0)->name(), QString("module"));
    QCOMPARE(mInstance->getParameters()->value(0)->getValue(), QString("10"));
    QCOMPARE(mInstance->getParameters()->value(1)->name(), QString("freq"));
    QCOMPARE(mInstance->getParameters()->value(1)->getValue(), QString("31.4"));

    QCOMPARE(mInstance->getPorts()->size(), 1);
    QCOMPARE(mInstance->getPorts()->value("clk")->port_->getDirection(), DirectionTypes::IN);
    QCOMPARE(mInstance->getPorts()->value("clk")->vectorBounds_.first, QString("62.8"));
    QCOMPARE(mInstance->getPorts()->value("clk")->vectorBounds_.second, QString("7"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> tst_HDLParser::addPort(QString const& portName, int portSize, 
    DirectionTypes::Direction direction, QSharedPointer<Component> component)
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, direction));
    port->setPortSize(portSize);
    component->getPorts()->append(port);

    return port;
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addParameter()
//-----------------------------------------------------------------------------
void tst_HDLParser::addParameter(QString const& name, QString const& value, QString const& valueID, QSharedPointer<Component> component)
{
    QSharedPointer<Parameter> parameter = QSharedPointer<Parameter>(new Parameter());
    parameter->setName(name);
    parameter->setValue(value);
    parameter->setValueId(valueID);
    parameter->setValueResolve("user");

    component->getParameters()->append(parameter);
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addParameter()
//-----------------------------------------------------------------------------
void tst_HDLParser::addParameter(QString const& name, QString const& value, QString const& valueID, QSharedPointer<Design> design)
{
    QSharedPointer<Parameter> parameter = QSharedPointer<Parameter>(new Parameter());
    parameter->setName(name);
    parameter->setValue(value);
    parameter->setValueId(valueID);
    parameter->setValueResolve("user");

    design->getParameters()->append(parameter);
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addModuleParameter()
//-----------------------------------------------------------------------------
void tst_HDLParser::addModuleParameter(QString const& name, QString const& value, QString const& valueID, QSharedPointer<Component> component)
{
    QSharedPointer<ModuleParameter> parameter = QSharedPointer<ModuleParameter>(new ModuleParameter());
    parameter->setName(name);
    parameter->setValue(value);
    parameter->setValueId(valueID);
    parameter->setValueResolve("user");

    if (component->getComponentInstantiations()->isEmpty())
    {
        QSharedPointer<ComponentInstantiation> newInstantiation (new ComponentInstantiation("testInstantiation"));
        newInstantiation->getModuleParameters()->append(parameter);
        component->getComponentInstantiations()->append(newInstantiation);

        topView_->setComponentInstantiationRef(newInstantiation->name());
    }
    else
    {
        component->getComponentInstantiations()->first()->getModuleParameters()->append(parameter);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testHierarchicalConnections()
//-----------------------------------------------------------------------------
void tst_HDLParser::testHierarchicalConnections()
{
    addPort("top_clk", 1, DirectionTypes::IN, topComponent_);
    addPort("data_to_instance", 8, DirectionTypes::IN, topComponent_);
    addPort("enable_to_instance", 1, DirectionTypes::IN, topComponent_);
    addPort("full_from_instance", 1, DirectionTypes::OUT, topComponent_);

    addInterfaceToComponent("clk_if", topComponent_, clkAbstractionVLNV_);
    addInterfaceToComponent("data_bus", topComponent_, dataAbstractionVLNV_);
    
    mapPortToInterface("top_clk", "CLK", "clk_if", topComponent_);
    mapPortToInterface("data_to_instance", "DATA", "data_bus", topComponent_);
    mapPortToInterface("enable_to_instance", "ENABLE", "data_bus", topComponent_);
    mapPortToInterface("full_from_instance", "FULL", "data_bus", topComponent_);

    createHierarchicalConnection("clk_if", "clk");
    createHierarchicalConnection("data_bus", "data");

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");
    QSharedPointer<View> activeView = addTestComponentToLibrary(instanceVlnv);
    addInstanceToDesign("instance1", instanceVlnv, activeView);

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 1);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("instance1");
    QCOMPARE(mInstance->getInterfaces()->size(), 2);

    QSharedPointer<MetaInterface> mInterface = mInstance->getInterfaces()->value("clk");
    QCOMPARE(mInterface->ports_.size(), 1);

    QSharedPointer<MetaPort> mPort = mInterface->ports_.value("clk");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("CLK");
    QCOMPARE(mpa->wire_->hierPorts_.first()->port_->name(), QString("top_clk"));

    mInterface = mInstance->getInterfaces()->value("data");
    QCOMPARE(mInterface->ports_.size(), 3);

    mPort = mInterface->ports_.value("data_in");
    mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->wire_->hierPorts_.first()->port_->name(), QString("data_to_instance"));

    mPort = mInterface->ports_.value("enable");
    mpa = mPort->upAssignments_.value("ENABLE");
    QCOMPARE(mpa->wire_->hierPorts_.first()->port_->name(), QString("enable_to_instance"));

    mPort = mInterface->ports_.value("full");
    mpa = mPort->upAssignments_.value("FULL");
    QCOMPARE(mpa->wire_->hierPorts_.first()->port_->name(), QString("full_from_instance"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::createHierarchicalConnection()
//-----------------------------------------------------------------------------
void tst_HDLParser::createHierarchicalConnection(QString const& topInterfaceRef, 
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
// Function: tst_HDLParser::testHierarchicalConnectionsWithExpressions()
//-----------------------------------------------------------------------------
void tst_HDLParser::testHierarchicalConnectionsWithExpressions()
{
    QSharedPointer<Port> clkPort (new Port("top_clk", DirectionTypes::IN));
    clkPort->setLeftBound("2+2");
    clkPort->setRightBound("0");
    topComponent_->getPorts()->append(clkPort);

    addInterfaceToComponent("clk_if", topComponent_, clkAbstractionVLNV_);;

    mapPortToInterface("top_clk", "CLK", "clk_if", topComponent_);

    createHierarchicalConnection("clk_if", "instanceInterface");

    VLNV instanceVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "TestInstance", "1.0");

    QSharedPointer<Component> instanceComponent(new Component(instanceVlnv));

    QSharedPointer<Parameter> componentParameter (new Parameter());
    componentParameter->setName("componentParameter");
    componentParameter->setValue("1");
	componentParameter->setValueId("instant_ID");
	componentParameter->setValueResolve("user");
	instanceComponent->getParameters()->append(componentParameter);

    QSharedPointer<Port> instanceClkPort (new Port("instance_clk", DirectionTypes::IN));
    instanceClkPort->setLeftBound(componentParameter->getValueId() + "*2");
    instanceClkPort->setRightBound("4-2*2");
    instanceComponent->getPorts()->append(instanceClkPort);

    addInterfaceToComponent("instanceInterface", instanceComponent, clkAbstractionVLNV_);
    mapPortToInterface("instance_clk", "CLK", "instanceInterface", instanceComponent);

	library_.addComponent(instanceComponent);

	QSharedPointer<View> activeView(new View("rtl"));
	activeView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	instanceComponent->getComponentInstantiations()->append(instantiation);
	instanceComponent->getViews()->append(activeView);

    addInstanceToDesign("instance1", instanceVlnv, activeView);

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 1);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("instance1");
    QCOMPARE(mInstance->getInterfaces()->size(), 1);
    QCOMPARE(mInstance->getParameters()->size(), 1);

    QCOMPARE(mInstance->getParameters()->value(0)->name(), QString("componentParameter"));
    QCOMPARE(mInstance->getParameters()->value(0)->getValue(), QString("1"));

    QSharedPointer<MetaInterface> mInterface = mInstance->getInterfaces()->value("instanceInterface");
    QCOMPARE(mInterface->ports_.size(), 1);

    QSharedPointer<MetaPort> mPort = mInterface->ports_.value("instance_clk");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("CLK");
    QCOMPARE(mpa->wire_->hierPorts_.first()->port_->name(), QString("top_clk"));
    QCOMPARE(mpa->logicalBounds_.first, QString("2"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addInterfaceToComponent()
//-----------------------------------------------------------------------------
QSharedPointer<BusInterface> tst_HDLParser::addInterfaceToComponent(QString const& interfaceName,
    QSharedPointer<Component> component,
    QSharedPointer<ConfigurableVLNVReference> absRef)
{
    QSharedPointer<BusInterface> busInterface(new BusInterface());
    busInterface->setName(interfaceName);
    component->getBusInterfaces()->append(busInterface);

    QSharedPointer<AbstractionType> testAbstraction (new AbstractionType());
    testAbstraction->setAbstractionRef(absRef);
    busInterface->getAbstractionTypes()->append(testAbstraction);

    return busInterface;
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser:::mapPortToInterface()
//-----------------------------------------------------------------------------
QSharedPointer<PortMap> tst_HDLParser::mapPortToInterface(QString const& portName, QString const& logicalName, 
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

    QSharedPointer<BusInterface> containingBusIf = component->getBusInterface(interfaceName);
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps = 
        containingBusIf->getAbstractionTypes()->first()->getPortMaps();

    portMaps->append(portMap);

    return portMap;
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::mapPortToInterface()
//-----------------------------------------------------------------------------
QSharedPointer<PortMap> tst_HDLParser::mapPortToInterface(QString const& portName, int left, int right, QString const& logicalName, 
    QString const& interfaceName, QSharedPointer<Component> component)
{
	QSharedPointer<PortMap> portMap(new PortMap());
	QSharedPointer<PortMap::LogicalPort> logPort(new PortMap::LogicalPort(logicalName));
	QSharedPointer<PortMap::PhysicalPort> physPort(new PortMap::PhysicalPort(portName));
	
    QSharedPointer<PartSelect> physicalPart (new PartSelect(QString::number(left), QString::number(right)));
    physPort->partSelect_ = physicalPart;

	portMap->setLogicalPort(logPort);
	portMap->setPhysicalPort(physPort);

    QSharedPointer<BusInterface> containingBus = component->getBusInterface(interfaceName);
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps = 
        containingBus->getAbstractionTypes()->first()->getPortMaps();

    if (!portMaps)
    {
        QSharedPointer<QList<QSharedPointer<PortMap> > > newPortMapList (new QList<QSharedPointer<PortMap> > ());

        if (containingBus->getAbstractionTypes()->isEmpty())
        {
            QSharedPointer<AbstractionType> testAbstraction (new AbstractionType());
            containingBus->getAbstractionTypes()->append(testAbstraction);

            if (logicalName == "CLK")
            {
                testAbstraction->setAbstractionRef(clkAbstractionVLNV_);
            }
            else
            {
                testAbstraction->setAbstractionRef(dataAbstractionVLNV_);
            }
        }
   
        portMaps = containingBus->getAbstractionTypes()->first()->getPortMaps();
    }
    
    portMaps->append(portMap);

    return portMap;
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addTestComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<View> tst_HDLParser::addTestComponentToLibrary(VLNV vlnv)
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

    addInterfaceToComponent("clk", instanceComponent, clkAbstractionVLNV_);

    mapPortToInterface("clk", "CLK", "clk", instanceComponent);

    addInterfaceToComponent("data", instanceComponent, dataAbstractionVLNV_);

    mapPortToInterface("data_in", "DATA", "data", instanceComponent);
    mapPortToInterface("enable", "ENABLE", "data", instanceComponent);
    mapPortToInterface("full", "FULL", "data", instanceComponent);

    library_.addComponent(instanceComponent);

	return activeView;
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addInstanceToDesign()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> tst_HDLParser::addInstanceToDesign(QString instanceName, VLNV instanceVlnv, QSharedPointer<View> activeView)
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
// Function: tst_HDLParser::testPortMapping()
//-----------------------------------------------------------------------------
/*void tst_HDLParser::testPortMapping()
{
    QSharedPointer<ConfigurableVLNVReference> abstractionVLNV(new ConfigurableVLNVReference(
        VLNV::ABSTRACTIONDEFINITION, "Test", "TestLibrary", "absDef", "1.0"));

    QSharedPointer<AbstractionDefinition> testAbstractionDefinition(new AbstractionDefinition());
    testAbstractionDefinition->setVlnv(*abstractionVLNV.data());
    library_.addComponent(testAbstractionDefinition);

    QSharedPointer<PortAbstraction> logicalPortHigh (new PortAbstraction());
    logicalPortHigh->setName("CUSTOM_HIGH");
    testAbstractionDefinition->getLogicalPorts()->append(logicalPortHigh);

    QSharedPointer<PortAbstraction> logicalPortLow (new PortAbstraction());
    logicalPortLow->setName("CUSTOM_LOW");
    testAbstractionDefinition->getLogicalPorts()->append(logicalPortLow);

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");

    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    addPort("test_out", 8, DirectionTypes::OUT, senderComponent);

    QSharedPointer<BusInterface> testIf = addInterfaceToComponent("data_bus", senderComponent, abstractionVLNV);
    testIf->setInterfaceMode(General::MASTER);    

    QSharedPointer<PortMap> enableLowPortMap(new PortMap());
    QSharedPointer<PortMap::LogicalPort> logPortLow(new PortMap::LogicalPort("CUSTOM_HIGH"));
    QSharedPointer<PortMap::PhysicalPort> physPortLow(new PortMap::PhysicalPort("test_out"));
    enableLowPortMap->setLogicalPort(logPortLow);
    enableLowPortMap->setPhysicalPort(physPortLow);

    QSharedPointer<PartSelect> lowPart (new PartSelect("7", "4"));
    physPortLow->partSelect_ = lowPart;

    QSharedPointer<Range> lowRange (new Range("3", "0"));
    logPortLow->range_ = lowRange;

    QSharedPointer<PortMap> enableHighPortMap(new PortMap());
    QSharedPointer<PortMap::LogicalPort> logPortHigh(new PortMap::LogicalPort("CUSTOM_LOW"));
    QSharedPointer<PortMap::PhysicalPort> physPortHigh(new PortMap::PhysicalPort("test_out"));
    enableHighPortMap->setLogicalPort(logPortHigh);
    enableHighPortMap->setPhysicalPort(physPortHigh);

    QSharedPointer<PartSelect> highPart (new PartSelect("3", "0"));
    physPortHigh->partSelect_ = highPart;

    QSharedPointer<Range> highRange (new Range("3", "0"));
    logPortHigh->range_ = highRange;

    QSharedPointer<AbstractionType> senderEnableAbstraction (new AbstractionType());
    testIf->getAbstractionTypes()->append(senderEnableAbstraction);

    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps = testIf->getPortMaps();
    portMaps->append(enableLowPortMap);
    portMaps->append(enableHighPortMap);

    QSharedPointer<View> sendView(new View("rtl"));
    sendView->setComponentInstantiationRef("instance1");

    QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
    senderComponent->getComponentInstantiations()->append(instantiation);
    senderComponent->getViews()->append(sendView);

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, sendView);

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 1);
    QSharedPointer<MetaInstance> mInstance0 = design->getInstances()->value("sender");

    QVERIFY(mInstance0->getInterfaces()->size() > 0);
    QVERIFY(mInstance0->getInterfaces()->first()->ports_.size() > 0);

    QSharedPointer<MetaPort> mPort = mInstance0->getInterfaces()->first()->ports_.value("test_out");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("CUSTOM_HIGH");
    QCOMPARE(mpa->logicalBounds_.first, QString("3"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));
    QCOMPARE(mpa->physicalBounds_.first, QString("7"));
    QCOMPARE(mpa->physicalBounds_.second, QString("4"));

    mpa = mPort->upAssignments_.value("CUSTOM_LOW");
    QCOMPARE(mpa->logicalBounds_.first, QString("3"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));
    QCOMPARE(mpa->physicalBounds_.first, QString("3"));
    QCOMPARE(mpa->physicalBounds_.second, QString("0"));
}*/

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testMasterToSlaveInterconnection()
//-----------------------------------------------------------------------------
void tst_HDLParser::testMasterToSlaveInterconnection()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> activeView = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV, activeView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    activeView = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("receiver", receiverVLNV, activeView);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 2);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("sender");
    QCOMPARE(mInstance->getInterfaces()->size(), 1);

    QSharedPointer<MetaInterface> mInterface = mInstance->getInterfaces()->value("data_bus");

    QSharedPointer<MetaPort> mPort = mInterface->ports_.value("data_out");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("7"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));
    QCOMPARE(mInterface->ports_.size(), 2);

    mPort = mInterface->ports_.value("enable_out");
    mpa = mPort->upAssignments_.value("ENABLE");
    QCOMPARE(mpa->logicalBounds_.first, QString("0"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mInstance = design->getInstances()->value("receiver");
    QCOMPARE(mInstance->getInterfaces()->size(), 1);

    mInterface = mInstance->getInterfaces()->value("data_bus");
    QCOMPARE(mInterface->ports_.size(), 2);

    mPort = mInterface->ports_.value("data_in");
    mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("7"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mPort = mInterface->ports_.value("enable_in");
    mpa = mPort->upAssignments_.value("ENABLE");
    QCOMPARE(mpa->logicalBounds_.first, QString("0"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));
    
    QCOMPARE(design->getInterconnections()->size(), 1);
    QSharedPointer<MetaInterconnection> mInterconnect = design->getInterconnections()->first();

    QCOMPARE(mInterconnect->wires_.size(), 2);
    QSharedPointer<MetaWire> mWire = mInterconnect->wires_.first();
    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("7"));
    QCOMPARE(mWire->bounds_.second, QString("0"));

    mWire = mInterconnect->wires_.last();
    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("0"));
    QCOMPARE(mWire->bounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testEmptyBounds()
//-----------------------------------------------------------------------------
void tst_HDLParser::testEmptyBounds()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));

    QSharedPointer<View> senderView(new View("view"));
    senderComponent->getViews()->append(senderView);

    QSharedPointer<ComponentInstantiation> sendCimp(new ComponentInstantiation("senderCimp"));
    senderView->setComponentInstantiationRef(sendCimp->name());
    senderComponent->getComponentInstantiations()->append(sendCimp);

    QSharedPointer<Port> senderPort = QSharedPointer<Port>(new Port("data_out", DirectionTypes::OUT));
    senderPort->setLeftBound("");
    senderPort->setRightBound("");
    senderComponent->getPorts()->append(senderPort);

    addInterfaceToComponent("data_bus", senderComponent, dataAbstractionVLNV_);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(General::MASTER);

    mapPortToInterface("data_out", "DATA", "data_bus", senderComponent);
    QSharedPointer<PortMap> dataMap = 
        senderComponent->getBusInterface("data_bus")->getAbstractionTypes()->first()->getPortMaps()->first();
    QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
    logPort->range_ = QSharedPointer<Range>(new Range("",""));

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, senderView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));

    QSharedPointer<View> receiverView(new View("view"));
    receiverComponent->getViews()->append(receiverView);

    QSharedPointer<ComponentInstantiation> recvCimp(new ComponentInstantiation("recvCimp"));
    receiverView->setComponentInstantiationRef(recvCimp->name());
    receiverComponent->getComponentInstantiations()->append(recvCimp);

    QSharedPointer<Port> receiverPort = QSharedPointer<Port>(new Port("data_in", DirectionTypes::IN));
    receiverPort->setLeftBound("");
    receiverPort->setRightBound("");
    receiverComponent->getPorts()->append(receiverPort);

    addInterfaceToComponent("data_bus", receiverComponent, dataAbstractionVLNV_);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(General::SLAVE);
    mapPortToInterface("data_in", "DATA", "data_bus", receiverComponent);

    library_.addComponent(receiverComponent);
    addInstanceToDesign("receiver", receiverVLNV, receiverView);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 2);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("sender");
    QCOMPARE(mInstance->getInterfaces()->size(), 1);

    QSharedPointer<MetaInterface> mInterface = mInstance->getInterfaces()->value("data_bus");
    QCOMPARE(mInterface->ports_.size(), 1);

    QSharedPointer<MetaPort> mPort = mInterface->ports_.value("data_out");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("0"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mInstance = design->getInstances()->value("receiver");
    QCOMPARE(mInstance->getInterfaces()->size(), 1);

    mInterface = mInstance->getInterfaces()->value("data_bus");
    QCOMPARE(mInterface->ports_.size(), 1);

    mPort = mInterface->ports_.value("data_in");
    mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("0"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    QCOMPARE(design->getInterconnections()->size(), 1);
    QSharedPointer<MetaInterconnection> mInterconnect = design->getInterconnections()->first();

    QCOMPARE(mInterconnect->wires_.size(), 1);
    QSharedPointer<MetaWire> mWire = mInterconnect->wires_.first();
    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("0"));
    QCOMPARE(mWire->bounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testMasterToSlaveInterconnectionWithExpressions()
//-----------------------------------------------------------------------------
void tst_HDLParser::testMasterToSlaveInterconnectionWithExpressions()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));

	QSharedPointer<View> senderView(new View("view"));
	senderComponent->getViews()->append(senderView);

	QSharedPointer<ComponentInstantiation> sendCimp(new ComponentInstantiation("senderCimp"));
	senderView->setComponentInstantiationRef(sendCimp->name());
	senderComponent->getComponentInstantiations()->append(sendCimp);

    QSharedPointer<Port> senderPort = QSharedPointer<Port>(new Port("data_out", DirectionTypes::OUT));
    senderPort->setLeftBound("20-2");
    senderPort->setRightBound("0");
    senderComponent->getPorts()->append(senderPort);

    addInterfaceToComponent("data_bus", senderComponent, dataAbstractionVLNV_);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(General::MASTER);

    mapPortToInterface("data_out", "DATA", "data_bus", senderComponent);
    QSharedPointer<PortMap> dataMap = 
        senderComponent->getBusInterface("data_bus")->getAbstractionTypes()->first()->getPortMaps()->first();
    QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
    logPort->range_ = QSharedPointer<Range>(new Range("7*2","0"));

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, senderView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
	QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));

	QSharedPointer<View> receiverView(new View("view"));
	receiverComponent->getViews()->append(receiverView);

	QSharedPointer<ComponentInstantiation> recvCimp(new ComponentInstantiation("recvCimp"));
	receiverView->setComponentInstantiationRef(recvCimp->name());
	receiverComponent->getComponentInstantiations()->append(recvCimp);

    QSharedPointer<Port> receiverPort = QSharedPointer<Port>(new Port("data_in", DirectionTypes::IN));
    receiverPort->setLeftBound("7+1");
    receiverPort->setRightBound("0");
    receiverComponent->getPorts()->append(receiverPort);

    addInterfaceToComponent("data_bus", receiverComponent, dataAbstractionVLNV_);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(General::SLAVE);
    mapPortToInterface("data_in", "DATA", "data_bus", receiverComponent);

    library_.addComponent(receiverComponent);
    addInstanceToDesign("receiver", receiverVLNV, receiverView);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 2);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("sender");
    QCOMPARE(mInstance->getInterfaces()->size(), 1);

    QSharedPointer<MetaInterface> mInterface = mInstance->getInterfaces()->value("data_bus");
    QCOMPARE(mInterface->ports_.size(), 1);

    QSharedPointer<MetaPort> mPort = mInterface->ports_.value("data_out");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("14"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mInstance = design->getInstances()->value("receiver");
    QCOMPARE(mInstance->getInterfaces()->size(), 1);

    mInterface = mInstance->getInterfaces()->value("data_bus");
    QCOMPARE(mInterface->ports_.size(), 1);

    mPort = mInterface->ports_.value("data_in");
    mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("8"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    QCOMPARE(design->getInterconnections()->size(), 1);
    QSharedPointer<MetaInterconnection> mInterconnect = design->getInterconnections()->first();

    QCOMPARE(mInterconnect->wires_.size(), 1);
    QSharedPointer<MetaWire> mWire = mInterconnect->wires_.first();
    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("14"));
    QCOMPARE(mWire->bounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addSenderComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<View> tst_HDLParser::addSenderComponentToLibrary(VLNV senderVLNV, General::InterfaceMode mode,
    bool createInterfaces /*= true*/)
{
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    addPort("enable_out", 1, DirectionTypes::OUT, senderComponent);
    addPort("data_out", 8, DirectionTypes::OUT, senderComponent);

    if (createInterfaces)
    {
        addInterfaceToComponent("data_bus", senderComponent, dataAbstractionVLNV_);
        senderComponent->getBusInterface("data_bus")->setInterfaceMode(mode);    
        mapPortToInterface("data_out", "DATA", "data_bus", senderComponent);
        QSharedPointer<PortMap> dataMap = 
            senderComponent->getBusInterface("data_bus")->getAbstractionTypes()->first()->getPortMaps()->first();
        QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
	    logPort->range_ = QSharedPointer<Range>(new Range("7","0"));

	    mapPortToInterface("enable_out", "ENABLE", "data_bus", senderComponent);
    }

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
// Function: tst_HDLParser::addReceiverComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<View> tst_HDLParser::addReceiverComponentToLibrary(VLNV receiverVLNV, General::InterfaceMode mode,
    bool createInterfaces /*= true*/)
{
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));
    addPort("enable_in", 1, DirectionTypes::IN, receiverComponent);
    addPort("data_in", 8, DirectionTypes::IN, receiverComponent);

    if (createInterfaces)
    {
        addInterfaceToComponent("data_bus", receiverComponent, dataAbstractionVLNV_);
        receiverComponent->getBusInterface("data_bus")->setInterfaceMode(mode);    
        mapPortToInterface("data_in", "DATA", "data_bus", receiverComponent);

        mapPortToInterface("enable_in", "ENABLE", "data_bus", receiverComponent);
    }

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
// Function: tst_HDLParser::addConnectionToDesign()
//-----------------------------------------------------------------------------
void tst_HDLParser::addConnectionToDesign(QString fromInstance, QString fromInterface, 
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
// Function: tst_HDLParser::testMasterToMultipleSlavesInterconnections()
//-----------------------------------------------------------------------------
void tst_HDLParser::testMasterToMultipleSlavesInterconnections()
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

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy(&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 3);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("receiver1");
    QCOMPARE(mInstance->getInterfaces()->size(), 1);

    QSharedPointer<MetaInterface> mInterface = mInstance->getInterfaces()->value("data_bus");
    QCOMPARE(mInterface->ports_.size(), 2);

    QSharedPointer<MetaPort> mPort = mInterface->ports_.value("data_in");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("7"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mPort = mInterface->ports_.value("enable_in");
    mpa = mPort->upAssignments_.value("ENABLE");
    QCOMPARE(mpa->logicalBounds_.first, QString("0"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mInstance = design->getInstances()->value("receiver2");
    QCOMPARE(mInstance->getInterfaces()->size(), 1);

    mInterface = mInstance->getInterfaces()->value("data_bus");
    QCOMPARE(mInterface->ports_.size(), 2);

    mPort = mInterface->ports_.value("data_in");
    mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("7"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mPort = mInterface->ports_.value("enable_in");
    mpa = mPort->upAssignments_.value("ENABLE");
    QCOMPARE(mpa->logicalBounds_.first, QString("0"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    QCOMPARE(design->getInterconnections()->size(), 1);
    QSharedPointer<MetaInterconnection> mInterconnect = design->getInterconnections()->first();

    QCOMPARE(mInterconnect->wires_.size(), 2);
    QSharedPointer<MetaWire> mWire = mInterconnect->wires_.first();
    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("7"));
    QCOMPARE(mWire->bounds_.second, QString("0"));

    mWire = mInterconnect->wires_.last();
    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("0"));
    QCOMPARE(mWire->bounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testInterconnectionToVaryingSizeLogicalMaps()
//-----------------------------------------------------------------------------
void tst_HDLParser::testInterconnectionToVaryingSizeLogicalMaps()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> sendView = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV, sendView);

    VLNV oneBitReceiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<View> recvView1 = addReceiverComponentToLibrary(oneBitReceiverVLNV, General::SLAVE);
    setReceiverComponentDataWidth(oneBitReceiverVLNV, 1);
    addInstanceToDesign("oneBitReceiver", oneBitReceiverVLNV, recvView1);
    
    VLNV fourBitReceiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver4bit", "1.0");
    QSharedPointer<View> recvView2 = addReceiverComponentToLibrary(fourBitReceiverVLNV, General::SLAVE);
    setReceiverComponentDataWidth(fourBitReceiverVLNV, 4);
    addInstanceToDesign("fourBitReceiver", fourBitReceiverVLNV, recvView2);

    VLNV sixteenBitReceiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver16bit", "1.0");
    QSharedPointer<View> recvView3 = addReceiverComponentToLibrary(sixteenBitReceiverVLNV, General::SLAVE);
    setReceiverComponentDataWidth(sixteenBitReceiverVLNV, 16);
    addInstanceToDesign("sixteenBitReceiver", sixteenBitReceiverVLNV, recvView3);

    addConnectionToDesign("sender", "data_bus", "oneBitReceiver", "data_bus");
    addConnectionToDesign("sender", "data_bus", "fourBitReceiver", "data_bus");
    addConnectionToDesign("sender", "data_bus", "sixteenBitReceiver", "data_bus");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 4);

    QSharedPointer<MetaInstance> mInstance0 = design->getInstances()->value("sender");
    QSharedPointer<MetaInstance> mInstance1 = design->getInstances()->value("oneBitReceiver");
    QSharedPointer<MetaInstance> mInstance2 = design->getInstances()->value("fourBitReceiver");
    QSharedPointer<MetaInstance> mInstance3 = design->getInstances()->value("sixteenBitReceiver");

    QCOMPARE(mInstance0->getInterfaces()->first()->ports_.size(), 2);
    QCOMPARE(mInstance1->getInterfaces()->first()->ports_.size(), 2);
    QCOMPARE(mInstance2->getInterfaces()->first()->ports_.size(), 2);
    QCOMPARE(mInstance3->getInterfaces()->first()->ports_.size(), 2);

    QSharedPointer<MetaPort> mPort = mInstance0->getInterfaces()->first()->ports_.value("data_out");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("7"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mPort = mInstance1->getInterfaces()->first()->ports_.value("data_in");
    mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("0"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mPort = mInstance2->getInterfaces()->first()->ports_.value("data_in");
    mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("3"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mPort = mInstance3->getInterfaces()->first()->ports_.value("data_in");
    mpa = mPort->upAssignments_.value("DATA");
    QCOMPARE(mpa->logicalBounds_.first, QString("15"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    QCOMPARE(design->getInterconnections()->size(), 1);
    QSharedPointer<MetaInterconnection> mInterconnect = design->getInterconnections()->first();

    QCOMPARE(mInterconnect->wires_.size(), 2);
    QSharedPointer<MetaWire> mWire = mInterconnect->wires_.first();
    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("15"));
    QCOMPARE(mWire->bounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::setComponentDataWidth()
//-----------------------------------------------------------------------------
void tst_HDLParser::setReceiverComponentDataWidth(VLNV receiverVLNV, int dataWidth)
{
    QSharedPointer<Component> component = library_.getModel(receiverVLNV).dynamicCast<Component>();
    QSharedPointer<Port> componentPort = component->getPort("data_in");
    componentPort->setLeftBound(QString::number(dataWidth - 1));
    componentPort->setRightBound(QString::number(0));

	QSharedPointer<PortMap> dataMap = 
        component->getBusInterface("data_bus")->getAbstractionTypes()->first()->getPortMaps()->first();

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
// Function: tst_HDLParser::testSlicedInterconnection()
//-----------------------------------------------------------------------------
void tst_HDLParser::testSlicedInterconnection()
{
	VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");

    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    addPort("enable_out_low", 1, DirectionTypes::OUT, senderComponent);
    addPort("enable_out_high", 1, DirectionTypes::OUT, senderComponent);

    addInterfaceToComponent("data_bus", senderComponent, dataAbstractionVLNV_);
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
	QSharedPointer<PortMap::PhysicalPort> physPortHigh(new PortMap::PhysicalPort("enable_out_high"));
    enableHighPortMap->setLogicalPort(logPortHigh);
    enableHighPortMap->setPhysicalPort(physPortHigh);

    QSharedPointer<PartSelect> highPart (new PartSelect("0", "0"));
    physPortHigh->partSelect_ = highPart;

    QSharedPointer<Range> highRange (new Range("1", "1"));
    logPortHigh->range_ = highRange;

    QSharedPointer<AbstractionType> senderEnableAbstraction (new AbstractionType());
    enableIf->getAbstractionTypes()->append(senderEnableAbstraction);

    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps =
        enableIf->getAbstractionTypes()->first()->getPortMaps();
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
    QSharedPointer<PortMap> enableMap =
        receiverComponent->getBusInterface("data_bus")->getAbstractionTypes()->first()->getPortMaps()->last();
    enableMap->getLogicalPort()->range_ = QSharedPointer<Range>(new Range("1", "0"));

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

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 2);

    QSharedPointer<MetaInstance> mInstance0 = design->getInstances()->value("sender");
    QSharedPointer<MetaInstance> mInstance1 = design->getInstances()->value("receiver");

    QVERIFY(mInstance0->getInterfaces()->first()->ports_.size() > 0);
    QVERIFY(mInstance1->getInterfaces()->first()->ports_.size() > 0);

    QSharedPointer<MetaPort> mPort = mInstance0->getInterfaces()->first()->ports_.value("enable_out_high");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("ENABLE");
    QCOMPARE(mpa->logicalBounds_.first, QString("1"));
    QCOMPARE(mpa->logicalBounds_.second, QString("1"));

    mPort = mInstance0->getInterfaces()->first()->ports_.value("enable_out_low");
    mpa = mPort->upAssignments_.value("ENABLE");
    QCOMPARE(mpa->logicalBounds_.first, QString("0"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    mPort = mInstance1->getInterfaces()->first()->ports_.value("enable_in");
    mpa = mPort->upAssignments_.value("ENABLE");
    QCOMPARE(mpa->logicalBounds_.first, QString("1"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    QCOMPARE(design->getInterconnections()->size(), 1);
    QSharedPointer<MetaInterconnection> mInterconnect = design->getInterconnections()->first();

    QVERIFY(mInterconnect->wires_.size() > 0);
    QSharedPointer<MetaWire> mWire = mInterconnect->wires_.last();
    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("1"));
    QCOMPARE(mWire->bounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testAbsDefDefault()
//-----------------------------------------------------------------------------
/*void tst_HDLParser::testAbsDefDefault()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");

    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    addPort("enable_out_low", 1, DirectionTypes::OUT, senderComponent);
    addPort("enable_out_high", 1, DirectionTypes::OUT, senderComponent);

    addInterfaceToComponent("data_bus", senderComponent, dataAbstractionVLNV_);
    QSharedPointer<BusInterface> enableIf = senderComponent->getBusInterface("data_bus");
    enableIf->setInterfaceMode(General::MASTER);    

    QSharedPointer<AbstractionType> senderEnableAbstraction (new AbstractionType());
    enableIf->getAbstractionTypes()->append(senderEnableAbstraction);

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
    enableMap->getLogicalPort()->range_ = QSharedPointer<Range>(new Range("2", "0"));

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
    wire->setDefaultValue("5");
    QSharedPointer<WirePort> wp(new WirePort);
    wp->setWidth("2");
    wire->setSlavePort(wp);
    wire->setMasterPort(wp);

    testAbstractionDefinition->getLogicalPorts()->append(logicalPort);

    enableIf->getAbstractionTypes()->first()->setAbstractionRef(abstractionVLNV);
    receiverComponent->getBusInterface("data_bus")->getAbstractionTypes()->first()->setAbstractionRef(abstractionVLNV);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 2);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("receiver");

    QVERIFY(mInstance->getInterfaces()->first()->ports_.size() > 0);

    QSharedPointer<MetaPort> mPort = mInstance->getInterfaces()->first()->ports_.value("enable_in");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("ENABLE");
    QCOMPARE(mpa->logicalBounds_.first, QString("2"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));
    QCOMPARE(mpa->defaultValue_, QString("5"));
}*/

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testPortDefaultValueInComponentInstance()
//-----------------------------------------------------------------------------
void tst_HDLParser::testPortDefaultValueInComponentInstance()
{
    VLNV tieOffVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestTieOff", "1.0");
    QSharedPointer<Component> tieOffComponent(new Component(tieOffVLNV));

    QSharedPointer<View> activeView(new View("rtl"));
    activeView->setComponentInstantiationRef("instance1");

    QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
    tieOffComponent->getComponentInstantiations()->append(instantiation);
    tieOffComponent->getViews()->append(activeView);

    QString instanceName = "tieOffer";

    QString unconnnectedIn = "unconnnectedIn";
    QString unconnnectedInOut = "unconnnectedInOut";
    QString unconnnectedOut = "unconnnectedOut";

    addPort(unconnnectedIn, 1, DirectionTypes::IN, tieOffComponent);
    addPort(unconnnectedInOut, 2, DirectionTypes::INOUT, tieOffComponent);
    addPort(unconnnectedOut, 3, DirectionTypes::OUT, tieOffComponent);

    addInterfaceToComponent("data_bus", tieOffComponent, dataAbstractionVLNV_);

    mapPortToInterface("unconnnectedIn", "DATA", "data_bus", tieOffComponent);
    mapPortToInterface("unconnnectedInOut", "ENABLE", "data_bus", tieOffComponent);
    mapPortToInterface("unconnnectedOut", "FULL", "data_bus", tieOffComponent);

    library_.addComponent(tieOffComponent);

    addInstanceToDesign(instanceName, tieOffVLNV, activeView);

    QSharedPointer<Port> defaultPort1 = tieOffComponent->getPort("unconnnectedIn");
    defaultPort1->setDefaultValue("1");
    QSharedPointer<Port> defaultPort2 = tieOffComponent->getPort("unconnnectedInOut");
    defaultPort2->setDefaultValue("35");
    QSharedPointer<Port> defaultPort3 = tieOffComponent->getPort("unconnnectedOut");
    defaultPort3->setDefaultValue("23");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();
    QCOMPARE(design->getInstances()->size(), 1);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("tieOffer");
    QCOMPARE(mInstance->getPorts()->size(), 3);

    QSharedPointer<MetaPort> mPort = mInstance->getPorts()->value("unconnnectedIn");
    QCOMPARE(mPort->defaultValue_, QString("1"));
    QCOMPARE(mPort->upAssignments_.size(), 0);
    //QCOMPARE(mPort->downAssignments_.size(), 0);

    mPort = mInstance->getPorts()->value("unconnnectedInOut");
    QCOMPARE(mPort->defaultValue_, QString("35"));
    QCOMPARE(mPort->upAssignments_.size(), 0);
    //QCOMPARE(mPort->downAssignments_.size(), 0);

    mPort = mInstance->getPorts()->value("unconnnectedOut");
    QCOMPARE(mPort->defaultValue_, QString("23"));
    QCOMPARE(mPort->upAssignments_.size(), 0);
    //QCOMPARE(mPort->downAssignments_.size(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testHierarchicalPortDefaultValue()
//-----------------------------------------------------------------------------
void tst_HDLParser::testHierarchicalPortDefaultValue()
{
    QSharedPointer<View> activeView(new View("rtl"));
    activeView->setComponentInstantiationRef("instance1");

    QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
    topComponent_->getComponentInstantiations()->append(instantiation);
    topComponent_->getViews()->append(activeView);

    QString instanceName = "tieOffer";

    QString unconnnectedIn = "unconnnectedIn";
    QString unconnnectedInOut = "unconnnectedInOut";
    QString unconnnectedOut = "unconnnectedOut";

    addPort(unconnnectedIn, 1, DirectionTypes::IN, topComponent_);
    addPort(unconnnectedInOut, 2, DirectionTypes::INOUT, topComponent_);
    addPort(unconnnectedOut, 3, DirectionTypes::OUT, topComponent_);

    addInterfaceToComponent("data_bus", topComponent_, dataAbstractionVLNV_);

    mapPortToInterface("unconnnectedIn", "DATA", "data_bus", topComponent_);
    mapPortToInterface("unconnnectedInOut", "ENABLE", "data_bus", topComponent_);
    mapPortToInterface("unconnnectedOut", "FULL", "data_bus", topComponent_);;

    QSharedPointer<Port> defaultPort1 = topComponent_->getPort("unconnnectedIn");
    defaultPort1->setDefaultValue("1");
    QSharedPointer<Port> defaultPort2 = topComponent_->getPort("unconnnectedInOut");
    defaultPort2->setDefaultValue("35");
    QSharedPointer<Port> defaultPort3 = topComponent_->getPort("unconnnectedOut");
    defaultPort3->setDefaultValue("23");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();
    QVERIFY(design->getTopInstance());

    QSharedPointer<MetaInstance> mInstance = design->getTopInstance();
    QCOMPARE(mInstance->getPorts()->size(), 3);

    QSharedPointer<MetaPort> mPort = mInstance->getPorts()->value("unconnnectedIn");
    QCOMPARE(mPort->defaultValue_, QString("1"));
    //QCOMPARE(mPort->upAssignments_.size(), 0);
    QCOMPARE(mPort->downAssignments_.size(), 0);

    mPort = mInstance->getPorts()->value("unconnnectedInOut");
    QCOMPARE(mPort->defaultValue_, QString("35"));
    //QCOMPARE(mPort->upAssignments_.size(), 0);
    QCOMPARE(mPort->downAssignments_.size(), 0);

    mPort = mInstance->getPorts()->value("unconnnectedOut");
    QCOMPARE(mPort->defaultValue_, QString("23"));
    //QCOMPARE(mPort->upAssignments_.size(), 0);
    QCOMPARE(mPort->downAssignments_.size(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testAdhocConnectionBetweenComponentInstances()
//-----------------------------------------------------------------------------
void tst_HDLParser::testAdhocConnectionBetweenComponentInstances()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> senderView = addSenderComponentToLibrary(senderVLNV, General::MASTER, false);
    addInstanceToDesign("sender", senderVLNV, senderView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<View> receiverView = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE, false);
    addInstanceToDesign("receiver1", receiverVLNV, receiverView);
    addInstanceToDesign("receiver2", receiverVLNV, receiverView);

    addAdhocConnection("enableAdHoc", "sender", "enable_out", "receiver1", "enable_in");
    addAdhocConnection("enableAdHoc", "sender", "enable_out", "receiver2", "enable_in");
    addAdhocConnection("dataAdHoc", "sender", "data_out", "receiver1", "data_in");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 3);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("sender");

    QSharedPointer<MetaPort> mPort = mInstance->getPorts()->value("data_out");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("dataAdHoc");
    QCOMPARE(mpa->physicalBounds_.first, QString("7"));
    QCOMPARE(mpa->physicalBounds_.second, QString("0"));

    mPort = mInstance->getPorts()->value("enable_out");
    mpa = mPort->upAssignments_.value("enableAdHoc");
    QCOMPARE(mpa->physicalBounds_.first, QString("0"));
    QCOMPARE(mpa->physicalBounds_.second, QString("0"));

    mInstance = design->getInstances()->value("receiver1");

    mPort = mInstance->getPorts()->value("data_in");
    mpa = mPort->upAssignments_.value("dataAdHoc");
    QCOMPARE(mpa->physicalBounds_.first, QString("7"));
    QCOMPARE(mpa->physicalBounds_.second, QString("0"));

    mPort = mInstance->getPorts()->value("enable_in");
    mpa = mPort->upAssignments_.value("enableAdHoc");
    QCOMPARE(mpa->physicalBounds_.first, QString("0"));
    QCOMPARE(mpa->physicalBounds_.second, QString("0"));

    mInstance = design->getInstances()->value("receiver2");

    mPort = mInstance->getPorts()->value("enable_in");
    mpa = mPort->upAssignments_.value("enableAdHoc");
    QCOMPARE(mpa->physicalBounds_.first, QString("0"));
    QCOMPARE(mpa->physicalBounds_.second, QString("0"));

    QCOMPARE(design->getAdHocWires()->size(), 2);
    QSharedPointer<MetaWire> mWire = design->getAdHocWires()->first();

    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("0"));
    QCOMPARE(mWire->bounds_.second, QString("0"));

    mWire = design->getAdHocWires()->last();
    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("7"));
    QCOMPARE(mWire->bounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testAdhocConnectionToVaryingSizePorts()
//-----------------------------------------------------------------------------
void tst_HDLParser::testAdhocConnectionToVaryingSizePorts()
{
    QSharedPointer<View> activeView(new View("rtl"));
    activeView->setComponentInstantiationRef("instance1");

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, activeView);
    senderComponent->getViews()->append(activeView);

    QSharedPointer<Port> senderPort = QSharedPointer<Port>(new Port("data_out", DirectionTypes::OUT));
    senderPort->setLeftBound("16");
    senderPort->setRightBound("4");
    senderComponent->getPorts()->append(senderPort);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<View> receiverView = addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("receiver", receiverVLNV, receiverView);

    addAdhocConnection("dataAdHoc", "sender", "data_out", "receiver", "data_in");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 2);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("sender");

    QSharedPointer<MetaPort> mPort = mInstance->getPorts()->value("data_out");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("dataAdHoc");
    QCOMPARE(mpa->physicalBounds_.first, QString("16"));
    QCOMPARE(mpa->physicalBounds_.second, QString("4"));
    QCOMPARE(mpa->logicalBounds_.first, QString("12"));
    QCOMPARE(mpa->logicalBounds_.second, QString("0"));

    QCOMPARE(design->getAdHocWires()->size(), 1);
    QSharedPointer<MetaWire> mWire = design->getAdHocWires()->first();

    mWire = design->getAdHocWires()->last();
    QCOMPARE(mWire->bounds_.first, QString("12"));
    QCOMPARE(mWire->bounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testAdhocConnectionWithPartSelect()
//-----------------------------------------------------------------------------
void tst_HDLParser::testAdhocConnectionWithPartSelect()
{
    QSharedPointer<View> senderView(new View("rtl"));
    senderView->setComponentInstantiationRef("instance1");

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, senderView);
    senderComponent->getViews()->append(senderView);

    QSharedPointer<Port> senderPort = QSharedPointer<Port>(new Port("data_out", DirectionTypes::OUT));
    senderPort->setLeftBound("16");
    senderPort->setRightBound("4");
    senderComponent->getPorts()->append(senderPort);

    QSharedPointer<View> receiverView(new View("rtl"));

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));
    library_.addComponent(receiverComponent);
    addInstanceToDesign("receiver", receiverVLNV, receiverView);
    receiverComponent->getViews()->append(receiverView);

    QSharedPointer<Port> receiverPort = QSharedPointer<Port>(new Port("data_in", DirectionTypes::IN));
    receiverPort->setLeftBound("7");
    receiverPort->setRightBound("0");
    receiverComponent->getPorts()->append(receiverPort);

    QSharedPointer<AdHocConnection> connection = addAdhocConnection("dataAdHoc", "sender", "data_out", "receiver", "data_in");

    QSharedPointer<PartSelect> ps1(new PartSelect);
    ps1->setLeftRange("8");
    ps1->setRightRange("3");
    connection->getInternalPortReferences()->first()->setPartSelect(ps1);

    QSharedPointer<PartSelect> ps2(new PartSelect);
    ps2->setLeftRange("2");
    ps2->setRightRange("0");
    connection->getInternalPortReferences()->last()->setPartSelect(ps2);

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 2);

    QSharedPointer<MetaInstance> mInstance1 = design->getInstances()->value("sender");

    QSharedPointer<MetaPort> mPort1 = mInstance1->getPorts()->value("data_out");
    QSharedPointer<MetaPortAssignment> mpa1 = mPort1->upAssignments_.value("dataAdHoc");
    QCOMPARE(mpa1->physicalBounds_.first, QString("8"));
    QCOMPARE(mpa1->physicalBounds_.second, QString("3"));
    QCOMPARE(mpa1->logicalBounds_.first, QString("5"));
    QCOMPARE(mpa1->logicalBounds_.second, QString("0"));

    QSharedPointer<MetaInstance> mInstance2 = design->getInstances()->value("receiver");

    QSharedPointer<MetaPort> mPort2 = mInstance2->getPorts()->value("data_in");
    QSharedPointer<MetaPortAssignment> mpa2 = mPort2->upAssignments_.value("dataAdHoc");
    QCOMPARE(mpa2->physicalBounds_.first, QString("2"));
    QCOMPARE(mpa2->physicalBounds_.second, QString("0"));
    QCOMPARE(mpa2->logicalBounds_.first, QString("2"));
    QCOMPARE(mpa2->logicalBounds_.second, QString("0"));

    QCOMPARE(design->getAdHocWires()->size(), 1);
    QSharedPointer<MetaWire> mWire = design->getAdHocWires()->first();

    mWire = design->getAdHocWires()->last();
    QCOMPARE(mWire->bounds_.first, QString("5"));
    QCOMPARE(mWire->bounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testAdhocTieOffInComponentInstance()
//-----------------------------------------------------------------------------
void tst_HDLParser::testAdhocTieOffInComponentInstance()
{
    VLNV tieOffVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestTieOff", "1.0");
	QSharedPointer<Component> tieOffComponent(new Component(tieOffVLNV));

	QSharedPointer<View> activeView(new View("rtl"));
	activeView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	tieOffComponent->getComponentInstantiations()->append(instantiation);
	tieOffComponent->getViews()->append(activeView);

    QString instanceName = "tieOffer";

    QString portDefaultName = "portDefault";
    QString defaultValueName = "defaultValue";
    QString zeroName = "zeroTieOff";
    QString oneName = "oneTieOff";
    QString naName = "n/aTieOff";
    QString numberedName = "numberedTieOff";
    QString outName = "tieOffOut";
    QString inOutName = "tieOffInOut";
    QString openName = "openTieOff";
    QString expressionName = "expressionTieOff";

    QSharedPointer<Port> portDefault = addPort(portDefaultName, 4, DirectionTypes::OUT, tieOffComponent);
    portDefault->setDefaultValue("20");
    QSharedPointer<Port> defaultPort = addPort(defaultValueName, 4, DirectionTypes::IN, tieOffComponent);
    defaultPort->setDefaultValue("35");

    addPort(zeroName, 2, DirectionTypes::IN, tieOffComponent);
    addPort(oneName, 4, DirectionTypes::IN, tieOffComponent);
    addPort(naName, 0, DirectionTypes::IN, tieOffComponent);
    addPort(numberedName, 10, DirectionTypes::IN, tieOffComponent);
    addPort(outName, 2, DirectionTypes::OUT, tieOffComponent);
    addPort(inOutName, 10, DirectionTypes::INOUT, tieOffComponent);
    addPort(openName, 1, DirectionTypes::IN, tieOffComponent);
    addPort(expressionName, 1, DirectionTypes::IN, tieOffComponent);

    library_.addComponent(tieOffComponent);

    addInstanceToDesign(instanceName, tieOffVLNV, activeView);

    QSharedPointer<Parameter> expressionParameter (new Parameter());
    expressionParameter->setName("expName");
    expressionParameter->setValueId("expID");
    expressionParameter->setValue("5");
    tieOffComponent->getParameters()->append(expressionParameter);

    addTieOffAdhocConnectionToInstancePort("default", instanceName, defaultValueName, "default_connection");
    addTieOffAdhocConnectionToInstancePort("0", instanceName, zeroName, "zero_connection");
    addTieOffAdhocConnectionToInstancePort("1", instanceName, oneName, "one_connection");
    addTieOffAdhocConnectionToInstancePort("abc", instanceName, naName, "n/a_connection");
    addTieOffAdhocConnectionToInstancePort("12", instanceName, numberedName, "number_connection");
    addTieOffAdhocConnectionToInstancePort("0", instanceName, outName, "out_connection");
    addTieOffAdhocConnectionToInstancePort("1", instanceName, inOutName, "inOut_connection");
    addTieOffAdhocConnectionToInstancePort("open", instanceName, openName, "open_connection");
    addTieOffAdhocConnectionToInstancePort("expID - 4", instanceName, expressionName, "expression_connection");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();
    QCOMPARE(design->getInstances()->size(), 1);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("tieOffer");
    QCOMPARE(mInstance->getPorts()->size(), 10);

    QSharedPointer<MetaPort> mPort = mInstance->getPorts()->value(portDefaultName);
    QCOMPARE(mPort->defaultValue_, QString("20"));

    mPort = mInstance->getPorts()->value(defaultValueName);
    QCOMPARE(mPort->upAssignments_.first()->defaultValue_, QString("35"));
    QVERIFY(!mPort->upAssignments_.first()->wire_);
    mPort = mInstance->getPorts()->value("expressionTieOff");
    QCOMPARE(mPort->upAssignments_.first()->defaultValue_, QString("expID - 4"));
    QVERIFY(!mPort->upAssignments_.first()->wire_);
    mPort = mInstance->getPorts()->value("n/aTieOff");
    QCOMPARE(mPort->upAssignments_.first()->defaultValue_, QString("abc"));
    QVERIFY(!mPort->upAssignments_.first()->wire_);
    mPort = mInstance->getPorts()->value("numberedTieOff");
    QCOMPARE(mPort->upAssignments_.first()->defaultValue_, QString("12"));
    QVERIFY(!mPort->upAssignments_.first()->wire_);
    mPort = mInstance->getPorts()->value("oneTieOff");
    QCOMPARE(mPort->upAssignments_.first()->defaultValue_, QString("1"));
    QVERIFY(!mPort->upAssignments_.first()->wire_);
    mPort = mInstance->getPorts()->value("openTieOff");
    QVERIFY(mPort->upAssignments_.size() < 1);
    mPort = mInstance->getPorts()->value("zeroTieOff");
    QCOMPARE(mPort->upAssignments_.first()->defaultValue_, QString("0"));
    QVERIFY(!mPort->upAssignments_.first()->wire_);
    mPort = mInstance->getPorts()->value("tieOffOut");
    QCOMPARE(mPort->upAssignments_.first()->defaultValue_, QString("0"));
    QVERIFY(!mPort->upAssignments_.first()->wire_);
    mPort = mInstance->getPorts()->value("tieOffInOut");
    QCOMPARE(mPort->upAssignments_.first()->defaultValue_, QString("1"));
    QVERIFY(!mPort->upAssignments_.first()->wire_);
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addAdhocConnection()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnection> tst_HDLParser::addAdhocConnection(QString const& connectionName, 
    QString const& sourceInstance, QString const& sourcePort, 
    QString const& targetInstance, QString const& targetPort)
{
    QSharedPointer<QList<QSharedPointer<PortReference> > > internalRefs(
        new QList<QSharedPointer<PortReference> >());

    QSharedPointer<QList<QSharedPointer<AdHocConnection> > > adHocConnections = design_->getAdHocConnections();
    for (int i = 0; i < adHocConnections->size(); i++)
    {
        if (adHocConnections->value(i)->name() == connectionName)
        {
            internalRefs  = adHocConnections->value(i)->getInternalPortReferences();
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

    return connection;
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addTieOffAdhocConnectionToInstancePort()
//-----------------------------------------------------------------------------
void tst_HDLParser::addTieOffAdhocConnectionToInstancePort(QString const& tieOffValue,
    QString const& instanceName, QString const& portName, QString const& connectionName)
{
    QSharedPointer<PortReference> referencedPort(new PortReference(portName, instanceName));

    QSharedPointer<AdHocConnection> connection(new AdHocConnection(connectionName));
    connection->setTiedValue(tieOffValue);
    connection->getInternalPortReferences()->append(referencedPort);

    design_->getAdHocConnections()->append(connection);
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testHierarchicalAdhocConnection()
//-----------------------------------------------------------------------------
void tst_HDLParser::testHierarchicalAdhocConnection()
{
    addPort("data_from_sender", 8, DirectionTypes::OUT, topComponent_);

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> activeView = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV, activeView);

    addHierAdhocConnection("data_from_sender", "sender", "data_out");

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 1);

    QCOMPARE(design->getAdHocWires()->size(), 1);

    QSharedPointer<MetaWire> mWire = design->getAdHocWires()->first();

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->last();

    /*QSharedPointer<MetaPort> mPort = mInstance->getPorts()->value("data_out");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.last();
    QCOMPARE(mpa->wire_, mWire);

    QSharedPointer<MetaPort> hierPort = design->getTopInstance()->getPorts()->first();
    QCOMPARE(mpa->wire_->hierPorts_.first(), hierPort);
    QSharedPointer<MetaPortAssignment> hierAssignment = hierPort->downAssignments_.first();
    QCOMPARE(hierAssignment->wire_, mWire);
    QCOMPARE(hierAssignment->wire_->hierPorts_.first(), hierPort);*/
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addHierAdhocConnection()
//-----------------------------------------------------------------------------
void tst_HDLParser::addHierAdhocConnection(QString const& topPort, 
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
// Function: tst_HDLParser::testAdHocConnectionBetweenMultipleComponentInstances()
//-----------------------------------------------------------------------------
void tst_HDLParser::testAdHocConnectionBetweenMultipleComponentInstances()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));

    QSharedPointer<Port> senderPort = QSharedPointer<Port>(new Port("data_out", DirectionTypes::OUT));
    senderPort->setLeftBound("7");
    senderPort->setRightBound("0");
    senderComponent->getPorts()->append(senderPort);

    addInterfaceToComponent("data_bus", senderComponent, dataAbstractionVLNV_);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(General::MASTER);

    mapPortToInterface("data_out", "DATA", "data_bus", senderComponent);

	QSharedPointer<View> view1(new View("view1"));
	senderComponent->getViews()->append(view1);
	QSharedPointer<ComponentInstantiation> instantiation1(new ComponentInstantiation("instance1"));
	view1->setComponentInstantiationRef(instantiation1->name());
	senderComponent->getComponentInstantiations()->append(instantiation1);

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, view1);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));

    QSharedPointer<Port> receiverPort = QSharedPointer<Port>(new Port("data_in", DirectionTypes::IN));
    receiverPort->setLeftBound("7");
    receiverPort->setRightBound("0");
    receiverComponent->getPorts()->append(receiverPort);

    addInterfaceToComponent("data_bus", receiverComponent, dataAbstractionVLNV_);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(General::SLAVE);
    mapPortToInterface("data_in", "DATA", "data_bus", receiverComponent);

	library_.addComponent(receiverComponent);
	QSharedPointer<View> view2(new View("view2"));
	receiverComponent->getViews()->append(view2);
	QSharedPointer<ComponentInstantiation> instantiation2(new ComponentInstantiation("instance2"));
	view2->setComponentInstantiationRef(instantiation2->name());
	receiverComponent->getComponentInstantiations()->append(instantiation2);

    addInstanceToDesign("receiver1", receiverVLNV, view2);
    addInstanceToDesign("receiver2", receiverVLNV, view2);

    QSharedPointer<AdHocConnection> multiConnection(new AdHocConnection("dataAdHoc"));

    QSharedPointer<PortReference> startReference(new PortReference("data_out", "sender"));
    QSharedPointer<PortReference> endReference1(new PortReference("data_in", "receiver1"));
    QSharedPointer<PortReference> endReference2(new PortReference("data_in", "receiver2"));

    multiConnection->getInternalPortReferences()->append(startReference);
    multiConnection->getInternalPortReferences()->append(endReference1);
    multiConnection->getInternalPortReferences()->append(endReference2);

    design_->getAdHocConnections()->append(multiConnection);

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 3);

    QCOMPARE(design->getAdHocWires()->size(), 1);
    QSharedPointer<MetaWire> mWire = design->getAdHocWires()->first();
    mWire = design->getAdHocWires()->last();
    QCOMPARE(mWire->hierPorts_.size(), 0);
    QCOMPARE(mWire->bounds_.first, QString("7"));
    QCOMPARE(mWire->bounds_.second, QString("0"));

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("sender");

    QSharedPointer<MetaPort> mPort = mInstance->getPorts()->value("data_out");
    QSharedPointer<MetaPortAssignment> mpa = mPort->upAssignments_.value("dataAdHoc");
    /*QCOMPARE(mpa->physicalBounds_.first, QString("7"));
    QCOMPARE(mpa->physicalBounds_.second, QString("0"));
    QCOMPARE(mpa->wire_, mWire);

    mInstance = design->getInstances()->value("receiver1");

    mPort = mInstance->getPorts()->value("data_in");
    mpa = mPort->upAssignments_.value("dataAdHoc");
    QCOMPARE(mpa->physicalBounds_.first, QString("7"));
    QCOMPARE(mpa->physicalBounds_.second, QString("0"));
    QCOMPARE(mpa->wire_, mWire);

    mInstance = design->getInstances()->value("receiver2");

    mPort = mInstance->getPorts()->value("data_in");
    mpa = mPort->upAssignments_.value("dataAdHoc");
    QCOMPARE(mpa->physicalBounds_.first, QString("7"));
    QCOMPARE(mpa->physicalBounds_.second, QString("0"));
    QCOMPARE(mpa->wire_, mWire);*/
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testInstanceParametersAreCulled()
//-----------------------------------------------------------------------------
void tst_HDLParser::testInstanceParametersAreCulled()
{
	QSharedPointer<View> activeView(new View("rtl"));
	activeView->setComponentInstantiationRef("instance1");

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");    
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    library_.addComponent(senderComponent);
    QSharedPointer<ComponentInstance> instace = addInstanceToDesign("sender", senderVLNV, activeView);

    QSharedPointer<Parameter> componentParameter(new Parameter());
    componentParameter->setValueId("parameterId2");
    componentParameter->setName("componentParameter");
    componentParameter->setValue("3");
    componentParameter->setValueResolve("user");

    senderComponent->getParameters()->append(componentParameter);

    QSharedPointer<ModuleParameter> moduleParameter(new ModuleParameter());
    moduleParameter->setValueId("parameterId");
    moduleParameter->setName("moduleParameter");
    moduleParameter->setValue("1");
    moduleParameter->setValueResolve("user");

    QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
    instantiation->getModuleParameters()->append(moduleParameter);

    senderComponent->getComponentInstantiations()->append(instantiation);
    senderComponent->getViews()->append(activeView);
    
    QSharedPointer<ConfigurableElementValue> parameterOverride(new ConfigurableElementValue());
    parameterOverride->setReferenceId("parameterId2");
    parameterOverride->setConfigurableValue("2");

    instace->getConfigurableElementValues()->append(parameterOverride);

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 1);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("sender");
    QCOMPARE(mInstance->getParameters()->size(), 1);
    QCOMPARE(mInstance->getModuleParameters()->size(), 1);

    QCOMPARE(mInstance->getParameters()->value(0)->name(), QString("componentParameter"));
    QCOMPARE(mInstance->getParameters()->value(0)->getValue(), QString("2"));
    QCOMPARE(mInstance->getModuleParameters()->value(0)->name(), QString("moduleParameter"));
    QCOMPARE(mInstance->getModuleParameters()->value(0)->getValue(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testDesignParametersAreUtilized()
//-----------------------------------------------------------------------------
void tst_HDLParser::testDesignParametersAreUtilized()
{
    QSharedPointer<View> activeView(new View("rtl"));
    activeView->setComponentInstantiationRef("instance1");

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");    
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    library_.addComponent(senderComponent);
    QSharedPointer<ComponentInstance> senderInstance = addInstanceToDesign("sender", senderVLNV, activeView);

    addParameter("designParameter", "1337", "designParameterId", design_);

    addParameter("instanceParameter", "1", "parameterId", senderComponent);

    QSharedPointer<ConfigurableElementValue> parameterOverride(new ConfigurableElementValue());
    parameterOverride->setReferenceId("parameterId");
    parameterOverride->setConfigurableValue("designParameterId");

    senderInstance->getConfigurableElementValues()->append(parameterOverride);

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 1);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("sender");
    QCOMPARE(mInstance->getParameters()->size(), 1);

    QCOMPARE(mInstance->getParameters()->value(0)->name(), QString("instanceParameter"));
    QCOMPARE(mInstance->getParameters()->value(0)->getValue(), QString("1337"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testTopComponentParametersAreUtilized()
//-----------------------------------------------------------------------------
void tst_HDLParser::testTopComponentParametersAreUtilized()
{
    QSharedPointer<View> activeView(new View("rtl"));
    activeView->setComponentInstantiationRef("instance1");

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");    
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    library_.addComponent(senderComponent);
    QSharedPointer<ComponentInstance> senderInstance = addInstanceToDesign("sender", senderVLNV, activeView);

    addParameter("componentParameter", "55", "componentParameterId", topComponent_);

    QSharedPointer<ConfigurableElementValue> designParameterOverride(new ConfigurableElementValue());
    designParameterOverride->setReferenceId("designParameterId");
    designParameterOverride->setConfigurableValue("componentParameterId");

    topComponent_->getModel()->getDesignInstantiations()->first()->getDesignReference()->
        getConfigurableElementValues()->append(designParameterOverride);

    addParameter("designParameter", "1337", "designParameterId", design_);

    addParameter("instanceParameter", "1", "parameterId", senderComponent);

    QSharedPointer<ConfigurableElementValue> componentParameterOverride(new ConfigurableElementValue());
    componentParameterOverride->setReferenceId("parameterId");
    componentParameterOverride->setConfigurableValue("designParameterId");

    senderInstance->getConfigurableElementValues()->append(componentParameterOverride);

    QList<QSharedPointer<MetaDesign> > designs = MetaDesign::parseHierarchy
        (&library_, input_, topView_);

    QCOMPARE(designs.size(), 1);
    QSharedPointer<MetaDesign> design = designs.first();

    QCOMPARE(design->getInstances()->size(), 1);

    QSharedPointer<MetaInstance> mInstance = design->getInstances()->value("sender");
    QCOMPARE(mInstance->getParameters()->size(), 1);

    QCOMPARE(mInstance->getParameters()->value(0)->name(), QString("instanceParameter"));
    QCOMPARE(mInstance->getParameters()->value(0)->getValue(), QString("55"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testFlatComponent()
//-----------------------------------------------------------------------------
void tst_HDLParser::testFlatComponent()
{
    addPort("clk", 1, DirectionTypes::IN, topComponent_);
    addPort("rst_n", 1, DirectionTypes::IN, topComponent_);
    addPort("dataOut", 8, DirectionTypes::OUT, topComponent_);
    addPort("dataIn", 8, DirectionTypes::IN, topComponent_);
    addModuleParameter("dataWidth", "8", "firstParameter", topComponent_);
    addModuleParameter("freq", "100000", "secondParameter", topComponent_);

    MessagePasser messages;
    QSharedPointer<MetaComponent> flatComponent =
        QSharedPointer<MetaComponent>(new MetaComponent(&messages, topComponent_, topView_));
    flatComponent->formatComponent();

    QCOMPARE(flatComponent->getModuleParameters()->size(), 2);
    QCOMPARE(flatComponent->getModuleParameters()->value(0)->name(), QString("dataWidth"));
    QCOMPARE(flatComponent->getModuleParameters()->value(0)->getValue(), QString("8"));
    QCOMPARE(flatComponent->getModuleParameters()->value(1)->name(), QString("freq"));
    QCOMPARE(flatComponent->getModuleParameters()->value(1)->getValue(), QString("100000"));

    QCOMPARE(flatComponent->getPorts()->size(), 4);
    QCOMPARE(flatComponent->getPorts()->value("clk")->port_->getDirection(), DirectionTypes::IN);

    QCOMPARE(flatComponent->getPorts()->value("dataIn")->port_->getDirection(), DirectionTypes::IN);
    QCOMPARE(flatComponent->getPorts()->value("dataIn")->vectorBounds_.first, QString("7"));
    QCOMPARE(flatComponent->getPorts()->value("dataIn")->vectorBounds_.second, QString("0"));

    QCOMPARE(flatComponent->getPorts()->value("rst_n")->port_->getDirection(), DirectionTypes::IN);

    QCOMPARE(flatComponent->getPorts()->value("dataOut")->port_->getDirection(), DirectionTypes::OUT);
    QCOMPARE(flatComponent->getPorts()->value("dataOut")->vectorBounds_.first, QString("7"));
    QCOMPARE(flatComponent->getPorts()->value("dataOut")->vectorBounds_.second, QString("0"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testFlatComponentExpressions()
//-----------------------------------------------------------------------------
void tst_HDLParser::testFlatComponentExpressions()
{
    addModuleParameter("module", "10", "firstParameter", topComponent_);
    addModuleParameter("freq", "firstParameter*3.14", "secondParameter", topComponent_);

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port("clk", DirectionTypes::IN));
    port->setLeftBound("secondParameter*2");
    port->setRightBound("2+5");
    topComponent_->getPorts()->append(port);

    MessagePasser messages;
    QSharedPointer<MetaComponent> flatComponent =
        QSharedPointer<MetaComponent>(new MetaComponent(&messages, topComponent_, topView_));
    flatComponent->formatComponent();

    QCOMPARE(flatComponent->getModuleParameters()->size(), 2);
    QCOMPARE(flatComponent->getModuleParameters()->value(0)->name(), QString("module"));
    QCOMPARE(flatComponent->getModuleParameters()->value(0)->getValue(), QString("10"));
    QCOMPARE(flatComponent->getModuleParameters()->value(1)->name(), QString("freq"));
    QCOMPARE(flatComponent->getModuleParameters()->value(1)->getValue(), QString("module*3.14"));

    QCOMPARE(flatComponent->getPorts()->size(), 1);
    QCOMPARE(flatComponent->getPorts()->value("clk")->port_->getDirection(), DirectionTypes::IN);
    QCOMPARE(flatComponent->getPorts()->value("clk")->vectorBounds_.first, QString("freq*2"));
    QCOMPARE(flatComponent->getPorts()->value("clk")->vectorBounds_.second, QString("2+5"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testParameterSorting()
//-----------------------------------------------------------------------------
void tst_HDLParser::testParameterSorting()
{
    QSharedPointer<ModuleParameter> moduleParameterFirst(new ModuleParameter());
    moduleParameterFirst->setName("moduleParameterFirst");
    moduleParameterFirst->setValue("1");
    moduleParameterFirst->setValueId("firstParameter");
    moduleParameterFirst->setValueResolve("user");

    QSharedPointer<ModuleParameter> moduleParameterSecond(new ModuleParameter());
    moduleParameterSecond->setName("moduleParameterSecond");
    moduleParameterSecond->setValue("firstParameter");
    moduleParameterSecond->setValueId("secondParameter");
    moduleParameterSecond->setValueResolve("user");

    QSharedPointer<ModuleParameter> moduleParameterThird(new ModuleParameter());
    moduleParameterThird->setName("moduleParameterThird");
    moduleParameterThird->setValue("secondParameter");
    moduleParameterThird->setValueId("thirdParameter");
    moduleParameterThird->setValueResolve("user");

    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters(new  QList<QSharedPointer<Parameter> >);
    parameters->append(moduleParameterThird);
    parameters->append(moduleParameterFirst);
    parameters->append(moduleParameterSecond);

    MetaComponent::sortParameters(parameters);

    QCOMPARE(parameters->size(), 3);

    QCOMPARE(parameters->at(0)->name(), QString("moduleParameterFirst"));
    QCOMPARE(parameters->at(0)->getValue(), QString("1"));
    QCOMPARE(parameters->at(1)->name(), QString("moduleParameterSecond"));
    QCOMPARE(parameters->at(1)->getValue(), QString("firstParameter"));
    QCOMPARE(parameters->at(2)->name(), QString("moduleParameterThird"));
    QCOMPARE(parameters->at(2)->getValue(), QString("secondParameter"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testParameterSorting2()
//-----------------------------------------------------------------------------
void tst_HDLParser::testParameterSorting2()
{
    QSharedPointer<ModuleParameter> moduleParameterFirst(new ModuleParameter());
    moduleParameterFirst->setName("moduleParameterFirst");
    moduleParameterFirst->setValue("secondParameter + fourthParameter");
    moduleParameterFirst->setValueId("firstParameter");
    moduleParameterFirst->setValueResolve("user");

    QSharedPointer<ModuleParameter> moduleParameterSecond(new ModuleParameter());
    moduleParameterSecond->setName("moduleParameterSecond");
    moduleParameterSecond->setValue("1");
    moduleParameterSecond->setValueId("secondParameter");
    moduleParameterSecond->setValueResolve("user");

    QSharedPointer<ModuleParameter> moduleParameterThird(new ModuleParameter());
    moduleParameterThird->setName("moduleParameterThird");
    moduleParameterThird->setValue("firstParameter");
    moduleParameterThird->setValueId("thirdParameter");
    moduleParameterThird->setValueResolve("user");

    QSharedPointer<ModuleParameter> moduleParameterFourth(new ModuleParameter());
    moduleParameterFourth->setName("moduleParameterFourth");
    moduleParameterFourth->setValue("4");
    moduleParameterFourth->setValueId("fourthParameter");
    moduleParameterFourth->setValueResolve("user");

    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters(new  QList<QSharedPointer<Parameter> >);
    parameters->append(moduleParameterFirst);
    parameters->append(moduleParameterSecond);
    parameters->append(moduleParameterThird);
    parameters->append(moduleParameterFourth);

    MetaComponent::sortParameters(parameters);

    QCOMPARE(parameters->size(), 4);

    QCOMPARE(parameters->at(0)->name(), QString("moduleParameterSecond"));
    QCOMPARE(parameters->at(0)->getValue(), QString("1"));
    QCOMPARE(parameters->at(1)->name(), QString("moduleParameterFourth"));
    QCOMPARE(parameters->at(1)->getValue(), QString("4"));
    QCOMPARE(parameters->at(2)->name(), QString("moduleParameterFirst"));
    QCOMPARE(parameters->at(2)->getValue(), QString("secondParameter + fourthParameter"));
    QCOMPARE(parameters->at(3)->name(), QString("moduleParameterThird"));
    QCOMPARE(parameters->at(3)->getValue(), QString("firstParameter"));
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testParameterSorting3()
//-----------------------------------------------------------------------------
void tst_HDLParser::testParameterSorting3()
{
    QSharedPointer<ModuleParameter> moduleParameterFirst(new ModuleParameter());
    moduleParameterFirst->setName("moduleParameterFirst");
    moduleParameterFirst->setValue("1");
    moduleParameterFirst->setValueId("firstParameter");
    moduleParameterFirst->setValueResolve("user");

    QSharedPointer<ModuleParameter> moduleParameterSecond(new ModuleParameter());
    moduleParameterSecond->setName("moduleParameterSecond");
    moduleParameterSecond->setValue("55");
    moduleParameterSecond->setValueId("secondParameter");
    moduleParameterSecond->setValueResolve("user");

    QSharedPointer<ModuleParameter> moduleParameterThird(new ModuleParameter());
    moduleParameterThird->setName("moduleParameterThird");
    moduleParameterThird->setValue("fifthParameter + secondParameter");
    moduleParameterThird->setValueId("thirdParameter");
    moduleParameterThird->setValueResolve("user");

    QSharedPointer<ModuleParameter> moduleParameterFourth(new ModuleParameter());
    moduleParameterFourth->setName("moduleParameterFourth");
    moduleParameterFourth->setValue("12");
    moduleParameterFourth->setValueId("fourthParameter");
    moduleParameterFourth->setValueResolve("user");

    QSharedPointer<ModuleParameter> moduleParameterFifth(new ModuleParameter());
    moduleParameterFifth->setName("moduleParameterFifth");
    moduleParameterFifth->setValue("fourthParameter - firstParameter");
    moduleParameterFifth->setValueId("fifthParameter");
    moduleParameterFifth->setValueResolve("user");

    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters(new  QList<QSharedPointer<Parameter> >);
    parameters->append(moduleParameterFirst);
    parameters->append(moduleParameterSecond);
    parameters->append(moduleParameterThird);
    parameters->append(moduleParameterFourth);
    parameters->append(moduleParameterFifth);

    MetaComponent::sortParameters(parameters);

    QCOMPARE(parameters->size(), 5);

    QCOMPARE(parameters->at(0)->name(), QString("moduleParameterSecond"));
    QCOMPARE(parameters->at(0)->getValue(), QString("55"));
    QCOMPARE(parameters->at(1)->name(), QString("moduleParameterFirst"));
    QCOMPARE(parameters->at(1)->getValue(), QString("1"));
    QCOMPARE(parameters->at(2)->name(), QString("moduleParameterFourth"));
    QCOMPARE(parameters->at(2)->getValue(), QString("12"));
    QCOMPARE(parameters->at(3)->name(), QString("moduleParameterFifth"));
    QCOMPARE(parameters->at(3)->getValue(), QString("fourthParameter - firstParameter"));
    QCOMPARE(parameters->at(4)->name(), QString("moduleParameterThird"));
    QCOMPARE(parameters->at(4)->getValue(), QString("fifthParameter + secondParameter"));
}

QTEST_APPLESS_MAIN(tst_HDLParser)

#include "tst_HDLParser.moc"

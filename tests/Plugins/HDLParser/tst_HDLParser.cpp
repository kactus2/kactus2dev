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
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <tests/MockObjects/LibraryMock.h>

#include <Plugins/common/HDLParser/HDLComponentParser.h>
#include <Plugins/common/HDLParser/HDLDesignParser.h>

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

    // Test cases:
    void testTopLevelComponent();
    void testTopLevelComponentExpressions();

    void testHierarchicalConnections();
    void testHierarchicalConnectionsWithExpressions();
    void testSlicedHierarchicalConnection();

    void testMasterToSlaveInterconnection();
    void testEmptyBounds();
    void testMasterToSlaveInterconnectionWithExpressions();
    void testMasterToMultipleSlavesInterconnections();
    void testInterconnectionToVaryingSizeLogicalMaps();
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
    void testAdHocConnectionBetweenMultipleComponentInstances();

    void testInstanceParametersAreCulled();
	void testTopComponentParametersAreUtilized();
	void testInstanceComponentParametersAreUtilized();

	void testParameterPropagationFromTop();
    void testParameterPropagationFromTop2();
    void testParameterPropagationFromTopWire();
    void testMultiLevelHierachy();

	void testParameterSorting();
    void testParameterSorting2();
    void testParameterSorting3();

private:

    void addPort( QString const& portName, int portSize, DirectionTypes::Direction direction, 
        QSharedPointer<Component> component);

    void addModuleParameter( QString const& name, QString const& value );

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
   
    //! The top level component for which the generator is run.
    QSharedPointer<Component> topComponent_;
    
    //! The design for which the generator is run.
    QSharedPointer<Design> design_;

	//! The design configuration for which the generator is run.
	QSharedPointer<DesignConfiguration> designConf_;

	QSharedPointer<View> topView_;

    //! The test mock for library interface.
    LibraryMock library_;
};

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::tst_HDLParser()
//-----------------------------------------------------------------------------
tst_HDLParser::tst_HDLParser(): topComponent_(), design_(), designConf_(), library_(this)
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

    VLNV designVlnv(VLNV::DESIGN, "Test", "TestLibrary", "TestDesign", "1.0");
	design_ = QSharedPointer<Design>(new Design(designVlnv));

	VLNV designConfVlnv(VLNV::DESIGNCONFIGURATION, "Test", "TestLibrary", "TestDesignConfiguration", "1.0");
	designConf_ = QSharedPointer<DesignConfiguration>(new DesignConfiguration(designConfVlnv));
	designConf_->setDesignRef(designVlnv);

	topView_ = QSharedPointer<View>(new View("topView"));

	topComponent_->getViews()->append(topView_);

    library_.clear();
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
    addModuleParameter("dataWidth", "8");
    addModuleParameter("freq", "100000");

    QSharedPointer<HDLComponentParser> componentParser =
    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));
    componentParser->parseComponent(topView_);
    QSharedPointer<GenerationComponent> parsed = componentParser->getParsedComponent();

    QCOMPARE( parsed->formattedParameters.size(), 2 );
    QCOMPARE( parsed->formattedParameters.at(0)->name(), QString("dataWidth") );
    QCOMPARE( parsed->formattedParameters.at(0)->getValue(), QString("8") );
    QCOMPARE( parsed->formattedParameters.at(1)->name(), QString("freq") );
    QCOMPARE( parsed->formattedParameters.at(1)->getValue(), QString("100000") );

    QCOMPARE( parsed->ports.size(), 4 );
    QCOMPARE( parsed->ports["clk"]->port->getDirection(), DirectionTypes::IN );

    QCOMPARE( parsed->ports["dataIn"]->port->getDirection(), DirectionTypes::IN );
    QCOMPARE( parsed->ports["dataIn"]->vectorBounds.first, QString("7") );
    QCOMPARE( parsed->ports["dataIn"]->vectorBounds.second, QString("0") );

    QCOMPARE( parsed->ports["rst_n"]->port->getDirection(), DirectionTypes::IN );

    QCOMPARE( parsed->ports["dataOut"]->port->getDirection(), DirectionTypes::OUT );
    QCOMPARE( parsed->ports["dataOut"]->vectorBounds.first, QString("7") );
    QCOMPARE( parsed->ports["dataOut"]->vectorBounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testTopLevelComponentExpressions()
//-----------------------------------------------------------------------------
void tst_HDLParser::testTopLevelComponentExpressions()
{
    addModuleParameter("module", "10");
    topComponent_->getComponentInstantiations()->first()->getModuleParameters()->first()->setValueId("MODEL-ID");

    QSharedPointer<Port> port = QSharedPointer<Port>(new Port("clk", DirectionTypes::IN));
    port->setLeftBound("MODEL-ID*2");
    port->setRightBound("2+5");
    topComponent_->getPorts()->append(port);

    QSharedPointer<HDLComponentParser> componentParser =
    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));
    componentParser->parseComponent(topView_);
    QSharedPointer<GenerationComponent> parsed = componentParser->getParsedComponent();

    QCOMPARE( parsed->formattedParameters.size(), 1 );
    QCOMPARE( parsed->formattedParameters.at(0)->name(), QString("module") );
    QCOMPARE( parsed->formattedParameters.at(0)->getValue(), QString("10") );

    QCOMPARE( parsed->ports.size(), 1 );
    QCOMPARE( parsed->ports["clk"]->port->getDirection(), DirectionTypes::IN );
    QCOMPARE( parsed->ports["clk"]->vectorBounds.first, QString("module*2") );
    QCOMPARE( parsed->ports["clk"]->vectorBounds.second, QString("2+5") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addPort()
//-----------------------------------------------------------------------------
void tst_HDLParser::addPort(QString const& portName, int portSize, 
    DirectionTypes::Direction direction, QSharedPointer<Component> component)
{
    QSharedPointer<Port> port = QSharedPointer<Port>(new Port(portName, direction));
    port->setPortSize(portSize);
    component->getPorts()->append(port);
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addModuleParameter()
//-----------------------------------------------------------------------------
void tst_HDLParser::addModuleParameter( QString const& name, QString const& value )
{
    QSharedPointer<ModuleParameter> parameter = QSharedPointer<ModuleParameter>(new ModuleParameter());
    parameter->setName(name);
    parameter->setValue(value);

    if (topComponent_->getComponentInstantiations()->isEmpty())
    {
        QSharedPointer<ComponentInstantiation> newInstantiation (new ComponentInstantiation("testInstantiation"));
        newInstantiation->getModuleParameters()->append(parameter);
        topComponent_->getComponentInstantiations()->append(newInstantiation);

        topView_->setComponentInstantiationRef(newInstantiation->name());
    }
    else
    {
        topComponent_->getComponentInstantiations()->first()->getModuleParameters()->append(parameter);
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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);
    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi = design->instances_["instance1"];

    QCOMPARE( gi->componentInstance_->getInstanceName(), QString("instance1") );
    QCOMPARE( gi->portAssignments_.size(), 4 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi->portAssignments_["clk"];
    QCOMPARE( gpa->topPort, QString("top_clk") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi->portAssignments_["data_in"];
    QCOMPARE( gpa->topPort, QString("data_to_instance") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi->portAssignments_["enable"];
    QCOMPARE( gpa->topPort, QString("enable_to_instance") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi->portAssignments_["full"];
    QCOMPARE( gpa->topPort, QString("full_from_instance") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
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

    addInterfaceToComponent("clk_if", topComponent_);

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

    addInterfaceToComponent("instanceInterface", instanceComponent);
    mapPortToInterface("instance_clk", "CLK", "instanceInterface", instanceComponent);

	library_.addComponent(instanceComponent);

	QSharedPointer<View> activeView(new View("rtl"));
	activeView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	instanceComponent->getComponentInstantiations()->append(instantiation);
	instanceComponent->getViews()->append(activeView);

    addInstanceToDesign("instance1", instanceVlnv, activeView);

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi = design->instances_["instance1"];

    QCOMPARE( gi->componentInstance_->getInstanceName(), QString("instance1") );
    QCOMPARE( gi->parameters.size(), 1 );
    QCOMPARE( gi->portAssignments_.size(), 1 );

    QCOMPARE( gi->parameters.at(0)->name(), QString("componentParameter") );
    QCOMPARE( gi->parameters.at(0)->getValue(), QString("1") );

    QSharedPointer<GenerationPortAssignMent> gpa = gi->portAssignments_["instance_clk"];
    QCOMPARE( gpa->topPort, QString("top_clk") );
    QCOMPARE( gpa->bounds.first, QString("2") );
    QCOMPARE( gpa->bounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testSlicedHierarchicalConnection()
//-----------------------------------------------------------------------------
void tst_HDLParser::testSlicedHierarchicalConnection()
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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi = design->instances_["instance1"];

    QCOMPARE( gi->portAssignments_.size(), 4 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi->portAssignments_["data_in"];
    QCOMPARE( gpa->topPort, QString("data_to_instance") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi->portAssignments_["enable"];
    QCOMPARE( gpa->topPort, QString("enable_to_instance") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi->portAssignments_["full"];
    QCOMPARE( gpa->topPort, QString("full_from_instance") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addInterfaceToComponent()
//-----------------------------------------------------------------------------
void tst_HDLParser::addInterfaceToComponent(QString const& interfaceName,
    QSharedPointer<Component> component)
{
    QSharedPointer<BusInterface> busInterface(new BusInterface());
    busInterface->setName(interfaceName);
    component->getBusInterfaces()->append(busInterface);
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addPortMap()
//-----------------------------------------------------------------------------
void tst_HDLParser::mapPortToInterface(QString const& portName, QString const& logicalName, 
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
            library_.addComponent(testAbstractionDefinition);

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
// Function: tst_HDLParser::mapPortToInterface()
//-----------------------------------------------------------------------------
void tst_HDLParser::mapPortToInterface(QString const& portName, int left, int right, QString const& logicalName, 
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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->interConnections_.size(), 1 );
    QCOMPARE( design->interConnections_.at(0)->wires_.size(), 2 );

    QSharedPointer<GenerationWire> gw0 = design->interConnections_.at(0)->wires_.first();
    QSharedPointer<GenerationWire> gw1 = design->interConnections_.at(0)->wires_.last();

    QCOMPARE( gw0->bounds.first, QString("7") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( gw1->bounds.first, QString("0") );
    QCOMPARE( gw1->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 2 );

    QSharedPointer<GenerationInstance> gi1 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi0 = design->instances_["receiver"];

    QCOMPARE( gi0->portAssignments_.size(), 2 );
    QCOMPARE( gi1->portAssignments_.size(), 2 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi0->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi1->portAssignments_["enable_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
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

    addInterfaceToComponent("data_bus", senderComponent);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(General::MASTER);

    mapPortToInterface("data_out", "DATA", "data_bus", senderComponent);
    QSharedPointer<PortMap> dataMap = senderComponent->getBusInterface("data_bus")->getPortMaps()->first();
    QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
    logPort->range_ = QSharedPointer<Range>( new Range("","") );

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

    addInterfaceToComponent("data_bus", receiverComponent);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(General::SLAVE);
    mapPortToInterface("data_in", "DATA", "data_bus", receiverComponent);

    library_.addComponent(receiverComponent);
    addInstanceToDesign("receiver", receiverVLNV, receiverView);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    QSharedPointer<HDLComponentParser> componentParser =

        QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
        QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
    designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->interConnections_.size(), 1 );
    QCOMPARE( design->interConnections_.at(0)->wires_.size(), 1 );

    QSharedPointer<GenerationWire> gw0 = design->interConnections_.at(0)->wires_.first();

    QCOMPARE( gw0->bounds.first, QString("0") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 2 );

    QSharedPointer<GenerationInstance> gi1 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi0 = design->instances_["receiver"];

    QCOMPARE( gi0->portAssignments_.size(), 1 );
    QCOMPARE( gi1->portAssignments_.size(), 1 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
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

    addInterfaceToComponent("data_bus", senderComponent);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(General::MASTER);

    mapPortToInterface("data_out", "DATA", "data_bus", senderComponent);
    QSharedPointer<PortMap> dataMap = senderComponent->getBusInterface("data_bus")->getPortMaps()->first();
    QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
    logPort->range_ = QSharedPointer<Range>( new Range("7*2","0") );

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

    addInterfaceToComponent("data_bus", receiverComponent);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(General::SLAVE);
    mapPortToInterface("data_in", "DATA", "data_bus", receiverComponent);

    library_.addComponent(receiverComponent);
    addInstanceToDesign("receiver", receiverVLNV, receiverView);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->interConnections_.size(), 1 );
    QCOMPARE( design->interConnections_.at(0)->wires_.size(), 1 );

    QSharedPointer<GenerationWire> gw0 = design->interConnections_.at(0)->wires_.first();

    QCOMPARE( gw0->bounds.first, QString("14") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 2 );

    QSharedPointer<GenerationInstance> gi1 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi0 = design->instances_["receiver"];

    QCOMPARE( gi0->portAssignments_.size(), 1 );
    QCOMPARE( gi1->portAssignments_.size(), 1 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("8") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("14") );
    QCOMPARE( gpa->bounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addSenderComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<View> tst_HDLParser::addSenderComponentToLibrary(VLNV senderVLNV, General::InterfaceMode mode)
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
// Function: tst_HDLParser::addReceiverComponentToLibrary()
//-----------------------------------------------------------------------------
QSharedPointer<View> tst_HDLParser::addReceiverComponentToLibrary(VLNV receiverVLNV, General::InterfaceMode mode)
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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->interConnections_.size(), 1 );
    QCOMPARE( design->interConnections_.at(0)->wires_.size(), 2 );

    QSharedPointer<GenerationWire> gw0 = design->interConnections_.at(0)->wires_.last();
    QSharedPointer<GenerationWire> gw1 = design->interConnections_.at(0)->wires_.first();

    QCOMPARE( gw0->bounds.first, QString("0") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( gw1->bounds.first, QString("7") );
    QCOMPARE( gw1->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 3 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi1 = design->instances_["receiver1"];
    QSharedPointer<GenerationInstance> gi2 = design->instances_["receiver2"];

    QCOMPARE( gi0->portAssignments_.size(), 2 );
    QCOMPARE( gi1->portAssignments_.size(), 2 );
    QCOMPARE( gi2->portAssignments_.size(), 2 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver1_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi0->portAssignments_["enable_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver1_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver1_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi1->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver1_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi2->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver1_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi2->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver1_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
        QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
    designParser->parseDesign(componentParser->getParsedComponent(), topView_);
    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->interConnections_.size(), 1 );
    QCOMPARE( design->interConnections_.at(0)->wires_.size(), 2 );

    QSharedPointer<GenerationWire> gw0 = design->interConnections_.at(0)->wires_.first();
    QCOMPARE( gw0->bounds.first, QString("15") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QSharedPointer<GenerationWire> gw1 = design->interConnections_.at(0)->wires_.last();
    QCOMPARE( gw1->bounds.first, QString("0") );
    QCOMPARE( gw1->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 4 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi1 = design->instances_["oneBitReceiver"];
    QSharedPointer<GenerationInstance> gi2 = design->instances_["fourBitReceiver"];
    QSharedPointer<GenerationInstance> gi3 = design->instances_["sixteenBitReceiver"];

    QCOMPARE( gi0->portAssignments_.size(), 2 );
    QCOMPARE( gi1->portAssignments_.size(), 2 );
    QCOMPARE( gi2->portAssignments_.size(), 2 );
    QCOMPARE( gi3->portAssignments_.size(), 2 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, gw0->name );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi0->portAssignments_["enable_out"];
    QCOMPARE( gpa->wire->name, gw1->name );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, gw0->name );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi1->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, gw1->name );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi2->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, gw0->name );
    QCOMPARE( gpa->bounds.first, QString("3") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi2->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, gw1->name );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi3->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, gw0->name );
    QCOMPARE( gpa->bounds.first, QString("15") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi3->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, gw1->name );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
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
// Function: tst_HDLParser::testSlicedInterconnection()
//-----------------------------------------------------------------------------
void tst_HDLParser::testSlicedInterconnection()
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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->interConnections_.size(), 1 );
    QCOMPARE( design->interConnections_.at(0)->wires_.size(), 2 );

    QSharedPointer<GenerationWire> gw0 = design->interConnections_.at(0)->wires_.last();

    QCOMPARE( gw0->bounds.first, QString("1") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 2 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi1 = design->instances_["receiver"];

    QCOMPARE( gi0->portAssignments_.size(), 2 );
    QCOMPARE( gi1->portAssignments_.size(), 2 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["enable_out_high"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("1") );
    QCOMPARE( gpa->bounds.second, QString("1") );
    gpa = gi0->portAssignments_["enable_out_low"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testMasterInterconnectionToMirroredMaster()
//-----------------------------------------------------------------------------
void tst_HDLParser::testMasterInterconnectionToMirroredMaster()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> sendView = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV, sendView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "BusComponent", "1.0");
    QSharedPointer<View> recvView = addReceiverComponentToLibrary(receiverVLNV, General::MIRROREDMASTER);
    addInstanceToDesign("receiver", receiverVLNV, recvView);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->interConnections_.size(), 1 );
    QCOMPARE( design->interConnections_.at(0)->wires_.size(), 2 );

    QSharedPointer<GenerationWire> gw0 = design->interConnections_.at(0)->wires_.first();
    QSharedPointer<GenerationWire> gw1 = design->interConnections_.at(0)->wires_.last();

    QCOMPARE( gw0->bounds.first, QString("7") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( gw1->bounds.first, QString("0") );
    QCOMPARE( gw1->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 2 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi1 = design->instances_["receiver"];

    QCOMPARE( gi0->portAssignments_.size(), 2 );
    QCOMPARE( gi1->portAssignments_.size(), 2 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi0->portAssignments_["enable_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi1->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testMirroredSlaveInterconnectionToSlaves()
//-----------------------------------------------------------------------------
void tst_HDLParser::testMirroredSlaveInterconnectionToSlaves()
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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->interConnections_.size(), 1 );
    QCOMPARE( design->interConnections_.at(0)->wires_.size(), 2 );

    QSharedPointer<GenerationWire> gw0 = design->interConnections_.at(0)->wires_.first();
    QSharedPointer<GenerationWire> gw1 = design->interConnections_.at(0)->wires_.last();

    QCOMPARE( gw0->bounds.first, QString("7") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( gw1->bounds.first, QString("0") );
    QCOMPARE( gw1->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 3 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi1 = design->instances_["bus1"];

    QCOMPARE( gi0->portAssignments_.size(), 2 );
    QCOMPARE( gi1->portAssignments_.size(), 2 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_bus1_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi0->portAssignments_["enable_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_bus1_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_bus1_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi1->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_bus1_ENABLE") );
    QCOMPARE( gpa->bounds.first, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testPortMapsWithoutBoundsInInterconnection()
//-----------------------------------------------------------------------------
void tst_HDLParser::testPortMapsWithoutBoundsInInterconnection()
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
    library_.addComponent(testAbstractionDefinition);

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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->interConnections_.size(), 1 );
    QCOMPARE( design->interConnections_.at(0)->wires_.size(), 1 );

    QSharedPointer<GenerationWire> gw0 = design->interConnections_.at(0)->wires_.first();

    QCOMPARE( gw0->bounds.first, QString("7") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 2 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi1 = design->instances_["receiver"];

    QCOMPARE( gi0->portAssignments_.size(), 1 );
    QCOMPARE( gi1->portAssignments_.size(), 1 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testAdhocConnectionBetweenTwoComponentInstances()
//-----------------------------------------------------------------------------
void tst_HDLParser::testAdhocConnectionBetweenComponentInstances()
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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->adHocs_.size(), 2 );

    QSharedPointer<GenerationWire> gw0 = design->adHocs_.at(0)->wire;
    QSharedPointer<GenerationWire> gw1 = design->adHocs_.at(1)->wire;

    QCOMPARE( gw0->bounds.first, QString("0") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( gw1->bounds.first, QString("7") );
    QCOMPARE( gw1->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 3 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];;
    QSharedPointer<GenerationInstance> gi1 = design->instances_["receiver1"];
    QSharedPointer<GenerationInstance> gi2 = design->instances_["receiver2"];

    QCOMPARE( gi0->portAssignments_.size(), 2 );
    QCOMPARE( gi1->portAssignments_.size(), 2 );
    QCOMPARE( gi2->portAssignments_.size(), 1 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, QString("dataAdHoc") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi0->portAssignments_["enable_out"];
    QCOMPARE( gpa->wire->name, QString("enableAdHoc") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("dataAdHoc") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
    gpa = gi1->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("enableAdHoc") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi2->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("enableAdHoc") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["tieOffer"];

    QCOMPARE( gi0->portAssignments_.size(), 9 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["defaultTieOff"];
    QCOMPARE( gpa->tieOff, QString("20") );
    gpa = gi0->portAssignments_["expressionTieOff"];
    QCOMPARE( gpa->tieOff, QString("expID - 4") );
    gpa = gi0->portAssignments_["n/aTieOff"];
    QCOMPARE( gpa->tieOff, QString("abc") );
    gpa = gi0->portAssignments_["numberedTieOff"];
    QCOMPARE( gpa->tieOff, QString("12") );
    gpa = gi0->portAssignments_["oneTieOff"];
    QCOMPARE( gpa->tieOff, QString("1") );
    gpa = gi0->portAssignments_["openTieOff"];
    QCOMPARE( gpa->tieOff, QString("") );
    gpa = gi0->portAssignments_["zeroTieOff"];
    QCOMPARE( gpa->tieOff, QString("0") );
    gpa = gi0->portAssignments_["tieOffOut"];
    QCOMPARE( gpa->tieOff, QString("") );
    gpa = gi0->portAssignments_["tieOffInOut"];
    QCOMPARE( gpa->tieOff, QString("1") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testMultipleAdhocConnectionsBetweenComponentInstances()
//-----------------------------------------------------------------------------
void tst_HDLParser::testMultipleAdhocConnectionsBetweenComponentInstances()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> sendView = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV, sendView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
   QSharedPointer<View> recvView =  addReceiverComponentToLibrary(receiverVLNV, General::SLAVE);
    addInstanceToDesign("receiver1", receiverVLNV, recvView);
    addInstanceToDesign("receiver2", receiverVLNV, recvView);

    addAdhocConnection("sender_enable_to_receiver1_enable", "sender", "enable_out", "receiver1", "enable_in");
    addAdhocConnection("sender_enable_to_receiver2_enable", "sender", "enable_out", "receiver2", "enable_in");

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->adHocs_.size(), 1 );

    QSharedPointer<GenerationWire> gw0 = design->adHocs_.at(0)->wire;

    QCOMPARE( gw0->bounds.first, QString("0") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 3 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi1 = design->instances_["receiver1"];
    QSharedPointer<GenerationInstance> gi2 = design->instances_["receiver2"];

    QCOMPARE( gi0->portAssignments_.size(), 1 );
    QCOMPARE( gi1->portAssignments_.size(), 1 );
    QCOMPARE( gi2->portAssignments_.size(), 1 );

    QSharedPointer<GenerationPortAssignMent> gpa;
    gpa = gi0->portAssignments_["enable_out"];
    QCOMPARE( gpa->wire->name, QString("sender_enable_to_receiver1_enable") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("sender_enable_to_receiver1_enable") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi2->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("sender_enable_to_receiver1_enable") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testAdHocConnectionBetweenComponentInstancesWithExpressions()
//-----------------------------------------------------------------------------
void tst_HDLParser::testAdHocConnectionBetweenComponentInstancesWithExpressions()
{
    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));

    QSharedPointer<Port> senderPort = QSharedPointer<Port>(new Port("enable_out", DirectionTypes::OUT));
    senderPort->setLeftBound("(10-9)*4");
    senderPort->setRightBound("2-2");
	senderComponent->getPorts()->append(senderPort);

	QSharedPointer<View> sendView(new View("rtl"));
	sendView->setComponentInstantiationRef("instance1");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	senderComponent->getComponentInstantiations()->append(instantiation);
	senderComponent->getViews()->append(sendView);

    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, sendView);

    VLNV receiverVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestReceiver", "1.0");
    QSharedPointer<Component> receiverComponent(new Component(receiverVLNV));

    QSharedPointer<Port> receiverPort = QSharedPointer<Port>(new Port("enable_in", DirectionTypes::IN));
    receiverPort->setLeftBound("2+2");
    receiverPort->setRightBound("0");
    receiverComponent->getPorts()->append(receiverPort);

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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->adHocs_.size(), 1 );

    QSharedPointer<GenerationWire> gw0 = design->adHocs_.at(0)->wire;

    QCOMPARE( gw0->bounds.first, QString("4") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 3 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi1 = design->instances_["receiver1"];
    QSharedPointer<GenerationInstance> gi2 = design->instances_["receiver2"];

    QCOMPARE( gi0->portAssignments_.size(), 1 );
    QCOMPARE( gi1->portAssignments_.size(), 1 );
    QCOMPARE( gi2->portAssignments_.size(), 1 );

    QSharedPointer<GenerationPortAssignMent> gpa;
    gpa = gi0->portAssignments_["enable_out"];
    QCOMPARE( gpa->wire->name, QString("enableAdHoc") );
    QCOMPARE( gpa->bounds.first, QString("4") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("enableAdHoc") );
    QCOMPARE( gpa->bounds.first, QString("4") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi2->portAssignments_["enable_in"];
    QCOMPARE( gpa->wire->name, QString("enableAdHoc") );
    QCOMPARE( gpa->bounds.first, QString("4") );
    QCOMPARE( gpa->bounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addAdhocConnection()
//-----------------------------------------------------------------------------
void tst_HDLParser::addAdhocConnection(QString const& connectionName, 
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
    addPort("enable_from_sender", 1, DirectionTypes::OUT, topComponent_);
    addPort("data_from_sender", 8, DirectionTypes::OUT, topComponent_);

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");
    QSharedPointer<View> activeView = addSenderComponentToLibrary(senderVLNV, General::MASTER);
    addInstanceToDesign("sender", senderVLNV, activeView);

    addHierAdhocConnection("enable_from_sender", "sender", "enable_out");
    addHierAdhocConnection("data_from_sender", "sender", "data_out");

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];

    QCOMPARE( gi0->portAssignments_.size(), 2 );

    QSharedPointer<GenerationPortAssignMent> gpa = gi0->portAssignments_["data_out"];
    QCOMPARE( gpa->topPort, QString("data_from_sender") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi0->portAssignments_["enable_out"];
    QCOMPARE( gpa->topPort, QString("enable_from_sender") );
    QCOMPARE( gpa->bounds.first, QString("0") );
    QCOMPARE( gpa->bounds.second, QString("0") );
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
// Function: tst_HDLParser::testHierarchicalAdHocTieOffValues()
//-----------------------------------------------------------------------------
void tst_HDLParser::testHierarchicalAdHocTieOffValues()
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

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 0 );

    QCOMPARE( design->portTiedValues_.size(), 9 );

    QCOMPARE( design->portTiedValues_["defaultTieOff"], QString("20") );
    QCOMPARE( design->portTiedValues_["expressionTieOff"], QString("expID - 4") );
    QCOMPARE( design->portTiedValues_["inOutTieOff"], QString("1") );
    QCOMPARE( design->portTiedValues_["n/aTieOff"], QString("abc") );
    QCOMPARE( design->portTiedValues_["numberedTieOff"], QString("12") );
    QCOMPARE( design->portTiedValues_["oneTieOff"], QString("1") );
    QCOMPARE( design->portTiedValues_["zeroTieOff"], QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::addTieOffConnectionToTopComponentPort()
//-----------------------------------------------------------------------------
void tst_HDLParser::addTieOffConnectionToTopComponentPort(QString const& tieOffValue,
    QString const& portName, QString const& connectionName)
{
    QSharedPointer<PortReference> newPortReference (new PortReference(portName));

    QSharedPointer<AdHocConnection> newConnection(new AdHocConnection(connectionName));
    newConnection->setTiedValue(tieOffValue);
    newConnection->getExternalPortReferences()->append(newPortReference);

    design_->getAdHocConnections()->append(newConnection);
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

    addInterfaceToComponent("data_bus", senderComponent);
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

    addInterfaceToComponent("data_bus", receiverComponent);
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

    QSharedPointer<AdHocConnection> multiConnection(new AdHocConnection("data_from_sender"));

    QSharedPointer<PortReference> startReference(new PortReference("data_out", "sender"));
    QSharedPointer<PortReference> endReference1(new PortReference("data_in", "receiver1"));
    QSharedPointer<PortReference> endReference2(new PortReference("data_in", "receiver2"));

    multiConnection->getInternalPortReferences()->append(startReference);
    multiConnection->getInternalPortReferences()->append(endReference1);
    multiConnection->getInternalPortReferences()->append(endReference2);

    design_->getAdHocConnections()->append(multiConnection);

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->adHocs_.size(), 1 );

    QCOMPARE( design->instances_.size(), 3 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi1 = design->instances_["receiver1"];
    QSharedPointer<GenerationInstance> gi2 = design->instances_["receiver2"];

    QCOMPARE( gi0->portAssignments_.size(), 1 );
    QCOMPARE( gi1->portAssignments_.size(), 1 );
    QCOMPARE( gi2->portAssignments_.size(), 1 );

    QSharedPointer<GenerationPortAssignMent> gpa;
    gpa = gi0->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, QString("data_from_sender") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi1->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("data_from_sender") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    gpa = gi2->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("data_from_sender") );
    QCOMPARE( gpa->bounds.first, QString("7") );
    QCOMPARE( gpa->bounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testInstanceModuleParametersAreWritten()
//-----------------------------------------------------------------------------
void tst_HDLParser::testInstanceParametersAreCulled()
{
	QSharedPointer<View> activeView(new View("rtl"));
	activeView->setComponentInstantiationRef("instance1");

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");    
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, activeView);

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
    parameterOverride->setReferenceId("parameterId");
    parameterOverride->setConfigurableValue("2");

    designConf_->getViewConfiguration("sender")->getViewConfigurableElements()->append(parameterOverride);

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];

    QCOMPARE( gi0->parameters.size(), 2 );

    QCOMPARE( gi0->parameters.at(0)->name(), QString("componentParameter") );
    QCOMPARE( gi0->parameters.at(0)->getValue(), QString("3") );
    QCOMPARE( gi0->parameters.at(1)->name(), QString("moduleParameter") );
    QCOMPARE( gi0->parameters.at(1)->getValue(), QString("2") );
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
	addInstanceToDesign("sender", senderVLNV, activeView);

	QSharedPointer<Parameter> componentParameter(new Parameter());
	componentParameter->setValueId("componentParameterId");
	componentParameter->setName("componentParameter");
	componentParameter->setValue("1337");
	componentParameter->setValueResolve("user");

	topComponent_->getParameters()->append(componentParameter);

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
	parameterOverride->setReferenceId("parameterId");
    parameterOverride->setConfigurableValue("componentParameterId");

    designConf_->getViewConfiguration("sender")->getViewConfigurableElements()->append(parameterOverride);

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];

    QCOMPARE( gi0->parameters.size(), 1 );

    QCOMPARE( gi0->parameters.at(0)->name(), QString("moduleParameter") );
    QCOMPARE( gi0->parameters.at(0)->getValue(), QString("1337") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testInstanceComponentParametersAreUtilized()
//-----------------------------------------------------------------------------
void tst_HDLParser::testInstanceComponentParametersAreUtilized()
{
	QSharedPointer<View> activeView(new View());
	activeView->setName("rtl");
	activeView->setComponentInstantiationRef("instance1");

	VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");    
	QSharedPointer<Component> senderComponent(new Component(senderVLNV));
	library_.addComponent(senderComponent);
	addInstanceToDesign("sender", senderVLNV, activeView);
	senderComponent->getViews()->append(activeView);

	QSharedPointer<Parameter> componentParameter(new Parameter());
	componentParameter->setValueId("cpId");
	componentParameter->setName("componentParameter");
	componentParameter->setValue("55");
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

	QSharedPointer<ConfigurableElementValue> parameterOverride(new ConfigurableElementValue());
	parameterOverride->setReferenceId("parameterId");
    parameterOverride->setConfigurableValue("cpId");

    designConf_->getViewConfiguration("sender")->getViewConfigurableElements()->append(parameterOverride);

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];

    QCOMPARE( gi0->parameters.size(), 2 );

    QCOMPARE( gi0->parameters.at(0)->name(), QString("componentParameter") );
    QCOMPARE( gi0->parameters.at(0)->getValue(), QString("55") );
    QCOMPARE( gi0->parameters.at(1)->name(), QString("moduleParameter") );
    QCOMPARE( gi0->parameters.at(1)->getValue(), QString("55") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testParameterPropagationFromTop()
//-----------------------------------------------------------------------------
void tst_HDLParser::testParameterPropagationFromTop()
{
    QSharedPointer<Parameter> topParameter(new Parameter());
    topParameter->setName("topParameter");
    topParameter->setValueId("topID");
    topParameter->setValue("10");
	topComponent_->getParameters()->append(topParameter);

	QSharedPointer<View> activeView(new View());
	activeView->setName("rtl");
	activeView->setComponentInstantiationRef("instance1");

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");    
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    library_.addComponent(senderComponent);
    addInstanceToDesign("sender", senderVLNV, activeView);

    QSharedPointer<ModuleParameter> moduleParameter(new ModuleParameter());
    moduleParameter->setValueId("parameterId");
    moduleParameter->setName("moduleParameter");
    moduleParameter->setValueResolve("user");
    moduleParameter->setValue("1");

    QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
    instantiation->getModuleParameters()->append(moduleParameter);

    senderComponent->getComponentInstantiations()->append(instantiation);
    senderComponent->getViews()->append(activeView);

    QSharedPointer<ConfigurableElementValue> parameterOverride(new ConfigurableElementValue());
    parameterOverride->setReferenceId("parameterId");
    parameterOverride->setConfigurableValue("topID");

    designConf_->getViewConfiguration("sender")->getViewConfigurableElements()->append(parameterOverride);

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];

    QCOMPARE( gi0->parameters.size(), 1 );

    QCOMPARE( gi0->parameters.at(0)->name(), QString("moduleParameter") );
    QCOMPARE( gi0->parameters.at(0)->getValue(), QString("10") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testParameterPropagationFromTop2()
//-----------------------------------------------------------------------------
void tst_HDLParser::testParameterPropagationFromTop2()
{
	QSharedPointer<Parameter> topParameter(new Parameter());
	topParameter->setName("topParameter");
	topParameter->setValueId("topID");
	topParameter->setValue("10");
	topComponent_->getParameters()->append(topParameter);

	QSharedPointer<View> activeView(new View());
	activeView->setName("rtl");
	activeView->setComponentInstantiationRef("instance1");

	VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");    
	QSharedPointer<Component> senderComponent(new Component(senderVLNV));
	library_.addComponent(senderComponent);
	addInstanceToDesign("sender", senderVLNV, activeView);

	QSharedPointer<Parameter> senderParameter(new Parameter());
	senderParameter->setName("senderParameter");
	senderParameter->setValueId("senderID");
	senderParameter->setValue("47");
	senderParameter->setValueResolve("user");
	senderComponent->getParameters()->append(senderParameter);

	QSharedPointer<ModuleParameter> moduleParameter(new ModuleParameter());
	moduleParameter->setValueId("parameterId");
	moduleParameter->setName("moduleParameter");
	moduleParameter->setValueResolve("generated");
	moduleParameter->setValue("senderID");

	QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
	instantiation->getModuleParameters()->append(moduleParameter);

	senderComponent->getComponentInstantiations()->append(instantiation);
	senderComponent->getViews()->append(activeView);

	QSharedPointer<ComponentInstance> senderInstance = design_->getComponentInstances()->first();

	QSharedPointer<ConfigurableElementValue> parameterOverride(new ConfigurableElementValue());
	parameterOverride->setReferenceId("senderID");
	parameterOverride->setConfigurableValue("topID");
    senderInstance->getConfigurableElementValues()->append(parameterOverride);

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];

    QCOMPARE( gi0->parameters.size(), 2 );

    QCOMPARE( gi0->parameters.at(0)->name(), QString("senderParameter") );
    QCOMPARE( gi0->parameters.at(0)->getValue(), QString("10") );
    QCOMPARE( gi0->parameters.at(1)->name(), QString("moduleParameter") );
    QCOMPARE( gi0->parameters.at(1)->getValue(), QString("10") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testParameterPropagationFromTopWire()
//-----------------------------------------------------------------------------
void tst_HDLParser::testParameterPropagationFromTopWire()
{
    QSharedPointer<Parameter> topParameter(new Parameter());
    topParameter->setName("topParameter");
    topParameter->setValueId("topID");
    topParameter->setValue("10");
    topComponent_->getParameters()->append(topParameter);

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

    QSharedPointer<Parameter> senderParameter(new Parameter());
    senderParameter->setName("senderParameter");
    senderParameter->setValueId("senderID");
    senderParameter->setValue("47");
    senderParameter->setValueResolve("user");
    senderComponent->getParameters()->append(senderParameter);

    QSharedPointer<ModuleParameter> moduleParameter(new ModuleParameter());
    moduleParameter->setValueId("parameterId");
    moduleParameter->setName("moduleParameter");
    moduleParameter->setValueResolve("user");
    moduleParameter->setValue("senderID");

    sendCimp->getModuleParameters()->append(moduleParameter);

    addInterfaceToComponent("data_bus", senderComponent);
    senderComponent->getBusInterface("data_bus")->setInterfaceMode(General::MASTER);

    mapPortToInterface("data_out", "DATA", "data_bus", senderComponent);
    QSharedPointer<PortMap> dataMap = senderComponent->getBusInterface("data_bus")->getPortMaps()->first();
    QSharedPointer<PortMap::LogicalPort> logPort = dataMap->getLogicalPort();
    logPort->range_ = QSharedPointer<Range>( new Range("7*senderID","0") );

    library_.addComponent(senderComponent);
    QSharedPointer<ComponentInstance> senderInstance = addInstanceToDesign("sender", senderVLNV, senderView);

    QSharedPointer<ConfigurableElementValue> parameterOverride(new ConfigurableElementValue());
    parameterOverride->setReferenceId("senderID");
    parameterOverride->setConfigurableValue("topID");
    senderInstance->getConfigurableElementValues()->append(parameterOverride);

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

    addInterfaceToComponent("data_bus", receiverComponent);
    receiverComponent->getBusInterface("data_bus")->setInterfaceMode(General::SLAVE);
    mapPortToInterface("data_in", "DATA", "data_bus", receiverComponent);

    library_.addComponent(receiverComponent);
    addInstanceToDesign("receiver", receiverVLNV, receiverView);

    addConnectionToDesign("sender", "data_bus", "receiver", "data_bus");

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
    QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
	designParser->parseDesign(componentParser->getParsedComponent(), topView_);

    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 1 );
    QSharedPointer<GenerationDesign> design = designs.first();

    QCOMPARE( design->interConnections_.size(), 1 );
    QCOMPARE( design->interConnections_.at(0)->wires_.size(), 1 );

    QSharedPointer<GenerationWire> gw0 = design->interConnections_.at(0)->wires_.first();

    QCOMPARE( gw0->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gw0->bounds.first, QString("70") );
    QCOMPARE( gw0->bounds.second, QString("0") );

    QCOMPARE( design->instances_.size(), 2 );

    QSharedPointer<GenerationInstance> gi1 = design->instances_["sender"];
    QSharedPointer<GenerationInstance> gi0 = design->instances_["receiver"];

    QSharedPointer<GenerationPortAssignMent> gpa;
    gpa = gi0->portAssignments_["data_in"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("8") );
    QCOMPARE( gpa->bounds.second, QString("0") );

    QCOMPARE( gi1->parameters.size(), 2 );

    QCOMPARE( gi1->parameters.at(0)->name(), QString("senderParameter") );
    QCOMPARE( gi1->parameters.at(0)->getValue(), QString("10") );
    QCOMPARE( gi1->parameters.at(1)->name(), QString("moduleParameter") );
    QCOMPARE( gi1->parameters.at(1)->getValue(), QString("10") );

    gpa = gi1->portAssignments_["data_out"];
    QCOMPARE( gpa->wire->name, QString("sender_to_receiver_DATA") );
    QCOMPARE( gpa->bounds.first, QString("70") );
    QCOMPARE( gpa->bounds.second, QString("0") );
}

//-----------------------------------------------------------------------------
// Function: tst_HDLParser::testMultiLevelHierachy()
//-----------------------------------------------------------------------------
void tst_HDLParser::testMultiLevelHierachy()
{
    VLNV slaveVlnv(VLNV::COMPONENT, "Test", "TestLibrary", "SlaveComponent", "1.0b");
    QSharedPointer<Component> slaveTopComponent = QSharedPointer<Component>(new Component(slaveVlnv));

    VLNV slaveDesignVlnv(VLNV::DESIGN, "Test", "TestLibrary", "SlaveDesign", "1.0b");
    QSharedPointer<Design> slaveDesign = QSharedPointer<Design>(new Design(slaveDesignVlnv));

    VLNV slaveDesignConfVlnv(VLNV::DESIGNCONFIGURATION, "Test", "TestLibrary", "SlaveDesignConfiguration", "1.0b");
    QSharedPointer<DesignConfiguration> slaveDesignConf = QSharedPointer<DesignConfiguration>(new DesignConfiguration(slaveDesignConfVlnv));
    slaveDesignConf->setDesignRef(slaveDesignVlnv);

    QSharedPointer<View> slaveTopView = QSharedPointer<View>(new View("topView"));
    QSharedPointer<DesignConfigurationInstantiation> disg(new DesignConfigurationInstantiation("SlaveDesignConfigurationInstantiation"));
    disg->setDesignConfigurationReference(QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference(slaveDesignConfVlnv)));
    slaveTopComponent->getDesignConfigurationInstantiations()->append(disg);
    slaveTopView->setDesignConfigurationInstantiationRef(disg->name());

    slaveTopComponent->getViews()->append(slaveTopView);

    library_.addComponent(slaveTopComponent);
    library_.addComponent(slaveDesign);
    library_.addComponent(slaveDesignConf);

    QSharedPointer<ComponentInstance> slaveInstance = addInstanceToDesign("hierSlave", slaveVlnv, slaveTopView);

    QSharedPointer<ConfigurableElementValue> parameterSlaveOverride(new ConfigurableElementValue());
    parameterSlaveOverride->setReferenceId("slaveTopID");
    parameterSlaveOverride->setConfigurableValue("topID");
    slaveInstance->getConfigurableElementValues()->append(parameterSlaveOverride);

    QSharedPointer<Parameter> slaveTopParameter(new Parameter());
    slaveTopParameter->setName("topParameter");
    slaveTopParameter->setValueId("slaveTopID");
    slaveTopParameter->setValue("12");
    slaveTopComponent->getParameters()->append(slaveTopParameter);

    QSharedPointer<Parameter> topParameter(new Parameter());
    topParameter->setName("topParameter");
    topParameter->setValueId("topID");
    topParameter->setValue("76");
    topComponent_->getParameters()->append(topParameter);

    QSharedPointer<View> activeView(new View());
    activeView->setName("rtl");
    activeView->setComponentInstantiationRef("instance1");

    VLNV senderVLNV(VLNV::COMPONENT, "Test", "TestLibrary", "TestSender", "1.0");    
    QSharedPointer<Component> senderComponent(new Component(senderVLNV));
    library_.addComponent(senderComponent);

    QSharedPointer<ConfigurableVLNVReference> componentVLNV (new ConfigurableVLNVReference(senderVLNV));
    QSharedPointer<ComponentInstance> senderInstance (new ComponentInstance("sender", componentVLNV));

    slaveDesign->getComponentInstances()->append(senderInstance);

    slaveDesignConf->addViewConfiguration("sender", activeView->name());

    QSharedPointer<Parameter> senderParameter(new Parameter());
    senderParameter->setName("senderParameter");
    senderParameter->setValueId("senderID");
    senderParameter->setValue("47");
    senderParameter->setValueResolve("user");
    senderComponent->getParameters()->append(senderParameter);

    QSharedPointer<ModuleParameter> moduleParameter(new ModuleParameter());
    moduleParameter->setValueId("parameterId");
    moduleParameter->setName("moduleParameter");
    moduleParameter->setValueResolve("generated");
    moduleParameter->setValue("senderID");

    QSharedPointer<ComponentInstantiation> instantiation(new ComponentInstantiation("instance1"));
    instantiation->getModuleParameters()->append(moduleParameter);

    senderComponent->getComponentInstantiations()->append(instantiation);
    senderComponent->getViews()->append(activeView);

    QSharedPointer<ConfigurableElementValue> parameterOverride(new ConfigurableElementValue());
    parameterOverride->setReferenceId("senderID");
    parameterOverride->setConfigurableValue("slaveTopID");
    senderInstance->getConfigurableElementValues()->append(parameterOverride);

    QSharedPointer<HDLComponentParser> componentParser =

    QSharedPointer<HDLComponentParser>(new HDLComponentParser(&library_, topComponent_));

    componentParser->parseComponent(topView_);
    QSharedPointer<HDLDesignParser> designParser =
        QSharedPointer<HDLDesignParser>(new HDLDesignParser(&library_, design_, designConf_));
    designParser->parseDesign(componentParser->getParsedComponent(), topView_);
    QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();

    QCOMPARE( designs.size(), 2 );
    QSharedPointer<GenerationDesign> design = designs.last();

    QCOMPARE( design->instances_.size(), 1 );

    QSharedPointer<GenerationInstance> gi0 = design->instances_["sender"];

    QCOMPARE( gi0->parameters.size(), 2 );

    QCOMPARE( gi0->parameters.at(0)->name(), QString("senderParameter") );
    QCOMPARE( gi0->parameters.at(0)->getValue(), QString("76") );
    QCOMPARE( gi0->parameters.at(1)->name(), QString("moduleParameter") );
    QCOMPARE( gi0->parameters.at(1)->getValue(), QString("76") );
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

	QList<QSharedPointer<Parameter> > parameters;
	parameters.append(moduleParameterThird);
	parameters.append(moduleParameterFirst);
	parameters.append(moduleParameterSecond);

    QList<QSharedPointer<Parameter> > sortedParameters(parameters);
	
    HDLComponentParser::sortParameters(parameters,sortedParameters);

    QCOMPARE( sortedParameters.size(), 3 );

    QCOMPARE( sortedParameters.at(0)->name(), QString("moduleParameterFirst") );
    QCOMPARE( sortedParameters.at(0)->getValue(), QString("1") );
    QCOMPARE( sortedParameters.at(1)->name(), QString("moduleParameterSecond") );
    QCOMPARE( sortedParameters.at(1)->getValue(), QString("firstParameter") );
    QCOMPARE( sortedParameters.at(2)->name(), QString("moduleParameterThird") );
    QCOMPARE( sortedParameters.at(2)->getValue(), QString("secondParameter") );
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

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(moduleParameterFirst);
    parameters.append(moduleParameterSecond);
    parameters.append(moduleParameterThird);
    parameters.append(moduleParameterFourth);

    QList<QSharedPointer<Parameter> > sortedParameters(parameters);

    HDLComponentParser::sortParameters(parameters,sortedParameters);

    QCOMPARE( sortedParameters.size(), 4 );

    QCOMPARE( sortedParameters.at(0)->name(), QString("moduleParameterSecond") );
    QCOMPARE( sortedParameters.at(0)->getValue(), QString("1") );
    QCOMPARE( sortedParameters.at(1)->name(), QString("moduleParameterFourth") );
    QCOMPARE( sortedParameters.at(1)->getValue(), QString("4") );
    QCOMPARE( sortedParameters.at(2)->name(), QString("moduleParameterFirst") );
    QCOMPARE( sortedParameters.at(2)->getValue(), QString("secondParameter + fourthParameter") );
    QCOMPARE( sortedParameters.at(3)->name(), QString("moduleParameterThird") );
    QCOMPARE( sortedParameters.at(3)->getValue(), QString("firstParameter") );
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

    QList<QSharedPointer<Parameter> > parameters;
    parameters.append(moduleParameterFirst);
    parameters.append(moduleParameterSecond);
    parameters.append(moduleParameterThird);
    parameters.append(moduleParameterFourth);
    parameters.append(moduleParameterFifth);

    QList<QSharedPointer<Parameter> > sortedParameters(parameters);

    HDLComponentParser::sortParameters(parameters,sortedParameters);

    QCOMPARE( sortedParameters.size(), 5 );

    QCOMPARE( sortedParameters.at(0)->name(), QString("moduleParameterSecond") );
    QCOMPARE( sortedParameters.at(0)->getValue(), QString("55") );
    QCOMPARE( sortedParameters.at(1)->name(), QString("moduleParameterFirst") );
    QCOMPARE( sortedParameters.at(1)->getValue(), QString("1") );
    QCOMPARE( sortedParameters.at(2)->name(), QString("moduleParameterFourth") );
    QCOMPARE( sortedParameters.at(2)->getValue(), QString("12") );
    QCOMPARE( sortedParameters.at(3)->name(), QString("moduleParameterFifth") );
    QCOMPARE( sortedParameters.at(3)->getValue(), QString("fourthParameter - firstParameter") );
    QCOMPARE( sortedParameters.at(4)->name(), QString("moduleParameterThird") );
    QCOMPARE( sortedParameters.at(4)->getValue(), QString("fifthParameter + secondParameter") );
}

QTEST_APPLESS_MAIN(tst_HDLParser)

#include "tst_HDLParser.moc"

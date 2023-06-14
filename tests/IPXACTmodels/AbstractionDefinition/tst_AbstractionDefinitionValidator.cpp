//-----------------------------------------------------------------------------
// AbstractionDefinition: tst_AbstractionDefinitionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 04.12.2015
//
// Description:
// Unit test for class AbstractionDefinitionValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/validators/AbstractionDefinitionValidator.h>
#include <IPXACTmodels/AbstractionDefinition/Packet.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/common/TimingConstraint.h>
#include <IPXACTmodels/common/CellSpecification.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <tests/MockObjects/LibraryMock.h>

#include <QtTest>

class tst_AbstractionDefinitionValidator : public QObject
{
	Q_OBJECT

public:
    tst_AbstractionDefinitionValidator();

private slots:
	void baseCase();
	void vlnvFail();
	void busFail();
	void extendFail();
    void extendedBusType();

    void getPortsFromExtendAbstraction();
    void testExtendedPortsNonEditableParameters();

	void paraFail();
	void paraPassStd22();
	void portFail();

	void portMatchOnlyInStd22();

	void wireFail();
	void noDublicatePorts();
	void invalidWirePort();
	void invalidTransactionalPort();
	void invalidTimingConstraint();
	void invalidCellSpec();
	void wirePortSuccessful();
	void transactionalSuccessful();
    void systemWirePortHasGroup();

	void wireQualifier();
	void packetsOnlyInStd22();
    void invalidPacketFieldWidthValue();
    void packetFieldValueWhenOpcode();
	void choicesOnlyInStd22();

private:
	//! The test mock for library interface.
	LibraryMock* library_;

    QSharedPointer<ExpressionParser> expressionParser_;

	//! VLNV for some bus referred by the abstraction definition.
	VLNV testBusDefVlnv_;

	//! Some valid abstraction port within the abstraction definition.
	QSharedPointer<PortAbstraction> port_;
};

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::tst_AbstractionDefinitionValidator()
//-----------------------------------------------------------------------------
tst_AbstractionDefinitionValidator::tst_AbstractionDefinitionValidator() :
library_(new LibraryMock(this)), expressionParser_(new SystemVerilogExpressionParser())
{
	testBusDefVlnv_ = VLNV(VLNV::BUSDEFINITION,"vendor","library","test_bus_def","version");
	QSharedPointer<BusDefinition> testBusDef(new BusDefinition);
	testBusDef->setVlnv(testBusDefVlnv_);
	library_->addComponent(testBusDef);

	port_ = QSharedPointer<PortAbstraction>(new PortAbstraction);
	port_->setLogicalName("trvv");

	QSharedPointer<WireAbstraction> wire(new WireAbstraction);
	port_->setWire(wire);
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::baseCase()
{
	VLNV vlnv(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(vlnv, Document::Revision::Std14));
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 0);
	QVERIFY(validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::vlnvFail()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::vlnvFail()
{
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 1);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::busFail()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::busFail()
{
    VLNV vlnv(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(vlnv, Document::Revision::Std14));
    AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(VLNV(VLNV::BUSDEFINITION,"vendor","library","bogus","version"));
	abs->getLogicalPorts()->append(port_);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 1);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::extendFail()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::extendFail()
{
    VLNV vlnv(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(vlnv, Document::Revision::Std14));
    AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	abs->setExtends(VLNV(VLNV::ABSTRACTIONDEFINITION,"vendor","library","bogus","version"));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

    QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.first(), QLatin1String("The bus definition vendor:library:bogus:version extended in "
        "abstraction definition vendor:library:name:version is not found in the library"));
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::extendedBusType()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::extendedBusType()
{
    VLNV extendBusVLNV(VLNV::BUSDEFINITION, "vendor", "library", "extend_test_bus_def", "version");
    QSharedPointer<BusDefinition> extendBusDef(new BusDefinition(extendBusVLNV, Document::Revision::Std14));

    VLNV busVLNV(VLNV::BUSDEFINITION, "vendor", "library", "test_bus_def", "version");
    QSharedPointer<BusDefinition> busDef(new BusDefinition(busVLNV, Document::Revision::Std14));

    VLNV extendAbsVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "bogus", "version");
    QSharedPointer<AbstractionDefinition> extendAbs(new AbstractionDefinition(extendAbsVLNV, Document::Revision::Std14));
    extendAbs->setBusType(extendBusVLNV);

    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));

    abs->setBusType(busVLNV);
    abs->getLogicalPorts()->append(port_);
    abs->setExtends(extendAbsVLNV);

    library_->addComponent(extendBusDef);
    library_->addComponent(extendAbs);
    library_->addComponent(busDef);
    library_->addComponent(abs);
    
    AbstractionDefinitionValidator validator(library_, expressionParser_);

    QString extendError = "The bus definition vendor:library:test_bus_def:version extended in abstraction "
        "definition vendor:library:name:version does not define extended abstraction definition bus type";

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abs);
    QCOMPARE(errorList.size(), 1);
    QCOMPARE(errorList.last(), extendError);
    QVERIFY(!validator.validate(abs));

    busDef->setExtends(extendBusVLNV);
    errorList.clear();
    validator.findErrorsIn(errorList, abs);
    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::getPortsFromExtendAbstraction()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::getPortsFromExtendAbstraction()
{
    VLNV extendBusVLNV(VLNV::BUSDEFINITION, "vendor", "library", "extend_test_bus_def", "version");
    QSharedPointer<BusDefinition> extendBusDef(new BusDefinition(extendBusVLNV, Document::Revision::Std14));

    VLNV busVLNV(VLNV::BUSDEFINITION, "vendor", "library", "test_bus_def", "version");
	QSharedPointer<BusDefinition> busDef(new BusDefinition(busVLNV, Document::Revision::Std14));
    busDef->setExtends(extendBusVLNV);

    VLNV extendAbsVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "bogus", "version");
    QSharedPointer<AbstractionDefinition> extendAbs(new AbstractionDefinition(extendAbsVLNV, Document::Revision::Std14));
    extendAbs->setBusType(extendBusVLNV);
    extendAbs->getLogicalPorts()->append(port_);

    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));
    abs->setBusType(busVLNV);
    abs->setExtends(extendAbsVLNV);

    for (auto logicalPort : *extendAbs->getLogicalPorts())
    {
        abs->getLogicalPorts()->append(logicalPort);
    }

    library_->addComponent(extendBusDef);
    library_->addComponent(extendAbs);
    library_->addComponent(busDef);
    library_->addComponent(abs);

    AbstractionDefinitionValidator validator(library_, expressionParser_);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abs);
    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::testExtendedPortsNonEditableParameters()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::testExtendedPortsNonEditableParameters()
{
    QStringList extendBusDefSystemGroups("system");

    VLNV extendBusVLNV(VLNV::BUSDEFINITION, "vendor", "library", "extend_test_bus_def", "version");
    QSharedPointer<BusDefinition> extendBusDef(new BusDefinition(extendBusVLNV, Document::Revision::Std14));
    extendBusDef->setSystemGroupNames(extendBusDefSystemGroups);

    VLNV busVLNV(VLNV::BUSDEFINITION, "vendor", "library", "test_bus_def", "version");
    QSharedPointer<BusDefinition> busDef(new BusDefinition(busVLNV, Document::Revision::Std14));
    busDef->setExtends(extendBusVLNV);

    QSharedPointer<WirePort> wirePort(new WirePort());
    wirePort->setDirection(DirectionTypes::IN);
    wirePort->setPresence(PresenceTypes::REQUIRED);
    wirePort->setWidth("10");
    wirePort->setSystemGroup(extendBusDefSystemGroups.first());

    QSharedPointer<WireAbstraction> extendWire(new WireAbstraction());
    extendWire->getSystemPorts()->append(wirePort);
    extendWire->setDefaultValue("0");
    extendWire->addQualifier(Qualifier::Data);

    QSharedPointer<PortAbstraction> extendWirePort(new PortAbstraction());
    extendWirePort->setName("ExtendWire");
    extendWirePort->setDescription("wireDescription");
    extendWirePort->setWire(extendWire);

    QSharedPointer<Protocol> transactionalProtocol(new Protocol());
    transactionalProtocol->setProtocolType("protocolType");
    transactionalProtocol->setPayloadName("payDay");
    transactionalProtocol->setPayloadType("generic");
    transactionalProtocol->setPayloadExtension("extend", false);

    QSharedPointer<TransactionalPort> transactionalPort(new TransactionalPort());
    transactionalPort->setPresence(PresenceTypes::REQUIRED);
    transactionalPort->setInitiative("requires");
    transactionalPort->setKind("tlm_port");
    transactionalPort->setBusWidth("3");
    transactionalPort->setSystemGroup(extendBusDefSystemGroups.first());
    transactionalPort->setProtocol(transactionalProtocol);

    QSharedPointer<TransactionalAbstraction> extendTransactional(new TransactionalAbstraction());
    extendTransactional->addQualifier(Qualifier::Data);
    extendTransactional->getSystemPorts()->append(transactionalPort);

    QSharedPointer<PortAbstraction> extendTransactionalPort(new PortAbstraction());
    extendTransactionalPort->setName("extendTransactional");
    extendTransactionalPort->setDescription("transactionalDescription");
    extendTransactionalPort->setTransactional(extendTransactional);

    VLNV extendAbsVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "bogus", "version");
    QSharedPointer<AbstractionDefinition> extendAbs(new AbstractionDefinition(extendAbsVLNV, Document::Revision::Std14));
    extendAbs->setBusType(extendBusVLNV);
    extendAbs->getLogicalPorts()->append(extendWirePort);
    extendAbs->getLogicalPorts()->append(extendTransactionalPort);

    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));
    abs->setBusType(busVLNV);
    abs->setExtends(extendAbsVLNV);

    QSharedPointer<PortAbstraction> absWirePort(new PortAbstraction(*extendWirePort.data()));
    QSharedPointer<PortAbstraction> absTransactionalPort(new PortAbstraction(*extendTransactionalPort.data()));
    abs->getLogicalPorts()->append(absWirePort);
    abs->getLogicalPorts()->append(absTransactionalPort);

    library_->addComponent(extendBusDef);
    library_->addComponent(extendAbs);
    library_->addComponent(busDef);
    library_->addComponent(abs);

    AbstractionDefinitionValidator validator(library_, expressionParser_);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abs);

    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(abs));

    wirePort->setDirection(DirectionTypes::OUT);
    extendWire->addQualifier(Qualifier::Address);
    extendWirePort->setDescription("newExtendWireDescription");
    transactionalPort->setInitiative("provides");
    transactionalPort->setKind("tlm_socket");
    transactionalPort->setBusWidth("4");
    transactionalProtocol->setProtocolType("newProtocol");
    transactionalProtocol->setPayloadName("newPayment");
    transactionalProtocol->setPayloadType("specific");
    transactionalProtocol->setPayloadExtension("extends", false);

    errorList.clear();
    validator.findErrorsIn(errorList, abs);
    QVERIFY(validator.validate(extendAbs));
    QVERIFY(validator.validate(abs) == false);

    QCOMPARE(errorList.size(), 9);
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::paraFail()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::paraFail()
{
	VLNV vlnv = VLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(vlnv, Document::Revision::Std14));
	AbstractionDefinitionValidator validator(library_, expressionParser_);
	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{'b11, 'b00}");
	parameter->setValueId("parameterid");
	parameter->setVectorLeft("4");
	parameter->setVectorRight("0");
	parameter->setVectorId("testId"); // VectorId is Std22 only

	abs->getParameters()->append(parameter);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 2);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::paraPassStd22()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::paraPassStd22()
{
    VLNV vlnv = VLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(vlnv, Document::Revision::Std22));
    AbstractionDefinitionValidator validator(library_, expressionParser_);
    abs->setBusType(testBusDefVlnv_);
    abs->getLogicalPorts()->append(port_);

    QSharedPointer<Parameter> parameter(new Parameter());
    parameter->setName("param");
    parameter->setType("bit");
    parameter->setValue("16");
    parameter->setValueId("parameterid");
    parameter->setVectorLeft("4");
    parameter->setVectorRight("0");
    parameter->setVectorId("testId");

    abs->getParameters()->append(parameter);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abs);

    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::portFail()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::portFail()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<PortAbstraction> port(new PortAbstraction);
	port->setIsPresent("eiole");
	abs->getLogicalPorts()->append(port);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 3);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::portMatchOnlyInStd22()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::portMatchOnlyInStd22()
{
    VLNV absVLNV14(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs14(new AbstractionDefinition(absVLNV14, Document::Revision::Std14));
    
    VLNV absVLNV22(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs22(new AbstractionDefinition(absVLNV22, Document::Revision::Std22));
    
	VLNV busVLNV22(VLNV::BUSDEFINITION, "vendor", "library", "extend_test_bus_def", "version");
    QSharedPointer<BusDefinition> busDef22(new BusDefinition(busVLNV22, Document::Revision::Std22));
	abs22->setBusType(busVLNV22);
    abs14->setBusType(testBusDefVlnv_);

	AbstractionDefinitionValidator validator(library_, expressionParser_);
	
    QSharedPointer<PortAbstraction> port14(new PortAbstraction(*port_));
	port14->setMatch(true);

    QSharedPointer<WireAbstraction> wire14(new WireAbstraction);
    port14->setWire(wire14);
	
	QSharedPointer<PortAbstraction> port22(new PortAbstraction(*port14));
	port22->setLogicalName("other_name"); //name must be unique within absDef

    QSharedPointer<WireAbstraction> wire22(new WireAbstraction);
    port22->setWire(wire22);

	abs14->getLogicalPorts()->append(port14);
	abs22->getLogicalPorts()->append(port22);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abs14);

    QCOMPARE(errorList.size(), 1);
    QVERIFY(!validator.validate(abs14));
	errorList.clear();

    validator.findErrorsIn(errorList, abs22);

    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(abs22));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::noDublicatePorts()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::noDublicatePorts()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<WireAbstraction> wire(new WireAbstraction);
	QSharedPointer<PortAbstraction> port1(new PortAbstraction);
	port1->setLogicalName("alias");
	abs->getLogicalPorts()->append(port1);
	port1->setWire(wire);

	QSharedPointer<PortAbstraction> port2(new PortAbstraction);
	port2->setLogicalName("alias");
	abs->getLogicalPorts()->append(port2);
	port2->setWire(wire);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 1);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::wireFail()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::wireFail()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));

	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<PortAbstraction> port(new PortAbstraction);
	port->setLogicalName("nimi");
	abs->getLogicalPorts()->append(port);
	QSharedPointer<WireAbstraction> wire(new WireAbstraction);
	wire->setDefaultValue("bogus");
	port->setWire(wire);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 1);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::invalidWirePort()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::invalidWirePort()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));

	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<PortAbstraction> port(new PortAbstraction);
	port->setLogicalName("nimi");
	abs->getLogicalPorts()->append(port);
	QSharedPointer<WireAbstraction> wire(new WireAbstraction);
	port->setWire(wire);

	QSharedPointer<WirePort> wirePort(new WirePort);
	wirePort->setDirection(DirectionTypes::DIRECTION_PHANTOM);
	wirePort->setPresence(PresenceTypes::UNKNOWN);
	wirePort->setWidth("width");
	wire->setMasterPort(wirePort);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 1);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::invalidTransactionalPort()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::invalidTransactionalPort()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));
	
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<PortAbstraction> port(new PortAbstraction);
	port->setLogicalName("nimi");
	abs->getLogicalPorts()->append(port);
	QSharedPointer<TransactionalAbstraction> transAbs(new TransactionalAbstraction);
	port->setTransactional(transAbs);

	QSharedPointer<TransactionalPort> transPort(new TransactionalPort);
	transPort->setBusWidth("yks");
	transPort->setInitiative("init");
	transPort->setKind("joo");
	transPort->setPresence(PresenceTypes::UNKNOWN);
	transAbs->setMasterPort(transPort);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 3);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::invalidTimingConstraint()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::invalidTimingConstraint()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));

	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<PortAbstraction> port(new PortAbstraction);
	port->setLogicalName("nimi");
	abs->getLogicalPorts()->append(port);
	QSharedPointer<WireAbstraction> wire(new WireAbstraction);
	port->setWire(wire);

	QSharedPointer<WirePort> wirePort(new WirePort);
	wirePort->setDirection(DirectionTypes::IN);
	wirePort->setPresence(PresenceTypes::ILLEGAL);
	wirePort->setWidth("47");
	wire->setMasterPort(wirePort);

	QSharedPointer<TimingConstraint> timing(new TimingConstraint);
	timing->setClockEdge(TimingConstraint::UNKNOWN);
	timing->setClockName("bogus");
	timing->setValue(100.01);
	wirePort->setTimingConstraint(timing);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 3);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::invalidCellSpec()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::invalidCellSpec()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));
	
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<PortAbstraction> port(new PortAbstraction);
	port->setLogicalName("nimi");
	abs->getLogicalPorts()->append(port);
	QSharedPointer<WireAbstraction> wire(new WireAbstraction);
	port->setWire(wire);

	QSharedPointer<WirePort> wirePort(new WirePort);
	wirePort->setDirection(DirectionTypes::IN);
	wirePort->setPresence(PresenceTypes::ILLEGAL);
	wirePort->setWidth("47");
	wire->setMasterPort(wirePort);

	QSharedPointer<CellSpecification> cell(new CellSpecification);
	cell->setCellClass(CellSpecification::NO_CLASS);
	cell->setCellFunction("bogus");
	cell->setCellStrength(CellSpecification::UNKNOWN);
	wirePort->setLoadConstraint(cell);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 2);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::wirePortSuccessful()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::wirePortSuccessful()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));

	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<PortAbstraction> port(new PortAbstraction);
	port->setLogicalName("nimi");
	abs->getLogicalPorts()->append(port);
	QSharedPointer<WireAbstraction> wire(new WireAbstraction);
	port->setWire(wire);

	QSharedPointer<WirePort> wirePort(new WirePort);
	wirePort->setDirection(DirectionTypes::IN);
	wirePort->setPresence(PresenceTypes::ILLEGAL);
	wirePort->setWidth("47");
	wire->setMasterPort(wirePort);

	QSharedPointer<TimingConstraint> timing(new TimingConstraint);
	timing->setClockEdge(TimingConstraint::RISE);
	timing->setClockName("nimi");
	timing->setValue(99.99);
	wirePort->setTimingConstraint(timing);

	QSharedPointer<CellSpecification> cell(new CellSpecification);
	cell->setCellFunction("xor2");
	cell->setCellStrength(CellSpecification::LOW);
	wirePort->setLoadConstraint(cell);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 0);
	QVERIFY(validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::transactionalSuccessful()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::transactionalSuccessful()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition(absVLNV, Document::Revision::Std14));
	
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<PortAbstraction> port(new PortAbstraction);
	port->setLogicalName("nimi");
	abs->getLogicalPorts()->append(port);
	QSharedPointer<TransactionalAbstraction> transAbs(new TransactionalAbstraction);
	port->setTransactional(transAbs);

	QSharedPointer<TransactionalPort> transPort(new TransactionalPort);
	transPort->setBusWidth("3");
	transPort->setInitiative("provides");
	transPort->setKind("tlm_socket");
	transPort->setPresence(PresenceTypes::REQUIRED);
	transAbs->setMasterPort(transPort);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 0);
	QVERIFY(validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::transactionalSuccessful()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::systemWirePortHasGroup()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abstraction(new AbstractionDefinition(absVLNV, Document::Revision::Std14));

    AbstractionDefinitionValidator validator(library_, expressionParser_);

    abstraction->setBusType(testBusDefVlnv_);
    
    QSharedPointer<PortAbstraction>systemPort(new PortAbstraction);
    systemPort->setLogicalName("system");

    QSharedPointer<WireAbstraction> wire(new WireAbstraction());
    QSharedPointer<WirePort> systemWire(new WirePort());

    wire->getSystemPorts()->append(systemWire);
    systemPort->setWire(wire);

    abstraction->getLogicalPorts()->append(systemPort);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abstraction);

    QCOMPARE(errorList.size(), 1);
    QVERIFY(validator.validate(abstraction) == false);

    QSharedPointer<BusDefinition> busDef(new BusDefinition());
    busDef->setVlnv(testBusDefVlnv_);
    QStringList systemGroupNames({ "testSystem" });
    busDef->setSystemGroupNames(systemGroupNames);
    library_->addComponent(busDef);

    systemWire->setSystemGroup("testSystem");

    errorList.clear();
    validator.findErrorsIn(errorList, abstraction);

    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(abstraction));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::wireQualifier()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::wireQualifier()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abstraction(new AbstractionDefinition(absVLNV, Document::Revision::Std14));

    AbstractionDefinitionValidator validator(library_, expressionParser_);

    abstraction->setBusType(testBusDefVlnv_);

	QSharedPointer<PortAbstraction> testPort(new PortAbstraction());
	testPort->setLogicalName("testPort");
	abstraction->getLogicalPorts()->append(testPort);

	QSharedPointer<WireAbstraction> wire(new WireAbstraction());
	testPort->setWire(wire);

	QSharedPointer<Qualifier> wireQualifier(new Qualifier());
	wireQualifier->setType(Qualifier::Protection);
	wireQualifier->setAttribute(Qualifier::Attribute::PowerEnableLevel, QStringLiteral("super high"));
	wire->setQualifier(wireQualifier);

    VLNV absVLNV2(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name2", "version");
    QSharedPointer<AbstractionDefinition> abstraction2(new AbstractionDefinition(absVLNV2, Document::Revision::Std22));

    abstraction2->setBusType(testBusDefVlnv_);

    QSharedPointer<PortAbstraction> testPort2(new PortAbstraction());
    testPort2->setLogicalName("testPort");
	abstraction2->getLogicalPorts()->append(testPort2);

    QSharedPointer<WireAbstraction> wire2(new WireAbstraction());
    testPort2->setWire(wire2);

    QSharedPointer<Qualifier> wireQualifier2(new Qualifier());
    wireQualifier2->setType(Qualifier::PowerEnable);
    wireQualifier2->setAttribute(Qualifier::Attribute::PowerEnableLevel, QStringLiteral("high"));
    wireQualifier2->setAttribute(Qualifier::Attribute::PowerDomainReference, QStringLiteral("testPowerDomain")); // port qualifier must not have powerDomainRef set when in an absDef

	wire2->setQualifier(wireQualifier2);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abstraction);
	validator.findErrorsIn(errorList, abstraction2);

    QCOMPARE(errorList.size(), 2);
    QVERIFY(!validator.validate(abstraction));
    QVERIFY(!validator.validate(abstraction2));

	wireQualifier->clear();
	wireQualifier->setType(Qualifier::Address);
	wireQualifier->setType(Qualifier::Data);

	wireQualifier2->setAttribute(Qualifier::Attribute::PowerDomainReference, QStringLiteral(""));

	errorList.clear();
    validator.findErrorsIn(errorList, abstraction);
    validator.findErrorsIn(errorList, abstraction2);

    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(abstraction));
    QVERIFY(validator.validate(abstraction2));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::packetsOnlyInStd22()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::packetsOnlyInStd22()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abstraction(new AbstractionDefinition(absVLNV, Document::Revision::Std22));
	
    AbstractionDefinitionValidator validator(library_, expressionParser_);

    abstraction->setBusType(testBusDefVlnv_);

    QSharedPointer<PortAbstraction> testPort(new PortAbstraction());
    testPort->setLogicalName("testPort");
	abstraction->getLogicalPorts()->append(testPort);

	QSharedPointer<WireAbstraction> wire(new WireAbstraction());
	testPort->setWire(wire);

	QSharedPointer<Packet> testPacket(new Packet()); // no packet name
	QSharedPointer<PacketField> testPacketField(new PacketField("testPacketField"));
	// no packetfield in packet
	// no width defined

	testPort->getPackets()->append(testPacket);


    VLNV absVLNV2(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name2", "version");
    QSharedPointer<AbstractionDefinition> abstraction2(new AbstractionDefinition(absVLNV2, Document::Revision::Std14));

    abstraction2->setBusType(testBusDefVlnv_);

    QSharedPointer<PortAbstraction> testPort2(new PortAbstraction());
    testPort2->setLogicalName("testPort2");
    abstraction2->getLogicalPorts()->append(testPort2);

    QSharedPointer<WireAbstraction> wire2(new WireAbstraction());
    testPort2->setWire(wire2);

    QSharedPointer<Packet> testPacket2(new Packet("testPacket2"));
    QSharedPointer<PacketField> testPacketField2(new PacketField("testPacketField2"));
	testPacket2->getPacketFields()->append(testPacketField2);

	testPort2->getPackets()->append(testPacket2);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abstraction);
    validator.findErrorsIn(errorList, abstraction2);
    QCOMPARE(errorList.size(), 3);
    QVERIFY(!validator.validate(abstraction));
    QVERIFY(!validator.validate(abstraction2));

	testPacket->getPacketFields()->append(testPacketField);
	testPacket->setName("testPacket");
	testPort2->getPackets()->clear();

	errorList.clear();
    validator.findErrorsIn(errorList, abstraction);
    validator.findErrorsIn(errorList, abstraction2);

    QCOMPARE(errorList.size(), 1); // still no width defined for testPacketField
    QVERIFY(!validator.validate(abstraction));
    QVERIFY(validator.validate(abstraction2));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::invalidPacketFieldWidthValue()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::invalidPacketFieldWidthValue()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abstraction(new AbstractionDefinition(absVLNV, Document::Revision::Std22));

    AbstractionDefinitionValidator validator(library_, expressionParser_);

    abstraction->setBusType(testBusDefVlnv_);

    QSharedPointer<PortAbstraction> testPort(new PortAbstraction());
    testPort->setLogicalName("testPort");
    abstraction->getLogicalPorts()->append(testPort);

    QSharedPointer<WireAbstraction> wire(new WireAbstraction());
    testPort->setWire(wire);

    QSharedPointer<Packet> testPacket(new Packet("testPacket"));
    QSharedPointer<PacketField> testPacketField(new PacketField("testPacketField"));
    testPacket->getPacketFields()->append(testPacketField);
    testPacketField->setWidth("2+2");
    testPacketField->setValue("'h10"); // error
    
    testPort->getPackets()->append(testPacket);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abstraction);
    QCOMPARE(errorList.size(), 1);
    QVERIFY(!validator.validate(abstraction));

    testPacketField->setWidth("8");

    errorList.clear();
    validator.findErrorsIn(errorList, abstraction);
    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(abstraction));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::packetFieldValueWhenOpcode()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::packetFieldValueWhenOpcode()
{
    // Packet field value must be constant, when qualifier is opcode.
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abstraction(new AbstractionDefinition(absVLNV, Document::Revision::Std22));

    AbstractionDefinitionValidator validator(library_, expressionParser_);

    abstraction->setBusType(testBusDefVlnv_);

    QSharedPointer<PortAbstraction> testPort(new PortAbstraction());
    testPort->setLogicalName("testPort");
    abstraction->getLogicalPorts()->append(testPort);

    QSharedPointer<WireAbstraction> wire(new WireAbstraction());
    testPort->setWire(wire);

    QSharedPointer<Packet> testPacket(new Packet("testPacket"));
    QSharedPointer<PacketField> testPacketField(new PacketField("testPacketField"));
    testPacket->getPacketFields()->append(testPacketField);
    testPacketField->setWidth("8");
    testPacketField->setValue("128+64"); // error

    QSharedPointer<Qualifier> testQualifier(new Qualifier());
    testQualifier->setType(Qualifier::Opcode);
    testPacketField->setQualifier(testQualifier);

    testPort->getPackets()->append(testPacket);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abstraction);
    QCOMPARE(errorList.size(), 2);
    QVERIFY(errorList.contains(QStringLiteral("Value of packet field testPacketField in packet testPacket within port testPort must be constant when qualifier is opcode.")));
    QVERIFY(!validator.validate(abstraction));

    testPacketField->setValue("128");

    errorList.clear();
    validator.findErrorsIn(errorList, abstraction);
    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(abstraction));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::choicesOnlyInStd22()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::choicesOnlyInStd22()
{
    VLNV absVLNV(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name", "version");
    QSharedPointer<AbstractionDefinition> abstraction(new AbstractionDefinition(absVLNV, Document::Revision::Std14));

    AbstractionDefinitionValidator validator(library_, expressionParser_);

    abstraction->setBusType(testBusDefVlnv_);

    QSharedPointer<PortAbstraction> testPort(new PortAbstraction());
    testPort->setLogicalName("testPort");
    abstraction->getLogicalPorts()->append(testPort);

    QSharedPointer<WireAbstraction> wire(new WireAbstraction());
    testPort->setWire(wire);

    QSharedPointer<Choice> testChoice(new Choice());
    abstraction->getChoices()->append(testChoice); // error 1

    VLNV busVLNV(VLNV::BUSDEFINITION, "vendor", "library", "testbusdef", "version");
    QSharedPointer<BusDefinition> busDef(new BusDefinition(busVLNV, Document::Revision::Std22));
    library_->addComponent(busDef);

    VLNV absVLNV2(VLNV::ABSTRACTIONDEFINITION, "vendor", "library", "name2", "version");
    QSharedPointer<AbstractionDefinition> abstraction2(new AbstractionDefinition(absVLNV2, Document::Revision::Std22));

    abstraction2->setBusType(busVLNV);

    QSharedPointer<PortAbstraction> testPort2(new PortAbstraction());
    testPort2->setLogicalName("testPort");
    abstraction2->getLogicalPorts()->append(testPort2);

    abstraction2->getChoices()->append(testChoice); // error 2 & 3: no name, no enumeration(s)

    QSharedPointer<WireAbstraction> wire2(new WireAbstraction());
    testPort2->setWire(wire2);

    QVector<QString> errorList;
    validator.findErrorsIn(errorList, abstraction);
    validator.findErrorsIn(errorList, abstraction2);
    QCOMPARE(errorList.size(), 3);
    QVERIFY(!validator.validate(abstraction));
    QVERIFY(!validator.validate(abstraction2));

    abstraction->getChoices()->clear();
    testChoice->setName("testChoice");
    
    QSharedPointer<Enumeration> testEnumeration(new Enumeration());
    testEnumeration->setValue("16");
    testChoice->enumerations()->append(testEnumeration);

    errorList.clear();
    validator.findErrorsIn(errorList, abstraction);
    validator.findErrorsIn(errorList, abstraction2);
    QCOMPARE(errorList.size(), 0);
    QVERIFY(validator.validate(abstraction));
    QVERIFY(validator.validate(abstraction2));
}

QTEST_APPLESS_MAIN(tst_AbstractionDefinitionValidator)

#include "tst_AbstractionDefinitionValidator.moc"

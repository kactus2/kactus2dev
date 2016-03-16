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

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <tests/MockObjects/LibraryMock.h>

#include <QtTest>
#include "IPXACTmodels/common/TimingConstraint.h"
#include "IPXACTmodels/common/CellSpecification.h"

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
	void paraFail();
	void portFail();
	void wireFail();
	void noDublicatePorts();
	void invalidWirePort();
	void invalidTransactionalPort();
	void invalidTimingConstraint();
	void invalidCellSpec();
	void wirePortSuccessful();
	void transactionalSuccessful();

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
	testBusDefVlnv_ = VLNV("type","vendor","library","test_bus_def","version");
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
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
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
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
	abs->setBusType(VLNV("type","vendor","library","bogus","version"));
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
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	abs->setExtends(VLNV("type","vendor","library","bogus","version"));

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 1);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::paraFail()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::paraFail()
{
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
	abs->setBusType(testBusDefVlnv_);
	abs->getLogicalPorts()->append(port_);

	QSharedPointer<Parameter> parameter (new Parameter());
	parameter->setName("param");
	parameter->setType("bit");
	parameter->setValue("{'b11, 'b00}");
	parameter->setValueId("parameterid");

	abs->getParameters()->append(parameter);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, abs);

	QCOMPARE(errorList.size(), 1);
	QVERIFY(!validator.validate(abs));
}

//-----------------------------------------------------------------------------
// Function: tst_AbstractionDefinitionValidator::portFail()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::portFail()
{
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
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
// Function: tst_AbstractionDefinitionValidator::noDublicatePorts()
//-----------------------------------------------------------------------------
void tst_AbstractionDefinitionValidator::noDublicatePorts()
{
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
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
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
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
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
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
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
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
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
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
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
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
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
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
	QSharedPointer<AbstractionDefinition> abs(new AbstractionDefinition);
	AbstractionDefinitionValidator validator(library_, expressionParser_);

	abs->setVlnv(VLNV("type","vendor","library","name","version"));
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

QTEST_APPLESS_MAIN(tst_AbstractionDefinitionValidator)

#include "tst_AbstractionDefinitionValidator.moc"

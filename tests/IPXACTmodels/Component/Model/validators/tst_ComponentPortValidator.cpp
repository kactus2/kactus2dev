//-----------------------------------------------------------------------------
// ComponentPort: tst_ComponentPortValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 25.11.2015
//
// Description:
// Unit test for class ComponentPortValidator.
//-----------------------------------------------------------------------------

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <IPXACTmodels/Component/validators/PortValidator.h>

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/Port.h>

#include <QtTest>

class tst_ComponentPortValidator : public QObject
{
	Q_OBJECT

public:
    tst_ComponentPortValidator();

private slots:
	void baseCase();
	void presenceFail();
	void arrayFail();
	void wireFail();
	void wireSuccess();
	void initiativeKindFail();
	void initiativeKindSuccess();
	void transactionalFail();
	void transactionalSuccess();
	void protocolFail();
	void protocolSuccess();

private:

	QSharedPointer<QList<QSharedPointer<View> > > views_;

    QSharedPointer<ExpressionParser> parser_;
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::tst_ComponentPortValidator()
//-----------------------------------------------------------------------------
tst_ComponentPortValidator::tst_ComponentPortValidator()
{
	views_ = QSharedPointer<QList<QSharedPointer<View> > >( new QList<QSharedPointer<View> > );
	QSharedPointer<View> testView( new View("test_view") );
	views_->append(testView);

    parser_ = QSharedPointer<SystemVerilogExpressionParser> (new SystemVerilogExpressionParser());
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::baseCase()
{
	QSharedPointer<Port> port( new Port );
    port->setLeftBound("4");
    port->setRightBound("4");
	PortValidator validator(parser_, views_);

	port->setName("joq");

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 0 );
    QVERIFY( validator.validate(port) );
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::presenceFail()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::presenceFail()
{
	QSharedPointer<Port> port( new Port );
	PortValidator validator(parser_, views_);

	port->setName("joq");
	port->setIsPresent("evoo");

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 1 );
    QVERIFY( !validator.validate(port) );
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::arrayFail()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::arrayFail()
{
	QSharedPointer<Port> port( new Port );
	PortValidator validator(parser_, views_);

	port->setName("joq");

	QSharedPointer<Array> arr( new Array("eka","toka") );
	port->getArrays()->append(arr);

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 2 );
    QVERIFY( !validator.validate(port) );
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::wireFail()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::wireFail()
{
	QSharedPointer<Port> port( new Port );
	PortValidator validator(parser_, views_);

	port->setName("joq");

	QSharedPointer<Wire> wire( new Wire );
	wire->setVectorLeftBound("bogus");
	wire->setVectorRightBound("myriad");
	port->setWire(wire);

	QSharedPointer<WireTypeDef> wiredef( new WireTypeDef );
	QStringList refs;
	refs.append("bogus view");
	wiredef->setViewRefs(refs);
	wire->getWireTypeDefs()->append(wiredef);

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 4 );
    QVERIFY( !validator.validate(port) );
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::wireSuccess()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::wireSuccess()
{
	QSharedPointer<Port> port( new Port );
	PortValidator validator(parser_, views_);

	port->setName("joq");

	QSharedPointer<Wire> wire( new Wire );
	wire->setVectorLeftBound("1");
	wire->setVectorRightBound("2");
    wire->setDirection(DirectionTypes::IN);
	port->setWire(wire);

	QSharedPointer<WireTypeDef> wiredef( new WireTypeDef );
	QStringList refs;
	refs.append("test_view");
	wiredef->setViewRefs(refs);
	wire->getWireTypeDefs()->append(wiredef);

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 0 );
    QVERIFY( validator.validate(port) );
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::initiativeKindFail()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::initiativeKindFail()
{
	QSharedPointer<Port> port( new Port );
	PortValidator validator(parser_, views_);

	port->setName("joq");

	QSharedPointer<Transactional> trans( new Transactional );
	trans->setInitiative("joq");
	trans->setKind("on");
	port->setTransactional(trans);

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 2 );
    QVERIFY( !validator.validate(port) );
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::initiativeKindSuccess()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::initiativeKindSuccess()
{
	QSharedPointer<Port> port( new Port );
	PortValidator validator(parser_, views_);

	port->setName("joq");

	QSharedPointer<Transactional> trans( new Transactional );
	trans->setInitiative("provides");
	trans->setKind("simple_socket");
	port->setTransactional(trans);

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 0 );
    QVERIFY( validator.validate(port) );
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::transactionalFail()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::transactionalFail()
{
	QSharedPointer<Port> port( new Port );
	PortValidator validator(parser_, views_);

	port->setName("joq");

	QSharedPointer<Transactional> trans( new Transactional );
	trans->setInitiative("requires");
	trans->setMinConnections("esa");
	trans->setMaxConnections("masa");
	trans->setBusWidth("eioo");
	port->setTransactional(trans);

	QSharedPointer<WireTypeDef> wiredef( new WireTypeDef );
	QStringList refs;
	refs.append("bogus view");
	wiredef->setViewRefs(refs);
	trans->getTransTypeDef()->append(wiredef);

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 4 );
    QVERIFY( !validator.validate(port) );
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::transactionalSuccess()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::transactionalSuccess()
{
	QSharedPointer<Port> port( new Port );
	PortValidator validator(parser_, views_);

	port->setName("joq");

	QSharedPointer<Transactional> trans( new Transactional );
	trans->setInitiative("requires");
	trans->setMinConnections("1");
	trans->setMaxConnections("10");
	trans->setBusWidth("16");
	port->setTransactional(trans);

	QSharedPointer<WireTypeDef> wiredef( new WireTypeDef );
	QStringList refs;
	refs.append("test_view");
	wiredef->setViewRefs(refs);
	trans->getTransTypeDef()->append(wiredef);

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 0 );
    QVERIFY( validator.validate(port) );
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::protocolFail()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::protocolFail()
{
	QSharedPointer<Port> port( new Port );
	PortValidator validator(parser_, views_);

	port->setName("joq");

	QSharedPointer<Transactional> trans( new Transactional );
	trans->setInitiative("requires");
	port->setTransactional(trans);

	QSharedPointer<Protocol> prot( new Protocol );
	prot->setPayloadExtension("eioota",true);
	prot->setPayloadName(" \t");
	prot->setProtocolType("type");
	trans->setProtocol(prot);

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 3 );
    QVERIFY( !validator.validate(port) );
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentPortValidator::protocolSuccess()
//-----------------------------------------------------------------------------
void tst_ComponentPortValidator::protocolSuccess()
{
	QSharedPointer<Port> port( new Port );
	PortValidator validator(parser_, views_);

	port->setName("joq");

	QSharedPointer<Transactional> trans( new Transactional );
	trans->setInitiative("requires");
	port->setTransactional(trans);

	QSharedPointer<Protocol> prot( new Protocol );
	prot->setPayloadExtension("mandatory",true);
	prot->setPayloadName("esa");
	prot->setProtocolType("tlm");
	prot->setPayloadType("generic");
	trans->setProtocol(prot);

	QVector<QString> errorList;
    validator.findErrorsIn(errorList, port, "test");

	QCOMPARE( errorList.size(), 0 );
    QVERIFY( validator.validate(port) );
}

QTEST_APPLESS_MAIN(tst_ComponentPortValidator)

#include "tst_ComponentPortValidator.moc"

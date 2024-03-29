//-----------------------------------------------------------------------------
// File: tst_ChannelValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 19.11.2015
//
// Description:
// Unit test for class ChannelValidator.
//-----------------------------------------------------------------------------


#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Channel.h>
#include <IPXACTmodels/Component/validators/ChannelValidator.h>

#include <KactusAPI/include/SystemVerilogExpressionParser.h>

#include <QtTest>

class tst_ChannelValidator : public QObject
{
	Q_OBJECT

public:
    tst_ChannelValidator();

private slots:
	void baseCase();
	void testOnlyOneInterfaceReferenceIsInvalid();
	void testReferenceToUnknownInterfaceIsInvalid();
    void testReferenceToNonMirroredInterfaceIsInvalid();

private:

    QSharedPointer<ExpressionParser> expressionParser_;
};

//-----------------------------------------------------------------------------
// Function: tst_ChannelValidator::tst_ChannelValidator()
//-----------------------------------------------------------------------------
tst_ChannelValidator::tst_ChannelValidator(): expressionParser_(new SystemVerilogExpressionParser())
{

}

//-----------------------------------------------------------------------------
// Function: tst_ChannelValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_ChannelValidator::baseCase()
{
    QSharedPointer<QList<QSharedPointer<BusInterface> > > availableInterfaces(
        new QList<QSharedPointer<BusInterface> >());

    QSharedPointer<BusInterface> firstInterface(new BusInterface());
    firstInterface->setName("first");
    firstInterface->setInterfaceMode(General::MIRRORED_MASTER);
    availableInterfaces->append(firstInterface);

    QSharedPointer<BusInterface> secondInterface(new BusInterface());
    secondInterface->setName("second");
    secondInterface->setInterfaceMode(General::MIRRORED_SLAVE);
    availableInterfaces->append(secondInterface);

    QSharedPointer<BusInterface> systemInterface(new BusInterface());
    systemInterface->setName("system");
    systemInterface->setInterfaceMode(General::MIRRORED_SYSTEM);
    availableInterfaces->append(systemInterface);

	QSharedPointer<Channel> channel(new Channel);
	channel->setName("testChannel");
	channel->setIsPresent("1");

    channel->getInterfaces()->append(QSharedPointer<Channel::BusInterfaceRef>(new Channel::BusInterfaceRef("first")));
    channel->getInterfaces()->append(QSharedPointer<Channel::BusInterfaceRef>(new Channel::BusInterfaceRef("second")));
    channel->getInterfaces()->append(QSharedPointer<Channel::BusInterfaceRef>(new Channel::BusInterfaceRef("system")));

	QVector<QString> errorList;
    ChannelValidator validator(expressionParser_, availableInterfaces);
	validator.findErrorsIn(errorList, channel, "test");

	QVERIFY(validator.validate(channel));
    QCOMPARE(errorList.size(), 0);
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelValidator::testOnlyOneInterfaceReferenceIsInvalid()
//-----------------------------------------------------------------------------
void tst_ChannelValidator::testOnlyOneInterfaceReferenceIsInvalid()
{
    QSharedPointer<QList<QSharedPointer<BusInterface> > > noInterfaces(new QList<QSharedPointer<BusInterface> >());

	QSharedPointer<Channel> channel(new Channel);
	channel->setName("testChannel");
    channel->setIsPresent("1");

    channel->getInterfaces()->append(QSharedPointer<Channel::BusInterfaceRef>(new Channel::BusInterfaceRef("nonExisting")));

	QVector<QString> errorList;
    ChannelValidator validator(expressionParser_, noInterfaces);
	validator.findErrorsIn(errorList, channel, "test");

	QVERIFY(!validator.validate(channel));
    QCOMPARE(errorList.size(), 2);
    QCOMPARE(errorList.first(), QStringLiteral("At least two interfaces must be defined for channel testChannel."));
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelValidator::testReferenceToUnknownInterfaceIsInvalid()
//-----------------------------------------------------------------------------
void tst_ChannelValidator::testReferenceToUnknownInterfaceIsInvalid()
{
    QSharedPointer<QList<QSharedPointer<BusInterface> > > availableInterfaces(
        new QList<QSharedPointer<BusInterface> >());

	QSharedPointer<Channel> channel(new Channel);
	channel->setName("testChannel");
	channel->setIsPresent("1");
 
    channel->getInterfaces()->append(QSharedPointer<Channel::BusInterfaceRef>(new Channel::BusInterfaceRef("nonExisting")));
    channel->getInterfaces()->append(QSharedPointer<Channel::BusInterfaceRef>(new Channel::BusInterfaceRef("")));

	QVector<QString> errorList;
    ChannelValidator validator(expressionParser_, availableInterfaces);
	validator.findErrorsIn(errorList, channel, "test");

	QVERIFY(!validator.validate(channel));
    QCOMPARE(errorList.size(), 2);
    QCOMPARE(errorList.last(), QStringLiteral(
        "Bus interface '' referenced within channel testChannel not found."));
    QCOMPARE(errorList.first(), QStringLiteral(
        "Bus interface 'nonExisting' referenced within channel testChannel not found."));
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelValidator::testReferenceToNonMirroredInterfaceIsInvalid()
//-----------------------------------------------------------------------------
void tst_ChannelValidator::testReferenceToNonMirroredInterfaceIsInvalid() 
{
    QSharedPointer<QList<QSharedPointer<BusInterface> > > availableInterfaces(
        new QList<QSharedPointer<BusInterface> >());

    QSharedPointer<BusInterface> slaveInterface(new BusInterface());
    slaveInterface->setName("slaveInterface");
    slaveInterface->setInterfaceMode(General::SLAVE);
    availableInterfaces->append(slaveInterface);

    QSharedPointer<BusInterface> masterInterface(new BusInterface());
    masterInterface->setName("masterInterface");
    masterInterface->setInterfaceMode(General::MASTER);
    availableInterfaces->append(masterInterface);

    QSharedPointer<BusInterface> systemInterface(new BusInterface());
    systemInterface->setName("systemInterface");
    systemInterface->setInterfaceMode(General::SYSTEM);
    availableInterfaces->append(systemInterface);

    QSharedPointer<Channel> channel(new Channel);
    channel->setName("testChannel");

    channel->getInterfaces()->append(QSharedPointer<Channel::BusInterfaceRef>(new Channel::BusInterfaceRef("slaveInterface")));
    channel->getInterfaces()->append(QSharedPointer<Channel::BusInterfaceRef>(new Channel::BusInterfaceRef("masterInterface")));
    channel->getInterfaces()->append(QSharedPointer<Channel::BusInterfaceRef>(new Channel::BusInterfaceRef("systemInterface")));

    QVector<QString> errorList;
    ChannelValidator validator(expressionParser_, availableInterfaces);
    validator.findErrorsIn(errorList, channel, "test");

    QVERIFY(!validator.validate(channel));
    QCOMPARE(errorList.size(), 3);
    QCOMPARE(errorList.at(0), QStringLiteral(
        "Bus interface 'slaveInterface' referenced within channel testChannel is not a mirrored interface."));
    QCOMPARE(errorList.at(1), QStringLiteral(
        "Bus interface 'masterInterface' referenced within channel testChannel is not a mirrored interface."));
    QCOMPARE(errorList.at(2), QStringLiteral(
        "Bus interface 'systemInterface' referenced within channel testChannel is not a mirrored interface."));
}


QTEST_APPLESS_MAIN(tst_ChannelValidator)

#include "tst_ChannelValidator.moc"

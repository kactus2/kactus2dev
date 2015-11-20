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

#include <IPXACTmodels/Component/validators/ChannelValidator.h>
#include <IPXACTmodels/Component/Channel.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <QtTest>

class tst_ChannelValidator : public QObject
{
	Q_OBJECT

public:
    tst_ChannelValidator();

private slots:
	void baseCase();
	void noIF();
	void ifNameFail();

private:
};

//-----------------------------------------------------------------------------
// Function: tst_ChannelValidator::tst_ChannelValidator()
//-----------------------------------------------------------------------------
tst_ChannelValidator::tst_ChannelValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ChannelValidator::baseCase()
//-----------------------------------------------------------------------------
void tst_ChannelValidator::baseCase()
{
	QSharedPointer<Channel> channel( new Channel );
	ChannelValidator validator;
	channel->setName("esa");

	channel->setIsPresent("1");
	QStringList interfaces;
	interfaces.append("jari");
	interfaces.append("pena");
	channel->setInterfaces(interfaces);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, channel, "test");

	QCOMPARE( errorList.size(), 0 );
	QVERIFY( validator.validate(channel) );
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelValidator::noIF()
//-----------------------------------------------------------------------------
void tst_ChannelValidator::noIF()
{
	QSharedPointer<Channel> channel( new Channel );
	ChannelValidator validator;
	channel->setName("esa");

	channel->setIsPresent("1");
	QStringList interfaces;
	interfaces.append("jari");
	channel->setInterfaces(interfaces);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, channel, "test");

	QCOMPARE( errorList.size(), 1 );
	QVERIFY( !validator.validate(channel) );
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelValidator::ifNameFail()
//-----------------------------------------------------------------------------
void tst_ChannelValidator::ifNameFail()
{
	QSharedPointer<Channel> channel( new Channel );
	ChannelValidator validator;
	channel->setName("esa");

	channel->setIsPresent("1");
	QStringList interfaces;
	interfaces.append("");
	interfaces.append(" \t");
	channel->setInterfaces(interfaces);

	QVector<QString> errorList;
	validator.findErrorsIn(errorList, channel, "test");

	QCOMPARE( errorList.size(), 2 );
	QVERIFY( !validator.validate(channel) );
}

QTEST_APPLESS_MAIN(tst_ChannelValidator)

#include "tst_ChannelValidator.moc"

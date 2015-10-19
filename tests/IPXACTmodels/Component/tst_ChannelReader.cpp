//-----------------------------------------------------------------------------
// File: tst_ChannelReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Unit test for class ChannelReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ChannelReader.h>

#include <QtTest>

class tst_ChannelReader : public QObject
{
    Q_OBJECT

public:
    tst_ChannelReader();

private slots:

    void testReadSimpleChannel();
    void testReadIsPresent();
	void testReadBusIfRef();
	void testReadBusIfRef2();
};

//-----------------------------------------------------------------------------
// Function: tst_ChannelReader::tst_ChannelReader()
//-----------------------------------------------------------------------------
tst_ChannelReader::tst_ChannelReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ChannelReader::testReadSimpleChannel()
//-----------------------------------------------------------------------------
void tst_ChannelReader::testReadSimpleChannel()
{
    QString documentContent(
        "<ipxact:channel>"
            "<ipxact:name>channel</ipxact:name>"
            "<ipxact:displayName>viewDisplay</ipxact:displayName>"
            "<ipxact:description>viewDescription</ipxact:description>"
        "</ipxact:channel>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode channelNode = document.firstChildElement("ipxact:channel");

    ChannelReader ChannelReader;
    QSharedPointer<Channel> testChannel = ChannelReader.createChannelFrom(channelNode);

    QCOMPARE(testChannel->name(), QString("channel"));
    QCOMPARE(testChannel->displayName(), QString("viewDisplay"));
    QCOMPARE(testChannel->description(), QString("viewDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelReader::testReadIsPresent()
//-----------------------------------------------------------------------------
void tst_ChannelReader::testReadIsPresent()
{
    QString documentContent(
        "<ipxact:channel>"
            "<ipxact:name>testChannel</ipxact:name>"
            "<ipxact:isPresent>4-3</ipxact:isPresent>"
        "</ipxact:channel>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode channelNode = document.firstChildElement("ipxact:channel");

    ChannelReader ChannelReader;
    QSharedPointer<Channel> testChannel = ChannelReader.createChannelFrom(channelNode);

    QCOMPARE(testChannel->name(), QString("testChannel"));
    QCOMPARE(testChannel->getIsPresent(), QString("4-3"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadBusIfRef()
//-----------------------------------------------------------------------------
void tst_ChannelReader::testReadBusIfRef()
{
	QString documentContent(
		"<ipxact:channel>"
		    "<ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:isPresent>4-3</ipxact:isPresent>"
		    "<ipxact:busInterfaceRef>"
		        "<ipxact:localName>interface1</ipxact:localName>"
		    "</ipxact:busInterfaceRef>"
		"</ipxact:channel>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode channelNode = document.firstChildElement("ipxact:channel");

	ChannelReader ChannelReader;
	QSharedPointer<Channel> testChannel = ChannelReader.createChannelFrom(channelNode);

	QCOMPARE(testChannel->name(), QString("testChannel"));
	QCOMPARE(testChannel->getIsPresent(), QString("4-3"));
	QCOMPARE(testChannel->getInterfaces().size(), 1);
	QCOMPARE(testChannel->getInterfaces().first(), QString("interface1"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadBusIfRef2()
//-----------------------------------------------------------------------------
void tst_ChannelReader::testReadBusIfRef2()
{
	QString documentContent(
		"<ipxact:channel>"
		"   <ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:isPresent>expression</ipxact:isPresent>"
		    "<ipxact:busInterfaceRef>"
		        "<ipxact:localName>firstInterface</ipxact:localName>"
		    "</ipxact:busInterfaceRef>"
		    "<ipxact:busInterfaceRef>"
		        "<ipxact:localName>secondInterface</ipxact:localName>"
		    "</ipxact:busInterfaceRef>"
		"</ipxact:channel>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode channelNode = document.firstChildElement("ipxact:channel");

	ChannelReader ChannelReader;
	QSharedPointer<Channel> testChannel = ChannelReader.createChannelFrom(channelNode);

	QCOMPARE(testChannel->name(), QString("testChannel"));
	QCOMPARE(testChannel->getIsPresent(), QString("expression"));
	QCOMPARE(testChannel->getInterfaces().size(), 2);
	QCOMPARE(testChannel->getInterfaces().first(), QString("firstInterface"));
	QCOMPARE(testChannel->getInterfaces().last(), QString("secondInterface"));
}

QTEST_APPLESS_MAIN(tst_ChannelReader)

#include "tst_ChannelReader.moc"

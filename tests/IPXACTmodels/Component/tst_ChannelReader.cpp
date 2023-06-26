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

    void testReadNamedChannel();
    void testReadNamedChannel2022();

    void testReadIsPresent();
    void testReadIsPresentNotAvailableIn2022();

	void testReadBusInterfaceReferences();
    void testReadBusInterfaceVendorExtensions2022();

    void testReadVendorExtensions2022();

};

//-----------------------------------------------------------------------------
// Function: tst_ChannelReader::tst_ChannelReader()
//-----------------------------------------------------------------------------
tst_ChannelReader::tst_ChannelReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ChannelReader::testReadNamedChannel()
//-----------------------------------------------------------------------------
void tst_ChannelReader::testReadNamedChannel()
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

    QSharedPointer<Channel> testChannel = ChannelReader::createChannelFrom(channelNode, Document::Revision::Std14);

    QCOMPARE(testChannel->name(), QString("channel"));
    QCOMPARE(testChannel->displayName(), QString("viewDisplay"));
    QCOMPARE(testChannel->description(), QString("viewDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelReader::testReadNamedChannel()
//-----------------------------------------------------------------------------
void tst_ChannelReader::testReadNamedChannel2022()
{
    QString documentContent(
        "<ipxact:channel>"
            "<ipxact:name>testChannel</ipxact:name>"
            "<ipxact:displayName>viewDisplay</ipxact:displayName>"
            "<ipxact:shortDescription>shortDisplay</ipxact:shortDescription>"
            "<ipxact:description>viewDescription</ipxact:description>"
        "</ipxact:channel>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode channelNode = document.firstChildElement("ipxact:channel");

    QSharedPointer<Channel> testChannel = ChannelReader::createChannelFrom(channelNode, Document::Revision::Std22);

    QCOMPARE(testChannel->name(), QString("testChannel"));
    QCOMPARE(testChannel->displayName(), QString("viewDisplay"));
    QCOMPARE(testChannel->shortDescription(), QString("shortDisplay"));
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
        "</ipxact:channel>");


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode channelNode = document.firstChildElement("ipxact:channel");

    QSharedPointer<Channel> testChannel = ChannelReader::createChannelFrom(channelNode, Document::Revision::Std14);

    QCOMPARE(testChannel->getIsPresent(), QString("4-3"));
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelReader::testReadIsPresentNotAvailableIn2022()
//-----------------------------------------------------------------------------
void tst_ChannelReader::testReadIsPresentNotAvailableIn2022()
{
    QString documentContent(
        "<ipxact:channel>"
            "<ipxact:name>testChannel</ipxact:name>"
            "<ipxact:isPresent>4-3</ipxact:isPresent>"
        "</ipxact:channel>");

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode channelNode = document.firstChildElement("ipxact:channel");

    QSharedPointer<Channel> testChannel = ChannelReader::createChannelFrom(channelNode, Document::Revision::Std22);

    QCOMPARE(testChannel->getIsPresent(), QString());
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadBusInterfaceReferences()
//-----------------------------------------------------------------------------
void tst_ChannelReader::testReadBusInterfaceReferences()
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

	QSharedPointer<Channel> testChannel = ChannelReader::createChannelFrom(channelNode, Document::Revision::Std14);

	QCOMPARE(testChannel->getIsPresent(), QString("expression"));
	QCOMPARE(testChannel->getInterfaces()->size(), 2);
	QCOMPARE(testChannel->getInterfaces()->first()->localName_, QString("firstInterface"));
	QCOMPARE(testChannel->getInterfaces()->last()->localName_, QString("secondInterface"));
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelReader::testReadBusInterfaceVendorExtensions()
//-----------------------------------------------------------------------------
void tst_ChannelReader::testReadBusInterfaceVendorExtensions2022()
{
    QString documentContent(
		"<ipxact:channel>"
		"   <ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:isPresent>expression</ipxact:isPresent>"
		    "<ipxact:busInterfaceRef>"
		        "<ipxact:localName>firstInterface</ipxact:localName>"
                "<ipxact:vendorExtensions>"
                    "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                "</ipxact:vendorExtensions>"
		    "</ipxact:busInterfaceRef>"

		"</ipxact:channel>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode channelNode = document.firstChildElement("ipxact:channel");

	QSharedPointer<Channel> testChannel = ChannelReader::createChannelFrom(channelNode, Document::Revision::Std22);

	QCOMPARE(testChannel->getIsPresent(), QString());
	QCOMPARE(testChannel->getInterfaces()->size(), 1);
    QCOMPARE(testChannel->getInterfaces()->first()->getVendorExtensions()->size(), 1);
    QCOMPARE(testChannel->getInterfaces()->first()->getVendorExtensions()->first()->type(), QString("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_CPUReader::testReadVendorExtensions()
//-----------------------------------------------------------------------------
void tst_ChannelReader::testReadVendorExtensions2022()
{
    QString documentContent(
        "<ipxact:channel>"
            "<ipxact:name>testChannel</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:channel>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode channelNode = document.firstChildElement("ipxact:channel");

    QSharedPointer<Channel> testChannel = ChannelReader::createChannelFrom(channelNode, Document::Revision::Std22);

    QCOMPARE(testChannel->getVendorExtensions()->size(), 1);
    QCOMPARE(testChannel->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_ChannelReader)

#include "tst_ChannelReader.moc"

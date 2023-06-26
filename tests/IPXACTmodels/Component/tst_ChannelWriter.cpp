//-----------------------------------------------------------------------------
// File: tst_ChannelWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Unit test for class ChannelWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ChannelWriter.h>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Value.h>

#include <QtTest>

class tst_ChannelWriter : public QObject
{
    Q_OBJECT

public:
	tst_ChannelWriter();

private slots:
	void init();
	void cleanup();

	void testWriteChannelNameGroup();
	void testWriteIsPresent();

	void testWriteBusRef();
	void testBusRefWriteVendorExtensions2022();

	void testWriteVendorExtensions2022();

private:

	QSharedPointer<Channel> testChannel_;
};

//-----------------------------------------------------------------------------
// Function: tst_ChannelWriter::tst_ChannelWriter()
//-----------------------------------------------------------------------------
tst_ChannelWriter::tst_ChannelWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ChannelWriter::init()
//-----------------------------------------------------------------------------
void tst_ChannelWriter::init()
{
	testChannel_ = QSharedPointer<Channel>(new Channel());
	testChannel_->setName("testChannel");
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ChannelWriter::cleanup()
{
	testChannel_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelWriter::testWriteChannelNameGroup()
//-----------------------------------------------------------------------------
void tst_ChannelWriter::testWriteChannelNameGroup()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testChannel_->setDisplayName("testDisplay");
    testChannel_->setDescription("testDescription");

	QString expectedOutput = 
		"<ipxact:channel>"
		    "<ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:displayName>testDisplay</ipxact:displayName>"
		    "<ipxact:description>testDescription</ipxact:description>"
		"</ipxact:channel>";

	ChannelWriter::writeChannel(xmlStreamWriter, testChannel_, Document::Revision::Std14);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelWriter::testWriteIsPresent()
//-----------------------------------------------------------------------------
void tst_ChannelWriter::testWriteIsPresent()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testChannel_->setIsPresent("4-3");

	QString expectedOutput(
		"<ipxact:channel>"
		    "<ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:isPresent>4-3</ipxact:isPresent>"
		"</ipxact:channel>"
		);

	ChannelWriter::writeChannel(xmlStreamWriter, testChannel_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelReader::testWriteBusRef()
//-----------------------------------------------------------------------------
void tst_ChannelWriter::testWriteBusRef()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testChannel_->setIsPresent("expression");

    QSharedPointer<Channel::BusInterfaceRef> firstRef(new Channel::BusInterfaceRef("firstIf"));

    QSharedPointer<Channel::BusInterfaceRef> secondRef(new Channel::BusInterfaceRef("secondIf"));

    testChannel_->getInterfaces()->append(firstRef);
    testChannel_->getInterfaces()->append(secondRef);

	QString expectedOutput(
		"<ipxact:channel>"
		    "<ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:isPresent>expression</ipxact:isPresent>"
		    "<ipxact:busInterfaceRef>"
		        "<ipxact:localName>firstIf</ipxact:localName>"
		    "</ipxact:busInterfaceRef>"
		    "<ipxact:busInterfaceRef>"
		        "<ipxact:localName>secondIf</ipxact:localName>"
		    "</ipxact:busInterfaceRef>"
		"</ipxact:channel>"
		);

	ChannelWriter::writeChannel(xmlStreamWriter, testChannel_, Document::Revision::Std14);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelWriter::testBusRefWriteVendorExtensions2022()
//-----------------------------------------------------------------------------
void tst_ChannelWriter::testBusRefWriteVendorExtensions2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Channel::BusInterfaceRef> busRef(new Channel::BusInterfaceRef("busIf"));

	busRef->getVendorExtensions()->append(QSharedPointer<VendorExtension>(
        new Kactus2Value("testExtension", "testValue")));

    testChannel_->getInterfaces()->append(busRef);

    QString expectedOutput =
        "<ipxact:channel>"
			"<ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:busInterfaceRef>"
		        "<ipxact:localName>busIf</ipxact:localName>"
				"<ipxact:vendorExtensions>"
					"<testExtension>testValue</testExtension>"
				"</ipxact:vendorExtensions>"
		    "</ipxact:busInterfaceRef>"
        "</ipxact:channel>";

    ChannelWriter::writeChannel(xmlStreamWriter, testChannel_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelWriter::testWriteVendorExtensions2022()
//-----------------------------------------------------------------------------
void tst_ChannelWriter::testWriteVendorExtensions2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);


	testChannel_->getVendorExtensions()->append(QSharedPointer<VendorExtension>(
		new Kactus2Value("testExtension", "testValue")));

    QString expectedOutput =
        "<ipxact:channel>"
			"<ipxact:name>testChannel</ipxact:name>"
			"<ipxact:vendorExtensions>"
				"<testExtension>testValue</testExtension>"
			"</ipxact:vendorExtensions>"
        "</ipxact:channel>";

	ChannelWriter::writeChannel(xmlStreamWriter, testChannel_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}


QTEST_APPLESS_MAIN(tst_ChannelWriter)

#include "tst_ChannelWriter.moc"

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
	void testWriteBusRef2();

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

	QString expectedOutput(
		"<ipxact:channel>"
		    "<ipxact:name>testChannel</ipxact:name>"
		"</ipxact:channel>"
		);

	ChannelWriter ChannelWriter;
	ChannelWriter.writeChannel(xmlStreamWriter, testChannel_);
	QCOMPARE(output, expectedOutput);

	expectedOutput.clear();
	output.clear();

	testChannel_->setDisplayName("testDisplay");
	expectedOutput = 
		"<ipxact:channel>"
		    "<ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:displayName>testDisplay</ipxact:displayName>"
		"</ipxact:channel>";

	ChannelWriter.writeChannel(xmlStreamWriter, testChannel_);
	QCOMPARE(output, expectedOutput);

	output.clear();
	expectedOutput.clear();

	testChannel_->setDescription("testDescription");
	expectedOutput = 
		"<ipxact:channel>"
		    "<ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:displayName>testDisplay</ipxact:displayName>"
		    "<ipxact:description>testDescription</ipxact:description>"
		"</ipxact:channel>";

	ChannelWriter.writeChannel(xmlStreamWriter, testChannel_);
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

	ChannelWriter ChannelWriter;
	ChannelWriter.writeChannel(xmlStreamWriter, testChannel_);

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

	QStringList refs = testChannel_->getInterfaces();
	refs.append("Joku");
	testChannel_->setInterfaces(refs);

	QString expectedOutput(
		"<ipxact:channel>"
		"<ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:isPresent>expression</ipxact:isPresent>"
		    "<ipxact:busInterfaceRef>"
		        "<ipxact:localName>Joku</ipxact:localName>"
		    "</ipxact:busInterfaceRef>"
		"</ipxact:channel>"
		);

	ChannelWriter ChannelWriter;
	ChannelWriter.writeChannel(xmlStreamWriter, testChannel_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ChannelReader::testWriteBusRef2()
//-----------------------------------------------------------------------------
void tst_ChannelWriter::testWriteBusRef2()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testChannel_->setIsPresent("expression");

	QStringList refs = testChannel_->getInterfaces();
	refs.append("Joku");
	refs.append("jotain");
	testChannel_->setInterfaces(refs);

	QString expectedOutput(
		"<ipxact:channel>"
		    "<ipxact:name>testChannel</ipxact:name>"
		    "<ipxact:isPresent>expression</ipxact:isPresent>"
		    "<ipxact:busInterfaceRef>"
		        "<ipxact:localName>Joku</ipxact:localName>"
		    "</ipxact:busInterfaceRef>"
		    "<ipxact:busInterfaceRef>"
		        "<ipxact:localName>jotain</ipxact:localName>"
		    "</ipxact:busInterfaceRef>"
		"</ipxact:channel>"
		);

	ChannelWriter channelWriter;
	channelWriter.writeChannel(xmlStreamWriter, testChannel_);

	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ChannelWriter)

#include "tst_ChannelWriter.moc"

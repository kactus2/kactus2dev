//-----------------------------------------------------------------------------
// File: tst_ApiDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 11.12.2015
//
// Description:
// Unit test for class ApiDefinitionWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/kactusExtensions/ApiDefinitionWriter.h>

#include <QtTest>

class tst_ApiDefinitionWriter : public QObject
{
    Q_OBJECT

public:
	tst_ApiDefinitionWriter();

private slots:
	void init();
	void cleanup();

	void baseCase();

private:

	QSharedPointer<ApiDefinition> testApiDefinition_;
};

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionWriter::tst_ApiDefinitionWriter()
//-----------------------------------------------------------------------------
tst_ApiDefinitionWriter::tst_ApiDefinitionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionWriter::init()
//-----------------------------------------------------------------------------
void tst_ApiDefinitionWriter::init()
{
	testApiDefinition_ = QSharedPointer<ApiDefinition>(new ApiDefinition());
}

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ApiDefinitionWriter::cleanup()
{
	testApiDefinition_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionWriter::baseCase()
//-----------------------------------------------------------------------------
void tst_ApiDefinitionWriter::baseCase()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QString expectedOutput(
		"<?xml version=\"1.0\"?>"
		"<ipxact:apiDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
		" xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\""
		" xmlns:kactus2=\"http://kactus2.cs.tut.fi\""
		" xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/"
		" http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
		"<ipxact:vendor></ipxact:vendor>"
		"<ipxact:library></ipxact:library>"
		"<ipxact:name></ipxact:name>"
		"<ipxact:version></ipxact:version>"
		"<kactus2:transferTypes/><kactus2:properties/>"
		"<ipxact:vendorExtensions/></ipxact:ApiDefinition>"
		);

	ApiDefinitionWriter apiDefinitionWriter;
	apiDefinitionWriter.writeApiDefinition(xmlStreamWriter, testApiDefinition_);
	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ApiDefinitionWriter)

#include "tst_ApiDefinitionWriter.moc"

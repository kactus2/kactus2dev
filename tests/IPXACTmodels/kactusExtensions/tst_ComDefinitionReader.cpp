//-----------------------------------------------------------------------------
// File: tst_ComDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// Unit test for class ComDefinitionReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/kactusExtensions/ComDefinitionReader.h>
#include <IPXACTmodels/kactusExtensions/ComDefinition.h>

#include <QtTest>

class tst_ComDefinitionReader : public QObject
{
    Q_OBJECT

public:
	tst_ComDefinitionReader();

private slots:
	void baseCase();
};

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionReader::tst_ComDefinitionReader()
//-----------------------------------------------------------------------------
tst_ComDefinitionReader::tst_ComDefinitionReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionReader::baseCase()
//-----------------------------------------------------------------------------
void tst_ComDefinitionReader::baseCase()
{
	QString documentContent(
		"<?xml version=\"1.0\"?>"
		"<kactus2:comDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
		" xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\""
		" xmlns:kactus2=\"http://kactus2.cs.tut.fi\""
		" xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/"
		" http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
		"<ipxact:vendor>me</ipxact:vendor>"
		"<ipxact:library>kurjasto</ipxact:library>"
		"<ipxact:name>def</ipxact:name>"
		"<ipxact:version>0.11</ipxact:version>"
		"<kactus2:transferTypes/>"
		"<kactus2:properties>"
		"<kactus2:property kactus2:name=\"max_transfer_size\" kactus2:required=\"true\""
		"kactus2:propertyType=\"integer\" kactus2:defaultValue=\"128\""
		"kactus2:description=\"Maximum size of a single transfer\"/>"
		"</kactus2:properties>"
		"<ipxact:vendorExtensions/>"
		"</kactus2:comDefinition>\n"
		);

    QDomDocument document;
    document.setContent(documentContent);

    ComDefinitionReader ComDefinitionReader;
    QSharedPointer<ComDefinition> testComDefinition = ComDefinitionReader.createComDefinitionFrom(document);

	QCOMPARE(testComDefinition->getVlnv().getVendor(), QString("me"));
	QCOMPARE(testComDefinition->getVlnv().getLibrary(), QString("kurjasto"));
	QCOMPARE(testComDefinition->getVlnv().getName(), QString("def"));
	QCOMPARE(testComDefinition->getVlnv().getVersion(), QString("0.11"));

	QSharedPointer<ComProperty> prop = testComDefinition->getProperties()->first();

	QCOMPARE(prop->name(), QString("max_transfer_size"));
	QCOMPARE(prop->isRequired(), true);
	QCOMPARE(prop->getType(), QString("integer"));
	QCOMPARE(prop->getDefaultValue(), QString("128"));
	QCOMPARE(prop->getDescription(), QString("Maximum size of a single transfer"));
}

QTEST_APPLESS_MAIN(tst_ComDefinitionReader)

#include "tst_ComDefinitionReader.moc"

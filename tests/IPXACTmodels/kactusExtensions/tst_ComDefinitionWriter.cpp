//-----------------------------------------------------------------------------
// File: tst_ComDefinitionWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 11.12.2015
//
// Description:
// Unit test for class ComDefinitionWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/kactusExtensions/ComDefinitionWriter.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_ComDefinitionWriter : public QObject
{
    Q_OBJECT

public:
	tst_ComDefinitionWriter();

private slots:
	void init();
	void cleanup();

    void testProperty();
	void allFields();

private:

	QSharedPointer<ComDefinition> testComDefinition_;
};

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionWriter::tst_ComDefinitionWriter()
//-----------------------------------------------------------------------------
tst_ComDefinitionWriter::tst_ComDefinitionWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionWriter::init()
//-----------------------------------------------------------------------------
void tst_ComDefinitionWriter::init()
{
	testComDefinition_ = QSharedPointer<ComDefinition>(new ComDefinition());
}

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ComDefinitionWriter::cleanup()
{
	testComDefinition_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionWriter::testProperty()
//-----------------------------------------------------------------------------
void tst_ComDefinitionWriter::testProperty()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testComDefinition_->setVlnv(VLNV(VLNV::APIDEFINITION,"me","kurjasto","def","0.11"));

    QSharedPointer<ComProperty> prop = QSharedPointer<ComProperty>( new ComProperty );
    testComDefinition_->getProperties()->append(prop);

    prop->setName("max_transfer_size");
    prop->setRequired( true);
    prop->setType("integer");
    prop->setDefaultValue("128");
    prop->setDescription("Maximum size of a single transfer");

    QString expectedOutput(
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
        "<kactus2:property name=\"max_transfer_size\" required=\"true\" "
        "propertyType=\"integer\" defaultValue=\"128\" "
        "description=\"Maximum size of a single transfer\"/>"
        "</kactus2:properties>"
        "</kactus2:comDefinition>\n"
        );

    ComDefinitionWriter comDefinitionWriter;
    comDefinitionWriter.writeComDefinition(xmlStreamWriter, testComDefinition_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionWriter::allFields()
//-----------------------------------------------------------------------------
void tst_ComDefinitionWriter::allFields()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testComDefinition_->setVlnv(VLNV(VLNV::APIDEFINITION,"me","kurjasto","def","0.11"));

	QSharedPointer<ComProperty> prop = QSharedPointer<ComProperty>( new ComProperty );
	testComDefinition_->getProperties()->append(prop);

	prop->setName("max_transfer_size");
	prop->setRequired( true);
	prop->setType("integer");
	prop->setDefaultValue("128");
    prop->setDescription("Maximum size of a single transfer");

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("vendorAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));
    QSharedPointer<QList<QSharedPointer<VendorExtension> > > vendorExtensions = testComDefinition_->getVendorExtensions();
    vendorExtensions->append(testExtension);

	QString expectedOutput(
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
		"<kactus2:property name=\"max_transfer_size\" required=\"true\" "
		"propertyType=\"integer\" defaultValue=\"128\" "
		"description=\"Maximum size of a single transfer\"/>"
        "</kactus2:properties>"
        "<ipxact:vendorExtensions>"
        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
        "</ipxact:vendorExtensions>"
		"</kactus2:comDefinition>\n"
		);

	ComDefinitionWriter comDefinitionWriter;
	comDefinitionWriter.writeComDefinition(xmlStreamWriter, testComDefinition_);
	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ComDefinitionWriter)

#include "tst_ComDefinitionWriter.moc"

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
    void testProperty();
    void allFields();
};

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionReader::tst_ComDefinitionReader()
//-----------------------------------------------------------------------------
tst_ComDefinitionReader::tst_ComDefinitionReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionReader::testProperty()
//-----------------------------------------------------------------------------
void tst_ComDefinitionReader::testProperty()
{
    QString documentContent(
        "<?xml version=\"1.0\"?>"
        "<kactus2:comDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
        " xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\""
        " xmlns:kactus2=\"http://kactus2.cs.tut.fi\""
        " xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/"
        " http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
        "<kactus2:properties>"
        "<kactus2:property name=\"default_target\" required=\"true\""
        "propertyType=\"string\" defaultValue=\"moon\""
        "description=\"Where stuff is sent by default.\"/>"
        "</kactus2:properties>"
        "</kactus2:comDefinition>\n"
        );

    QDomDocument document;
    document.setContent(documentContent);

    ComDefinitionReader ComDefinitionReader;
    QSharedPointer<ComDefinition> testComDefinition = ComDefinitionReader.createComDefinitionFrom(document);

    QCOMPARE(testComDefinition->getProperties()->size(), 1);

    QSharedPointer<ComProperty> prop = testComDefinition->getProperties()->first();

    QCOMPARE(prop->name(), QString("default_target"));
    QCOMPARE(prop->isRequired(), true);
    QCOMPARE(prop->getType(), QString("string"));
    QCOMPARE(prop->getDefaultValue(), QString("moon"));
    QCOMPARE(prop->getDescription(), QString("Where stuff is sent by default."));
}

//-----------------------------------------------------------------------------
// Function: tst_ComDefinitionReader::allFields()
//-----------------------------------------------------------------------------
void tst_ComDefinitionReader::allFields()
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
        "<kactus2:property name=\"max_transfer_size\" required=\"true\""
        "propertyType=\"integer\" defaultValue=\"128\""
        "description=\"Maximum size of a single transfer\"/>"
        "</kactus2:properties>"
        "<ipxact:vendorExtensions>"
        "<testExtension vendorAttribute=\"extension\">testValue</testExtension>"
        "</ipxact:vendorExtensions>"
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

    QCOMPARE(testComDefinition->getProperties()->size(), 1);

    QSharedPointer<ComProperty> prop = testComDefinition->getProperties()->first();

    QCOMPARE(prop->name(), QString("max_transfer_size"));
    QCOMPARE(prop->isRequired(), true);
    QCOMPARE(prop->getType(), QString("integer"));
    QCOMPARE(prop->getDefaultValue(), QString("128"));
    QCOMPARE(prop->getDescription(), QString("Maximum size of a single transfer"));

    QCOMPARE(testComDefinition->getVendorExtensions()->size(), 1);
}

QTEST_APPLESS_MAIN(tst_ComDefinitionReader)

#include "tst_ComDefinitionReader.moc"

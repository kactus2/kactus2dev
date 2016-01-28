//-----------------------------------------------------------------------------
// File: tst_ApiDefinitionReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 28.01.2016
//
// Description:
// Unit test for class ApiDefinitionReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/kactusExtensions/ApiDefinitionReader.h>
#include <IPXACTmodels/kactusExtensions/ApiDefinition.h>

#include <QtTest>

class tst_ApiDefinitionReader : public QObject
{
    Q_OBJECT

public:
	tst_ApiDefinitionReader();

private slots:
	void baseCase();
};

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionReader::tst_ApiDefinitionReader()
//-----------------------------------------------------------------------------
tst_ApiDefinitionReader::tst_ApiDefinitionReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ApiDefinitionReader::baseCase()
//-----------------------------------------------------------------------------
void tst_ApiDefinitionReader::baseCase()
{
	QString documentContent(
		"<?xml version=\"1.0\"?>"
		"<kactus2:apiDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
		" xmlns:ipxact=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014\""
		" xmlns:kactus2=\"http://kactus2.cs.tut.fi\""
		" xsi:schemaLocation=\"http://www.accellera.org/XMLSchema/IPXACT/1685-2014/"
		" http://www.accellera.org/XMLSchema/IPXACT/1685-2014/index.xsd\">"
		"<ipxact:vendor>me</ipxact:vendor>"
		"<ipxact:library>kurjasto</ipxact:library>"
		"<ipxact:name>def</ipxact:name>"
		"<ipxact:version>0.11</ipxact:version>"
		"<kactus2:comDefinitionRef ipxact:vendor=\"he\" ipxact:library=\"s-lib\" ipxact:name=\"cdef\" ipxact:version=\"1.337\"/>"
		"<kactus2:dataTypes><kactus2:dataType kactus2:name=\"new item\"/></kactus2:dataTypes>"
		"<kactus2:functions>"
		"<kactus2:function kactus2:name=\"hjjhjh\" kactus2:description=\"yryry\">"
		"<kactus2:returnValue kactus2:type=\"void\" kactus2:description=\"lolol\"/>"
		"</kactus2:function>"
		"</kactus2:functions>"
		"<ipxact:vendorExtensions/>"
		"</kactus2:apiDefinition>\n"
		);

    QDomDocument document;
    document.setContent(documentContent);

    ApiDefinitionReader ApiDefinitionReader;
    QSharedPointer<ApiDefinition> testApiDefinition = ApiDefinitionReader.createApiDefinitionFrom(document);

	QCOMPARE(testApiDefinition->getVlnv().getVendor(), QString("me"));
	QCOMPARE(testApiDefinition->getVlnv().getLibrary(), QString("kurjasto"));
	QCOMPARE(testApiDefinition->getVlnv().getName(), QString("def"));
	QCOMPARE(testApiDefinition->getVlnv().getVersion(), QString("0.11"));

	QCOMPARE(testApiDefinition->getComDefinitionRef().getVendor(), QString("he"));
	QCOMPARE(testApiDefinition->getComDefinitionRef().getLibrary(), QString("s-lib"));
	QCOMPARE(testApiDefinition->getComDefinitionRef().getName(), QString("cdef"));
	QCOMPARE(testApiDefinition->getComDefinitionRef().getVersion(), QString("1.337"));

	QCOMPARE(testApiDefinition->getDataTypes()->first(), QString("new item"));

	QCOMPARE(testApiDefinition->getFunctions()->first()->name(), QString("hjjhjh"));
	QCOMPARE(testApiDefinition->getFunctions()->first()->getDescription(), QString("yryry"));
	QCOMPARE(testApiDefinition->getFunctions()->first()->getReturnValueType(), QString("void"));
	QCOMPARE(testApiDefinition->getFunctions()->first()->getReturnValueDescription(), QString("lolol"));
}

QTEST_APPLESS_MAIN(tst_ApiDefinitionReader)

#include "tst_ApiDefinitionReader.moc"

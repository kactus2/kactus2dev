//-----------------------------------------------------------------------------
// File: tst_ComponentGeneratorReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Unit test for class ComponentGeneratorReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ComponentGeneratorReader.h>
#include <IPXACTmodels/VendorExtension.h>

#include <QtTest>

class tst_ComponentGeneratorReader : public QObject
{
    Q_OBJECT

public:
    tst_ComponentGeneratorReader();

private slots:

    void testReadSimpleComponentGenerator();
	void testReadAttributes();
	void testReadGroups();
	void testReadParameters();
	void testRead2Parameters();
	void testReadPhase();
	void testReadApiType();
	void testReadVendorExtension();
	void testReadTransportMethods();
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::tst_ComponentGeneratorReader()
//-----------------------------------------------------------------------------
tst_ComponentGeneratorReader::tst_ComponentGeneratorReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadSimpleComponentGenerator()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadSimpleComponentGenerator()
{
    QString documentContent(
		"<ipxact:componentGenerator>"
		"<ipxact:name>test</ipxact:name>"
		"<ipxact:displayName>Display</ipxact:displayName>"
		"<ipxact:description>Description</ipxact:description>"
        "</ipxact:componentGenerator>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

    ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator = ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->name(), QString("test"));
	QCOMPARE(testComponentGenerator->displayName(), QString("Display"));
	QCOMPARE(testComponentGenerator->description(), QString("Description"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadAttributes()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadAttributes()
{
	QString documentContent(
		"<ipxact:componentGenerator scope=\"entity\" hidden=\"true\">"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator = ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->getScope(), ComponentGenerator::ENTITY);
	QCOMPARE(testComponentGenerator->getHidden(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadGroups()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadGroups()
{
	QString documentContent(
		"<ipxact:componentGenerator>"
		"<ipxact:group>test</ipxact:group>"
		"<ipxact:group>toka</ipxact:group>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator = ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->getGroups().size(), 2);
	QCOMPARE(testComponentGenerator->getGroups().first(), QString("test"));
	QCOMPARE(testComponentGenerator->getGroups().last(), QString("toka"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadParameters()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadParameters()
{
	QString documentContent(
		"<ipxact:componentGenerator>"
		"<ipxact:parameters>"
		"<ipxact:parameter parameterId=\"joq\" prompt=\"Parm 1\""
		"type=\"shortint\" resolve=\"user\">"
		"<ipxact:name>Esko</ipxact:name>"
		"<ipxact:description>First generator parameter.</ipxact:description>"
		"<ipxact:value>5</ipxact:value>"
		"</ipxact:parameter>"
		"</ipxact:parameters>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator = ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->getParameters().first()->name(), QString("Esko"));
	QCOMPARE(testComponentGenerator->getParameters().first()->getValueId(), QString("joq"));
	QCOMPARE(testComponentGenerator->getParameters().first()->getValue(), QString("5"));
	QCOMPARE(testComponentGenerator->getParameters().first()->getType(), QString("shortint"));
	QCOMPARE(testComponentGenerator->getParameters().first()->getAttribute("prompt"), QString("Parm 1"));
	QCOMPARE(testComponentGenerator->getParameters().first()->getAttribute("resolve"), QString("user"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testRead2Parameters()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testRead2Parameters()
{
	QString documentContent(
		"<ipxact:componentGenerator>"
		"<ipxact:parameters>"
		"<ipxact:parameter parameterId=\"joq\" prompt=\"Parm 1\""
		"type=\"shortint\" resolve=\"user\">"
		"<ipxact:name>Esko</ipxact:name>"
		"<ipxact:description>First generator parameter.</ipxact:description>"
		"<ipxact:value>5</ipxact:value>"
		"</ipxact:parameter>"
		"<ipxact:parameter parameterId=\"ev0\" prompt=\"Parm 1\""
		"type=\"shortint\" resolve=\"user\">"
		"<ipxact:name>Mikko</ipxact:name>"
		"<ipxact:description>First generator parameter.</ipxact:description>"
		"<ipxact:value>1337</ipxact:value>"
		"</ipxact:parameter>"
		"</ipxact:parameters>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator = ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->getParameters().first()->name(), QString("Esko"));
	QCOMPARE(testComponentGenerator->getParameters().size(), 2);
	QCOMPARE(testComponentGenerator->getParameters().first()->name(), QString("Esko"));
	QCOMPARE(testComponentGenerator->getParameters().first()->getValueId(), QString("joq"));
	QCOMPARE(testComponentGenerator->getParameters().first()->getValue(), QString("5"));
	QCOMPARE(testComponentGenerator->getParameters().last()->name(), QString("Mikko"));
	QCOMPARE(testComponentGenerator->getParameters().last()->getValueId(), QString("ev0"));
	QCOMPARE(testComponentGenerator->getParameters().last()->getValue(), QString("1337"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadPhase()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadPhase()
{
	QString documentContent(
		"<ipxact:componentGenerator>"
		"<ipxact:phase>13.37</ipxact:phase>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator =
		ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->getPhase(), 13.37);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadApiType()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadApiType()
{
	QString documentContent(
		"<ipxact:componentGenerator>"
		"<ipxact:apiType>TGI_2014_EXTENDED</ipxact:apiType>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator =
		ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->getApiType(), ComponentGenerator::TGI_2014_EXTENDED);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadVendorExtension()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadVendorExtension()
{
	QString documentContent(
		"<ipxact:componentGenerator>"
		"<ipxact:vendorExtensions>"
		"<ulina kolina=\"eaa\"/>"
		"</ipxact:vendorExtensions>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator =
		ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->getVendorExtensions()->first()->type(), QString("ulina"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadTransportMethods()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadTransportMethods()
{
	QString documentContent(
		"<ipxact:componentGenerator>"
		"<ipxact:transportMethods>"
		"<ipxact:transportMethod>eka</ipxact:transportMethod>"
		"<ipxact:transportMethod>toka</ipxact:transportMethod>"
		"</ipxact:transportMethods>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator =
		ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->getTransportMethods().size(), 2);
	QCOMPARE(testComponentGenerator->getTransportMethods().first(), QString("eka"));
	QCOMPARE(testComponentGenerator->getTransportMethods().last(), QString("toka"));
}

QTEST_APPLESS_MAIN(tst_ComponentGeneratorReader)

#include "tst_ComponentGeneratorReader.moc"

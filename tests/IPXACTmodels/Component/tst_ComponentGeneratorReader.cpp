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
#include <IPXACTmodels/common/VendorExtension.h>

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
            "<ipxact:generatorExe>/bin/generator.exe</ipxact:generatorExe>"
        "</ipxact:componentGenerator>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode componentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

    ComponentGeneratorReader componentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator =
        componentGeneratorReader.createComponentGeneratorFrom(componentGeneratorNode);

	QCOMPARE(testComponentGenerator->name(), QStringLiteral("test"));
	QCOMPARE(testComponentGenerator->displayName(), QStringLiteral("Display"));
	QCOMPARE(testComponentGenerator->description(), QStringLiteral("Description"));
    QCOMPARE(testComponentGenerator->getGeneratorExe(), QStringLiteral("/bin/generator.exe"));
    	
    QVERIFY(testComponentGenerator->getHidden().toString().isEmpty());
    QVERIFY(testComponentGenerator->getScope() == ComponentGenerator::NO_SCOPE);
    QVERIFY(testComponentGenerator->getPhase().isEmpty());
    QVERIFY(testComponentGenerator->getApiType() == ComponentGenerator::EMPTY_API_TYPE);    
    QVERIFY(testComponentGenerator->getTransportMethods().isEmpty());
    QVERIFY(testComponentGenerator->getGroups().isEmpty());
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadAttributes()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadAttributes()
{
	QString documentContent(
		"<ipxact:componentGenerator hidden=\"true\" scope=\"entity\">"
		"</ipxact:componentGenerator>"
	);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode componentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader componentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator = 
        componentGeneratorReader.createComponentGeneratorFrom(componentGeneratorNode);

	QCOMPARE(testComponentGenerator->getScope(), ComponentGenerator::ENTITY);
	QCOMPARE(testComponentGenerator->getHidden().toBool(), true);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadGroups()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadGroups()
{
	QString documentContent(
        "<ipxact:componentGenerator scope=\"instance\">"
		    "<ipxact:group>testGroup</ipxact:group>"
		    "<ipxact:group>secondTestGroup</ipxact:group>"
		"</ipxact:componentGenerator>"
	);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode componentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader componentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator = 
        componentGeneratorReader.createComponentGeneratorFrom(componentGeneratorNode);

	QCOMPARE(testComponentGenerator->getGroups().size(), 2);
	QCOMPARE(testComponentGenerator->getGroups().first(), QStringLiteral("testGroup"));
	QCOMPARE(testComponentGenerator->getGroups().last(), QStringLiteral("secondTestGroup"));

    QCOMPARE(testComponentGenerator->getScope(), ComponentGenerator::INSTANCE);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadParameters()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadParameters()
{
	QString documentContent(
		"<ipxact:componentGenerator>"
		    "<ipxact:parameters>"
		        "<ipxact:parameter parameterId=\"id\" prompt=\"Parm 1\" type=\"shortint\" resolve=\"user\">"
		            "<ipxact:name>testParameter</ipxact:name>"
		            "<ipxact:description>First generator parameter.</ipxact:description>"
		            "<ipxact:value>5</ipxact:value>"
		        "</ipxact:parameter>"
		    "</ipxact:parameters>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode componentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader componentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator = 
        componentGeneratorReader.createComponentGeneratorFrom(componentGeneratorNode);

	QCOMPARE(testComponentGenerator->getParameters()->first()->name(), QStringLiteral("testParameter"));
	QCOMPARE(testComponentGenerator->getParameters()->first()->getValueId(), QStringLiteral("id"));
	QCOMPARE(testComponentGenerator->getParameters()->first()->getValue(), QStringLiteral("5"));
	QCOMPARE(testComponentGenerator->getParameters()->first()->getType(), QStringLiteral("shortint"));
	QCOMPARE(testComponentGenerator->getParameters()->first()->getAttribute("prompt"), QStringLiteral("Parm 1"));
	QCOMPARE(testComponentGenerator->getParameters()->first()->getAttribute("resolve"), QStringLiteral("user"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testRead2Parameters()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testRead2Parameters()
{
	QString documentContent(
		"<ipxact:componentGenerator>"
		    "<ipxact:parameters>"
		        "<ipxact:parameter parameterId=\"id1\">"
		            "<ipxact:name>testParameter</ipxact:name>"
		            "<ipxact:description>First generator parameter.</ipxact:description>"
		            "<ipxact:value>5</ipxact:value>"
		        "</ipxact:parameter>"
		        "<ipxact:parameter parameterId=\"id2\">"
		            "<ipxact:name>secondTestParameter</ipxact:name>"
		            "<ipxact:value>1337</ipxact:value>"
		        "</ipxact:parameter>"
		    "</ipxact:parameters>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode componentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader componentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator = 
        componentGeneratorReader.createComponentGeneratorFrom(componentGeneratorNode);

	QCOMPARE(testComponentGenerator->getParameters()->size(), 2);
	QCOMPARE(testComponentGenerator->getParameters()->first()->name(), QStringLiteral("testParameter"));
	QCOMPARE(testComponentGenerator->getParameters()->first()->getValueId(), QStringLiteral("id1"));
	QCOMPARE(testComponentGenerator->getParameters()->first()->getValue(), QStringLiteral("5"));

	QCOMPARE(testComponentGenerator->getParameters()->last()->name(), QStringLiteral("secondTestParameter"));
	QCOMPARE(testComponentGenerator->getParameters()->last()->getValueId(), QStringLiteral("id2"));
	QCOMPARE(testComponentGenerator->getParameters()->last()->getValue(), QStringLiteral("1337"));
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

	QDomNode componentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader componentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator =
		componentGeneratorReader.createComponentGeneratorFrom(componentGeneratorNode);

	QCOMPARE(testComponentGenerator->getPhase(), QStringLiteral("13.37"));
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

	QDomNode componentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader componentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator =
		componentGeneratorReader.createComponentGeneratorFrom(componentGeneratorNode);

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
		        "<testExtension attribute=\"value\"/>"
		    "</ipxact:vendorExtensions>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator =
		ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->getVendorExtensions()->first()->type(), QStringLiteral("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorReader::testReadTransportMethods()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorReader::testReadTransportMethods()
{
	QString documentContent(
		"<ipxact:componentGenerator>"
		    "<ipxact:transportMethods>"
		        "<ipxact:transportMethod>file</ipxact:transportMethod>"
		    "</ipxact:transportMethods>"
		"</ipxact:componentGenerator>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode ComponentGeneratorNode = document.firstChildElement("ipxact:componentGenerator");

	ComponentGeneratorReader ComponentGeneratorReader;
	QSharedPointer<ComponentGenerator> testComponentGenerator =
		ComponentGeneratorReader.createComponentGeneratorFrom(ComponentGeneratorNode);

	QCOMPARE(testComponentGenerator->getTransportMethods().size(), 1);
	QCOMPARE(testComponentGenerator->getTransportMethods().first(), QStringLiteral("file"));
}

QTEST_APPLESS_MAIN(tst_ComponentGeneratorReader)

#include "tst_ComponentGeneratorReader.moc"

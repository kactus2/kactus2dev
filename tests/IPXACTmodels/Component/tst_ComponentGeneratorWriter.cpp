//-----------------------------------------------------------------------------
// File: tst_ComponentGeneratorWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 05.10.2015
//
// Description:
// Unit test for class ComponentGeneratorWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ComponentGeneratorWriter.h>

#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

#include <QtTest>

class tst_ComponentGeneratorWriter : public QObject
{
    Q_OBJECT

public:
	tst_ComponentGeneratorWriter();

private slots:
	void init();
	void cleanup();

	void testWriteAttributes();
	void testWriteComponentGenerator();
	void testWritePhase();
	void testWriteAPI();
	void testWriteGeneratorExe();
	void testWriteGroups();
	void testWriteParameter();
    void testWriteCompleteGenerator();

private:

	QSharedPointer<ComponentGenerator> testComponentGenerator_;
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::tst_ComponentGeneratorWriter()
//-----------------------------------------------------------------------------
tst_ComponentGeneratorWriter::tst_ComponentGeneratorWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::init()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::init()
{
	testComponentGenerator_ = QSharedPointer<ComponentGenerator>(new ComponentGenerator());
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::cleanup()
{
	testComponentGenerator_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteAttributes()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteAttributes()
{
    testComponentGenerator_->setName("testGenerator");
	testComponentGenerator_->setHidden(true);
	testComponentGenerator_->setScope(ComponentGenerator::ENTITY);

	QString expectedOutput(
		"<ipxact:componentGenerator hidden=\"true\" scope=\"entity\">"
		    "<ipxact:name>testGenerator</ipxact:name>"		
		"</ipxact:componentGenerator>"
		);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteComponentGenerator()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteComponentGenerator()
{
	testComponentGenerator_->setName("test");
	testComponentGenerator_->setDisplayName("testDisplay");
	testComponentGenerator_->setDescription("testDescription");

	QString expectedOutput(
		"<ipxact:componentGenerator>"
		    "<ipxact:name>test</ipxact:name>"
		    "<ipxact:displayName>testDisplay</ipxact:displayName>"
		    "<ipxact:description>testDescription</ipxact:description>"
		"</ipxact:componentGenerator>"
		);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWritePhase()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWritePhase()
{
    testComponentGenerator_->setName("testGenerator");
	testComponentGenerator_->setPhase("13.37");

	QString expectedOutput(
		"<ipxact:componentGenerator>"
		    "<ipxact:name>testGenerator</ipxact:name>"
		    "<ipxact:phase>13.37</ipxact:phase>"
		"</ipxact:componentGenerator>"
		);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteAPI()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteAPI()
{
    testComponentGenerator_->setName("testGenerator");
	testComponentGenerator_->setApiType(ComponentGenerator::TGI_2014_EXTENDED);

	QString expectedOutput(
		"<ipxact:componentGenerator>"
		    "<ipxact:name>testGenerator</ipxact:name>"
		    "<ipxact:apiType>TGI_2014_EXTENDED</ipxact:apiType>"
		"</ipxact:componentGenerator>"
		);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteGeneratorExe()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteGeneratorExe()
{
    testComponentGenerator_->setName("testGenerator");
	testComponentGenerator_->setGeneratorExe("/bin/bash.exe");

	QString expectedOutput(
		"<ipxact:componentGenerator>"
		    "<ipxact:name>testGenerator</ipxact:name>"
		    "<ipxact:generatorExe>/bin/bash.exe</ipxact:generatorExe>"
		"</ipxact:componentGenerator>"
		);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteGroups()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteGroups()
{
    testComponentGenerator_->setName("testGenerator");

	QStringList groups;
	groups.append("makeGenerator");
	groups.append("hdlGenerator");
	testComponentGenerator_->setGroups(groups);

	QString expectedOutput(
		"<ipxact:componentGenerator>"
		    "<ipxact:name>testGenerator</ipxact:name>"
		    "<ipxact:group>makeGenerator</ipxact:group>"
		    "<ipxact:group>hdlGenerator</ipxact:group>"
		"</ipxact:componentGenerator>"
		);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteParameter()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteParameter()
{
	QSharedPointer<Parameter> testParameter(new Parameter());
	testParameter->setName("generatorParameter");
	testParameter->setValueId("id1");
	testParameter->setValue("5");
	testParameter->setType("shortint");
	testParameter->setAttribute("prompt", "Parm 1");
	testParameter->setAttribute("resolve", "user");
	testParameter->setDescription("First generator parameter.");

	testComponentGenerator_->getParameters()->append(testParameter);

	QString expectedOutput(
		"<ipxact:componentGenerator>"
		    "<ipxact:name></ipxact:name>"
		    "<ipxact:parameters>"
		        "<ipxact:parameter parameterId=\"id1\" prompt=\"Parm 1\" resolve=\"user\" type=\"shortint\">"
		            "<ipxact:name>generatorParameter</ipxact:name>"
		            "<ipxact:description>First generator parameter.</ipxact:description>"
		            "<ipxact:value>5</ipxact:value>"
		        "</ipxact:parameter>"
		    "</ipxact:parameters>"
		"</ipxact:componentGenerator>"
		);

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteCompleteGenerator()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteCompleteGenerator()
{
    testComponentGenerator_->setHidden(false);
    testComponentGenerator_->setScope(ComponentGenerator::INSTANCE);
    testComponentGenerator_->setName("fullGenerator");
    testComponentGenerator_->setPhase("id1");
    
    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setName("phaseParameter");
    testParameter->setValueId("id1");
    testParameter->setValue("1");
 
    testComponentGenerator_->getParameters()->append(testParameter);

    testComponentGenerator_->setApiType(ComponentGenerator::TGI_2009);
    
    QStringList transportMethods;
    transportMethods << "file";
    testComponentGenerator_->setTransportMethods(transportMethods);

    testComponentGenerator_->setGeneratorExe("/bin/tools/generators/do_all.exe");
    
    QSharedPointer<VendorExtension> testExtension(new Kactus2Placeholder("kactus2:testExtension"));
    testComponentGenerator_->getVendorExtensions()->append(testExtension);

    QStringList testGroups;
    testGroups << "globalGenerators";
    testComponentGenerator_->setGroups(testGroups);

    QString expectedOutput(
        "<ipxact:componentGenerator hidden=\"false\" scope=\"instance\">"
            "<ipxact:name>fullGenerator</ipxact:name>"
            "<ipxact:phase>id1</ipxact:phase>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"id1\">"
                    "<ipxact:name>phaseParameter</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
            "<ipxact:apiType>TGI_2009</ipxact:apiType>"
            "<ipxact:transportMethods>"
                "<ipxact:transportMethod>file</ipxact:transportMethod>"
            "</ipxact:transportMethods>"
            "<ipxact:generatorExe>/bin/tools/generators/do_all.exe</ipxact:generatorExe>"
            "<ipxact:vendorExtensions>"
                "<kactus2:testExtension/>"
            "</ipxact:vendorExtensions>"
            "<ipxact:group>globalGenerators</ipxact:group>"            
        "</ipxact:componentGenerator>"
        );

    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    ComponentGeneratorWriter ComponentGeneratorWriter;
    ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ComponentGeneratorWriter)

#include "tst_ComponentGeneratorWriter.moc"

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
#include <IPXACTmodels/VendorExtension.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/GenericVendorExtension.h>

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
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testComponentGenerator_->setHidden(true);
	testComponentGenerator_->setScope(ComponentGenerator::ENTITY);

	QString expectedOutput(
		"<ipxact:componentGenerator hidden=\"true\" scope=\"entity\">"
		"<ipxact:name></ipxact:name>"
		"<ipxact:apiType>TGI_2014_BASE</ipxact:apiType>"
		"</ipxact:componentGenerator>"
		);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteComponentGenerator()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteComponentGenerator()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testComponentGenerator_->setName("test");
	testComponentGenerator_->setDisplayName("testDisplay");
	testComponentGenerator_->setDescription("testDescription");

	QString expectedOutput(
		"<ipxact:componentGenerator hidden=\"false\" scope=\"instance\">"
		"<ipxact:name>test</ipxact:name>"
		"<ipxact:displayName>testDisplay</ipxact:displayName>"
		"<ipxact:description>testDescription</ipxact:description>"
		"<ipxact:apiType>TGI_2014_BASE</ipxact:apiType>"
		"</ipxact:componentGenerator>"
		);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWritePhase()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWritePhase()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testComponentGenerator_->setPhase(13.37);

	QString expectedOutput(
		"<ipxact:componentGenerator hidden=\"false\" scope=\"instance\">"
		"<ipxact:name></ipxact:name>"
		"<ipxact:phase>13.37</ipxact:phase>"
		"<ipxact:apiType>TGI_2014_BASE</ipxact:apiType>"
		"</ipxact:componentGenerator>"
		);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteAPI()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteAPI()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testComponentGenerator_->setApiType(ComponentGenerator::TGI_2014_EXTENDED);

	QString expectedOutput(
		"<ipxact:componentGenerator hidden=\"false\" scope=\"instance\">"
		"<ipxact:name></ipxact:name>"
		"<ipxact:apiType>TGI_2014_EXTENDED</ipxact:apiType>"
		"</ipxact:componentGenerator>"
		);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteGeneratorExe()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteGeneratorExe()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testComponentGenerator_->setGeneratorExe("baker street");

	QString expectedOutput(
		"<ipxact:componentGenerator hidden=\"false\" scope=\"instance\">"
		"<ipxact:name></ipxact:name>"
		"<ipxact:apiType>TGI_2014_BASE</ipxact:apiType>"
		"<ipxact:generatorExe>baker street</ipxact:generatorExe>"
		"</ipxact:componentGenerator>"
		);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteGroups()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteGroups()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QStringList groups;
	groups.append("esa");
	groups.append("pekka");
	testComponentGenerator_->setGroups(groups);

	QString expectedOutput(
		"<ipxact:componentGenerator hidden=\"false\" scope=\"instance\">"
		"<ipxact:name></ipxact:name>"
		"<ipxact:apiType>TGI_2014_BASE</ipxact:apiType>"
		"<ipxact:group>esa</ipxact:group>"
		"<ipxact:group>pekka</ipxact:group>"
		"</ipxact:componentGenerator>"
		);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentGeneratorWriter::testWriteParameter()
//-----------------------------------------------------------------------------
void tst_ComponentGeneratorWriter::testWriteParameter()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QSharedPointer<Parameter> para1(new Parameter());
	para1->setName("Esko");
	para1->setValueId("joq");
	para1->setValue("5");
	para1->setType("shortint");
	para1->setAttribute("prompt", "Parm 1");
	para1->setAttribute("resolve", "user");
	para1->setDescription("First generator parameter.");

	QList<QSharedPointer<Parameter> > parameters;
	parameters.append(para1);
	testComponentGenerator_->setParameters(parameters);

	QString expectedOutput(
		"<ipxact:componentGenerator hidden=\"false\" scope=\"instance\">"
		"<ipxact:name></ipxact:name>"
		"<ipxact:apiType>TGI_2014_BASE</ipxact:apiType>"
		"<ipxact:parameters>"
		"<ipxact:parameter parameterId=\"joq\" prompt=\"Parm 1\""
		" resolve=\"user\" type=\"shortint\">"
		"<ipxact:name>Esko</ipxact:name>"
		"<ipxact:description>First generator parameter.</ipxact:description>"
		"<ipxact:value>5</ipxact:value>"
		"</ipxact:parameter>"
		"</ipxact:parameters>"
		"</ipxact:componentGenerator>"
		);

	ComponentGeneratorWriter ComponentGeneratorWriter;
	ComponentGeneratorWriter.writeComponentGenerator(xmlStreamWriter, testComponentGenerator_);
	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ComponentGeneratorWriter)

#include "tst_ComponentGeneratorWriter.moc"

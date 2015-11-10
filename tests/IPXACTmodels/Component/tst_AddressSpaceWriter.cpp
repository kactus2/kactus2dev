//-----------------------------------------------------------------------------
// File: tst_AddressSpaceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Unit test for class AddressSpaceWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/AddressSpaceWriter.h>
#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>

#include <QtTest>

class tst_AddressSpaceWriter : public QObject
{
	Q_OBJECT

public:
	tst_AddressSpaceWriter();

	private slots:
		void init();
		void cleanup();

		void testWriteAddressSpaceNameGroup();
		void testWriteIsPresent();
		void testWriteParameter();
		void testWriteVendorExtension();
		void testWriteBlockSize();
		void testWriteAddressUnitBits();
		void testWriteSegments();
		void writeAddressBlocks();

private:

	QSharedPointer<AddressSpace> testAddressSpace_;
};

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceWriter::tst_AddressSpaceWriter()
//-----------------------------------------------------------------------------
tst_AddressSpaceWriter::tst_AddressSpaceWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceWriter::init()
//-----------------------------------------------------------------------------
void tst_AddressSpaceWriter::init()
{
	testAddressSpace_ = QSharedPointer<AddressSpace>(new AddressSpace());
	testAddressSpace_->setName("testAddressSpace");
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_AddressSpaceWriter::cleanup()
{
	testAddressSpace_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceWriter::testWriteAddressSpaceNameGroup()
//-----------------------------------------------------------------------------
void tst_AddressSpaceWriter::testWriteAddressSpaceNameGroup()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testAddressSpace_->setDisplayName("testDisplay");
	testAddressSpace_->setDescription("testDescription");

	QString expectedOutput = 
		"<ipxact:addressSpace>"
		"<ipxact:name>testAddressSpace</ipxact:name>"
		"<ipxact:displayName>testDisplay</ipxact:displayName>"
		"<ipxact:description>testDescription</ipxact:description>"
        "<ipxact:range></ipxact:range>"
		"<ipxact:width></ipxact:width>"
		"</ipxact:addressSpace>"
		;

	AddressSpaceWriter AddressSpaceWriter;
	AddressSpaceWriter.writeAddressSpace(xmlStreamWriter, testAddressSpace_);
	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceWriter::testWriteIsPresent()
//-----------------------------------------------------------------------------
void tst_AddressSpaceWriter::testWriteIsPresent()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testAddressSpace_->setIsPresent("4-3");

	QString expectedOutput(
		"<ipxact:addressSpace>"
		"<ipxact:name>testAddressSpace</ipxact:name>"
		"<ipxact:isPresent>4-3</ipxact:isPresent>"
        "<ipxact:range></ipxact:range>"
        "<ipxact:width></ipxact:width>"
		"</ipxact:addressSpace>"
		);

	AddressSpaceWriter AddressSpaceWriter;
	AddressSpaceWriter.writeAddressSpace(xmlStreamWriter, testAddressSpace_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::testWriteParameter()
//-----------------------------------------------------------------------------
void tst_AddressSpaceWriter::testWriteParameter()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testAddressSpace_->setIsPresent("4-3");

	QSharedPointer<Parameter> para1(new Parameter());
	para1->setName("Esko");
	para1->setValueId("joq");
	para1->setValue("5");
	para1->setType("shortint");
	para1->setAttribute("prompt", "Parm 1");
	para1->setAttribute("resolve", "user");
	para1->setDescription("First generator parameter.");

    testAddressSpace_->getParameters()->append(para1);

	QString expectedOutput(
		"<ipxact:addressSpace>"
		"<ipxact:name>testAddressSpace</ipxact:name>"
		"<ipxact:isPresent>4-3</ipxact:isPresent>"
        "<ipxact:range></ipxact:range>"
        "<ipxact:width></ipxact:width>"
		"<ipxact:parameters>"
		"<ipxact:parameter parameterId=\"joq\" prompt=\"Parm 1\""
		" resolve=\"user\" type=\"shortint\">"
		"<ipxact:name>Esko</ipxact:name>"
		"<ipxact:description>First generator parameter.</ipxact:description>"
		"<ipxact:value>5</ipxact:value>"
		"</ipxact:parameter>"
		"</ipxact:parameters>"
		"</ipxact:addressSpace>"
		);

	AddressSpaceWriter AddressSpaceWriter;
	AddressSpaceWriter.writeAddressSpace(xmlStreamWriter, testAddressSpace_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::testWriteVendorExtension()
//-----------------------------------------------------------------------------
void tst_AddressSpaceWriter::testWriteVendorExtension()
{
	QDomDocument document;
	QDomElement extensionNode = document.createElement("ulina");
	extensionNode.setAttribute("kolina", "eaa");
	extensionNode.appendChild(document.createTextNode("testValue"));

	QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

	testAddressSpace_->getVendorExtensions()->append(testExtension);

	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testAddressSpace_->setName("joq");

	QString expectedOutput(
		"<ipxact:addressSpace>"
		"<ipxact:name>joq</ipxact:name>"
        "<ipxact:range></ipxact:range>"
        "<ipxact:width></ipxact:width>"
		"<ipxact:vendorExtensions>"
		"<ulina kolina=\"eaa\">"
		"testValue"
		"</ulina>"
		"</ipxact:vendorExtensions>"
		"</ipxact:addressSpace>"
		);

	AddressSpaceWriter AddressSpaceWriter;
	AddressSpaceWriter.writeAddressSpace(xmlStreamWriter, testAddressSpace_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::testWriteBlockSize()
//-----------------------------------------------------------------------------
void tst_AddressSpaceWriter::testWriteBlockSize()
{
	QDomDocument document;

	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    testAddressSpace_->setRange("testRange");
	testAddressSpace_->setWidth("123");

	QString expectedOutput(
		"<ipxact:addressSpace>"
		"<ipxact:name>testAddressSpace</ipxact:name>"
        "<ipxact:range>testRange</ipxact:range>"
        "<ipxact:width>123</ipxact:width>"
		"</ipxact:addressSpace>"
		);

	AddressSpaceWriter AddressSpaceWriter;
	AddressSpaceWriter.writeAddressSpace(xmlStreamWriter, testAddressSpace_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceWriter::testWriteAddressUnitBits()
//-----------------------------------------------------------------------------
void tst_AddressSpaceWriter::testWriteAddressUnitBits()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	testAddressSpace_->setAddressUnitBits("44");

	QString expectedOutput(
		"<ipxact:addressSpace>"
		"<ipxact:name>testAddressSpace</ipxact:name>"
        "<ipxact:range></ipxact:range>"
        "<ipxact:width></ipxact:width>"
		"<ipxact:addressUnitBits>44</ipxact:addressUnitBits>"
		"</ipxact:addressSpace>"
		);

	AddressSpaceWriter AddressSpaceWriter;
	AddressSpaceWriter.writeAddressSpace(xmlStreamWriter, testAddressSpace_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceWriter::testWriteSegments()
//-----------------------------------------------------------------------------
void tst_AddressSpaceWriter::testWriteSegments()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QSharedPointer<Segment> seg1( new Segment() );
	seg1->setName("segment1");
	seg1->setIsPresent("4-3");
	seg1->setOffset("0x10000000");
	seg1->setRange("32M");

	QSharedPointer<Segment> seg2( new Segment() );
	seg2->setName("segment2");
	seg2->setDisplayName("display");
	seg2->setDescription("description");
	seg2->setIsPresent("11");
	seg2->setOffset("0x00000080");
	seg2->setRange("64");

    testAddressSpace_->getSegments()->append(seg1);
    testAddressSpace_->getSegments()->append(seg2);

	QString expectedOutput(
		"<ipxact:addressSpace>"
		"<ipxact:name>testAddressSpace</ipxact:name>"
        "<ipxact:range></ipxact:range>"
        "<ipxact:width></ipxact:width>"
		"<ipxact:segments>"
		"<ipxact:segment>"
		"<ipxact:name>segment1</ipxact:name>"
		"<ipxact:isPresent>4-3</ipxact:isPresent>"
		"<ipxact:addressOffset>0x10000000</ipxact:addressOffset>"
		"<ipxact:range>32M</ipxact:range>"
		"</ipxact:segment>"
		"<ipxact:segment>"
		"<ipxact:name>segment2</ipxact:name>"
		"<ipxact:displayName>display</ipxact:displayName>"
		"<ipxact:description>description</ipxact:description>"
		"<ipxact:isPresent>11</ipxact:isPresent>"
		"<ipxact:addressOffset>0x00000080</ipxact:addressOffset>"
		"<ipxact:range>64</ipxact:range>"
		"</ipxact:segment>"
		"</ipxact:segments>"
		"</ipxact:addressSpace>"
		);

	AddressSpaceWriter AddressSpaceWriter;
	AddressSpaceWriter.writeAddressSpace(xmlStreamWriter, testAddressSpace_);

	QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceWriter::writeAddressBlocks()
//-----------------------------------------------------------------------------
void tst_AddressSpaceWriter::writeAddressBlocks()
{
	QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

	QSharedPointer<Field> simpleField(new Field("simpleField"));
	simpleField->setBitOffset("fieldOffset");
	simpleField->setBitWidth("fieldWidth");

	QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
	testRegister->setDisplayName("displayed");
	testRegister->setDescription("described");
	testRegister->getFields()->append(simpleField);

	QSharedPointer<RegisterFile> containedRegisterFile
		(new RegisterFile("contained", "containedOffset", "containedRange"));

	QSharedPointer<AddressBlock> testAddressBlock =
		QSharedPointer<AddressBlock>(new AddressBlock("testBlock", "Tur-quan Masters"));
	testAddressBlock->setRange("Kuusistolinna");
	testAddressBlock->setWidth("Taidemuseo");
	testAddressBlock->getRegisterData()->append(testRegister);
	testAddressBlock->getRegisterData()->append(containedRegisterFile);

	QSharedPointer<MemoryMapBase> testMemoryMapBase( new MemoryMapBase());
	testMemoryMapBase->getMemoryBlocks()->append(testAddressBlock);
	testAddressSpace_->setLocalMemoryMap(testMemoryMapBase);

	QString expectedOutput(
		"<ipxact:addressSpace>"
		"<ipxact:name>testAddressSpace</ipxact:name>"
        "<ipxact:range></ipxact:range>"
        "<ipxact:width></ipxact:width>"
		"<ipxact:localMemoryMap>"
		"<ipxact:name></ipxact:name>"
		"<ipxact:addressBlock>"
		"<ipxact:name>testBlock</ipxact:name>"
		"<ipxact:baseAddress>Tur-quan Masters</ipxact:baseAddress>"
		"<ipxact:range>Kuusistolinna</ipxact:range>"
		"<ipxact:width>Taidemuseo</ipxact:width>"
		"<ipxact:register>"
		"<ipxact:name>testRegister</ipxact:name>"
		"<ipxact:displayName>displayed</ipxact:displayName>"
		"<ipxact:description>described</ipxact:description>"
		"<ipxact:addressOffset>offset</ipxact:addressOffset>"
		"<ipxact:size>10</ipxact:size>"
		"<ipxact:field>"
		"<ipxact:name>simpleField</ipxact:name>"
		"<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
		"<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
		"</ipxact:field>"
		"</ipxact:register>"
		"<ipxact:registerFile>"
		"<ipxact:name>contained</ipxact:name>"
		"<ipxact:addressOffset>containedOffset</ipxact:addressOffset>"
		"<ipxact:range>containedRange</ipxact:range>"
		"</ipxact:registerFile>"
		"</ipxact:addressBlock>"
		"</ipxact:localMemoryMap>"
		"</ipxact:addressSpace>"
		);

	AddressSpaceWriter addressSpaceWriter;
	addressSpaceWriter.writeAddressSpace(xmlStreamWriter, testAddressSpace_);
	QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_AddressSpaceWriter)

#include "tst_AddressSpaceWriter.moc"

//-----------------------------------------------------------------------------
// File: tst_AddressSpaceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 30.09.2015
//
// Description:
// Unit test for class AddressSpaceReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/AddressSpaceReader.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>

#include <QtTest>

class tst_AddressSpaceReader : public QObject
{
    Q_OBJECT

public:
    tst_AddressSpaceReader();

private slots:

    void testReadSimpleAddressSpace();
	void testReadIsPresent();
	void testReadParameters();
	void testReadBlockSize();
	void testReadAddressUnitBits();
	void testReadVendorExtensions();
	void testReadSegments();
	void readAddressBlocks();
};

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::tst_AddressSpaceReader()
//-----------------------------------------------------------------------------
tst_AddressSpaceReader::tst_AddressSpaceReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::testReadSimpleAddressSpace()
//-----------------------------------------------------------------------------
void tst_AddressSpaceReader::testReadSimpleAddressSpace()
{
    QString documentContent(
        "<ipxact:addressSpace>"
            "<ipxact:name>AddressSpace</ipxact:name>"
            "<ipxact:displayName>viewDisplay</ipxact:displayName>"
            "<ipxact:description>viewDescription</ipxact:description>"
        "</ipxact:addressSpace>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode AddressSpaceNode = document.firstChildElement("ipxact:addressSpace");

    AddressSpaceReader AddressSpaceReader;
    QSharedPointer<AddressSpace> testAddressSpace = AddressSpaceReader.createAddressSpaceFrom(AddressSpaceNode);

    QCOMPARE(testAddressSpace->name(), QString("AddressSpace"));
    QCOMPARE(testAddressSpace->displayName(), QString("viewDisplay"));
    QCOMPARE(testAddressSpace->description(), QString("viewDescription"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::testReadIsPresent()
//-----------------------------------------------------------------------------
void tst_AddressSpaceReader::testReadIsPresent()
{
    QString documentContent(
        "<ipxact:addressSpace>"
            "<ipxact:name>testAddressSpace</ipxact:name>"
            "<ipxact:isPresent>4-3</ipxact:isPresent>"
        "</ipxact:addressSpace>"
        );


    QDomDocument document;
    document.setContent(documentContent);

    QDomNode AddressSpaceNode = document.firstChildElement("ipxact:addressSpace");

    AddressSpaceReader AddressSpaceReader;
    QSharedPointer<AddressSpace> testAddressSpace = AddressSpaceReader.createAddressSpaceFrom(AddressSpaceNode);

    QCOMPARE(testAddressSpace->name(), QString("testAddressSpace"));
    QCOMPARE(testAddressSpace->getIsPresent(), QString("4-3"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::testReadParameters()
//-----------------------------------------------------------------------------
void tst_AddressSpaceReader::testReadParameters()
{
	QString documentContent(
		"<ipxact:addressSpace>"
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
		"</ipxact:addressSpace>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode AddressSpaceNode = document.firstChildElement("ipxact:addressSpace");

	AddressSpaceReader addressSpaceReader;
	QSharedPointer<AddressSpace> testAddressSpace = addressSpaceReader.createAddressSpaceFrom(AddressSpaceNode);

	QCOMPARE(testAddressSpace->getParameters()->size(), 2);
	QCOMPARE(testAddressSpace->getParameters()->first()->name(), QString("Esko"));
	QCOMPARE(testAddressSpace->getParameters()->first()->getValueId(), QString("joq"));
	QCOMPARE(testAddressSpace->getParameters()->first()->getValue(), QString("5"));
	QCOMPARE(testAddressSpace->getParameters()->last()->name(), QString("Mikko"));
	QCOMPARE(testAddressSpace->getParameters()->last()->getValueId(), QString("ev0"));
	QCOMPARE(testAddressSpace->getParameters()->last()->getValue(), QString("1337"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::testReadBlockSize()
//-----------------------------------------------------------------------------
void tst_AddressSpaceReader::testReadBlockSize()
{
	QString documentContent(
		"<ipxact:addressSpace>"
		"<ipxact:range>testRange</ipxact:range>"
		"<ipxact:width>123</ipxact:width>"
		"</ipxact:addressSpace>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode AddressSpaceNode = document.firstChildElement("ipxact:addressSpace");

	AddressSpaceReader AddressSpaceReader;
	QSharedPointer<AddressSpace> testAddressSpace = AddressSpaceReader.createAddressSpaceFrom(AddressSpaceNode);

    QCOMPARE(testAddressSpace->getRange(), QString("testRange"));
	QCOMPARE(testAddressSpace->getWidth(), QString("123"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::testReadAddressUnitBits()
//-----------------------------------------------------------------------------
void tst_AddressSpaceReader::testReadAddressUnitBits()
{
	QString documentContent(
		"<ipxact:addressSpace>"
		"<ipxact:addressUnitBits>15</ipxact:addressUnitBits>"
		"</ipxact:addressSpace>"
		);


	QDomDocument document;
	document.setContent(documentContent);

	QDomNode AddressSpaceNode = document.firstChildElement("ipxact:addressSpace");

	AddressSpaceReader AddressSpaceReader;
	QSharedPointer<AddressSpace> testAddressSpace = AddressSpaceReader.createAddressSpaceFrom(AddressSpaceNode);

	QCOMPARE(testAddressSpace->getAddressUnitBits(), QString("15"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::testReadVendorExtensions()
//-----------------------------------------------------------------------------
void tst_AddressSpaceReader::testReadVendorExtensions()
{
	QString documentContent(
		"<ipxact:addressSpace>"
		"<ipxact:vendorExtensions>"
		"<ulina kolina=\"eaa\"/>"
		"</ipxact:vendorExtensions>"
		"</ipxact:addressSpace>"
		);

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode addressSpaceNode = document.firstChildElement("ipxact:addressSpace");

	AddressSpaceReader AddressSpaceReader;
	QSharedPointer<AddressSpace> testAddressSpace = AddressSpaceReader.createAddressSpaceFrom(addressSpaceNode);

	QCOMPARE(testAddressSpace->getVendorExtensions()->first()->type(), QString("ulina"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::testReadSegments()
//-----------------------------------------------------------------------------
void tst_AddressSpaceReader::testReadSegments()
{
	QString documentContent(
		"<ipxact:addressSpace>"
		"<ipxact:segments>"
		"<ipxact:segment>"
		"<ipxact:name>segment1</ipxact:name>"
		"<ipxact:isPresent>4-3</ipxact:isPresent>"
		"<ipxact:addressOffset eka=\"toka\">0x10000000</ipxact:addressOffset>"
		"<ipxact:range joku=\"jotain\">32M</ipxact:range>"
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

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode addressSpaceNode = document.firstChildElement("ipxact:addressSpace");

	AddressSpaceReader AddressSpaceReader;
	QSharedPointer<AddressSpace> testAddressSpace = AddressSpaceReader.createAddressSpaceFrom(addressSpaceNode);

	QCOMPARE(testAddressSpace->getSegments()->size(), 2);

	QSharedPointer<Segment> segment1 = testAddressSpace->getSegments()->first();

	QCOMPARE(segment1->getIsPresent(), QString("4-3"));
	QCOMPARE(segment1->getAddressOffset(), QString("0x10000000"));
	QCOMPARE(segment1->getOffsetAttributes().firstKey(), QString("eka"));
	QCOMPARE(segment1->getOffsetAttributes().first(), QString("toka"));
	QCOMPARE(segment1->getRange(), QString("32M"));
	QCOMPARE(segment1->getRangeAttributes().firstKey(), QString("joku"));
	QCOMPARE(segment1->getRangeAttributes().first(), QString("jotain"));

	QSharedPointer<Segment> segment2 = testAddressSpace->getSegments()->last();

	QCOMPARE(segment2->name(), QString("segment2"));
	QCOMPARE(segment2->description(), QString("description"));
	QCOMPARE(segment2->displayName(), QString("display"));
	QCOMPARE(segment2->getIsPresent(), QString("11"));
	QCOMPARE(segment2->getAddressOffset(), QString("0x00000080"));
	QCOMPARE(segment2->getRange(), QString("64"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressSpaceReader::readAddressBlocks()
//-----------------------------------------------------------------------------
void tst_AddressSpaceReader::readAddressBlocks()
{
	QString documentContent(
		"<ipxact:addressSpace>"
		"<ipxact:localMemoryMap>"
		"<ipxact:name>testMemoryMapBase</ipxact:name>"
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

	QDomDocument document;
	document.setContent(documentContent);

	QDomNode addressSpaceNode = document.firstChildElement("ipxact:addressSpace");

	AddressSpaceReader AddressSpaceReader;
	QSharedPointer<AddressSpace> testAddressSpace = AddressSpaceReader.createAddressSpaceFrom(addressSpaceNode);

	QSharedPointer<MemoryMapBase> testMemoryMapBase = testAddressSpace->getLocalMemoryMap();

	QCOMPARE(testMemoryMapBase->getMemoryBlocks()->size(), 1);

	QSharedPointer<AddressBlock> testAddressBlock =
		testMemoryMapBase->getMemoryBlocks()->first().dynamicCast<AddressBlock>();
	QCOMPARE(testAddressBlock.isNull(), false);
	QCOMPARE(testAddressBlock->name(), QString("testBlock"));
	QCOMPARE(testAddressBlock->getBaseAddress(), QString("Tur-quan Masters"));
	QCOMPARE(testAddressBlock->getRange(), QString("Kuusistolinna"));
	QCOMPARE(testAddressBlock->getWidth(), QString("Taidemuseo"));

	QCOMPARE(testAddressBlock->getRegisterData()->size(), 2);

	QSharedPointer<Register> containedRegister =
		testAddressBlock->getRegisterData()->first().dynamicCast<Register>();
	QCOMPARE(containedRegister.isNull(), false);
	QCOMPARE(containedRegister->name(), QString("testRegister"));
	QCOMPARE(containedRegister->displayName(), QString("displayed"));
	QCOMPARE(containedRegister->description(), QString("described"));
	QCOMPARE(containedRegister->getSize(), QString("10"));

	QCOMPARE(containedRegister->getFields()->size(), 1);
	QCOMPARE(containedRegister->getFields()->first()->name(), QString("simpleField"));
	QCOMPARE(containedRegister->getFields()->first()->getBitOffset(), QString("fieldOffset"));
	QCOMPARE(containedRegister->getFields()->first()->getBitWidth(), QString("fieldWidth"));

	QSharedPointer<RegisterFile> containedFile =
		testAddressBlock->getRegisterData()->last().dynamicCast<RegisterFile>();
	QCOMPARE(containedFile.isNull(), false);
	QCOMPARE(containedFile->name(), QString("contained"));
	QCOMPARE(containedFile->getAddressOffset(), QString("containedOffset"));
	QCOMPARE(containedFile->getRange(), QString("containedRange"));
}

QTEST_APPLESS_MAIN(tst_AddressSpaceReader)

#include "tst_AddressSpaceReader.moc"

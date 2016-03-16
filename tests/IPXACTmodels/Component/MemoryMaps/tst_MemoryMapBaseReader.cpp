//-----------------------------------------------------------------------------
// File: tst_MemoryMapBaseReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Unit test for class MemoryMapBaseReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/MemoryMapBaseReader.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryRemap.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_MemoryMapBaseReader : public QObject
{
    Q_OBJECT

public:
    tst_MemoryMapBaseReader();

private slots:

    void readSimpleMemoryMapBase();
    void readIsPresent();
    void readAddressBlocks();
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseReader::tst_MemoryMapBaseReader()
//-----------------------------------------------------------------------------
tst_MemoryMapBaseReader::tst_MemoryMapBaseReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseReader::readSimpleMemoryMapBase()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseReader::readSimpleMemoryMapBase()
{
    QString documentContent(
        "<ipxact:MemoryMapBase>"
            "<ipxact:name>testMemoryMapBase</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
        "</ipxact:MemoryMapBase>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode MemoryMapBaseNode = document.firstChildElement("ipxact:MemoryMapBase");

    MemoryMapBaseReader MemoryMapBaseReader;
    QSharedPointer<MemoryMapBase> testMemoryMapBase( new MemoryMapBase );
	MemoryMapBaseReader.readMemoryMapBase(MemoryMapBaseNode, testMemoryMapBase);

    QCOMPARE(testMemoryMapBase->name(), QString("testMemoryMapBase"));
    QCOMPARE(testMemoryMapBase->displayName(), QString("displayed"));
    QCOMPARE(testMemoryMapBase->description(), QString("described"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseReader::readIsPresent()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseReader::readIsPresent()
{
    QString documentContent(
        "<ipxact:MemoryMapBase>"
            "<ipxact:name>testMemoryMapBase</ipxact:name>"
            "<ipxact:isPresent>8/8-1</ipxact:isPresent>"
        "</ipxact:MemoryMapBase>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode MemoryMapBaseNode = document.firstChildElement("ipxact:MemoryMapBase");

	MemoryMapBaseReader MemoryMapBaseReader;
	QSharedPointer<MemoryMapBase> testMemoryMapBase( new MemoryMapBase );
	MemoryMapBaseReader.readMemoryMapBase(MemoryMapBaseNode, testMemoryMapBase);

    QCOMPARE(testMemoryMapBase->name(), QString("testMemoryMapBase"));
    QCOMPARE(testMemoryMapBase->getIsPresent(), QString("8/8-1"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseReader::readAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseReader::readAddressBlocks()
{
    QString documentContent(
        "<ipxact:MemoryMapBase>"
            "<ipxact:name>testMemoryMapBase</ipxact:name>"
            "<ipxact:addressBlock>"
                "<ipxact:name>testBlock</ipxact:name>"
                "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
                "<ipxact:range>Kzer-Za</ipxact:range>"
                "<ipxact:width>Kohr-Ah</ipxact:width>"
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
        "</ipxact:MemoryMapBase>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode MemoryMapBaseNode = document.firstChildElement("ipxact:MemoryMapBase");

	MemoryMapBaseReader MemoryMapBaseReader;
	QSharedPointer<MemoryMapBase> testMemoryMapBase( new MemoryMapBase );
	MemoryMapBaseReader.readMemoryMapBase(MemoryMapBaseNode, testMemoryMapBase);

    QCOMPARE(testMemoryMapBase->name(), QString("testMemoryMapBase"));

    QCOMPARE(testMemoryMapBase->getMemoryBlocks()->size(), 1);

    QSharedPointer<AddressBlock> testAddressBlock =
        testMemoryMapBase->getMemoryBlocks()->first().dynamicCast<AddressBlock>();
    QCOMPARE(testAddressBlock.isNull(), false);
    QCOMPARE(testAddressBlock->name(), QString("testBlock"));
    QCOMPARE(testAddressBlock->getBaseAddress(), QString("StarControl"));
    QCOMPARE(testAddressBlock->getRange(), QString("Kzer-Za"));
    QCOMPARE(testAddressBlock->getWidth(), QString("Kohr-Ah"));

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

QTEST_APPLESS_MAIN(tst_MemoryMapBaseReader)

#include "tst_MemoryMapBaseReader.moc"

//-----------------------------------------------------------------------------
// File: tst_MemoryMapBaseWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 01.10.2015
//
// Description:
// Unit test for class MemoryMapBaseWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/MemoryMapBaseWriter.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryRemap.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_MemoryMapBaseWriter : public QObject
{
    Q_OBJECT

public:
    tst_MemoryMapBaseWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void writeSimpleMemoryMapBase();
    void writeIsPresent();
    void writeAddressBlocks();

private:

    QSharedPointer<MemoryMapBase> testMemoryMapBase_;
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseWriter::tst_MemoryMapBaseWriter()
//-----------------------------------------------------------------------------
tst_MemoryMapBaseWriter::tst_MemoryMapBaseWriter():
testMemoryMapBase_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseWriter::init()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseWriter::init()
{
    testMemoryMapBase_ = QSharedPointer<MemoryMapBase>(new MemoryMapBase("testMemoryMapBase"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseWriter::cleanup()
{
    testMemoryMapBase_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseWriter::writeSimpleMemoryMapBase()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseWriter::writeSimpleMemoryMapBase()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testMemoryMapBase_->setDisplayName("displayed");
    testMemoryMapBase_->setDescription("described");

    QString expectedOutput(
            "<ipxact:name>testMemoryMapBase</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
        );

    MemoryMapBaseWriter MemoryMapBaseWriter;
    MemoryMapBaseWriter.writeMemoryMapBase(xmlStreamWriter, testMemoryMapBase_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseWriter::writeIsPresent()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testMemoryMapBase_->setIsPresent("8/8-1");

    QString expectedOutput(
            "<ipxact:name>testMemoryMapBase</ipxact:name>"
            "<ipxact:isPresent>8/8-1</ipxact:isPresent>"
        );

    MemoryMapBaseWriter MemoryMapBaseWriter;
    MemoryMapBaseWriter.writeMemoryMapBase(xmlStreamWriter, testMemoryMapBase_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapBaseWriter::writeAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapBaseWriter::writeAddressBlocks()
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
        QSharedPointer<AddressBlock>(new AddressBlock("testBlock", "StarControl"));
    testAddressBlock->setRange("Kzer-Za");
    testAddressBlock->setWidth("Kohr-Ah");
    testAddressBlock->getRegisterData()->append(testRegister);
    testAddressBlock->getRegisterData()->append(containedRegisterFile);

	testMemoryMapBase_->getMemoryBlocks()->append(testAddressBlock);

    QString expectedOutput(
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
		);

    MemoryMapBaseWriter MemoryMapBaseWriter;
    MemoryMapBaseWriter.writeMemoryMapBase(xmlStreamWriter, testMemoryMapBase_);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_MemoryMapBaseWriter)

#include "tst_MemoryMapBaseWriter.moc"

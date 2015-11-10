//-----------------------------------------------------------------------------
// File: tst_MemoryMapWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: <Name>
// Date: <Date in the format dd.mm.yyyy>
//
// Description:
// Unit test for class MemoryMapWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/MemoryMapWriter.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_MemoryMapWriter : public QObject
{
    Q_OBJECT

public:
    tst_MemoryMapWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void writeSimpleMemoryMap();
    void writeIsPresent();
    void writeAddressBlocks();

    void writeSimpleMemoryRemap();
    void writeMemoryRemapAddressBlocks();

    void writeAddressUnitBits();
    void writeShared();

    void writeVendorExtensions();

private:

    QSharedPointer<MemoryMap> testMemoryMap_;
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::tst_MemoryMapWriter()
//-----------------------------------------------------------------------------
tst_MemoryMapWriter::tst_MemoryMapWriter():
testMemoryMap_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_MemoryMapWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_MemoryMapWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::init()
//-----------------------------------------------------------------------------
void tst_MemoryMapWriter::init()
{
    testMemoryMap_ = QSharedPointer<MemoryMap>(new MemoryMap("testMemoryMap"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_MemoryMapWriter::cleanup()
{
    testMemoryMap_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::writeSimpleMemoryMap()
//-----------------------------------------------------------------------------
void tst_MemoryMapWriter::writeSimpleMemoryMap()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testMemoryMap_->setDisplayName("displayed");
    testMemoryMap_->setDescription("described");

    QString expectedOutput(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
        "</ipxact:memoryMap>"
        );

    MemoryMapWriter memoryMapWriter;
    memoryMapWriter.writeMemoryMap(xmlStreamWriter, testMemoryMap_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void tst_MemoryMapWriter::writeIsPresent()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testMemoryMap_->setIsPresent("8/8-1");

    QString expectedOutput(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:isPresent>8/8-1</ipxact:isPresent>"
        "</ipxact:memoryMap>"
        );

    MemoryMapWriter memoryMapWriter;
    memoryMapWriter.writeMemoryMap(xmlStreamWriter, testMemoryMap_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::writeAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapWriter::writeAddressBlocks()
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

    testMemoryMap_->getMemoryBlocks()->append(testAddressBlock);

    QString expectedOutput(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
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
        "</ipxact:memoryMap>"
        );

    MemoryMapWriter memoryMapWriter;
    memoryMapWriter.writeMemoryMap(xmlStreamWriter, testMemoryMap_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::writeMemoryRemaps()
//-----------------------------------------------------------------------------
void tst_MemoryMapWriter::writeSimpleMemoryRemap()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<MemoryRemap> testMemoryRemap (new MemoryRemap("memoryRemap", QString("remappedState")));
    testMemoryRemap->setDisplayName("displayedRemap");
    testMemoryRemap->setDescription("describedRemap");
    testMemoryRemap->setIsPresent("1");

    testMemoryMap_->getMemoryRemaps()->append(testMemoryRemap);

    QString expectedOutput(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:memoryRemap state=\"remappedState\">"
                "<ipxact:name>memoryRemap</ipxact:name>"
                "<ipxact:displayName>displayedRemap</ipxact:displayName>"
                "<ipxact:description>describedRemap</ipxact:description>"
                "<ipxact:isPresent>1</ipxact:isPresent>"
            "</ipxact:memoryRemap>"
        "</ipxact:memoryMap>"
        );

    MemoryMapWriter memoryMapWriter;
    memoryMapWriter.writeMemoryMap(xmlStreamWriter, testMemoryMap_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::writeMemoryRemapAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapWriter::writeMemoryRemapAddressBlocks()
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

    QSharedPointer<MemoryRemap> testMemoryRemap (new MemoryRemap("memoryRemap", QString("remappedState")));
    testMemoryRemap->getMemoryBlocks()->append(testAddressBlock);

    testMemoryMap_->getMemoryRemaps()->append(testMemoryRemap);

    QString expectedOutput(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:memoryRemap state=\"remappedState\">"
                "<ipxact:name>memoryRemap</ipxact:name>"
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
            "</ipxact:memoryRemap>"
        "</ipxact:memoryMap>"
        );

    MemoryMapWriter memoryMapWriter;
    memoryMapWriter.writeMemoryMap(xmlStreamWriter, testMemoryMap_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapWriter::writeAddressUnitBits()
//-----------------------------------------------------------------------------
 void tst_MemoryMapWriter::writeAddressUnitBits()
 {
     QString output;
     QXmlStreamWriter xmlStreamWriter(&output);

     testMemoryMap_->setAddressUnitBits("8");

     QString expectedOutput(
         "<ipxact:memoryMap>"
             "<ipxact:name>testMemoryMap</ipxact:name>"
             "<ipxact:addressUnitBits>8</ipxact:addressUnitBits>"
         "</ipxact:memoryMap>"
         );

     MemoryMapWriter memoryMapWriter;
     memoryMapWriter.writeMemoryMap(xmlStreamWriter, testMemoryMap_);
     QCOMPARE(output, expectedOutput);
 }

 //-----------------------------------------------------------------------------
 // Function: tst_MemoryMapWriter::writeShared()
 //-----------------------------------------------------------------------------
 void tst_MemoryMapWriter::writeShared()
 {
     QString output;
     QXmlStreamWriter xmlStreamWriter(&output);

     testMemoryMap_->setShared(true);

     QString expectedOutput(
         "<ipxact:memoryMap>"
             "<ipxact:name>testMemoryMap</ipxact:name>"
             "<ipxact:shared>yes</ipxact:shared>"
         "</ipxact:memoryMap>"
         );

     MemoryMapWriter memoryMapWriter;
     memoryMapWriter.writeMemoryMap(xmlStreamWriter, testMemoryMap_);
     QCOMPARE(output, expectedOutput);
 }

 //-----------------------------------------------------------------------------
 // Function: tst_MemoryMapWriter::writeVendorExtensions()
 //-----------------------------------------------------------------------------
 void tst_MemoryMapWriter::writeVendorExtensions()
 {
     QString output;
     QXmlStreamWriter xmlStreamWriter(&output);

     QDomDocument document;
     QDomElement extensionNode = document.createElement("testExtension");
     extensionNode.setAttribute("testExtensionAttribute", "extension");
     extensionNode.appendChild(document.createTextNode("testValue"));

     QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

     testMemoryMap_->getVendorExtensions()->append(testExtension);

     QString expectedOutput(
         "<ipxact:memoryMap>"
             "<ipxact:name>testMemoryMap</ipxact:name>"
             "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
             "</ipxact:vendorExtensions>"
         "</ipxact:memoryMap>"
         );

     MemoryMapWriter memoryMapWriter;
     memoryMapWriter.writeMemoryMap(xmlStreamWriter, testMemoryMap_);
     QCOMPARE(output, expectedOutput);
 }

QTEST_APPLESS_MAIN(tst_MemoryMapWriter)

#include "tst_MemoryMapWriter.moc"

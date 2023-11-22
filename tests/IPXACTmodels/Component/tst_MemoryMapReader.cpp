//-----------------------------------------------------------------------------
// File: tst_MemoryMapReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 01.10.2015
//
// Description:
// Unit test for class MemoryMapReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/MemoryMapReader.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>

#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_MemoryMapReader : public QObject
{
    Q_OBJECT

public:
    tst_MemoryMapReader();

private slots:

    void readSimpleMemoryMap();
    void readSimpleMemoryMap2022();

    void readIsPresent();
    void readAddressBlocks();
    void readSubspaceMap();

    void readSimpleMemoryRemap();
    void readMemoryRemapAddressBlocks();

    void readMemoryRemap2022();
    void readMemoryRemapWithSubspaceMap();

    void readAddressUnitBits();
    void readSharedValue();

    void readVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::tst_MemoryMapReader()
//-----------------------------------------------------------------------------
tst_MemoryMapReader::tst_MemoryMapReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readSimpleMemoryMap()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readSimpleMemoryMap()
{
    QString documentContent(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
        "</ipxact:memoryMap>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, Document::Revision::Std14);

    QCOMPARE(testMemoryMap->name(), QString("testMemoryMap"));
    QCOMPARE(testMemoryMap->displayName(), QString("displayed"));
    QCOMPARE(testMemoryMap->description(), QString("described"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readSimpleMemoryMap2022()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readSimpleMemoryMap2022()
{
    QString documentContent(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
            "<ipxact:memoryMapDefinitionRef typeDefinitions=\"testDefinitions\">testMemMapDef</ipxact:memoryMapDefinitionRef>"
        "</ipxact:memoryMap>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, Document::Revision::Std22);

    QCOMPARE(testMemoryMap->name(), QString("testMemoryMap"));
    QCOMPARE(testMemoryMap->displayName(), QString("displayed"));
    QCOMPARE(testMemoryMap->description(), QString("described"));

    QCOMPARE(testMemoryMap->getMemoryMapDefinitionReference(), QString("testMemMapDef"));
    QCOMPARE(testMemoryMap->getTypeDefinitionsReference(), QString("testDefinitions"));

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readIsPresent()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readIsPresent()
{
    QString documentContent(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:isPresent>8/8-1</ipxact:isPresent>"
        "</ipxact:memoryMap>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, Document::Revision::Std14);

    QCOMPARE(testMemoryMap->name(), QString("testMemoryMap"));
    QCOMPARE(testMemoryMap->getIsPresent(), QString("8/8-1"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readAddressBlocks()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, Document::Revision::Std14);

    QCOMPARE(testMemoryMap->name(), QString("testMemoryMap"));

    QCOMPARE(testMemoryMap->getMemoryBlocks()->size(), 1);

    QSharedPointer<AddressBlock> testAddressBlock =
        testMemoryMap->getMemoryBlocks()->first().dynamicCast<AddressBlock>();
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

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readSubspaceMap()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readSubspaceMap()
{
    QString documentContent(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:subspaceMap initiatorRef=\"testInitiator\">"
                "<ipxact:name>testSubspaceMap</ipxact:name>"
                "<ipxact:baseAddress>'h01</ipxact:baseAddress>"
            "</ipxact:subspaceMap>"
        "</ipxact:memoryMap>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, 
        Document::Revision::Std22);

    QSharedPointer<SubSpaceMap> parsedSubSpace;

    QCOMPARE(testMemoryMap->getMemoryBlocks()->size(), 1);
    QVERIFY(parsedSubSpace = testMemoryMap->getMemoryBlocks()->first().dynamicCast<SubSpaceMap>());

    QCOMPARE(parsedSubSpace->getInitiatorReference(), QString("testInitiator"));
    QCOMPARE(parsedSubSpace->getBaseAddress(), QString("'h01"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readSimpleMemoryMap()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readSimpleMemoryRemap()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, Document::Revision::Std14);

    QCOMPARE(testMemoryMap->name(), QString("testMemoryMap"));

    QCOMPARE(testMemoryMap->getMemoryRemaps()->size(), 1);
    QCOMPARE(testMemoryMap->getMemoryBlocks()->size(), 0);

    QSharedPointer<MemoryRemap> testMemoryRemap = testMemoryMap->getMemoryRemaps()->first();
    QCOMPARE(testMemoryRemap->name(), QString("memoryRemap"));
    QCOMPARE(testMemoryRemap->displayName(), QString("displayedRemap"));
    QCOMPARE(testMemoryRemap->description(), QString("describedRemap"));

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readMemoryRemapAddressBlocks()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readMemoryRemapAddressBlocks()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, Document::Revision::Std14);

    QCOMPARE(testMemoryMap->name(), QString("testMemoryMap"));

    QCOMPARE(testMemoryMap->getMemoryRemaps()->size(), 1);
    QCOMPARE(testMemoryMap->getMemoryBlocks()->size(), 0);

    QSharedPointer<MemoryRemap> testMemoryRemap = testMemoryMap->getMemoryRemaps()->first();
    QCOMPARE(testMemoryRemap->name(), QString("memoryRemap"));

    QCOMPARE(testMemoryRemap->getMemoryBlocks()->size(), 1);

    QSharedPointer<AddressBlock> testAddressBlock =
        testMemoryRemap->getMemoryBlocks()->first().dynamicCast<AddressBlock>();
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

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readMemoryRemap2022()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readMemoryRemap2022()
{
    // Test with definition std22 reference.
    QString documentContent(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:memoryRemap>"
                "<ipxact:name>memoryRemap</ipxact:name>"
                "<ipxact:displayName>displayedRemap</ipxact:displayName>"
                "<ipxact:description>describedRemap</ipxact:description>"
                "<ipxact:modeRef priority=\"0\">testMode</ipxact:modeRef>"
                "<ipxact:modeRef priority=\"1\">mode2</ipxact:modeRef>"
                "<ipxact:remapDefinitionRef typeDefinitions=\"testDefinitions\">testRemapDef</ipxact:remapDefinitionRef>"
                "<ipxact:vendorExtensions>"
                    "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                "</ipxact:vendorExtensions>"
            "</ipxact:memoryRemap>"
        "</ipxact:memoryMap>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, Document::Revision::Std22);

    QCOMPARE(testMemoryMap->name(), QString("testMemoryMap"));

    QCOMPARE(testMemoryMap->getMemoryRemaps()->size(), 1);
    QCOMPARE(testMemoryMap->getMemoryBlocks()->size(), 0);

    QSharedPointer<MemoryRemap> testMemoryRemap = testMemoryMap->getMemoryRemaps()->first();
    QCOMPARE(testMemoryRemap->name(), QString("memoryRemap"));
    QCOMPARE(testMemoryRemap->displayName(), QString("displayedRemap"));
    QCOMPARE(testMemoryRemap->description(), QString("describedRemap"));
    QCOMPARE(testMemoryRemap->getModeReferences()->size(), 2);
    QCOMPARE(testMemoryRemap->getModeReferences()->first()->getReference(), QString("testMode"));
    QCOMPARE(testMemoryRemap->getModeReferences()->back()->getPriority(), 1);

    QCOMPARE(testMemoryRemap->getMemoryRemapDefinitionReference(), QString("testRemapDef"));
    QCOMPARE(testMemoryRemap->getTypeDefinitionsReference(), QString("testDefinitions"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readMemoryRemapWithSubspaceMap()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readMemoryRemapWithSubspaceMap()
{
    QString documentContent(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:memoryRemap>"
                "<ipxact:name>memoryRemap</ipxact:name>"
                "<ipxact:displayName>displayedRemap</ipxact:displayName>"
                "<ipxact:description>describedRemap</ipxact:description>"
                "<ipxact:subspaceMap initiatorRef=\"testInitiator\">"
                    "<ipxact:name>testSubspaceMap</ipxact:name>"
                    "<ipxact:baseAddress>'h01</ipxact:baseAddress>"
                "</ipxact:subspaceMap>"
                "<ipxact:vendorExtensions>"
                    "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                "</ipxact:vendorExtensions>"
            "</ipxact:memoryRemap>"
        "</ipxact:memoryMap>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, 
        Document::Revision::Std22);

    QSharedPointer<MemoryRemap> parsedRemap;
    QSharedPointer<SubSpaceMap> parsedSubSpace;

    QCOMPARE(testMemoryMap->getMemoryBlocks()->size(), 0);
    QCOMPARE(testMemoryMap->getMemoryRemaps()->size(), 1);

    parsedRemap = testMemoryMap->getMemoryRemaps()->first();
    
    QVERIFY(parsedSubSpace = parsedRemap->getMemoryBlocks()->first().dynamicCast<SubSpaceMap>());

    QCOMPARE(parsedSubSpace->getInitiatorReference(), QString("testInitiator"));
    QCOMPARE(parsedSubSpace->getBaseAddress(), QString("'h01"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readAddressUnitBits()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readAddressUnitBits()
{
    QString documentContent(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:addressUnitBits>8</ipxact:addressUnitBits>"
        "</ipxact:memoryMap>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, Document::Revision::Std14);

    QCOMPARE(testMemoryMap->name(), QString("testMemoryMap"));
    QCOMPARE(testMemoryMap->getAddressUnitBits(), QString("8"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readSharedValue()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readSharedValue()
{
    QString documentContent(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:shared>yes</ipxact:shared>"
        "</ipxact:memoryMap>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, Document::Revision::Std14);

    QCOMPARE(testMemoryMap->name(), QString("testMemoryMap"));
    QCOMPARE(testMemoryMap->getShared(), QString("true"));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_MemoryMapReader::readVendorExtensions()
{
    QString documentContent(
        "<ipxact:memoryMap>"
            "<ipxact:name>testMemoryMap</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:memoryMap>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode memoryMapNode = document.firstChildElement("ipxact:memoryMap");

    QSharedPointer<MemoryMap> testMemoryMap = MemoryMapReader::createMemoryMapFrom(memoryMapNode, Document::Revision::Std14);

    QCOMPARE(testMemoryMap->name(), QString("testMemoryMap"));
    QCOMPARE(testMemoryMap->getVendorExtensions()->size(), 1);
    QCOMPARE(testMemoryMap->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_MemoryMapReader)

#include "tst_MemoryMapReader.moc"

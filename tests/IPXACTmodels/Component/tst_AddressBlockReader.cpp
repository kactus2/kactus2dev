//-----------------------------------------------------------------------------
// File: tst_AddressBlockReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 30.09.2015
//
// Description:
// Unit test for class AddressBlockReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/AddressBlockReader.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/MemoryArray.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_AddressBlockReader : public QObject
{
    Q_OBJECT

public:

    tst_AddressBlockReader();

private slots:

    void readSimpleAddressBlock();
    void readIsPresent();
    void readTypeIdentifier();

    void readUsage();
    void readVolatile();
    void readAccess();
    void readParameters();

    void readRegisterData();

    void readMisalignmentAllowed2022();
    void readMemoryArray2022();
    void readAddressBlockDefinitionRef2022();
    void readAccessPolicies2022();

    void readVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::tst_AddressBlockReader()
//-----------------------------------------------------------------------------
tst_AddressBlockReader::tst_AddressBlockReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readSimpleAddressBlock()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readSimpleAddressBlock()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std14);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));
    QCOMPARE(testAddressBlock->displayName(), QString("displayed"));
    QCOMPARE(testAddressBlock->description(), QString("described"));
    QCOMPARE(testAddressBlock->getBaseAddress(), QString("StarControl"));
    QCOMPARE(testAddressBlock->getRange(), QString("Kzer-Za"));
    QCOMPARE(testAddressBlock->getWidth(), QString("Kohr-Ah"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readIsPresent()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readIsPresent()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:isPresent>presence</ipxact:isPresent>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std14);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));
    QCOMPARE(testAddressBlock->getIsPresent(), QString("presence"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readTypeIdentifier()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readTypeIdentifier()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:typeIdentifier>identifier</ipxact:typeIdentifier>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std14);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));
    QCOMPARE(testAddressBlock->getTypeIdentifier(), QString("identifier"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readUsage()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readUsage()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
            "<ipxact:usage>register</ipxact:usage>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std14);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));
    QCOMPARE(testAddressBlock->getUsage(), General::REGISTER);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readVolatile()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readVolatile()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
            "<ipxact:volatile>true</ipxact:volatile>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std14);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));
    QCOMPARE(testAddressBlock->getVolatile(), QString("true"));

    documentContent = 
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
            "<ipxact:volatile>false</ipxact:volatile>"
        "</ipxact:addressBlock>"
        ;

    document.setContent(documentContent);
    addressBlockNode = document.firstChildElement("ipxact:addressBlock");
    testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std14);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));
    QCOMPARE(testAddressBlock->getVolatile(), QString("false"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readAccess()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readAccess()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
            "<ipxact:access>writeOnce</ipxact:access>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std14);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));
    QCOMPARE(testAddressBlock->getAccess(), AccessTypes::WRITEONCE);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readParameters()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readParameters()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"testID\">"
                    "<ipxact:name>testParameter</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std14);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));

    QCOMPARE(testAddressBlock->getParameters()->size(), 1);

    QSharedPointer<Parameter> testParameter = testAddressBlock->getParameters()->first();
    QCOMPARE(testParameter->name(), QString("testParameter"));
    QCOMPARE(testParameter->getValueId(), QString("testID"));
    QCOMPARE(testParameter->getValue(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readRegisterData()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readRegisterData()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std14);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));

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
// Function: tst_AddressBlockReader::readMisallignmentAllowed2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readMisalignmentAllowed2022()
{
    QString documentContent(
        "<ipxact:addressBlock misalignmentAllowed=\"false\">"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);
    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std22);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));
    QCOMPARE(testAddressBlock->getMisalignmentAllowed(), QString("false"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readMemoryArray2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readMemoryArray2022()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:array>"
                "<ipxact:dim indexVar=\"testDim\">1+1</ipxact:dim>"
                "<ipxact:dim>4</ipxact:dim>"
                "<ipxact:stride>4</ipxact:stride>"
            "</ipxact:array>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"    
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);
    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std22);

    QCOMPARE(testAddressBlock->getMemoryArray()->getDimensions()->size(), 2);
    
    QCOMPARE(testAddressBlock->getMemoryArray()->getDimensions()->first()->indexVar_, QString("testDim"));
    QCOMPARE(testAddressBlock->getMemoryArray()->getDimensions()->first()->value_, QString("1+1"));
    
    QCOMPARE(testAddressBlock->getMemoryArray()->getDimensions()->last()->indexVar_, QString(""));
    QCOMPARE(testAddressBlock->getMemoryArray()->getDimensions()->last()->value_, QString("4"));

    QCOMPARE(testAddressBlock->getMemoryArray()->getStride(), QString("4"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readAddressBlockDefinitionRef2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readAddressBlockDefinitionRef2022()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:addressBlockDefinitionRef typeDefinitions=\"testDefinitions\">someBlock</ipxact:addressBlockDefinitionRef>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);
    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std22);

    QCOMPARE(testAddressBlock->getAddressBlockDefinitionRef(), QString("someBlock"));
    QCOMPARE(testAddressBlock->getTypeDefinitionsRef(), QString("testDefinitions"));
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readAccessPolicies2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readAccessPolicies2022()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
            "<ipxact:accessPolicies>"
                "<ipxact:accessPolicy>"
                    "<ipxact:modeRef priority=\"1\">testMode</ipxact:modeRef>"
                    "<ipxact:access>read-only</ipxact:access>"
                "</ipxact:accessPolicy>"
                "<ipxact:accessPolicy>"
                    "<ipxact:modeRef priority=\"2\">testMode2</ipxact:modeRef>"
                "</ipxact:accessPolicy>"
            "</ipxact:accessPolicies>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);
    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std22);

    QCOMPARE(testAddressBlock->getAccessPolicies()->size(), 2);
    QCOMPARE(testAddressBlock->getAccessPolicies()->first()->getModeReferences()->first()->getReference(), QString("testMode"));
    QCOMPARE(testAddressBlock->getAccessPolicies()->first()->getAccess(), AccessTypes::READ_ONLY);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_AddressBlockReader::readVendorExtensions()
{
    QString documentContent(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:addressBlock>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode addressBlockNode = document.firstChildElement("ipxact:addressBlock");

    QSharedPointer<AddressBlock> testAddressBlock = AddressBlockReader::createAddressBlockFrom(addressBlockNode, Document::Revision::Std14);

    QCOMPARE(testAddressBlock->name(), QString("testBlock"));

    QCOMPARE(testAddressBlock->getVendorExtensions()->size(), 1);
    QCOMPARE(testAddressBlock->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_AddressBlockReader)

#include "tst_AddressBlockReader.moc"

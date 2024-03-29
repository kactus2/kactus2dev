//-----------------------------------------------------------------------------
// File: tst_AddressBlockWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.09.2015
//
// Description:
// Unit test for class AddressBlockWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/AddressBlockWriter.h>
#include <IPXACTmodels/Component/AddressBlock.h>

#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/MemoryArray.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_AddressBlockWriter : public QObject
{
    Q_OBJECT

public:
    tst_AddressBlockWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void writeSimpleAddressBlock();
    void writeIsPresent();
    void writeTypeIdentifier();
    
    void writeUsage();
    void writeVolatile();
    void writeAccess();
    void writeParameters();

    void writeRegisterData();

    void writeMisalignmentAllowed2022();
    void writeMemoryArray2022();
    void writeAddressBlockDefinitionRef2022();
    void writeAccessPolicies2022();

    void writeVendorExtensions();

private:

    QSharedPointer<AddressBlock> testAddressBlock_;
};

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::tst_AddressBlockWriter()
//-----------------------------------------------------------------------------
tst_AddressBlockWriter::tst_AddressBlockWriter():
testAddressBlock_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::init()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::init()
{
    testAddressBlock_ = QSharedPointer<AddressBlock>(new AddressBlock("testBlock", "StarControl"));
    testAddressBlock_->setRange("Kzer-Za");
    testAddressBlock_->setWidth("Kohr-Ah");
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::cleanup()
{
    testAddressBlock_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeSimpleAddressBlock()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testAddressBlock_->setDisplayName("displayed");
    testAddressBlock_->setDescription("described");

    QString expectedOutput(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
        "</ipxact:addressBlock>"
        );

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeIsPresent()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testAddressBlock_->setIsPresent("presence");

    QString expectedOutput(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:isPresent>presence</ipxact:isPresent>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
        "</ipxact:addressBlock>"
        );

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeTypeIdentifier()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeTypeIdentifier()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testAddressBlock_->setTypeIdentifier("identifier");

    QString expectedOutput(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:typeIdentifier>identifier</ipxact:typeIdentifier>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
        "</ipxact:addressBlock>"
        );

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeUsage()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeUsage()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testAddressBlock_->setUsage(General::REGISTER);

    QString expectedOutput(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
            "<ipxact:usage>register</ipxact:usage>"
        "</ipxact:addressBlock>"
        );

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeVolatile()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeVolatile()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testAddressBlock_->setVolatile(true);

    QString expectedOutput(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
            "<ipxact:volatile>true</ipxact:volatile>"
        "</ipxact:addressBlock>"
        );

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeAccess()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeAccess()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testAddressBlock_->setAccess(AccessTypes::WRITEONCE);

    QString expectedOutput(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
            "<ipxact:access>writeOnce</ipxact:access>"
        "</ipxact:addressBlock>"
        );

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeParameters()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    testAddressBlock_->getParameters()->append(testParameter);

    QString expectedOutput(
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

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeRegisterData()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeRegisterData()
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

    testAddressBlock_->getRegisterData()->append(testRegister);
    testAddressBlock_->getRegisterData()->append(containedRegisterFile);

    QString expectedOutput(
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

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeMisalignmentAllowed2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeMisalignmentAllowed2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testAddressBlock_->setMisalignmentAllowed(false);

    QString expectedOutput(
        "<ipxact:addressBlock misalignmentAllowed=\"false\">"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
        "</ipxact:addressBlock>"
        );

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeMemoryArray2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeMemoryArray2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<MemoryArray> memArray(new MemoryArray());
    QSharedPointer<MemoryArray::Dimension> testDim(new MemoryArray::Dimension());
    testDim->indexVar_ = "testVar";
    testDim->value_ = "2";

    memArray->getDimensions()->append(testDim);
    memArray->setStride("4");

    testAddressBlock_->setMemoryArray(memArray);

    QString expectedOutput(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:array>"
                "<ipxact:dim indexVar=\"testVar\">2</ipxact:dim>"
                "<ipxact:stride>4</ipxact:stride>"
            "</ipxact:array>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:range>Kzer-Za</ipxact:range>"
            "<ipxact:width>Kohr-Ah</ipxact:width>"
        "</ipxact:addressBlock>"
        );

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeAddressBlockDefinitionRef2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeAddressBlockDefinitionRef2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testAddressBlock_->setWidth("");
    testAddressBlock_->setRange("");

    testAddressBlock_->setAddressBlockDefinitionRef("someOtherAddressBlock");
    testAddressBlock_->setTypeDefinitionsRef("someTypeDefinitions");

    QString expectedOutput(
        "<ipxact:addressBlock>"
            "<ipxact:name>testBlock</ipxact:name>"
            "<ipxact:baseAddress>StarControl</ipxact:baseAddress>"
            "<ipxact:addressBlockDefinitionRef typeDefinitions=\"someTypeDefinitions\">someOtherAddressBlock</ipxact:addressBlockDefinitionRef>"
        "</ipxact:addressBlock>"
        );

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeAccessPolicies2022()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeAccessPolicies2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<AccessPolicy> testAccessPolicy1(new AccessPolicy());
    QSharedPointer<AccessPolicy> testAccessPolicy2(new AccessPolicy());

    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    QSharedPointer<ModeReference> modeRef2(new ModeReference());

    modeRef1->setPriority(1);
    modeRef1->setReference("testMode");
    
    modeRef2->setPriority(2);
    modeRef2->setReference("testMode2");

    testAccessPolicy1->getModeReferences()->append(modeRef1);
    testAccessPolicy1->setAccess(AccessTypes::READ_ONLY);

    testAccessPolicy2->getModeReferences()->append(modeRef2);

    testAddressBlock_->getAccessPolicies()->append(testAccessPolicy1);
    testAddressBlock_->getAccessPolicies()->append(testAccessPolicy2);

    QString expectedOutput(
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

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AddressBlockWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void tst_AddressBlockWriter::writeVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testAddressBlock_->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
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

    AddressBlockWriter::writeAddressBlock(xmlStreamWriter, testAddressBlock_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_AddressBlockWriter)

#include "tst_AddressBlockWriter.moc"

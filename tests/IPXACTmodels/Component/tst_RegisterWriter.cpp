//-----------------------------------------------------------------------------
// File: tst_RegisterWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.09.2015
//
// Description:
// Unit test for class RegisterWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/RegisterWriter.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/AlternateRegister.h>

#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/MemoryArray.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_RegisterWriter : public QObject
{
    Q_OBJECT

public:
    tst_RegisterWriter();

private slots:

    void writeSimpleRegister();
    void writeRegisterIsPresent();
    void writeRegisterDimension();
    void writeRegisterTypeIdentifier();
    void writeRegisterVolatile();
    void writeRegisterAccess();
    void writeRegisterFields();
    void writeRegisterAlternateRegisters();

    void writeRegisterAlternateRegisters2022();

    void writeRegisterMemoryArray2022();
    void writeRegisterDefinitionReference2022();
    void writeRegisterAccessPolicies2022();

    void writeRegisterParameters();
    void writeRegisterVendorExtensions();

    void writeSimpleRegisterFile();
    void writeRegisterFileIsPresent();
    void writeRegisterFileDimension();
    void writeRegisterFileTypeIdentifier();
    void writeRegisterFileRegisterData();

    void writeRegisterFileDefinitionReference2022();
    void writeRegisterFileMemoryArray2022();
    void writeRegisterFileAccessPolicies2022();

    void writeRegisterFileParameters();
    void writeRegisterFileVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::tst_RegisterWriter()
//-----------------------------------------------------------------------------
tst_RegisterWriter::tst_RegisterWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeSimpleRegister()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeSimpleRegister()
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

    QString expectedOutput(
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
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterIsPresent()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterIsPresent()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
    testRegister->setIsPresent("4-3");
    testRegister->getFields()->append(simpleField);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:isPresent>4-3</ipxact:isPresent>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
        "</ipxact:register>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterDimension()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterDimension()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
    testRegister->setDimension("16");
    testRegister->getFields()->append(simpleField);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:dim>16</ipxact:dim>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
        "</ipxact:register>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterTypeIdentifier()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterTypeIdentifier()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
    testRegister->setTypeIdentifier("identifier");
    testRegister->getFields()->append(simpleField);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:typeIdentifier>identifier</ipxact:typeIdentifier>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
        "</ipxact:register>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterVolatile()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterVolatile()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
    testRegister->setVolatile(true);
    testRegister->getFields()->append(simpleField);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:volatile>true</ipxact:volatile>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
        "</ipxact:register>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterAccess()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterAccess()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
    testRegister->getFields()->append(simpleField);
    testRegister->setAccess(AccessTypes::WRITEONCE);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:access>writeOnce</ipxact:access>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
        "</ipxact:register>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterFields()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterFields()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Field> otherField(new Field("unsimplified"));
    otherField->setBitOffset("OtherOff");
    otherField->setBitWidth("otherWidth");

    QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
    testRegister->getFields()->append(simpleField);
    testRegister->getFields()->append(otherField);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
            "<ipxact:field>"
                "<ipxact:name>unsimplified</ipxact:name>"
                "<ipxact:bitOffset>OtherOff</ipxact:bitOffset>"
                "<ipxact:bitWidth>otherWidth</ipxact:bitWidth>"
            "</ipxact:field>"
        "</ipxact:register>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeAlternateRegister()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterAlternateRegisters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<Field> alternateField(new Field("Hades"));
    alternateField->setBitOffset("alternativeOffset");
    alternateField->setBitWidth("alternativeWidth");

    QSharedPointer<AlternateRegister> alternateRegister (new AlternateRegister("alteredBeast", "wolf"));
    alternateRegister->setDisplayName("alteredDisplay");
    alternateRegister->setDescription("alteredDescription");
    alternateRegister->setIsPresent("presence");
    alternateRegister->setTypeIdentifier("Volgarr");
    alternateRegister->setVolatile(false);
    alternateRegister->setAccess(AccessTypes::READ_WRITE);
    alternateRegister->getFields()->append(alternateField);

    alternateRegister->getParameters()->append(testParameter);
    alternateRegister->getVendorExtensions()->append(testExtension);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
    testRegister->getFields()->append(simpleField);
    testRegister->getAlternateRegisters()->append(alternateRegister);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
            "<ipxact:alternateRegisters>"
                "<ipxact:alternateRegister>"
                    "<ipxact:name>alteredBeast</ipxact:name>"
                    "<ipxact:displayName>alteredDisplay</ipxact:displayName>"
                    "<ipxact:description>alteredDescription</ipxact:description>"
                    "<ipxact:isPresent>presence</ipxact:isPresent>"
                    "<ipxact:alternateGroups>"
                        "<ipxact:alternateGroup>wolf</ipxact:alternateGroup>"
                    "</ipxact:alternateGroups>"
                    "<ipxact:typeIdentifier>Volgarr</ipxact:typeIdentifier>"
                    "<ipxact:volatile>false</ipxact:volatile>"
                    "<ipxact:access>read-write</ipxact:access>"
                    "<ipxact:field>"
                        "<ipxact:name>Hades</ipxact:name>"
                        "<ipxact:bitOffset>alternativeOffset</ipxact:bitOffset>"
                        "<ipxact:bitWidth>alternativeWidth</ipxact:bitWidth>"
                    "</ipxact:field>"
                    "<ipxact:parameters>"
                        "<ipxact:parameter parameterId=\"testID\">"
                            "<ipxact:name>testParameter</ipxact:name>"
                            "<ipxact:value>1</ipxact:value>"
                        "</ipxact:parameter>"
                    "</ipxact:parameters>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:alternateRegister>"
            "</ipxact:alternateRegisters>"
        "</ipxact:register>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterAlternateRegisters2022()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterAlternateRegisters2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    
    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    modeRef1->setReference("testMode");
    modeRef1->setPriority("0");

    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    modeRef2->setReference("testMode2");
    modeRef2->setPriority("1");

    QSharedPointer<ModeReference> modeRef3(new ModeReference(*modeRef1));
    modeRef3->setReference("testMode3");
    modeRef3->setPriority("2");


    QSharedPointer<AccessPolicy> accessPolicy1(new AccessPolicy());
    accessPolicy1->getModeReferences()->append(modeRef1);
    accessPolicy1->setAccess(AccessTypes::READ_ONLY);

    QSharedPointer<AccessPolicy> accessPolicy2(new AccessPolicy());
    accessPolicy2->getModeReferences()->append(modeRef2);
    accessPolicy2->getModeReferences()->append(modeRef3);

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));
    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<Field> altField(new Field("Hades"));
    altField->setBitOffset("alternativeOffset");
    altField->setBitWidth("alternativeWidth");

    QSharedPointer<AlternateRegister> testAlternateRegister(new AlternateRegister("alteredBeast"));
    testAlternateRegister->setDisplayName("alteredDisplay");
    testAlternateRegister->setShortDescription("testShortDescription");
    testAlternateRegister->setDescription("alteredDescription");
    
    testAlternateRegister->getModeReferences()->append(modeRef1);
    testAlternateRegister->getModeReferences()->append(modeRef2);
    
    testAlternateRegister->getAccessPolicies()->append(accessPolicy1);
    testAlternateRegister->getAccessPolicies()->append(accessPolicy2);

    testAlternateRegister->setVolatile(false);
    testAlternateRegister->setTypeIdentifier("Volgarr");
    testAlternateRegister->getParameters()->append(testParameter);
    testAlternateRegister->getVendorExtensions()->append(testExtension);

    testAlternateRegister->getFields()->append(altField);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
    testRegister->getFields()->append(simpleField);
    testRegister->getAlternateRegisters()->append(testAlternateRegister);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
            "<ipxact:alternateRegisters>"
                "<ipxact:alternateRegister>"
                    "<ipxact:name>alteredBeast</ipxact:name>"
                    "<ipxact:displayName>alteredDisplay</ipxact:displayName>"
                    "<ipxact:shortDescription>testShortDescription</ipxact:shortDescription>"
                    "<ipxact:description>alteredDescription</ipxact:description>"
                    "<ipxact:modeRef priority=\"0\">testMode</ipxact:modeRef>"
                    "<ipxact:modeRef priority=\"1\">testMode2</ipxact:modeRef>"
                    "<ipxact:typeIdentifier>Volgarr</ipxact:typeIdentifier>"
                    "<ipxact:volatile>false</ipxact:volatile>"
                    "<ipxact:accessPolicies>"
                        "<ipxact:accessPolicy>"
                            "<ipxact:modeRef priority=\"0\">testMode</ipxact:modeRef>"
                            "<ipxact:access>read-only</ipxact:access>"
                        "</ipxact:accessPolicy>"
                        "<ipxact:accessPolicy>"
                            "<ipxact:modeRef priority=\"1\">testMode2</ipxact:modeRef>"
                            "<ipxact:modeRef priority=\"2\">testMode3</ipxact:modeRef>"
                        "</ipxact:accessPolicy>"
                    "</ipxact:accessPolicies>"
                    "<ipxact:field>"
                        "<ipxact:name>Hades</ipxact:name>"
                        "<ipxact:bitOffset>alternativeOffset</ipxact:bitOffset>"
                        "<ipxact:bitWidth>alternativeWidth</ipxact:bitWidth>"
                    "</ipxact:field>"
                    "<ipxact:parameters>"
                        "<ipxact:parameter parameterId=\"testID\">"
                            "<ipxact:name>testParameter</ipxact:name>"
                            "<ipxact:value>1</ipxact:value>"
                        "</ipxact:parameter>"
                    "</ipxact:parameters>"
                    "<ipxact:vendorExtensions>"
                        "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
                    "</ipxact:vendorExtensions>"
                "</ipxact:alternateRegister>"
            "</ipxact:alternateRegisters>"
        "</ipxact:register>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterMemoryArray2022()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterMemoryArray2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<MemoryArray::Dimension> dim1(new MemoryArray::Dimension());
    dim1->indexVar_ = "testDim";
    dim1->value_ = "1+1";
    
    QSharedPointer<MemoryArray::Dimension> dim2(new MemoryArray::Dimension());
    dim2->value_ = "4";

    QSharedPointer<MemoryArray> memArray(new MemoryArray());
    memArray->setStride("4");
    memArray->getDimensions()->append(dim1);
    memArray->getDimensions()->append(dim2);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Register> testRegister(new Register("testRegister", "offset", "10"));
    testRegister->setDisplayName("displayed");
    testRegister->setShortDescription("testShortDesc");
    testRegister->setDescription("described");
    testRegister->getFields()->append(simpleField);
    testRegister->setMemoryArray(memArray);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:shortDescription>testShortDesc</ipxact:shortDescription>"
            "<ipxact:description>described</ipxact:description>"
            "<ipxact:array>"
                "<ipxact:dim indexVar=\"testDim\">1+1</ipxact:dim>"
                "<ipxact:dim>4</ipxact:dim>"
                "<ipxact:stride>4</ipxact:stride>"
            "</ipxact:array>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
        "</ipxact:register>"
    );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterDefinitionReference2022()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterDefinitionReference2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Register> testRegister(new Register("testRegister", "offset"));
    testRegister->setRegisterDefinitionReference("testRegisterDef");
    testRegister->setTypeDefinitionsReference("testTypeDefinitions");

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:registerDefinitionRef typeDefinitions=\"testTypeDefinitions\">testRegisterDef</ipxact:registerDefinitionRef>"
        "</ipxact:register>"
    );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterAccessPolicies2022()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterAccessPolicies2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    modeRef1->setReference("testMode");
    modeRef1->setPriority("0");

    QSharedPointer<AccessPolicy> accessPolicy1(new AccessPolicy());
    accessPolicy1->getModeReferences()->append(modeRef1);
    accessPolicy1->setAccess(AccessTypes::READ_ONLY);


    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    modeRef2->setReference("testMode2");
    modeRef2->setPriority("1");

    QSharedPointer<ModeReference> modeRef3(new ModeReference(*modeRef1));
    modeRef3->setReference("testMode3");
    modeRef3->setPriority("2");

    QSharedPointer<AccessPolicy> accessPolicy2(new AccessPolicy());
    accessPolicy2->getModeReferences()->append(modeRef2);
    accessPolicy2->getModeReferences()->append(modeRef3);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Register> testRegister(new Register("testRegister", "offset", "10"));
    testRegister->getFields()->append(simpleField);
    testRegister->getAccessPolicies()->append(accessPolicy1);
    testRegister->getAccessPolicies()->append(accessPolicy2);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:accessPolicies>"
                "<ipxact:accessPolicy>"
                    "<ipxact:modeRef priority=\"0\">testMode</ipxact:modeRef>"
                    "<ipxact:access>read-only</ipxact:access>"
                "</ipxact:accessPolicy>"
                "<ipxact:accessPolicy>"
                    "<ipxact:modeRef priority=\"1\">testMode2</ipxact:modeRef>"
                    "<ipxact:modeRef priority=\"2\">testMode3</ipxact:modeRef>"
                "</ipxact:accessPolicy>"
            "</ipxact:accessPolicies>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
        "</ipxact:register>"
    );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterParameters()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
    testRegister->getFields()->append(simpleField);
    testRegister->getParameters()->append(testParameter);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"testID\">"
                    "<ipxact:name>testParameter</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:register>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterVendorExtensions()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Field> simpleField(new Field("simpleField"));
    simpleField->setBitOffset("fieldOffset");
    simpleField->setBitWidth("fieldWidth");

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<Register> testRegister (new Register("testRegister", "offset", "10"));
    testRegister->setDimension("16");
    testRegister->getFields()->append(simpleField);
    testRegister->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:register>"
            "<ipxact:name>testRegister</ipxact:name>"
            "<ipxact:dim>16</ipxact:dim>"
            "<ipxact:addressOffset>offset</ipxact:addressOffset>"
            "<ipxact:size>10</ipxact:size>"
            "<ipxact:field>"
                "<ipxact:name>simpleField</ipxact:name>"
                "<ipxact:bitOffset>fieldOffset</ipxact:bitOffset>"
                "<ipxact:bitWidth>fieldWidth</ipxact:bitWidth>"
            "</ipxact:field>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:register>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegister, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeSimpleRegisterFile()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeSimpleRegisterFile()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<RegisterFile> testRegisterFile (new RegisterFile("testFile", "fileOffset", "8"));
    testRegisterFile->setDisplayName("displayed");
    testRegisterFile->setDescription("described");

    QString expectedOutput(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
        "</ipxact:registerFile>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegisterFile, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterFileIsPresent()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterFileIsPresent()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<RegisterFile> testRegisterFile (new RegisterFile("testFile", "fileOffset", "8"));
    testRegisterFile->setIsPresent("14/2-7");

    QString expectedOutput(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:isPresent>14/2-7</ipxact:isPresent>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
        "</ipxact:registerFile>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegisterFile, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterFileDimension()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterFileDimension()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<RegisterFile> testRegisterFile (new RegisterFile("testFile", "fileOffset", "8"));
    testRegisterFile->setDimension("DimensionProbability");

    QString expectedOutput(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:dim>DimensionProbability</ipxact:dim>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
        "</ipxact:registerFile>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegisterFile, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterFileTypeIdentifier()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterFileTypeIdentifier()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<RegisterFile> testRegisterFile (new RegisterFile("testFile", "fileOffset", "8"));
    testRegisterFile->setTypeIdentifier("papersPlease");

    QString expectedOutput(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:typeIdentifier>papersPlease</ipxact:typeIdentifier>"
            "<ipxact:range>8</ipxact:range>"
        "</ipxact:registerFile>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegisterFile, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterFileRegisterData()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterFileRegisterData()
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

    QSharedPointer<RegisterFile> testRegisterFile (new RegisterFile("testFile", "fileOffset", "8"));
    testRegisterFile->getRegisterData()->append(testRegister);
    testRegisterFile->getRegisterData()->append(containedRegisterFile);

    QString expectedOutput(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
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
        "</ipxact:registerFile>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegisterFile, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterFileDefinitionReference2022()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterFileDefinitionReference2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<RegisterFile> testRegisterFile(new RegisterFile("testFile", "fileOffset"));
    testRegisterFile->setDisplayName("displayed");
    testRegisterFile->setShortDescription("testShortDescription");
    testRegisterFile->setDescription("described");

    testRegisterFile->setRegisterFileDefinitionReference("testRegisterFileDefinition");
    testRegisterFile->setTypeDefinitionsReference("testTypeDefinitions");

    QString expectedOutput(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:shortDescription>testShortDescription</ipxact:shortDescription>"
            "<ipxact:description>described</ipxact:description>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:registerFileDefinitionRef typeDefinitions=\"testTypeDefinitions\">testRegisterFileDefinition</ipxact:registerFileDefinitionRef>"
        "</ipxact:registerFile>"
    );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegisterFile, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterFileMemoryArray2022()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterFileMemoryArray2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<MemoryArray::Dimension> dim1(new MemoryArray::Dimension());
    dim1->indexVar_ = "testDim";
    dim1->value_ = "1+1";

    QSharedPointer<MemoryArray::Dimension> dim2(new MemoryArray::Dimension());
    dim2->value_ = "4";

    QSharedPointer<MemoryArray> memArray(new MemoryArray());
    memArray->setStride("4");
    memArray->getDimensions()->append(dim1);
    memArray->getDimensions()->append(dim2);

    QSharedPointer<RegisterFile> testRegisterFile(new RegisterFile("testFile", "fileOffset", "8"));
    testRegisterFile->setMemoryArray(memArray);

    QString expectedOutput(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:array>"
                "<ipxact:dim indexVar=\"testDim\">1+1</ipxact:dim>"
                "<ipxact:dim>4</ipxact:dim>"
                "<ipxact:stride>4</ipxact:stride>"
            "</ipxact:array>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
        "</ipxact:registerFile>"
    );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegisterFile, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterFileAccessPolicies2022()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterFileAccessPolicies2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    modeRef1->setReference("testMode");
    modeRef1->setPriority("0");

    QSharedPointer<AccessPolicy> accessPolicy1(new AccessPolicy());
    accessPolicy1->getModeReferences()->append(modeRef1);
    accessPolicy1->setAccess(AccessTypes::READ_ONLY);


    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    modeRef2->setReference("testMode2");
    modeRef2->setPriority("1");

    QSharedPointer<ModeReference> modeRef3(new ModeReference(*modeRef1));
    modeRef3->setReference("testMode3");
    modeRef3->setPriority("2");

    QSharedPointer<AccessPolicy> accessPolicy2(new AccessPolicy());
    accessPolicy2->getModeReferences()->append(modeRef2);
    accessPolicy2->getModeReferences()->append(modeRef3);

    QSharedPointer<RegisterFile> testRegisterFile(new RegisterFile("testFile", "fileOffset", "8"));
    testRegisterFile->getAccessPolicies()->append(accessPolicy1);
    testRegisterFile->getAccessPolicies()->append(accessPolicy2);

    QString expectedOutput(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
            "<ipxact:accessPolicies>"
                "<ipxact:accessPolicy>"
                    "<ipxact:modeRef priority=\"0\">testMode</ipxact:modeRef>"
                    "<ipxact:access>read-only</ipxact:access>"
                "</ipxact:accessPolicy>"
                "<ipxact:accessPolicy>"
                    "<ipxact:modeRef priority=\"1\">testMode2</ipxact:modeRef>"
                    "<ipxact:modeRef priority=\"2\">testMode3</ipxact:modeRef>"
                "</ipxact:accessPolicy>"
            "</ipxact:accessPolicies>"
        "</ipxact:registerFile>"
    );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegisterFile, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterFileParameters()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterFileParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    QSharedPointer<RegisterFile> testRegisterFile (new RegisterFile("testFile", "fileOffset", "8"));
    testRegisterFile->setDimension("DimensionProbability");
    testRegisterFile->getParameters()->append(testParameter);

    QString expectedOutput(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:dim>DimensionProbability</ipxact:dim>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"testID\">"
                    "<ipxact:name>testParameter</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:registerFile>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegisterFile, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterWriter::writeRegisterFileVendorExtensions()
//-----------------------------------------------------------------------------
void tst_RegisterWriter::writeRegisterFileVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    QSharedPointer<RegisterFile> testRegisterFile (new RegisterFile("testFile", "fileOffset", "8"));
    testRegisterFile->setDimension("DimensionProbability");
    testRegisterFile->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:dim>DimensionProbability</ipxact:dim>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:registerFile>"
        );

    RegisterWriter::writeRegisterData(xmlStreamWriter, testRegisterFile, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_RegisterWriter)

#include "tst_RegisterWriter.moc"

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

    void writeRegisterParameters();
    void writeRegisterVendorExtensions();

    void writeSimpleRegisterFile();
    void writeRegisterFileIsPresent();
    void writeRegisterFileDimension();
    void writeRegisterFileTypeIdentifier();
    void writeRegisterFileRegisterData();

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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegister);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegister);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegister);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegister);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegister);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegister);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegister);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegister);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegister);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegister);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegisterFile);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegisterFile);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegisterFile);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegisterFile);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegisterFile);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegisterFile);
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

    RegisterWriter registerWriter;
    registerWriter.writeRegisterData(xmlStreamWriter, testRegisterFile);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_RegisterWriter)

#include "tst_RegisterWriter.moc"

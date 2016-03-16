//-----------------------------------------------------------------------------
// File: tst_RegisterReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.09.2015
//
// Description:
// Unit test for class RegisterReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/RegisterReader.h>
#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/AlternateRegister.h>
#include <IPXACTmodels/Component/Field.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>
#include <QDomDocument>

class tst_RegisterReader : public QObject
{
    Q_OBJECT

public:
    tst_RegisterReader();

private slots:

    void readSimpleRegister();
    void readRegisterIsPresent();
    void readRegisterDimension();
    void readRegisterTypeIdentifier();
    void readRegisterVolatile();
    void readRegisterAccess();
    void readRegisterFields();
    void readRegisterAlternateRegisters();

    void readRegisterParameters();
    void readRegisterVendorExtensions();

    void readSimpleRegisterFile();
    void readRegisterFileIsPresent();
    void readRegisterFileDimension();
    void readRegisterFileTypeIdentifier();
    void readRegisterFileRegisterData();

    void readRegisterFileParameters();
    void readRegisterFileVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::tst_RegisterReader()
//-----------------------------------------------------------------------------
tst_RegisterReader::tst_RegisterReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readSimpleRegister()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readSimpleRegister()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerNode = document.firstChildElement("ipxact:register");

    RegisterReader registerReader;
    QSharedPointer<Register> testRegister = registerReader.createRegisterfrom(registerNode);

    QCOMPARE(testRegister->name(), QString("testRegister"));
    QCOMPARE(testRegister->displayName(), QString("displayed"));
    QCOMPARE(testRegister->description(), QString("described"));
    QCOMPARE(testRegister->getAddressOffset(), QString("offset"));
    QCOMPARE(testRegister->getSize(), QString("10"));

    QCOMPARE(testRegister->getFields()->size(), 1);
    QCOMPARE(testRegister->getFields()->first()->name(), QString("simpleField"));
    QCOMPARE(testRegister->getFields()->first()->getBitOffset(), QString("fieldOffset"));
    QCOMPARE(testRegister->getFields()->first()->getBitWidth(), QString("fieldWidth"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterIsPresent()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterIsPresent()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerNode = document.firstChildElement("ipxact:register");

    RegisterReader registerReader;
    QSharedPointer<Register> testRegister = registerReader.createRegisterfrom(registerNode);

    QCOMPARE(testRegister->name(), QString("testRegister"));
    QCOMPARE(testRegister->getIsPresent(), QString("4-3"));

    QCOMPARE(testRegister->getFields()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterDimension()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterDimension()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerNode = document.firstChildElement("ipxact:register");

    RegisterReader registerReader;
    QSharedPointer<Register> testRegister = registerReader.createRegisterfrom(registerNode);

    QCOMPARE(testRegister->name(), QString("testRegister"));
    QCOMPARE(testRegister->getDimension(), QString("16"));

    QCOMPARE(testRegister->getFields()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterTypeIdentifier()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterTypeIdentifier()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerNode = document.firstChildElement("ipxact:register");

    RegisterReader registerReader;
    QSharedPointer<Register> testRegister = registerReader.createRegisterfrom(registerNode);

    QCOMPARE(testRegister->name(), QString("testRegister"));
    QCOMPARE(testRegister->getTypeIdentifier(), QString("identifier"));
    QCOMPARE(testRegister->getVolatile(), QString(""));

    QCOMPARE(testRegister->getFields()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterVolatile()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterVolatile()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerNode = document.firstChildElement("ipxact:register");

    RegisterReader registerReader;
    QSharedPointer<Register> testRegister = registerReader.createRegisterfrom(registerNode);

    QCOMPARE(testRegister->name(), QString("testRegister"));
    QCOMPARE(testRegister->getVolatile(), QString("true"));

    QCOMPARE(testRegister->getFields()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterAccess()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterAccess()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerNode = document.firstChildElement("ipxact:register");

    RegisterReader registerReader;
    QSharedPointer<Register> testRegister = registerReader.createRegisterfrom(registerNode);

    QCOMPARE(testRegister->name(), QString("testRegister"));
    QCOMPARE(testRegister->getAccess(), AccessTypes::WRITEONCE);

    QCOMPARE(testRegister->getFields()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterFields()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterFields()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerNode = document.firstChildElement("ipxact:register");

    RegisterReader registerReader;
    QSharedPointer<Register> testRegister = registerReader.createRegisterfrom(registerNode);

    QCOMPARE(testRegister->name(), QString("testRegister"));

    QCOMPARE(testRegister->getFields()->size(), 2);
    QCOMPARE(testRegister->getFields()->first()->name(), QString("simpleField"));
    QCOMPARE(testRegister->getFields()->first()->getBitOffset(), QString("fieldOffset"));
    QCOMPARE(testRegister->getFields()->first()->getBitWidth(), QString("fieldWidth"));
    QCOMPARE(testRegister->getFields()->last()->name(), QString("unsimplified"));
    QCOMPARE(testRegister->getFields()->last()->getBitOffset(), QString("OtherOff"));
    QCOMPARE(testRegister->getFields()->last()->getBitWidth(), QString("otherWidth"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterAlternateRegisters()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterAlternateRegisters()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerNode = document.firstChildElement("ipxact:register");

    RegisterReader registerReader;
    QSharedPointer<Register> testRegister = registerReader.createRegisterfrom(registerNode);

    QCOMPARE(testRegister->name(), QString("testRegister"));

    QCOMPARE(testRegister->getFields()->size(), 1);

    QCOMPARE(testRegister->getAlternateRegisters()->size(), 1);
    QSharedPointer<AlternateRegister> alternateRegister = testRegister->getAlternateRegisters()->first();
    QCOMPARE(alternateRegister->name(), QString("alteredBeast"));
    QCOMPARE(alternateRegister->displayName(), QString("alteredDisplay"));
    QCOMPARE(alternateRegister->description(), QString("alteredDescription"));
    QCOMPARE(alternateRegister->getIsPresent(), QString("presence"));
    QCOMPARE(alternateRegister->getTypeIdentifier(), QString("Volgarr"));
    QCOMPARE(alternateRegister->getVolatile(), QString("false"));
    QCOMPARE(alternateRegister->getAccess(), AccessTypes::READ_WRITE);

    QCOMPARE(alternateRegister->getAlternateGroups()->size(), 1);
    QCOMPARE(alternateRegister->getAlternateGroups()->first(), QString("wolf"));

    QCOMPARE(alternateRegister->getFields()->size(), 1);
    QCOMPARE(alternateRegister->getFields()->first()->name(), QString("Hades"));
    QCOMPARE(alternateRegister->getFields()->first()->getBitOffset(), QString("alternativeOffset"));
    QCOMPARE(alternateRegister->getFields()->first()->getBitWidth(), QString("alternativeWidth"));

    QCOMPARE(alternateRegister->getParameters()->size(), 1);
    QCOMPARE(alternateRegister->getParameters()->first()->name(), QString("testParameter"));
    QCOMPARE(alternateRegister->getParameters()->first()->getValueId(), QString("testID"));
    QCOMPARE(alternateRegister->getParameters()->first()->getValue(), QString("1"));

    QCOMPARE(alternateRegister->getVendorExtensions()->size(), 1);
    QCOMPARE(alternateRegister->getVendorExtensions()->first()->type(), QString("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterParameters()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterParameters()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerNode = document.firstChildElement("ipxact:register");

    RegisterReader registerReader;
    QSharedPointer<Register> testRegister = registerReader.createRegisterfrom(registerNode);

    QCOMPARE(testRegister->name(), QString("testRegister"));

    QCOMPARE(testRegister->getFields()->size(), 1);

    QCOMPARE(testRegister->getParameters()->size(), 1);
    QCOMPARE(testRegister->getParameters()->first()->getValueId(), QString("testID"));
    QCOMPARE(testRegister->getParameters()->first()->name(), QString("testParameter"));
    QCOMPARE(testRegister->getParameters()->first()->getValue(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterVendorExtensions()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterVendorExtensions()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerNode = document.firstChildElement("ipxact:register");

    RegisterReader registerReader;
    QSharedPointer<Register> testRegister = registerReader.createRegisterfrom(registerNode);

    QCOMPARE(testRegister->name(), QString("testRegister"));

    QCOMPARE(testRegister->getFields()->size(), 1);

    QCOMPARE(testRegister->getVendorExtensions()->size(), 1);
    QCOMPARE(testRegister->getVendorExtensions()->first()->type(), QString("testExtension"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readSimpleRegisterFile()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readSimpleRegisterFile()
{
    QString documentContent(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:displayName>displayed</ipxact:displayName>"
            "<ipxact:description>described</ipxact:description>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
        "</ipxact:registerFile>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerFileNode = document.firstChildElement("ipxact:registerFile");

    RegisterReader registerReader;
    QSharedPointer<RegisterFile> testRegisterFile = registerReader.createRegisterFileFrom(registerFileNode);

    QCOMPARE(testRegisterFile->name(), QString("testFile"));
    QCOMPARE(testRegisterFile->displayName(), QString("displayed"));
    QCOMPARE(testRegisterFile->description(), QString("described"));
    QCOMPARE(testRegisterFile->getAddressOffset(), QString("fileOffset"));
    QCOMPARE(testRegisterFile->getRange(), QString("8"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::parseRegisterFileIsPresent()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterFileIsPresent()
{
    QString documentContent(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:isPresent>14/2-7</ipxact:isPresent>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
        "</ipxact:registerFile>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerFileNode = document.firstChildElement("ipxact:registerFile");

    RegisterReader registerReader;
    QSharedPointer<RegisterFile> testRegisterFile = registerReader.createRegisterFileFrom(registerFileNode);

    QCOMPARE(testRegisterFile->name(), QString("testFile"));
    QCOMPARE(testRegisterFile->getIsPresent(), QString("14/2-7"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterFileDimension()
{
    QString documentContent(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:dim>DimensionProbability</ipxact:dim>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
        "</ipxact:registerFile>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerFileNode = document.firstChildElement("ipxact:registerFile");

    RegisterReader registerReader;
    QSharedPointer<RegisterFile> testRegisterFile = registerReader.createRegisterFileFrom(registerFileNode);

    QCOMPARE(testRegisterFile->name(), QString("testFile"));
    QCOMPARE(testRegisterFile->getDimension(), QString("DimensionProbability"));
};

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterFileTypeIdentifier()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterFileTypeIdentifier()
{
    QString documentContent(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:typeIdentifier>papersPlease</ipxact:typeIdentifier>"
            "<ipxact:range>8</ipxact:range>"
        "</ipxact:registerFile>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerFileNode = document.firstChildElement("ipxact:registerFile");

    RegisterReader registerReader;
    QSharedPointer<RegisterFile> testRegisterFile = registerReader.createRegisterFileFrom(registerFileNode);

    QCOMPARE(testRegisterFile->name(), QString("testFile"));
    QCOMPARE(testRegisterFile->getTypeIdentifier(), QString("papersPlease"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterFileRegisterData()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterFileRegisterData()
{
    QString documentContent(
        "<ipxact:registerFile>"
            "<ipxact:name>testFile</ipxact:name>"
            "<ipxact:addressOffset>fileOffset</ipxact:addressOffset>"
            "<ipxact:range>8</ipxact:range>"
            "<ipxact:register>"
                "<ipxact:name>testRegister</ipxact:name>"
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerFileNode = document.firstChildElement("ipxact:registerFile");

    RegisterReader registerReader;
    QSharedPointer<RegisterFile> testRegisterFile = registerReader.createRegisterFileFrom(registerFileNode);

    QCOMPARE(testRegisterFile->name(), QString("testFile"));
    
    QCOMPARE(testRegisterFile->getRegisterData()->size(), 2);
    
    QSharedPointer<Register> containedRegister =
        testRegisterFile->getRegisterData()->first().dynamicCast<Register>();
    QCOMPARE(containedRegister.isNull(), false);
    QCOMPARE(containedRegister->name(), QString("testRegister"));
    QCOMPARE(containedRegister->getAddressOffset(), QString("offset"));
    QCOMPARE(containedRegister->getSize(), QString("10"));
    QCOMPARE(containedRegister->getFields()->size(), 1);
    QCOMPARE(containedRegister->getFields()->first()->name(), QString("simpleField"));
    QCOMPARE(containedRegister->getFields()->first()->getBitOffset(), QString("fieldOffset"));
    QCOMPARE(containedRegister->getFields()->first()->getBitWidth(), QString("fieldWidth"));

    QSharedPointer<RegisterFile> containedFile =
        testRegisterFile->getRegisterData()->last().dynamicCast<RegisterFile>();
    QCOMPARE(containedFile.isNull(), false);
    QCOMPARE(containedFile->name(), QString("contained"));
    QCOMPARE(containedFile->getAddressOffset(), QString("containedOffset"));
    QCOMPARE(containedFile->getRange(), QString("containedRange"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterFileParameters()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterFileParameters()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerFileNode = document.firstChildElement("ipxact:registerFile");

    RegisterReader registerReader;
    QSharedPointer<RegisterFile> testRegisterFile = registerReader.createRegisterFileFrom(registerFileNode);

    QCOMPARE(testRegisterFile->name(), QString("testFile"));

    QCOMPARE(testRegisterFile->getParameters()->size(), 1);
    QCOMPARE(testRegisterFile->getParameters()->first()->getValueId(), QString("testID"));
    QCOMPARE(testRegisterFile->getParameters()->first()->name(), QString("testParameter"));
    QCOMPARE(testRegisterFile->getParameters()->first()->getValue(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_RegisterReader::readRegisterFileVendorExtensions()
//-----------------------------------------------------------------------------
void tst_RegisterReader::readRegisterFileVendorExtensions()
{
    QString documentContent(
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

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode registerFileNode = document.firstChildElement("ipxact:registerFile");

    RegisterReader registerReader;
    QSharedPointer<RegisterFile> testRegisterFile = registerReader.createRegisterFileFrom(registerFileNode);

    QCOMPARE(testRegisterFile->name(), QString("testFile"));
    
    QCOMPARE(testRegisterFile->getVendorExtensions()->size(), 1);
    QCOMPARE(testRegisterFile->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_RegisterReader)

#include "tst_RegisterReader.moc"

//-----------------------------------------------------------------------------
// File: tst_FieldReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: <Name>
// Date: <Date in the format dd.mm.yyyy>
//
// Description:
// Unit test for class FieldReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/Component/FieldReader.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/MemoryArray.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/FieldReset.h>
#include <IPXACTmodels/Component/FieldReference.h>

#include <QtTest>

class tst_FieldReader : public QObject
{
    Q_OBJECT

public:
    tst_FieldReader();

private slots:

    void readSimpleField();
    void readFieldID();
    void readIsPresent();
    void readMemoryArray2022();
    void readFieldDefinitionRef();
    void readResets();

    void readTypeIdentifier();
    void readVolatile();
    void readFieldReference();
    void readFieldAccessPolicies();
    void readAccess();
    void readEnumeratedValues();
    void readModifiedWritevalue();
    void readWriteValueConstraint();
    void readReadAction();
    void readTestable();
    void readReserved();

    void readParameters();

    void readVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::tst_FieldReader()
//-----------------------------------------------------------------------------
tst_FieldReader::tst_FieldReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readSimpleField()
//-----------------------------------------------------------------------------
void tst_FieldReader::readSimpleField()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:description>testDescription</ipxact:description>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->name(), QString("testField"));
    QCOMPARE(testField->displayName(), QString("testDisplay"));
    QCOMPARE(testField->description(), QString("testDescription"));
    QCOMPARE(testField->getBitOffset(), QString("Baldur's"));
    QCOMPARE(testField->getBitWidth(), QString("Gate"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readFieldID()
//-----------------------------------------------------------------------------
void tst_FieldReader::readFieldID()
{
    QString documentContent(
        "<ipxact:field fieldID=\"AM\">"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->name(), QString("testField"));
    QCOMPARE(testField->getBitOffset(), QString("Baldur's"));
    QCOMPARE(testField->getBitWidth(), QString("Gate"));
    QCOMPARE(testField->getId(), QString("AM"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readIsPresent()
//-----------------------------------------------------------------------------
void tst_FieldReader::readIsPresent()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:isPresent>presence*2</ipxact:isPresent>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getIsPresent(), QString("presence*2"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readMemoryArray2022()
//-----------------------------------------------------------------------------
void tst_FieldReader::readMemoryArray2022()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:array>"
                "<ipxact:dim indexVar=\"i\">8</ipxact:dim>"
                "<ipxact:bitStride>8</ipxact:bitStride>"
            "</ipxact:array>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std22);

    auto memArray = testField->getMemoryArray();
    auto dimension = memArray->getDimensions()->first();
    auto bitStride = memArray->getStride();

    QCOMPARE(dimension->value_, QString("8"));
    QCOMPARE(dimension->indexVar_, QString("i"));
    QCOMPARE(bitStride, QString("8"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readFieldDefinitionRef()
//-----------------------------------------------------------------------------
void tst_FieldReader::readFieldDefinitionRef()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:isPresent>presence*2</ipxact:isPresent>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:fieldDefinitionRef typeDefinitions=\"testTypeDefinition\">TESTDEF</ipxact:fieldDefinitionRef>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std22);

    QCOMPARE(testField->getFieldDefinitionRef(), QString("TESTDEF"));
    QCOMPARE(testField->getTypeDefinitionsRef(), QString("testTypeDefinition"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readResets()
//-----------------------------------------------------------------------------
void tst_FieldReader::readResets()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:resets>"
                "<ipxact:reset resetTypeRef=\"referencedReset\">"
                    "<ipxact:value>8'b0110_1101</ipxact:value>"
                    "<ipxact:mask>8'b1100_0011</ipxact:mask>"
                "</ipxact:reset>"
            "</ipxact:resets>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getResets()->count(), 1);

    QSharedPointer<FieldReset> testReset = testField->getResets()->first();
    QCOMPARE(testReset->getResetTypeReference(), QString("referencedReset"));
    QCOMPARE(testReset->getResetValue(), QString("8'b0110_1101"));
    QCOMPARE(testReset->getResetMask(), QString("8'b1100_0011"));

    documentContent =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:resets>"
                "<ipxact:reset resetTypeRef=\"referencedReset\">"
                    "<ipxact:value>8'b0110_1101</ipxact:value>"
                    "<ipxact:mask>8'b1100_0011</ipxact:mask>"
                "</ipxact:reset>"
                "<ipxact:reset>"
                    "<ipxact:value>8'b0110_0000</ipxact:value>"
                "</ipxact:reset>"
            "</ipxact:resets>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        ;

    document.setContent(documentContent);
    fieldNode = document.firstChildElement("ipxact:field");
    testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getResets()->count(), 2);

    QSharedPointer<FieldReset> firstReset = testField->getResets()->first();
    QSharedPointer<FieldReset> lastReset = testField->getResets()->last();

    QCOMPARE(firstReset->getResetTypeReference(), QString("referencedReset"));
    QCOMPARE(firstReset->getResetValue(), QString("8'b0110_1101"));
    QCOMPARE(firstReset->getResetMask(), QString("8'b1100_0011"));

    QCOMPARE(lastReset->getResetTypeReference(), QString(""));
    QCOMPARE(lastReset->getResetValue(), QString("8'b0110_0000"));
    QCOMPARE(lastReset->getResetMask(), QString(""));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readTypeIdentifier()
//-----------------------------------------------------------------------------
void tst_FieldReader::readTypeIdentifier()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:typeIdentifier>testTypeIdentifier</ipxact:typeIdentifier>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getTypeIdentifier(), QString("testTypeIdentifier"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readVolatile()
//-----------------------------------------------------------------------------
void tst_FieldReader::readVolatile()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:volatile>true</ipxact:volatile>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getVolatile().toString(), QString("true"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readFieldReference()
//-----------------------------------------------------------------------------
void tst_FieldReader::readFieldReference()
{
    // Test reading ipxact:aliasOf.
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:aliasOf>"
                "<ipxact:addressSpaceRef addressSpaceRef=\"testAddressSpace\"/>"
                "<ipxact:memoryMapRef memoryMapRef=\"testMemoryMap\"/>"
                "<ipxact:memoryRemapRef memoryRemapRef=\"testMemoryRemap\"/>"
                "<ipxact:bankRef bankRef=\"testBankRef\"/>"
                "<ipxact:bankRef bankRef=\"testBankRef2\"/>"
                "<ipxact:addressBlockRef addressBlockRef=\"testAddressBlock\">"
                    "<ipxact:indices>"
                        "<ipxact:index>1</ipxact:index>"
                        "<ipxact:index>2</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:addressBlockRef>"
                "<ipxact:registerFileRef registerFileRef=\"testRegisterFile\">"
                    "<ipxact:indices>"
                        "<ipxact:index>0</ipxact:index>"
                        "<ipxact:index>1</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:registerFileRef>"
                "<ipxact:registerFileRef registerFileRef=\"testRegisterFile2\">"
                    "<ipxact:indices>"
                        "<ipxact:index>3</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:registerFileRef>"
                "<ipxact:registerRef registerRef=\"testRegister\">"
                    "<ipxact:indices>"
                        "<ipxact:index>0</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:registerRef>"
                "<ipxact:alternateRegisterRef alternateRegisterRef=\"testAlternateRegister\"/>"
                "<ipxact:fieldRef fieldRef=\"testField\">"
                    "<ipxact:indices>"
                        "<ipxact:index>6</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:fieldRef>"
            "</ipxact:aliasOf>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std22);

    auto fieldReference = testField->getFieldReference();

    QCOMPARE(fieldReference->getReference(FieldReference::Type::ADDRESS_SPACE)->reference_, QString("testAddressSpace"));
    QCOMPARE(fieldReference->getReference(FieldReference::Type::MEMORY_MAP)->reference_, QString("testMemoryMap"));
    QCOMPARE(fieldReference->getReference(FieldReference::Type::MEMORY_REMAP)->reference_, QString("testMemoryRemap"));
    
    QCOMPARE(fieldReference->getMultipleReference(FieldReference::Type::BANK)->at(0)->reference_, QString("testBankRef"));
    QCOMPARE(fieldReference->getMultipleReference(FieldReference::Type::BANK)->at(1)->reference_, QString("testBankRef2"));

    QCOMPARE(fieldReference->getReference(FieldReference::Type::ADDRESS_BLOCK)->reference_, QString("testAddressBlock"));
    QCOMPARE(fieldReference->getReference(FieldReference::Type::ADDRESS_BLOCK)->indices_.first(), QString("1"));
    QCOMPARE(fieldReference->getReference(FieldReference::Type::ADDRESS_BLOCK)->indices_.at(1), QString("2"));

    QCOMPARE(fieldReference->getMultipleReference(FieldReference::Type::REGISTER_FILE)->first()->reference_, QString("testRegisterFile"));
    QCOMPARE(fieldReference->getMultipleReference(FieldReference::Type::REGISTER_FILE)->first()->indices_.first(), QString("0"));
    QCOMPARE(fieldReference->getMultipleReference(FieldReference::Type::REGISTER_FILE)->first()->indices_.at(1), QString("1"));

    QCOMPARE(fieldReference->getMultipleReference(FieldReference::Type::REGISTER_FILE)->at(1)->reference_, QString("testRegisterFile2"));
    QCOMPARE(fieldReference->getMultipleReference(FieldReference::Type::REGISTER_FILE)->at(1)->indices_.first(), QString("3"));

    QCOMPARE(fieldReference->getReference(FieldReference::Type::REGISTER)->reference_, QString("testRegister"));
    QCOMPARE(fieldReference->getReference(FieldReference::Type::REGISTER)->indices_.first(), QString("0"));

    QCOMPARE(fieldReference->getReference(FieldReference::Type::ALTERNATE_REGISTER)->reference_, QString("testAlternateRegister"));

    QCOMPARE(fieldReference->getReference(FieldReference::Type::FIELD)->reference_, QString("testField"));
    QCOMPARE(fieldReference->getReference(FieldReference::Type::FIELD)->indices_.first(), QString("6"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readFieldAccessPolicies()
//-----------------------------------------------------------------------------
void tst_FieldReader::readFieldAccessPolicies()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>8</ipxact:bitOffset>"
            "<ipxact:bitWidth>16</ipxact:bitWidth>"
            "<ipxact:fieldAccessPolicies>"
                "<ipxact:fieldAccessPolicy>"
                    "<ipxact:modeRef priority=\"0\">testMode</ipxact:modeRef>"
                    "<ipxact:fieldAccessPolicyDefinitionRef typeDefinitions=\"testDefinition\"/>"
                "</ipxact:fieldAccessPolicy>"
            "</ipxact:fieldAccessPolicies>"
        "</ipxact:field>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");
    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std22);

    QCOMPARE(testField->getFieldAccessPolicies()->size(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readAccess()
//-----------------------------------------------------------------------------
void tst_FieldReader::readAccess()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:access>read-write</ipxact:access>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getAccess(), AccessTypes::READ_WRITE);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readEnumeratedValues()
//-----------------------------------------------------------------------------
void tst_FieldReader::readEnumeratedValues()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:enumeratedValues>"
                "<ipxact:enumeratedValue>"
                    "<ipxact:name>testEnumeration</ipxact:name>"
                    "<ipxact:value>DaiGurren</ipxact:value>"
                "</ipxact:enumeratedValue>"
            "</ipxact:enumeratedValues>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getEnumeratedValues()->size(), 1);

    QSharedPointer<EnumeratedValue> enumeratedValue = testField->getEnumeratedValues()->first();
    QCOMPARE(enumeratedValue->name(), QString("testEnumeration"));
    QCOMPARE(enumeratedValue->getValue(), QString("DaiGurren"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readModifiedWritevalue()
//-----------------------------------------------------------------------------
void tst_FieldReader::readModifiedWritevalue()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:modifiedWriteValue>zeroToClear</ipxact:modifiedWriteValue>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getModifiedWrite(), General::ZERO_TO_CLEAR);

    documentContent =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:modifiedWriteValue modify=\"modifier\">oneToToggle</ipxact:modifiedWriteValue>"
        "</ipxact:field>"
        ;

    document.setContent(documentContent);
    fieldNode = document.firstChildElement("ipxact:field");
    testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getModifiedWrite(), General::ONE_TO_TOGGLE);
    QCOMPARE(testField->getModifiedWriteModify(), QString("modifier"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readWriteValueConstraint()
//-----------------------------------------------------------------------------
void tst_FieldReader::readWriteValueConstraint()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:writeAsRead>true</ipxact:writeAsRead>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getWriteConstraint().isNull(), false);
    QCOMPARE(testField->getWriteConstraint()->getType(), WriteValueConstraint::WRITE_AS_READ);

    documentContent =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:useEnumeratedValues>true</ipxact:useEnumeratedValues>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:field>"
        ;

    document.setContent(documentContent);
    fieldNode = document.firstChildElement("ipxact:field");
    testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getWriteConstraint().isNull(), false);
    QCOMPARE(testField->getWriteConstraint()->getType(), WriteValueConstraint::USE_ENUM);

    documentContent =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:minimum>0</ipxact:minimum>"
                "<ipxact:maximum>10</ipxact:maximum>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:field>"
        ;

    document.setContent(documentContent);
    fieldNode = document.firstChildElement("ipxact:field");
    testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getWriteConstraint().isNull(), false);
    QCOMPARE(testField->getWriteConstraint()->getType(), WriteValueConstraint::MIN_MAX);
    QCOMPARE(testField->getWriteConstraint()->getMinimum(), QString("0"));
    QCOMPARE(testField->getWriteConstraint()->getMaximum(), QString("10"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readReadAction()
//-----------------------------------------------------------------------------
void tst_FieldReader::readReadAction()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:readAction>set</ipxact:readAction>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getReadAction(), General::READ_SET);

    documentContent =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:readAction modify=\"modifier\">clear</ipxact:readAction>"
        "</ipxact:field>"
        ;

    document.setContent(documentContent);
    fieldNode = document.firstChildElement("ipxact:field");
    testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getReadAction(), General::READ_CLEAR);
    QCOMPARE(testField->getReadActionModify(), QString("modifier"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readTestable()
//-----------------------------------------------------------------------------
void tst_FieldReader::readTestable()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:testable>false</ipxact:testable>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getTestable().toString(), QString("false"));

    documentContent =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:testable testConstraint=\"restore\">true</ipxact:testable>"
        "</ipxact:field>"
        ;

    document.setContent(documentContent);
    fieldNode = document.firstChildElement("ipxact:field");
    testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getTestable().toString(), QString("true"));
    QCOMPARE(testField->getTestConstraint(), General::TEST_RESTORE);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readReserved()
//-----------------------------------------------------------------------------
void tst_FieldReader::readReserved()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:reserved>reserveWarning</ipxact:reserved>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getReserved(), QString("reserveWarning"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readParameters()
//-----------------------------------------------------------------------------
void tst_FieldReader::readParameters()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"testID\">"
                    "<ipxact:name>testParameter</ipxact:name>"
                    "<ipxact:value>1</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getParameters()->size(), 1);
    QCOMPARE(testField->getParameters()->first()->name(), QString("testParameter"));
    QCOMPARE(testField->getParameters()->first()->getValue(), QString("1"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_FieldReader::readVendorExtensions()
{
    QString documentContent(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:field>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    QSharedPointer<Field> testField = FieldReader::createFieldFrom(fieldNode, Document::Revision::Std14);

    QCOMPARE(testField->getVendorExtensions()->size(), 1);
    QCOMPARE(testField->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_FieldReader)

#include "tst_FieldReader.moc"

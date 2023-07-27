//-----------------------------------------------------------------------------
// File: tst_FieldWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.09.2015
//
// Description:
// Unit test for class FieldWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/GenericVendorExtension.h>
#include <IPXACTmodels/common/Parameter.h>

#include <IPXACTmodels/Component/FieldWriter.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/Component/EnumeratedValue.h>
#include <IPXACTmodels/Component/WriteValueConstraint.h>
#include <IPXACTmodels/Component/FieldReset.h>
#include <IPXACTmodels/Component/MemoryArray.h>
#include <IPXACTmodels/Component/FieldReference.h>

#include <QtTest>

class tst_FieldWriter : public QObject
{
    Q_OBJECT

public:
    tst_FieldWriter();

private slots:
    
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void writeSimpleField();
    void writeFieldID();
    void writeIsPresent();
    void writeMemoryArray2022();
    void writeFieldDefinitionRef();
    void writeResets();

    void writeTypeIdentifier();
    void writeVolatile();
    void writeFieldReference2022();
    void writeAccess();
    void writeEnumeratedValues();
    void writeModifiedWriteValue();
    void writeWriteValueConstraint();
    void writeReadAction();
    void writeTestable();
    void writeReserved();

    void writeParameters();

    void writeVendorExtensions();

private:

    QSharedPointer<Field> testField_;
};

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::tst_FieldWriter()
//-----------------------------------------------------------------------------
tst_FieldWriter::tst_FieldWriter() :
testField_()
{
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_FieldWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_FieldWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::init()
//-----------------------------------------------------------------------------
void tst_FieldWriter::init()
{
    testField_ = QSharedPointer<Field>(new Field("testField"));
    testField_->setBitOffset("Baldur's");
    testField_->setBitWidth("Gate");
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_FieldWriter::cleanup()
{
    testField_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeSimpleField()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeSimpleField()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();

    testField_->setDisplayName("testDisplay");
    testField_->setDescription("testDescription");

    expectedOutput =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:displayName>testDisplay</ipxact:displayName>"
            "<ipxact:description>testDescription</ipxact:description>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        ;

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeFieldID()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeFieldID()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testField_->setId("AM");

    QString expectedOutput(
        "<ipxact:field fieldID=\"AM\">"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeIsPresent()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeIsPresent()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testField_->setIsPresent("presence*2");

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:isPresent>presence*2</ipxact:isPresent>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeMemoryArray2022()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeMemoryArray2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<MemoryArray> memArray(new MemoryArray());
    QSharedPointer<MemoryArray::Dimension> dim(new MemoryArray::Dimension({ QString("8"), QString("i") }));
    memArray->getDimensions()->append(dim);
    memArray->setStride("16");
    testField_->setMemoryArray(memArray);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:array>"
                "<ipxact:dim indexVar=\"i\">8</ipxact:dim>"
                "<ipxact:bitStride>16</ipxact:bitStride>"
            "</ipxact:array>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeFieldDefinitionRef()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeFieldDefinitionRef()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testField_->setFieldDefinitionRef("TESTDEF");
    testField_->setTypeDefinitionsRef("testTypeDefinitionsRef");
    testField_->setBitWidth("");

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:fieldDefinitionRef typeDefinitions=\"testTypeDefinitionsRef\">TESTDEF</ipxact:fieldDefinitionRef>"
        "</ipxact:field>"
    );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeResets()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeResets()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<FieldReset> firstReset(new FieldReset());
    firstReset->setResetTypeReference("HARD");
    firstReset->setResetValue("8'b0000_0000");
    firstReset->setResetMask("8'b1111_1111");
    QSharedPointer<FieldReset> lastReset(new FieldReset());
    lastReset->setResetTypeReference("referencedReset");
    lastReset->setResetValue("8'b0110_1101");
    lastReset->setResetMask("8'b1100_0011");

    testField_->getResets()->append(firstReset);
    testField_->getResets()->append(lastReset);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:resets>"
                "<ipxact:reset resetTypeRef=\"HARD\">"
                    "<ipxact:value>8'b0000_0000</ipxact:value>"
                    "<ipxact:mask>8'b1111_1111</ipxact:mask>"
                "</ipxact:reset>"
                "<ipxact:reset resetTypeRef=\"referencedReset\">"
                    "<ipxact:value>8'b0110_1101</ipxact:value>"
                    "<ipxact:mask>8'b1100_0011</ipxact:mask>"
                "</ipxact:reset>"
            "</ipxact:resets>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeTypeIdentifier()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeTypeIdentifier()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testField_->setTypeIdentifier("testTypeIdentifier");

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:typeIdentifier>testTypeIdentifier</ipxact:typeIdentifier>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeVolatile()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeVolatile()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testField_->setVolatile(true);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:volatile>true</ipxact:volatile>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeFieldReference2022()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeFieldReference2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<FieldReference> fieldReference(new FieldReference());

    testField_->setBitWidth("");

    QSharedPointer<FieldReference::IndexedReference> addressSpace(new FieldReference::IndexedReference({ QString("testAddressSpace"), QList<QString>() }));
    fieldReference->setReference(addressSpace, FieldReference::Type::ADDRESS_SPACE);

    QSharedPointer<FieldReference::IndexedReference> memoryMap(new FieldReference::IndexedReference({ QString("testMemoryMap"), QList<QString>() }));
    fieldReference->setReference(memoryMap, FieldReference::Type::MEMORY_MAP);

    QSharedPointer<FieldReference::IndexedReference> memoryRemap(new FieldReference::IndexedReference({ QString("testMemoryRemap"), QList<QString>() }));
    fieldReference->setReference(memoryRemap, FieldReference::Type::MEMORY_REMAP);

    QSharedPointer<FieldReference::IndexedReference> bank1(new FieldReference::IndexedReference({ QString("testBank"), QList<QString>() }));
    fieldReference->setReference(bank1, FieldReference::Type::BANK);

    QSharedPointer<FieldReference::IndexedReference> bank2(new FieldReference::IndexedReference({ QString("testBank2"), QList<QString>() }));
    fieldReference->setReference(bank2, FieldReference::Type::BANK);
    
    QSharedPointer<FieldReference::IndexedReference> addressBlock(new FieldReference::IndexedReference({ QString("testAddressBlock"), QList<QString>({ "0", "1" }) }));
    fieldReference->setReference(addressBlock, FieldReference::Type::ADDRESS_BLOCK);

    QSharedPointer<FieldReference::IndexedReference> regFile1(new FieldReference::IndexedReference({ QString("testRegisterFile1"), QList<QString>({ "2", "3" }) }));
    fieldReference->setReference(regFile1, FieldReference::Type::REGISTER_FILE);

    QSharedPointer<FieldReference::IndexedReference> regFile2(new FieldReference::IndexedReference({ QString("testRegisterFile2"), QList<QString>() }));
    fieldReference->setReference(regFile2, FieldReference::Type::REGISTER_FILE);

    QSharedPointer<FieldReference::IndexedReference> testRegister(new FieldReference::IndexedReference({ QString("testRegister"), QList<QString>() }));
    fieldReference->setReference(testRegister, FieldReference::Type::REGISTER);

    QSharedPointer<FieldReference::IndexedReference> alternateRegister(new FieldReference::IndexedReference({ QString("testAlternateRegister"), QList<QString>() }));
    fieldReference->setReference(alternateRegister, FieldReference::Type::ALTERNATE_REGISTER);

    QSharedPointer<FieldReference::IndexedReference> field(new FieldReference::IndexedReference({ QString("testField"), QList<QString>({ "8" })} ));
    fieldReference->setReference(field, FieldReference::Type::FIELD);

    testField_->setFieldReference(fieldReference);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:aliasOf>"
                "<ipxact:addressSpaceRef addressSpaceRef=\"testAddressSpace\"/>"
                "<ipxact:memoryMapRef memoryMapRef=\"testMemoryMap\"/>"
                "<ipxact:memoryRemapRef memoryRemapRef=\"testMemoryRemap\"/>"
                "<ipxact:bankRef bankRef=\"testBank\"/>"
                "<ipxact:bankRef bankRef=\"testBank2\"/>"
                "<ipxact:addressBlockRef addressBlockRef=\"testAddressBlock\">"
                    "<ipxact:indices>"
                        "<ipxact:index>0</ipxact:index>"
                        "<ipxact:index>1</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:addressBlockRef>"
                "<ipxact:registerFileRef registerFileRef=\"testRegisterFile1\">"
                    "<ipxact:indices>"
                        "<ipxact:index>2</ipxact:index>"
                        "<ipxact:index>3</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:registerFileRef>"
                "<ipxact:registerFileRef registerFileRef=\"testRegisterFile2\"/>"
                "<ipxact:registerRef registerRef=\"testRegister\"/>"
                "<ipxact:alternateRegisterRef alternateRegisterRef=\"testAlternateRegister\"/>"
                "<ipxact:fieldRef fieldRef=\"testField\">"
                    "<ipxact:indices>"
                        "<ipxact:index>8</ipxact:index>"
                    "</ipxact:indices>"
                "</ipxact:fieldRef>"
            "</ipxact:aliasOf>"
        "</ipxact:field>"
    );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeAccess()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeAccess()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testField_->setAccess(AccessTypes::READ_WRITE);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:access>read-write</ipxact:access>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeEnumeratedValues()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeEnumeratedValues()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<EnumeratedValue> enumeration (new EnumeratedValue("testEnumeration", "DaiGurren"));
    testField_->getEnumeratedValues()->append(enumeration);

    QString expectedOutput(
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

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeModifiedWriteValue()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeModifiedWriteValue()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testField_->setModifiedWrite(General::ZERO_TO_CLEAR);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:modifiedWriteValue>zeroToClear</ipxact:modifiedWriteValue>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();
    testField_->setModifiedWriteModify("modifier");

    expectedOutput =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:modifiedWriteValue modify=\"modifier\">zeroToClear</ipxact:modifiedWriteValue>"
        "</ipxact:field>"
        ;

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeWriteValueConstraint()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeWriteValueConstraint()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<WriteValueConstraint> writeConstraint (new WriteValueConstraint());
    writeConstraint->setType(WriteValueConstraint::WRITE_AS_READ);

    testField_->setWriteConstraint(writeConstraint);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:writeAsRead>true</ipxact:writeAsRead>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();
    writeConstraint->setType(WriteValueConstraint::USE_ENUM);

    expectedOutput =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:useEnumeratedValues>true</ipxact:useEnumeratedValues>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:field>"
        ;

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();
    writeConstraint->setMinimum("0");
    writeConstraint->setMaximum("10");

    expectedOutput =
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

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeReadAction()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeReadAction()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testField_->setReadAction(General::READ_SET);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:readAction>set</ipxact:readAction>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();

    testField_->setReadAction(General::READ_CLEAR);
    testField_->setReadActionModify("modifier");

    expectedOutput =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:readAction modify=\"modifier\">clear</ipxact:readAction>"
        "</ipxact:field>"
        ;

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeTestable()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeTestable()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testField_->setTestable(false);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:testable>false</ipxact:testable>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();

    testField_->setTestable(true);
    testField_->setTestConstraint(General::TEST_RESTORE);

    expectedOutput =
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:testable testConstraint=\"restore\">true</ipxact:testable>"
        "</ipxact:field>"
        ;

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeReserved()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeReserved()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testField_->setReserved("reserveWarning");

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:reserved>reserveWarning</ipxact:reserved>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeParameters()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Parameter> testParameter(new Parameter());
    testParameter->setValueId("testID");
    testParameter->setName("testParameter");
    testParameter->setValue("1");

    testField_->getParameters()->append(testParameter);

    QString expectedOutput(
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

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void tst_FieldWriter::writeVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testField_->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:field>"
            "<ipxact:name>testField</ipxact:name>"
            "<ipxact:bitOffset>Baldur's</ipxact:bitOffset>"
            "<ipxact:bitWidth>Gate</ipxact:bitWidth>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:field>"
        );

    FieldWriter::writeField(xmlStreamWriter, testField_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_FieldWriter)

#include "tst_FieldWriter.moc"

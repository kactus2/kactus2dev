//-----------------------------------------------------------------------------
// File: tst_FieldReferenceReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 1.8.2023
//
// Description:
// Unit test for FieldReferenceReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/FieldReference.h>
#include <IPXACTmodels/Component/FieldReferenceReader.h>

#include <QtTest>
#include <QDomDocument>

class tst_FieldReferenceReader : public QObject
{
    Q_OBJECT

public:
    tst_FieldReferenceReader();

private slots:

    void readFieldReference();
};

//-----------------------------------------------------------------------------
// Function: tst_FieldReferenceReader::tst_FieldReferenceReader()
//-----------------------------------------------------------------------------
tst_FieldReferenceReader::tst_FieldReferenceReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FieldReferenceReader::readFieldReference()
//-----------------------------------------------------------------------------
void tst_FieldReferenceReader::readFieldReference()
{
    QString documentContent(
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
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode startNode = document.firstChildElement("ipxact:aliasOf");

    auto fieldReference = FieldReferenceReader::createFieldReferenceFrom(startNode);

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

QTEST_APPLESS_MAIN(tst_FieldReferenceReader)

#include "tst_FieldReferenceReader.moc"

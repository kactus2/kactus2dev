//-----------------------------------------------------------------------------
// File: tst_FieldReferenceWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 1.8.2023
//
// Description:
// Unit test for FieldReferenceWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/FieldReference.h>
#include <IPXACTmodels/Component/FieldReferenceWriter.h>

#include <QtTest>
#include <QXmlStreamWriter>

class tst_FieldReferenceWriter : public QObject
{
    Q_OBJECT

public:
    tst_FieldReferenceWriter();

private slots:

    void writeFieldReference();
};

//-----------------------------------------------------------------------------
// Function: tst_FieldReferenceWriter::tst_FieldReferenceWriter()
//-----------------------------------------------------------------------------
tst_FieldReferenceWriter::tst_FieldReferenceWriter()
{
    
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReferenceWriter::writeFieldReference()
//-----------------------------------------------------------------------------
void tst_FieldReferenceWriter::writeFieldReference()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<FieldReference> fieldReference(new FieldReference());
    
    QSharedPointer<FieldReference::IndexedReference> addressSpace(new FieldReference::IndexedReference(QString("testAddressSpace"), QList<QString>()));
    fieldReference->setReference(addressSpace, FieldReference::Type::ADDRESS_SPACE);

    QSharedPointer<FieldReference::IndexedReference> memoryMap(new FieldReference::IndexedReference(QString("testMemoryMap"), QList<QString>()));
    fieldReference->setReference(memoryMap, FieldReference::Type::MEMORY_MAP);

    QSharedPointer<FieldReference::IndexedReference> memoryRemap(new FieldReference::IndexedReference(QString("testMemoryRemap"), QList<QString>()));
    fieldReference->setReference(memoryRemap, FieldReference::Type::MEMORY_REMAP);

    QSharedPointer<FieldReference::IndexedReference> bank1(new FieldReference::IndexedReference(QString("testBank"), QList<QString>()));
    fieldReference->setReference(bank1, FieldReference::Type::BANK);

    QSharedPointer<FieldReference::IndexedReference> bank2(new FieldReference::IndexedReference(QString("testBank2"), QList<QString>()));
    fieldReference->setReference(bank2, FieldReference::Type::BANK);

    QSharedPointer<FieldReference::IndexedReference> addressBlock(new FieldReference::IndexedReference(QString("testAddressBlock"), QList<QString>({ "0", "1" })));
    fieldReference->setReference(addressBlock, FieldReference::Type::ADDRESS_BLOCK);

    QSharedPointer<FieldReference::IndexedReference> regFile1(new FieldReference::IndexedReference(QString("testRegisterFile1"), QList<QString>({ "2", "3" })));
    fieldReference->setReference(regFile1, FieldReference::Type::REGISTER_FILE);

    QSharedPointer<FieldReference::IndexedReference> regFile2(new FieldReference::IndexedReference(QString("testRegisterFile2"), QList<QString>()));
    fieldReference->setReference(regFile2, FieldReference::Type::REGISTER_FILE);

    QSharedPointer<FieldReference::IndexedReference> testRegister(new FieldReference::IndexedReference(QString("testRegister"), QList<QString>()));
    fieldReference->setReference(testRegister, FieldReference::Type::REGISTER);

    QSharedPointer<FieldReference::IndexedReference> alternateRegister(new FieldReference::IndexedReference(QString("testAlternateRegister"), QList<QString>()));
    fieldReference->setReference(alternateRegister, FieldReference::Type::ALTERNATE_REGISTER);

    QSharedPointer<FieldReference::IndexedReference> field(new FieldReference::IndexedReference(QString("testField"), QList<QString>({ "8" })));
    fieldReference->setReference(field, FieldReference::Type::FIELD);

    QString expectedOutput(
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
    );

    FieldReferenceWriter::writeFieldReference(xmlStreamWriter, fieldReference);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_FieldReferenceWriter)

#include "tst_FieldReferenceWriter.moc"

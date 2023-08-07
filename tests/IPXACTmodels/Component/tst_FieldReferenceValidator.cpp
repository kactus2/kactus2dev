//-----------------------------------------------------------------------------
// File: tst_FieldReferenceValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 3.8.2023
//
// Description:
// Unit test for FieldReferenceValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/FieldReference.h>
#include <IPXACTmodels/Component/FieldReferenceValidator.h>

#include <QtTest>

class tst_FieldReferenceValidator : public QObject
{
    Q_OBJECT

public:
    tst_FieldReferenceValidator();

private slots:

    void testValidateFieldReference();
};

//-----------------------------------------------------------------------------
// Function: tst_FieldReferenceValidator::tst_FieldReferenceValidator()
//-----------------------------------------------------------------------------
tst_FieldReferenceValidator::tst_FieldReferenceValidator()
{
    
}

//-----------------------------------------------------------------------------
// Function: tst_FieldReferenceValidator::testValidateFieldReference()
//-----------------------------------------------------------------------------
void tst_FieldReferenceValidator::testValidateFieldReference()
{
    QSharedPointer<FieldReference> testReference(new FieldReference());

    QSharedPointer<FieldReference::IndexedReference> addressSpaceRef(
        new FieldReference::IndexedReference(QString("testAddressSpace"), QList<QString>()));

    QSharedPointer<FieldReference::IndexedReference> memoryMapRef(
        new FieldReference::IndexedReference(QString("testMemoryMap"), QList<QString>()));

    testReference->setReference(addressSpaceRef, FieldReference::ADDRESS_SPACE);
    testReference->setReference(memoryMapRef, FieldReference::MEMORY_MAP);
    
    QStringList errors;
    FieldReferenceValidator::findErrorsIn(errors, testReference, "");
    
    QCOMPARE(errors.size(), 2); // address space and memory map, no field ref
    QVERIFY(FieldReferenceValidator::validate(testReference) == false);

    // Remove memory map ref.
    testReference->clearReference(FieldReference::MEMORY_MAP);
    
    QSharedPointer<FieldReference::IndexedReference> memoryRemapRef(
        new FieldReference::IndexedReference(QString("testMemoryRemap"), QList<QString>()));
    testReference->setReference(memoryRemapRef, FieldReference::MEMORY_REMAP);

    errors.clear();
    FieldReferenceValidator::findErrorsIn(errors, testReference, "");
    QCOMPARE(errors.size(), 2); // address space and memory remap, no field ref
    QVERIFY(FieldReferenceValidator::validate(testReference) == false);

    // Remove memory remap ref.
    testReference->clearReference(FieldReference::MEMORY_REMAP);
    
    errors.clear();
    FieldReferenceValidator::findErrorsIn(errors, testReference, "");
    QCOMPARE(errors.size(), 1); //  no field ref
    QVERIFY(FieldReferenceValidator::validate(testReference) == false);


    QSharedPointer<FieldReference::IndexedReference> fieldRef(
        new FieldReference::IndexedReference(QString("testField"), QList<QString>()));
    testReference->setReference(fieldRef, FieldReference::FIELD);

    errors.clear();
    FieldReferenceValidator::findErrorsIn(errors, testReference, "");
    QCOMPARE(errors.size(), 0);
    QVERIFY(FieldReferenceValidator::validate(testReference));
}

QTEST_APPLESS_MAIN(tst_FieldReferenceValidator)

#include "tst_FieldReferenceValidator.moc"

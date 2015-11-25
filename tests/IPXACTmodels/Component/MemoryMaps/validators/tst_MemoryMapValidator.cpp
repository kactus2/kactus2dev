//-----------------------------------------------------------------------------
// File: tst_MemoryMapValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.11.2015
//
// Description:
// Unit test for class MemoryMapValidator.
//-----------------------------------------------------------------------------

#include <QtTest>

class tst_MemoryMapValidator : public QObject
{
    Q_OBJECT

public:
    tst_MemoryMapValidator();

private slots:
    void testNameIsValid();
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::tst_MemoryMapValidator()
//-----------------------------------------------------------------------------
tst_MemoryMapValidator::tst_MemoryMapValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryMapValidator::testNameIsValid()
//-----------------------------------------------------------------------------
void tst_MemoryMapValidator::testNameIsValid()
{
    QVERIFY2(false, "Failing here on purpose.");
}

QTEST_APPLESS_MAIN(tst_MemoryMapValidator)

#include "tst_MemoryMapValidator.moc"

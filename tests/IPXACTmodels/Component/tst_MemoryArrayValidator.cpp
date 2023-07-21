//-----------------------------------------------------------------------------
// File: tst_MemoryArrayValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 21.07.2023
//
// Description:
// Unit test for class MemoryArrayValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/MemoryArrayValidator.h>
#include <IPXACTmodels/Component/MemoryArray.h>

#include <QXmlStreamWriter>

#include <QtTest>

class tst_MemoryArrayValidator : public QObject
{
    Q_OBJECT

public:
    tst_MemoryArrayValidator();

private slots:

    void testArrayHasDimension();
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayValidator::tst_MemoryArrayValidator()
//-----------------------------------------------------------------------------
tst_MemoryArrayValidator::tst_MemoryArrayValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayValidator::testArrayHasDimension()
//-----------------------------------------------------------------------------
void tst_MemoryArrayValidator::testArrayHasDimension()
{
    QSharedPointer<MemoryArray> memoryArray(new MemoryArray());
       
    QList<QString> errorList;
    MemoryArrayValidator::findErrorsInMemoryArray(errorList, memoryArray, "test");

    QCOMPARE(errorList.size(), 1);
    QVERIFY(!MemoryArrayValidator::validateMemoryArray(memoryArray));

    errorList.clear();

    memoryArray->setStride("16");
    
    MemoryArrayValidator::findErrorsInMemoryArray(errorList, memoryArray, "test");

    QCOMPARE(errorList.size(), 1);
    QVERIFY(!MemoryArrayValidator::validateMemoryArray(memoryArray));

    errorList.clear();

    QSharedPointer<MemoryArray::Dimension> dim(new MemoryArray::Dimension);
    dim->dimension_ = QString("8");

    memoryArray->getDimensions()->append(dim);

    MemoryArrayValidator::findErrorsInMemoryArray(errorList, memoryArray, "test");

    QCOMPARE(errorList.size(), 0);
    QVERIFY(MemoryArrayValidator::validateMemoryArray(memoryArray));
}

QTEST_APPLESS_MAIN(tst_MemoryArrayValidator)

#include "tst_MemoryArrayValidator.moc"

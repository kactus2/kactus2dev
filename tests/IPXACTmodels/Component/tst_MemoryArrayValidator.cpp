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

#include <IPXACTmodels/Component/validators/MemoryArrayValidator.h>
#include <IPXACTmodels/Component/MemoryArray.h>

#include <QXmlStreamWriter>

#include <QtTest>

class tst_MemoryArrayValidator : public QObject
{
    Q_OBJECT

public:
    tst_MemoryArrayValidator();

private slots:

    void init();
    void cleanup();

    void testArrayHasDimension();
    void testDimensionAndStrideValue();

private:
    QSharedPointer<MemoryArray> memArray_;
    QList<QString> errorList_;

    QSharedPointer<SystemVerilogExpressionParser> expressionParser_;
    QSharedPointer<MemoryArrayValidator> validator_;
};

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayValidator::tst_MemoryArrayValidator()
//-----------------------------------------------------------------------------
tst_MemoryArrayValidator::tst_MemoryArrayValidator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayValidator::init()
//-----------------------------------------------------------------------------
void tst_MemoryArrayValidator::init()
{
    memArray_ = QSharedPointer<MemoryArray>(new MemoryArray());
    expressionParser_ = QSharedPointer<SystemVerilogExpressionParser>(new SystemVerilogExpressionParser());
    validator_ = QSharedPointer<MemoryArrayValidator>(new MemoryArrayValidator(expressionParser_));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayValidator::cleanup()
//-----------------------------------------------------------------------------
void tst_MemoryArrayValidator::cleanup()
{
    memArray_.clear();
    errorList_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayValidator::testArrayHasDimension()
//-----------------------------------------------------------------------------
void tst_MemoryArrayValidator::testArrayHasDimension()
{
    validator_->findErrorsIn(errorList_, memArray_, "test");

    QCOMPARE(errorList_.size(), 1);
    QVERIFY(!validator_->validate(memArray_));

    errorList_.clear();

    QSharedPointer<MemoryArray::Dimension> dim(new MemoryArray::Dimension);
    dim->value_ = QString("8");

    memArray_->getDimensions()->append(dim);

    validator_->findErrorsIn(errorList_, memArray_, "test");

    QCOMPARE(errorList_.size(), 0);
    QVERIFY(validator_->validate(memArray_));
}

//-----------------------------------------------------------------------------
// Function: tst_MemoryArrayValidator::testDimensionAndStrideValue()
//-----------------------------------------------------------------------------
void tst_MemoryArrayValidator::testDimensionAndStrideValue()
{
    QSharedPointer<MemoryArray::Dimension> dim(new MemoryArray::Dimension);
    dim->value_ = QString("invalidValue");

    memArray_->getDimensions()->append(dim);
    memArray_->setStride("invalid");

    validator_->findErrorsIn(errorList_, memArray_, "test");

    QCOMPARE(errorList_.size(), 2);
    QVERIFY(!validator_->validate(memArray_));

    errorList_.clear();
    dim->value_ = QString("16");
    memArray_->setStride("8");

    QCOMPARE(errorList_.size(), 0);
    QVERIFY(validator_->validate(memArray_));
}

QTEST_APPLESS_MAIN(tst_MemoryArrayValidator)

#include "tst_MemoryArrayValidator.moc"

#include "ComparatorTest.h"

#include <QtTest>

//-----------------------------------------------------------------------------
// Function: ComparatorTest::ComparatorTest()
//-----------------------------------------------------------------------------
ComparatorTest::ComparatorTest()
{

}

//-----------------------------------------------------------------------------
// Function: ComparatorTest::~ComparatorTest()
//-----------------------------------------------------------------------------
ComparatorTest::~ComparatorTest()
{

}

//-----------------------------------------------------------------------------
// Function: ComparatorTest::verifyDiffResultIs()
//-----------------------------------------------------------------------------
void ComparatorTest::verifyDiffElementAndType(QSharedPointer<IPXactDiff> diff, QString const& expectedElement, 
    QString const& expectedName, IPXactDiff::DiffType expectedType) const
{
    QCOMPARE(diff->element(), expectedElement);
    QCOMPARE(diff->name(), expectedName);
    QVERIFY2(diff->changeType() == expectedType, "Diff returned wrong type.");
}

//-----------------------------------------------------------------------------
// Function: ComparatorTest::verifyModificationIs()
//-----------------------------------------------------------------------------
void ComparatorTest::verifyModificationIs(IPXactDiff::Modification const& modification, 
    QString const& element, QString const& previousValue, QString const& newValue) const
{
    QCOMPARE(modification.modifiedElement, element);
    QCOMPARE(modification.previousValue, previousValue);
    QCOMPARE(modification.newValue, newValue);
}
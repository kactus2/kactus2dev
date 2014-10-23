//-----------------------------------------------------------------------------
// File: ComparatorTest.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.10.2014
//
// Description:
// Base class for all comparator unit tests.
//-----------------------------------------------------------------------------

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
// Function: ComparatorTest::createTestInputsForStringValues()
//-----------------------------------------------------------------------------
void ComparatorTest::createTestInputsForStringValues()
{
    QTest::addColumn<QString>("referenceValue");
    QTest::addColumn<QString>("subjectValue");
    QTest::addColumn<bool>("expectedComparison");

    QTest::newRow("Same values") <<"0" << "0" << true;
    QTest::newRow("Different values") << "0" << "1" << false;
}

//-----------------------------------------------------------------------------
// Function: ComparatorTest::createTestInputForStringListValues()
//-----------------------------------------------------------------------------
void ComparatorTest::createTestInputForStringListValues()
{
    QTest::addColumn<QStringList>("referenceList");
    QTest::addColumn<QStringList>("subjectList");
    QTest::addColumn<bool>("expectedComparison");

    QTest::newRow("Identical lists") << QStringList("0") << QStringList("0") << true;
    QTest::newRow("Different lists") << QStringList("0") << QStringList("1") << false;

    QStringList shortList;
    shortList << "0" << "1";

    QStringList longList;
    longList << "0" << "1" << "2" << "3" << "4" << "5";

    QTest::newRow("Different size lists") << shortList << longList << false;

    QStringList inOrder;
    inOrder << "0" << "1" << "2" << "3";

    QStringList outOfOrder;
    outOfOrder << "1" << "3" << "0" << "2";

    QTest::newRow("Lists in different order") << inOrder << outOfOrder << true;
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
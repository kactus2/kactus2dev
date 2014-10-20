//-----------------------------------------------------------------------------
// File: tst_VLNVComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Unit test for class VLNVComparator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/vlnv.h>

#include <wizards/common/VLNVComparator/VLNVComparator.h>
#include <wizards/common/IPXactDiff.h>

#include <tests/wizards/common/ComparatorTest.h>

class tst_VLNVComparator : public QObject, public ComparatorTest
{
    Q_OBJECT

public:
    tst_VLNVComparator();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testComparingTwoNullsAreEquivalent();
    void testComparingToNullIsNotEquivalent();
    void testComparingWithSelfIsEquivalent();
    void testComparingDifferentVLNVsIsNotEqual();

    void testDiffVLNVChanges();
    
    void testDiffVLNVChanges_data();
    void testDiffVLNVMultipleChanges();
    
    void testDiffForAddingVLVNisAdd();
    void testDiffForRemovingVLVNisRemove();
    void testDiffForSelfIsNoChange();

private:
    
    VLNVComparator* makeComparator();

    VLNVComparator* comparator_;
};

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::tst_VLNVComparator()
//-----------------------------------------------------------------------------
tst_VLNVComparator::tst_VLNVComparator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::initTestCase()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::initTestCase()
{
    comparator_ = makeComparator();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::cleanupTestCase()
{
    delete comparator_;
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::makeComparator()
//-----------------------------------------------------------------------------
VLNVComparator* tst_VLNVComparator::makeComparator()
{
    return new VLNVComparator;
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::testComparingTwoNullsAreEquivalent()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::testComparingTwoNullsAreEquivalent()
{
    QVERIFY2(comparator_->compare(QSharedPointer<VLNV>(0), QSharedPointer<VLNV>(0)), "Two nulls should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::testComparingNullToComponentIsNotEquivalent()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::testComparingToNullIsNotEquivalent()
{
    QSharedPointer<VLNV> vlnv(new VLNV());

    QVERIFY2(!comparator_->compare(QSharedPointer<VLNV>(0), vlnv), "Comparison to null should not be equal.");
    QVERIFY2(!comparator_->compare(vlnv, QSharedPointer<VLNV>(0)), "Comparison to null should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::testComparingWithSelfIsEquivalent()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::testComparingWithSelfIsEquivalent()
{
    QSharedPointer<VLNV> vlnv(new VLNV());
    QVERIFY2(comparator_->compare(vlnv, vlnv), "Comparing with self should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::testComparingWithDifferentVLNVIsNotEqual()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::testComparingDifferentVLNVsIsNotEqual()
{
    QSharedPointer<VLNV> reference(new VLNV(VLNV::COMPONENT, "TUT:Tests:TestComponent:1.0"));   
    QSharedPointer<VLNV> vlnv(new VLNV(VLNV::COMPONENT, "Vendor:Library:Name:Version"));   

    QVERIFY2(!comparator_->compare(reference, vlnv), "Different VLVNs should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::testDiffVLNVChanges()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::testDiffVLNVChanges()
{
    QFETCH(QString, vlnv);
    QFETCH(QString, modifiedElement);
    QFETCH(QString, previousValue);

    QSharedPointer<VLNV> reference(new VLNV(VLNV::COMPONENT, "TUT:Tests:TestComponent:1.0"));   
    QSharedPointer<VLNV> other(new VLNV(VLNV::COMPONENT, vlnv));   

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, other);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "vlnv", "TUT:Tests:TestComponent:1.0", IPXactDiff::MODIFICATION);

    QList<IPXactDiff::Modification> modifications = diff.first()->getChangeList();
    QCOMPARE(modifications.count(), 1);
    verifyModificationIs(modifications.first(), modifiedElement, previousValue, "X");
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::testComparingWithDifferentVLNVIsNotEqual_data()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::testDiffVLNVChanges_data()
{
    QTest::addColumn<QString>("vlnv");    
    QTest::addColumn<QString>("modifiedElement");    
    QTest::addColumn<QString>("previousValue");   

    QTest::newRow("Different vendor") << "X:Tests:TestComponent:1.0" << "vendor" << "TUT";
    QTest::newRow("Different library") << "TUT:X:TestComponent:1.0" << "library" << "Tests";
    QTest::newRow("Different name") << "TUT:Tests:X:1.0" << "name" << "TestComponent";
    QTest::newRow("Different version") << "TUT:Tests:TestComponent:X" << "version" << "1.0";
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::testDiffVLNVMultipleChanges()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::testDiffVLNVMultipleChanges()
{
    QSharedPointer<VLNV> reference(new VLNV(VLNV::COMPONENT, "TUT:Tests:TestComponent:1.0"));   
    QSharedPointer<VLNV> other(new VLNV(VLNV::COMPONENT, "X:Y:Z:1"));   

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, other);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "vlnv", "TUT:Tests:TestComponent:1.0", IPXactDiff::MODIFICATION);

    QList<IPXactDiff::Modification> modifications = diff.first()->getChangeList();
    QCOMPARE(modifications.count(), 4);

    IPXactDiff::Modification vendorChange = modifications.at(0);
    verifyModificationIs(vendorChange, "vendor", "TUT", "X");

    IPXactDiff::Modification libraryChange = modifications.at(1);
    verifyModificationIs(libraryChange, "library", "Tests", "Y");

    IPXactDiff::Modification nameChange = modifications.at(2);
    verifyModificationIs(nameChange, "name", "TestComponent", "Z");

    IPXactDiff::Modification versionChange = modifications.at(3);
    verifyModificationIs(versionChange, "version", "1.0", "1");
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::testDiffForAddingVLVNisAdd()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::testDiffForAddingVLVNisAdd()
{
    QSharedPointer<VLNV> reference(new VLNV());
    QSharedPointer<VLNV> component(new VLNV(VLNV::COMPONENT, "TUT:Tests:TestComponent:1.0"));   

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, component);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "vlnv", ":::", IPXactDiff::ADD);
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::testDiffForRemovingVLVNisRemove()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::testDiffForRemovingVLVNisRemove()
{
    QSharedPointer<VLNV> reference(new VLNV(VLNV::COMPONENT, "TUT:Tests:TestComponent:1.0"));   
    QSharedPointer<VLNV> component(new VLNV());   

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, component);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "vlnv", "TUT:Tests:TestComponent:1.0", IPXactDiff::REMOVE);
}

//-----------------------------------------------------------------------------
// Function: tst_VLNVComparator::testDiffForSelfIsNoChange()
//-----------------------------------------------------------------------------
void tst_VLNVComparator::testDiffForSelfIsNoChange()
{
    QSharedPointer<VLNV> reference(new VLNV(VLNV::COMPONENT, "TUT:Tests:TestComponent:1.0"));   

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, reference);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "vlnv", "TUT:Tests:TestComponent:1.0", IPXactDiff::NO_CHANGE);
}

QTEST_APPLESS_MAIN(tst_VLNVComparator)

#include "tst_VLNVComparator.moc"
    
//-----------------------------------------------------------------------------
// File: tst_ViewComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.10.2014
//
// Description:
// Unit test for class ViewComparator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QSharedPointer>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/vlnv.h>

#include <wizards/common/ViewComparator/ViewComparator.h>
#include <wizards/common/IPXactDiff.h>

#include <tests/wizards/common/ComparatorTest.h>


class tst_ViewComparator : public QObject, public ComparatorTest
{
    Q_OBJECT

public:
    tst_ViewComparator();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testViewsNamesComparison();
    void testViewsNamesComparison_data();
    void testViewsWithDifferentHierarchyRefsIsNotEqual();
    void testHierarchicalAndNonHierarchicalViewIsNotEqual();
    void testViewsEnvIdenrtifierComparison();
    void testViewsEnvIdenrtifierComparison_data();
    void testViewsLanguageComparison();
    void testViewsLanguageComparison_data();
    void testViewsModelNameComparison();
    void testViewsModelNameComparison_data();
    void testViewsFileSetRefComparison();
    void testViewsFileSetRefComparison_data();
    void testDiffViewChangeToNonHierarchical();
    void testDiffViewChangeToHierarchical();
    void testDiffViewChangeHierarchyRef();
    void testDiffLanguage();
    void testDiffLanguage_data();
    void testDiffModelName();
    void testDiffModelName_data();
    void testDiffEnvIdentifiers();
    void testDiffFileSetRef();

private:
    
    ViewComparator* makeComparator();

   
    ViewComparator* comparator_;
};

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::tst_ViewComparator()
//-----------------------------------------------------------------------------
tst_ViewComparator::tst_ViewComparator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::initTestCase()
//-----------------------------------------------------------------------------
void tst_ViewComparator::initTestCase()
{
    comparator_ = makeComparator();
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_ViewComparator::cleanupTestCase()
{
    delete comparator_;
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::makeComparator()
//-----------------------------------------------------------------------------
ViewComparator* tst_ViewComparator::makeComparator()
{
    return new ViewComparator;
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsNamesComparison()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsNamesComparison()
{    
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<View> reference(new View(referenceValue));    

    QSharedPointer<View> subject(new View(subjectValue));    

    QVERIFY(comparator_->compare(reference, subject) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testSimilarViewsIsEqual_data()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsNamesComparison_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsWithDifferentHierarchyRefsIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsWithDifferentHierarchyRefsIsNotEqual()
{
    QSharedPointer<View> reference(new View());     
    reference->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT:TestLibrary:Design:1.0"));

    QSharedPointer<View> subject(new View());
    subject->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "X:X:X:X"));

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Views with views with different hierarchy refs order should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testHierarchicalAndNonHierarchicalViewIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testHierarchicalAndNonHierarchicalViewIsNotEqual()
{
    QSharedPointer<View> reference(new View());    
    reference->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT:TestLibrary:Design:1.0"));

    QSharedPointer<View> subject(new View());

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Views with hierarchical and non-hierarchical views should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsEnvIdenrtifierComparison(()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsEnvIdenrtifierComparison()
{
    QFETCH(QStringList, referenceList);
    QFETCH(QStringList, subjectList);
    QFETCH(bool, expectedComparison);

    QSharedPointer<View> reference(new View());    
    reference->setEnvIdentifiers(referenceList);

    QSharedPointer<View> subject(new View());
    subject->setEnvIdentifiers(subjectList);

    QVERIFY(comparator_->compare(reference, subject) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsEnvIdenrtifierComparison_data()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsEnvIdenrtifierComparison_data()
{
    createTestInputForStringListValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsLanguageComparison()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsLanguageComparison()
{
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<View> reference(new View(""));    
    reference->setLanguage(referenceValue);

    QSharedPointer<View> subject(new View(""));
    subject->setLanguage(subjectValue);

    QVERIFY(comparator_->compare(reference, subject) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsLanguageComparison_data()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsLanguageComparison_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsModelNameComparison()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsModelNameComparison()
{
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<View> reference(new View());    
    reference->setModelName(referenceValue);

    QSharedPointer<View> subject(new View());
    subject->setModelName(subjectValue);

    QVERIFY(comparator_->compare(reference, subject) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsModelNameComparison_data()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsModelNameComparison_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsFileSetRefComparison()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsFileSetRefComparison()
{    
    QFETCH(QStringList, referenceList);
    QFETCH(QStringList, subjectList);
    QFETCH(bool, expectedComparison);

    QSharedPointer<View> reference(new View());    
    reference->setFileSetRefs(referenceList);

    QSharedPointer<View> subject(new View());
    subject->setFileSetRefs(subjectList);

    QVERIFY(comparator_->compare(reference, subject) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsFileSetRefComparison_data()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsFileSetRefComparison_data()
{
    createTestInputForStringListValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testDiffViewChangeToNonHierarchical()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testDiffViewChangeToNonHierarchical()
{
    QSharedPointer<View> reference(new View());    
    reference->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT:TestLibrary:Design:1.0"));

    QSharedPointer<View> subject(new View());

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    verifyDiffElementAndType(diff.first(), "view", "", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "hierarchy", "hierarchical", "non-hierarchical");
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testDiffViewChangeToHierarchical()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testDiffViewChangeToHierarchical()
{
    QSharedPointer<View> reference(new View());    

    QSharedPointer<View> subject(new View());
    subject->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT:TestLibrary:Design:1.0"));

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    verifyDiffElementAndType(diff.first(), "view", "", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "hierarchy", "non-hierarchical", "hierarchical");
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testDiffViewChangeToHierarchical()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testDiffViewChangeHierarchyRef()
{
    QSharedPointer<View> reference(new View("testView"));    
    reference->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT:TestLibrary:Design:1.0"));

    QSharedPointer<View> subject(new View("testView"));
    subject->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "X:X:X:X"));

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    verifyDiffElementAndType(diff.first(), "view", "testView", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "hierarchy ref", "TUT:TestLibrary:Design:1.0", 
        "X:X:X:X");
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testDiffLanguage()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testDiffLanguage()
{
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<View> reference(new View("testView"));    
    reference->setLanguage(referenceValue);

    QSharedPointer<View> subject(new View("testView"));        
    subject->setLanguage(subjectValue);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    if (expectedComparison)
    {
        verifyDiffElementAndType(diff.first(), "view", "", IPXactDiff::NO_CHANGE);   
    }
    else
    {
        verifyDiffElementAndType(diff.first(), "view", "testView", IPXactDiff::MODIFICATION);
        verifyModificationIs(diff.first()->getChangeList().first(), "language", referenceValue, subjectValue);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testDiffLanguage_data()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testDiffLanguage_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testDiffModelName()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testDiffModelName()
{
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<View> reference(new View("testView"));    
    reference->setModelName(referenceValue);

    QSharedPointer<View> subject(new View("testView"));        
    subject->setModelName(subjectValue);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    if (expectedComparison)
    {
        verifyDiffElementAndType(diff.first(), "view", "", IPXactDiff::NO_CHANGE);   
    }
    else
    {
        verifyDiffElementAndType(diff.first(), "view", "testView", IPXactDiff::MODIFICATION);
        verifyModificationIs(diff.first()->getChangeList().first(), "model name", referenceValue, subjectValue);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testDiffModelName_data()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testDiffModelName_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testDiffEnvIdentifiers()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testDiffEnvIdentifiers()
{
    QSharedPointer<View> reference(new View("view1"));    
    QStringList referenceList;
    referenceList << "0:0:0" << "1:1:1";
    reference->setEnvIdentifiers(referenceList);

    QSharedPointer<View> subject(new View("view1"));
    QStringList subjectList;
    subjectList << "2:2:2" << "3:3:3";
    subject->setEnvIdentifiers(subjectList);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "view", "view1", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "environment identifiers", "0:0:0, 1:1:1", "2:2:2, 3:3:3");
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testDiffFileSetRef()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testDiffFileSetRef()
{
    QSharedPointer<View> reference(new View("view1"));    
    QStringList referenceList;
    referenceList << "src" << "docs";
    reference->setFileSetRefs(referenceList);

    QSharedPointer<View> subject(new View("view1"));        
    QStringList subjectList;
    subjectList << "src" << "headers";
    subject->setFileSetRefs(subjectList);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "view", "view1", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "file set ref", "src, docs", "src, headers");
}

QTEST_APPLESS_MAIN(tst_ViewComparator)

#include "tst_ViewComparator.moc"
    
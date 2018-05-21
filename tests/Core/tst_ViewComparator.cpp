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

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/Model.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/VLNV.h>

#include <wizards/common/ViewComparator/ViewComparator.h>
#include <wizards/common/IPXactDiff.h>

#include "ComparatorTest.h"


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
    void testViewsEnvIdenrtifierComparison();
    void testViewsEnvIdenrtifierComparison_data();

    void testDiffEnvIdentifiers();

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
// Function: tst_ViewComparator::testViewsEnvIdenrtifierComparison(()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsEnvIdenrtifierComparison()
{
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<View> reference(new View());    
    QSharedPointer<View::EnvironmentIdentifier> referenceIdentifier(
        new View::EnvironmentIdentifier(referenceValue));
    reference->addEnvIdentifier(referenceIdentifier);

    QSharedPointer<View> subject(new View());
    QSharedPointer<View::EnvironmentIdentifier> subjectIdentifier(new View::EnvironmentIdentifier(subjectValue));
    subject->addEnvIdentifier(subjectIdentifier);

    QVERIFY(comparator_->compare(reference, subject) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testViewsEnvIdenrtifierComparison_data()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testViewsEnvIdenrtifierComparison_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ViewComparator::testDiffEnvIdentifiers()
//-----------------------------------------------------------------------------
void tst_ViewComparator::testDiffEnvIdentifiers()
{
    QSharedPointer<View> reference(new View("view1"));    

    QSharedPointer<View::EnvironmentIdentifier> firstReferenceIdentifier(new View::EnvironmentIdentifier(
        QStringLiteral("0"), QStringLiteral("0"), QStringLiteral("0")));
    reference->addEnvIdentifier(firstReferenceIdentifier);

    QSharedPointer<View::EnvironmentIdentifier> secondReferenceIdentifier(new View::EnvironmentIdentifier(
        QStringLiteral("1"), QStringLiteral("1"), QStringLiteral("1")));
    reference->addEnvIdentifier(secondReferenceIdentifier);

    QSharedPointer<View> subject(new View("view1"));

    QSharedPointer<View::EnvironmentIdentifier> firstSubjectIdentifier(new View::EnvironmentIdentifier(
        QStringLiteral("2"), QStringLiteral("2"), QStringLiteral("2")));
    subject->addEnvIdentifier(firstSubjectIdentifier);

    QSharedPointer<View::EnvironmentIdentifier> secondSubjectIdentifier(new View::EnvironmentIdentifier(
        QStringLiteral("3"), QStringLiteral("3"), QStringLiteral("3")));
    subject->addEnvIdentifier(secondSubjectIdentifier);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "view", "view1", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "environment identifiers", "0:0:0, 1:1:1", "2:2:2, 3:3:3");
}


QTEST_APPLESS_MAIN(tst_ViewComparator)

#include "tst_ViewComparator.moc"
    
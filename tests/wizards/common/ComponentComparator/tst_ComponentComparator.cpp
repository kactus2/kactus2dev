//-----------------------------------------------------------------------------
// File: tst_ComponentComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.10.2014
//
// Description:
// Unit test for class ComponentComparator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QSharedPointer>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/vlnv.h>

#include <wizards/common/ComponentComparator/ComponentComparator.h>
#include <wizards/common/IPXactDiff.h>

#include <tests/wizards/common/ComparatorTest.h>

class tst_ComponentComparator : public QObject, public ComparatorTest
{
    Q_OBJECT

public:
    tst_ComponentComparator();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testComparingTwoNullsAreEquivalent();
    void testComparingNullToEmptyElementIsNotEquivalent();
    void testComparingWithSelfIsEquivalent();

    void testComparingWithDifferentVLNVIsNotEqual();
    void testComparingWithDifferentVLNVIsNotEqual_data();

    void testModelParameterInOneComponentIsNotEqual();
    void testSimilarModelParametersIsEqual();
    void testDifferentModelParametersIsNotEqual();
    void testDifferentModelParameterValuesIsNotEqual();
    void testMultipleModelParametersWithOneDifferenceIsNotEqual();

    void testViewInOneComponentIsNotEqual();
    void testDifferentViewsIsNotEqual();

    void testViewsNamesComparison();
    void testViewsNamesComparison_data();
    
    void testDifferentNumberOfViewsIsNotEqual();
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

    void testDiffOfTwoNullsIsNoChange();

    void testDiffToSameComponentIsNoChange();
    void testDiffToNullReferenceIsAdd();
    void testDiffToNullComponentIsRemove();

    void testDiffViewInSubjectComponentIsAdd();
    void testDiffViewInReferenceComponentIsRemove();
    void testDiffToSameViewIsNoChange();

    void testDiffViewChangeToNonHierarchical();
    void testDiffViewChangeToHierarchical();
    void testDiffViewChangeHierarchyRef();

    void testDiffLanguage();
    void testDiffLanguage_data();

    void testDiffModelName();
    void testDiffModelName_data();
    
    void testDiffEnvIdentifiers();

    void testDiffFileSetRef();

    void testDiffMultipleViewsChanged();

private:

    ComponentComparator* makeComparator();

    void addModelParameter(QSharedPointer<Component> component, QString const& name, QString const& value) const;
   

    //! The comparator to test.
    ComponentComparator* comparator_;
};

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::tst_ComponentComparator()
//-----------------------------------------------------------------------------
tst_ComponentComparator::tst_ComponentComparator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::initTestCase()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::initTestCase()
{
    comparator_ = makeComparator();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::cleanupTestCase()
{
    delete comparator_;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::makeComparator()
//-----------------------------------------------------------------------------
ComponentComparator* tst_ComponentComparator::makeComparator()
{
    return new ComponentComparator;
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testComparingTwoNullsAreEquivalent()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testComparingTwoNullsAreEquivalent()
{
    QVERIFY2(comparator_->compare(QSharedPointer<Component>(0), QSharedPointer<Component>(0)),
        "Two nulls should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testComparingNullToEmptyElementIsNotEquivalent()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testComparingNullToEmptyElementIsNotEquivalent()
{
    QSharedPointer<Component> component(new Component());

    QVERIFY2(!comparator_->compare(QSharedPointer<Component>(0), component), 
        "Comparison to null should not be equal.");
    QVERIFY2(!comparator_->compare(component, QSharedPointer<Component>(0)), 
        "Comparison to null should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testComparingWithSelfIsEquivalent()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testComparingWithSelfIsEquivalent()
{
    QSharedPointer<Component> component(new Component());
    QVERIFY2(comparator_->compare(component, component), "Comparing with self should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testComparingWithDifferentVLNVIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testComparingWithDifferentVLNVIsNotEqual()
{
    QFETCH(QString, vlnv);    

    QSharedPointer<Component> reference(new Component(VLNV(VLNV::COMPONENT, "TUT:Tests:TestComponent:1.0")));   
    QSharedPointer<Component> component(new Component(VLNV(VLNV::COMPONENT, vlnv)));   

    QVERIFY2(!comparator_->compare(reference, component), "Components with different VLVN should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testComparingWithDifferentVLNVIsNotEqual_data()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testComparingWithDifferentVLNVIsNotEqual_data()
{
    QTest::addColumn<QString>("vlnv");    

    QTest::newRow("Empty VLNV")  << ":::";
    QTest::newRow("Different vendor") << "X:Tests:TestComponent:1.0";
    QTest::newRow("Different library") << "TUT:X:TestComponent:1.0";
    QTest::newRow("Different name") << "TUT:Tests:X:1.0";
    QTest::newRow("Different version") << "TUT:Tests:TestComponent:X";
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testModelParameterInOneComponentIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testModelParameterInOneComponentIsNotEqual()
{
    QSharedPointer<Component> reference(new Component());
    QSharedPointer<Component> component(new Component());

    QSharedPointer<ModelParameter> modelParameter(new ModelParameter());
    component->getModel()->addModelParameter(modelParameter);

    QVERIFY2(!comparator_->compare(reference, component), 
        "Components with different model parameters should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testSimilarModelParametersIsEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testSimilarModelParametersIsEqual()
{
    QSharedPointer<Component> reference(new Component());    
    addModelParameter(reference, "id", "1");

    QSharedPointer<Component> component(new Component());
    addModelParameter(component, "id", "1");

    QVERIFY2(comparator_->compare(reference, component), 
        "Components with same model parameter should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDifferentModelParametersIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDifferentModelParametersIsNotEqual()
{
    QSharedPointer<Component> reference(new Component());    
    addModelParameter(reference, "id", "1");

    QSharedPointer<Component> component(new Component());
    addModelParameter(component, "reference id", "1");

    QVERIFY2(!comparator_->compare(reference, component), 
        "Components with different model parameters should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDifferentModelParameterValuesIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDifferentModelParameterValuesIsNotEqual()
{
    QSharedPointer<Component> reference(new Component());    
    addModelParameter(reference, "id", "1");

    QSharedPointer<Component> component(new Component());
    addModelParameter(component, "id", "200");

    QVERIFY2(!comparator_->compare(reference, component), 
        "Components with different model parameter values should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testMultipleModelParametersWithOneDifferenceIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testMultipleModelParametersWithOneDifferenceIsNotEqual()
{
    QSharedPointer<Component> reference(new Component());    
    addModelParameter(reference, "addr_width", "16");
    addModelParameter(reference, "data_width", "32");
    addModelParameter(reference, "id", "1");

    QSharedPointer<Component> component(new Component());    
    addModelParameter(component, "addr_width", "16");    
    addModelParameter(component, "data_width", "32");
    addModelParameter(component, "clk_freq", "100000");    

    QVERIFY2(!comparator_->compare(reference, component), 
        "Components with different model parameters should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewInOneComponentIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewInOneComponentIsNotEqual()
{
    QSharedPointer<Component> reference(new Component());    
    reference->addView(new View());

    QSharedPointer<Component> component(new Component());    

    QVERIFY2(!comparator_->compare(reference, component), 
        "Components with view in other should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDifferentViewsIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDifferentViewsIsNotEqual()
{
    QSharedPointer<Component> reference(new Component());    
    reference->addView(new View("view1"));
    reference->addView(new View("view2"));

    QSharedPointer<Component> component(new Component());    
    component->addView(new View("view1"));
    component->addView(new View("unknown view"));

    QVERIFY2(!comparator_->compare(reference, component), 
        "Components with different views should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewsNamesComparison()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsNamesComparison()
{    
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<Component> reference(new Component());    
    reference->addView(new View(referenceValue));

    QSharedPointer<Component> component(new Component());    
    component->addView(new View(subjectValue));

    QVERIFY(comparator_->compare(reference, component) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testSimilarViewsIsEqual_data()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsNamesComparison_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDifferentNumberOfViewsIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDifferentNumberOfViewsIsNotEqual()
{
    QSharedPointer<Component> reference(new Component());    
    reference->addView(new View("0"));

    QSharedPointer<Component> component(new Component());    
    component->addView(new View("0"));
    component->addView(new View("1"));

    QVERIFY2(!comparator_->compare(reference, component), 
        "Components with views in different order should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewsWithDifferentHierarchyRefsIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsWithDifferentHierarchyRefsIsNotEqual()
{
    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("");    
    referenceView->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT:TestLibrary:Design:1.0"));
    reference->addView(referenceView);

    QSharedPointer<Component> subject(new Component());
    View* subjectView = new View("");    
    subjectView->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "X:X:X:X"));
    subject->addView(subjectView);

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Components with views with different hierarchy refs order should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testHierarchicalAndNonHierarchicalViewIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testHierarchicalAndNonHierarchicalViewIsNotEqual()
{
    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("");
    referenceView->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT:TestLibrary:Design:1.0"));
    reference->addView(referenceView);

    QSharedPointer<Component> subject(new Component());
    View* subjectView = new View("");
    subject->addView(subjectView);

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Components with hierarchical and non-hierarchical views should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewsEnvIdenrtifierComparison(()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsEnvIdenrtifierComparison()
{
    QFETCH(QStringList, referenceList);
    QFETCH(QStringList, subjectList);
    QFETCH(bool, expectedComparison);

    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("");
    referenceView->setEnvIdentifiers(referenceList);
    reference->addView(referenceView);

    QSharedPointer<Component> subject(new Component());
    View* subjectView = new View("");
    subjectView->setEnvIdentifiers(subjectList);
    subject->addView(subjectView);

    QVERIFY(comparator_->compare(reference, subject) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewsEnvIdenrtifierComparison_data()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsEnvIdenrtifierComparison_data()
{
    createTestInputForStringListValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewsLanguageComparison()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsLanguageComparison()
{
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("");
    referenceView->setLanguage(referenceValue);
    reference->addView(referenceView);

    QSharedPointer<Component> subject(new Component());
    View* subjectView = new View("");
    subjectView->setLanguage(subjectValue);
    subject->addView(subjectView);

    QVERIFY(comparator_->compare(reference, subject) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewsLanguageComparison_data()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsLanguageComparison_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewsModelNameComparison()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsModelNameComparison()
{
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("");
    referenceView->setModelName(referenceValue);
    reference->addView(referenceView);

    QSharedPointer<Component> subject(new Component());
    View* subjectView = new View("");
    subjectView->setModelName(subjectValue);
    subject->addView(subjectView);

    QVERIFY(comparator_->compare(reference, subject) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewsModelNameComparison_data()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsModelNameComparison_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewsFileSetRefComparison()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsFileSetRefComparison()
{    
    QFETCH(QStringList, referenceList);
    QFETCH(QStringList, subjectList);
    QFETCH(bool, expectedComparison);

    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("");
    referenceView->setFileSetRefs(referenceList);
    reference->addView(referenceView);

    QSharedPointer<Component> subject(new Component());
    View* subjectView = new View("");
    subjectView->setFileSetRefs(subjectList);
    subject->addView(subjectView);

    QVERIFY(comparator_->compare(reference, subject) == expectedComparison);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testViewsFileSetRefComparison_data()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testViewsFileSetRefComparison_data()
{
    createTestInputForStringListValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffOfTwoNullsIsNoChange()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffOfTwoNullsIsNoChange()
{
    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(QSharedPointer<Component>(0), 
        QSharedPointer<Component>(0));

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "component", "", IPXactDiff::NO_CHANGE);   
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffToSameComponentIsNoChange()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffToSameComponentIsNoChange()
{
    QSharedPointer<Component> component(new Component());

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(component, component);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "component", "", IPXactDiff::NO_CHANGE);   
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffToNullReferenceIsAdd()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffToNullReferenceIsAdd()
{
    QSharedPointer<Component> component(new Component());

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(QSharedPointer<Component>(0), component);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "component", "", IPXactDiff::ADD);     
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffToNullComponentIsRemove()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffToNullComponentIsRemove()
{
    QSharedPointer<Component> component(new Component());

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(component, QSharedPointer<Component>(0));

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "component", "", IPXactDiff::REMOVE);   
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffViewInSubjectComponentIsAdd()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffViewInSubjectComponentIsAdd()
{
    QSharedPointer<Component> reference(new Component());    
    
    QSharedPointer<Component> subject(new Component());    
    subject->addView(new View("testView"));

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "view", "testView", IPXactDiff::ADD);   
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffViewInReferenceComponentIsRemove()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffViewInReferenceComponentIsRemove()
{
    QSharedPointer<Component> reference(new Component());    
    reference->addView(new View("testView"));

    QSharedPointer<Component> subject(new Component());        

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "view", "testView", IPXactDiff::REMOVE);   
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffToSameViewIsNoChange()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffToSameViewIsNoChange()
{
    QSharedPointer<Component> reference(new Component());    
    reference->addView(new View());

    QSharedPointer<Component> subject(new Component());        
    subject->addView(new View());

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "component", "", IPXactDiff::NO_CHANGE);   
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffViewChangeToNonHierarchical()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffViewChangeToNonHierarchical()
{
    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("");
    referenceView->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT:TestLibrary:Design:1.0"));
    reference->addView(referenceView);

    QSharedPointer<Component> subject(new Component());
    subject->addView(new View(""));

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    verifyDiffElementAndType(diff.first(), "view", "", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "hierarchy", "hierarchical", "non-hierarchical");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffViewChangeToHierarchical()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffViewChangeToHierarchical()
{
    QSharedPointer<Component> reference(new Component());    
    reference->addView(new View(""));

    QSharedPointer<Component> subject(new Component());
    View* subjectView = new View("");    
    subjectView->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT:TestLibrary:Design:1.0"));
    subject->addView(subjectView);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    verifyDiffElementAndType(diff.first(), "view", "", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "hierarchy", "non-hierarchical", "hierarchical");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffViewChangeToHierarchical()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffViewChangeHierarchyRef()
{
    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("testView");    
    referenceView->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "TUT:TestLibrary:Design:1.0"));
    reference->addView(referenceView);

    QSharedPointer<Component> subject(new Component());
    View* subjectView = new View("testView");    
    subjectView->setHierarchyRef(VLNV(VLNV::DESIGNCONFIGURATION, "X:X:X:X"));
    subject->addView(subjectView);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    verifyDiffElementAndType(diff.first(), "view", "testView", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "hierarchy ref", "TUT:TestLibrary:Design:1.0", 
        "X:X:X:X");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffLanguage()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffLanguage()
{
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("testView");
    referenceView->setLanguage(referenceValue);
    reference->addView(referenceView);    

    QSharedPointer<Component> subject(new Component());        
    View* subjectView = new View("testView");
    subjectView->setLanguage(subjectValue);
    subject->addView(subjectView);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    if (expectedComparison)
    {
        verifyDiffElementAndType(diff.first(), "component", "", IPXactDiff::NO_CHANGE);   
    }
    else
    {
        verifyDiffElementAndType(diff.first(), "view", "testView", IPXactDiff::MODIFICATION);
        verifyModificationIs(diff.first()->getChangeList().first(), "language", referenceValue, subjectValue);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffLanguage_data()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffLanguage_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffModelName()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffModelName()
{
    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("testView");
    referenceView->setModelName(referenceValue);
    reference->addView(referenceView);    

    QSharedPointer<Component> subject(new Component());        
    View* subjectView = new View("testView");
    subjectView->setModelName(subjectValue);
    subject->addView(subjectView);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    if (expectedComparison)
    {
        verifyDiffElementAndType(diff.first(), "component", "", IPXactDiff::NO_CHANGE);   
    }
    else
    {
        verifyDiffElementAndType(diff.first(), "view", "testView", IPXactDiff::MODIFICATION);
        verifyModificationIs(diff.first()->getChangeList().first(), "model name", referenceValue, subjectValue);
    }
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffModelName_data()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffModelName_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffEnvIdentifiers()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffEnvIdentifiers()
{
    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("view1");
    QStringList referenceList;
    referenceList << "0:0:0" << "1:1:1";
    referenceView->setEnvIdentifiers(referenceList);
    reference->addView(referenceView);

    QSharedPointer<Component> subject(new Component());
    View* subjectView = new View("view1");
    QStringList subjectList;
    subjectList << "2:2:2" << "3:3:3";
    subjectView->setEnvIdentifiers(subjectList);
    subject->addView(subjectView);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "view", "view1", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "environment identifiers", "0:0:0, 1:1:1", "2:2:2, 3:3:3");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffFileSetRef()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffFileSetRef()
{
    QSharedPointer<Component> reference(new Component());    
    View* referenceView = new View("view1");
    QStringList referenceList;
    referenceList << "src" << "docs";
    referenceView->setFileSetRefs(referenceList);
    reference->addView(referenceView);    

    QSharedPointer<Component> subject(new Component());        
    View* subjectView = new View("view1");
    QStringList subjectList;
    subjectList << "src" << "headers";
    subjectView->setFileSetRefs(subjectList);
    subject->addView(subjectView);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "view", "view1", IPXactDiff::MODIFICATION);
    verifyModificationIs(diff.first()->getChangeList().first(), "file set ref", "src, docs", "src, headers");
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::testDiffMultipleViewsChanged()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::testDiffMultipleViewsChanged()
{
    QSharedPointer<Component> reference(new Component());       
    reference->addView(new View("view1"));
    reference->addView(new View("view2"));

    QSharedPointer<Component> subject(new Component());    
    subject->addView(new View("view2"));    
    subject->getViews().first()->setModelName("model");
    subject->addView(new View("view3"));    
    subject->addView(new View("view4"));    

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 4);

    verifyDiffElementAndType(diff.at(0), "view", "view1", IPXactDiff::REMOVE);
    verifyDiffElementAndType(diff.at(1), "view", "view2", IPXactDiff::MODIFICATION);
    verifyDiffElementAndType(diff.at(2), "view", "view3", IPXactDiff::ADD);
    verifyDiffElementAndType(diff.at(3), "view", "view4", IPXactDiff::ADD);
}

//-----------------------------------------------------------------------------
// Function: tst_ComponentComparator::addModelParameter()
//-----------------------------------------------------------------------------
void tst_ComponentComparator::addModelParameter(QSharedPointer<Component> component, QString const& name, 
    QString const& value) const
{
    QSharedPointer<ModelParameter> otherModelParameter(new ModelParameter());
    otherModelParameter->setName(name);
    otherModelParameter->setValue(value);
    component->getModel()->addModelParameter(otherModelParameter);
}

QTEST_APPLESS_MAIN(tst_ComponentComparator)

#include "tst_ComponentComparator.moc"
    
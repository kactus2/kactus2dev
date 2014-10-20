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

    void testDiffOfTwoNullsIsNoChange();

    void testDiffToSameComponentIsNoChange();
    void testDiffToNullReferenceIsAdd();
    void testDiffToNullComponentIsRemove();

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
    QVERIFY2(comparator_->compare(QSharedPointer<Component>(0), 
        QSharedPointer<Component>(0)), "Two nulls should be equal.");
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
    
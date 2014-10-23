//-----------------------------------------------------------------------------
// File: tst_ModelParameterComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 16.10.2014
//
// Description:
// Unit test for class ModelParameterComparator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QSharedPointer>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/vlnv.h>

#include <wizards/common/ModelParameterComparator/ModelParameterComparator.h>
#include <wizards/common/IPXactDiff.h>

#include <tests/wizards/common/ComparatorTest.h>


class tst_ModelParameterComparator : public QObject, public ComparatorTest
{
    Q_OBJECT

public:
    tst_ModelParameterComparator();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testComparingTwoNullsAreEquivalent();
    void testComparingNullToComponentIsNotEquivalent();
    void testComparingWithSelfIsEquivalent();

    void testComparingTwoEmptyListsIsEquivalent();
    void testComparingWithSameListIsEquivalent();

    void testModelParameterInOneListIsNotEqual();
    void testSimilarModelParametersIsEqual();
    void testDifferentModelParametersIsNotEqual();
    void testDifferentModelParameterValuesIsNotEqual();
    void testMultipleModelParametersWithOneDifferenceIsNotEqual();
    void testModelParametersInDifferentOrderIsEqual();

    void testDiffModelParameters();
   
    void testDiffModelParametersInDifferentOrderIsNoChange();

    void testDiffOneOfManyModelParametersChanged();
    void testDiffMultipleModelParametersChanged();
    void testDiffOfTwoEmptyListsIsNoChange();
    void testDiffToSameComponentIsNoChange();

private:
    
    ModelParameterComparator* makeComparator();

    void addModelParameter(QList<QSharedPointer<ModelParameter> >& list, 
        QString const& name, QString const& value) const;

    ModelParameterComparator* comparator_;
};

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::tst_ModelParameterComparator()
//-----------------------------------------------------------------------------
tst_ModelParameterComparator::tst_ModelParameterComparator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::initTestCase()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::initTestCase()
{
    comparator_ = makeComparator();
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::cleanupTestCase()
{
    delete comparator_;
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::makeComparator()
//-----------------------------------------------------------------------------
ModelParameterComparator* tst_ModelParameterComparator::makeComparator()
{
    return new ModelParameterComparator;
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testComparingTwoNullsAreEquivalent()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testComparingTwoNullsAreEquivalent()
{
    QVERIFY2(comparator_->compare(QSharedPointer<ModelParameter>(0), 
        QSharedPointer<ModelParameter>(0)), "Two nulls should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testComparingNullToComponentIsNotEquivalent()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testComparingNullToComponentIsNotEquivalent()
{
    QSharedPointer<ModelParameter> modelParameter(new ModelParameter());

    QVERIFY2(!comparator_->compare(QSharedPointer<ModelParameter>(0), modelParameter), 
        "Comparison to null should not be equal.");
    QVERIFY2(!comparator_->compare(modelParameter, QSharedPointer<ModelParameter>(0)), 
        "Comparison to null should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testComparingWithSelfIsEquivalent()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testComparingWithSelfIsEquivalent()
{
    QSharedPointer<ModelParameter> modelParameter(new ModelParameter());
    QVERIFY2(comparator_->compare(modelParameter, modelParameter), "Comparing with self should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testComparingTwoEmptyListsIsEquivalent()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testComparingTwoEmptyListsIsEquivalent()
{
    QVERIFY2(comparator_->compare(QList<QSharedPointer<ModelParameter> >(), 
        QList<QSharedPointer<ModelParameter> >()), "Two empty lists should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testComparingWithSelfIsEquivalent()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testComparingWithSameListIsEquivalent()
{
    QList<QSharedPointer<ModelParameter> > list;

    QVERIFY2(comparator_->compare(list, list), "Comparing with self should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testModelParameterInOneComponentIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testModelParameterInOneListIsNotEqual()
{
    QList<QSharedPointer<ModelParameter> > reference;
    QList<QSharedPointer<ModelParameter> > subject;
    
    QSharedPointer<ModelParameter> modelParameter(new ModelParameter());
    reference.append(modelParameter);

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Lists with different model parameters should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testSimilarModelParametersIsEqual()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testSimilarModelParametersIsEqual()
{
    QList<QSharedPointer<ModelParameter> > reference;
    addModelParameter(reference, "id", "1");

    QList<QSharedPointer<ModelParameter> > subject;
    addModelParameter(subject, "id", "1");

    QVERIFY2(comparator_->compare(reference, subject), "Lists with same model parameter should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testDifferentModelParametersIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testDifferentModelParametersIsNotEqual()
{
    QList<QSharedPointer<ModelParameter> > reference;
    addModelParameter(reference, "id", "1");

    QList<QSharedPointer<ModelParameter> > subject;    
    addModelParameter(subject, "bridge count", "1");

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Lists with different model parameters should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testDifferentModelParameterValuesIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testDifferentModelParameterValuesIsNotEqual()
{
    QList<QSharedPointer<ModelParameter> > reference;
    addModelParameter(reference, "id", "1");

    QList<QSharedPointer<ModelParameter> > subject;
    addModelParameter(subject, "id", "200");

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Lists with different model parameter values should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testMultipleModelParametersWithOneDifferenceIsNotEqual()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testMultipleModelParametersWithOneDifferenceIsNotEqual()
{
    QList<QSharedPointer<ModelParameter> > reference;  
    addModelParameter(reference, "addr_width", "16");
    addModelParameter(reference, "data_width", "32");
    addModelParameter(reference, "id", "1");

    QList<QSharedPointer<ModelParameter> > subject;  
    addModelParameter(subject, "addr_width", "16");    
    addModelParameter(subject, "data_width", "32");
    addModelParameter(subject, "clk_freq", "100000");    

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Lists with different model parameters should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testModelParametersInDifferentOrderIsEqual()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testModelParametersInDifferentOrderIsEqual()
{
    QList<QSharedPointer<ModelParameter> > reference;   
    addModelParameter(reference, "addr_width", "16");
    addModelParameter(reference, "data_width", "32");
    addModelParameter(reference, "id", "1");

    QList<QSharedPointer<ModelParameter> > subject;  
    addModelParameter(subject, "data_width", "32");
    addModelParameter(subject, "id", "1");    
    addModelParameter(subject, "addr_width", "16");    

    QVERIFY2(comparator_->compare(reference, subject), 
        "Lists with same model parameters in different order should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testDiffOfTwoEmptyListsIsNoChange()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testDiffOfTwoEmptyListsIsNoChange()
{
    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(QList<QSharedPointer<ModelParameter> >(),
        QList<QSharedPointer<ModelParameter> >());

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "model parameter", "", IPXactDiff::NO_CHANGE);   
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testDiffToSameComponentIsNoChange()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testDiffToSameComponentIsNoChange()
{
    QList<QSharedPointer<ModelParameter> > reference;   

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, reference);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "model parameter", "", IPXactDiff::NO_CHANGE);   
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testDiffModelParameters()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testDiffModelParameters()
{
    QList<QSharedPointer<ModelParameter> > reference;  
    QSharedPointer<ModelParameter> referenceModelParameter(new ModelParameter());
    referenceModelParameter->setName("id");
    referenceModelParameter->setUsageType("nontyped");
    referenceModelParameter->setValue("1");
    reference.append(referenceModelParameter);

    QList<QSharedPointer<ModelParameter> > subject;  
    QSharedPointer<ModelParameter> subjectModelParameter(new ModelParameter());
    subjectModelParameter->setName("id");
    subjectModelParameter->setDataType("int");
    subjectModelParameter->setValue("200");
    subject.append(subjectModelParameter);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "model parameter", "id", IPXactDiff::MODIFICATION);

    QList<IPXactDiff::Modification> modifications = diff.first()->getChangeList();
    QCOMPARE(modifications.count(), 3);

    IPXactDiff::Modification valueTypeChange = modifications.at(0);
    verifyModificationIs(valueTypeChange, "value", "1", "200");

    IPXactDiff::Modification dataTypeChange = modifications.at(1);
    verifyModificationIs(dataTypeChange, "data type", "", "int");
   
    IPXactDiff::Modification usageTypeChange = modifications.at(2);
    verifyModificationIs(usageTypeChange, "usage type", "nontyped", "");    
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testDiffModelParametersInDifferentOrderIsNoChange()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testDiffModelParametersInDifferentOrderIsNoChange()
{
    QList<QSharedPointer<ModelParameter> > reference;    
    addModelParameter(reference, "addr_width", "16");
    addModelParameter(reference, "data_width", "32");
    addModelParameter(reference, "id", "1");

    QList<QSharedPointer<ModelParameter> > subject;  
    addModelParameter(subject, "data_width", "32");
    addModelParameter(subject, "id", "1");    
    addModelParameter(subject, "addr_width", "16");    

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);    
    verifyDiffElementAndType(diff.first(), "model parameter", "", IPXactDiff::NO_CHANGE);
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testDiffOneOfManyModelParametersChanged()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testDiffOneOfManyModelParametersChanged()
{
    QList<QSharedPointer<ModelParameter> > reference;      
    addModelParameter(reference, "addr_width", "1");
    addModelParameter(reference, "data_width", "1");    

    QList<QSharedPointer<ModelParameter> > subject;    
    addModelParameter(subject, "addr_width", "1");
    addModelParameter(subject, "data_width", "64");    

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "model parameter", "data_width", IPXactDiff::MODIFICATION);
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::testDiffMultipleModelParametersChanged()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::testDiffMultipleModelParametersChanged()
{
    QList<QSharedPointer<ModelParameter> > reference;      
    addModelParameter(reference, "addr_width", "16");
    addModelParameter(reference, "data_width", "32");    

    QList<QSharedPointer<ModelParameter> > subject;    
    addModelParameter(subject, "addr_width", "8");
    addModelParameter(subject, "data_width", "64");    
    
    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 2);
    verifyDiffElementAndType(diff.first(), "model parameter", "addr_width", IPXactDiff::MODIFICATION);
    verifyDiffElementAndType(diff.last(), "model parameter", "data_width", IPXactDiff::MODIFICATION);
}

//-----------------------------------------------------------------------------
// Function: tst_ModelParameterComparator::addModelParameter()
//-----------------------------------------------------------------------------
void tst_ModelParameterComparator::addModelParameter(QList<QSharedPointer<ModelParameter> >& list,
    QString const& name, QString const& value) const
{
    QSharedPointer<ModelParameter> newModelParameter(new ModelParameter());
    newModelParameter->setName(name);
    newModelParameter->setValue(value);

    list.append(newModelParameter);
}

QTEST_APPLESS_MAIN(tst_ModelParameterComparator)

#include "tst_ModelParameterComparator.moc"
    
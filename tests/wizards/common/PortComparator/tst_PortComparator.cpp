//-----------------------------------------------------------------------------
// File: tst_PortComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.10.2014
//
// Description:
// Unit test for class PortComparator.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <QSharedPointer>

#include <IPXACTmodels/port.h>

#include <wizards/common/PortComparator/PortComparator.h>
#include <wizards/common/IPXactDiff.h>

#include <tests/wizards/common/ComparatorTest.h>

Q_DECLARE_METATYPE(General::Direction)

class tst_PortComparator : public QObject, public ComparatorTest
{
    Q_OBJECT

public:
    tst_PortComparator();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testComparingTwoNullsIsEqual();
    void testComparisonToNullIsNotEqual();
    void testComparingWithSelfIsEqual();

    void testElementComparison();
    void testElementComparison_data();

    void testDirectionComparison();
    void testDirectionComparison_data();

    void testLeftBoundComparison();
    void testLeftBoundComparison_data();

    void testRightBoundComparison();
    void testRightBoundComparison_data();

    void testComparingTwoEmptyListsIsEqual();
    void testComparingWithSameListIsEqual();

    void testPortInOneListIsNotEqual();
    void testSimilarPortsIsEqual();
    void testDifferentPortsIsNotEqual();

    void testMultiplePortsWithOneDifferenceIsNotEqual();
    void testPortsInDifferentOrderIsEqual();

    void testDiffElementChange();
    void testDiffElementChange_data();

    void testDiffPorts();
   
    void testDiffPortsInDifferentOrderIsNoChange();

    void testDiffOfTwoEmptyListsIsNoChange();
    void testDiffToSameComponentIsNoChange();

private:
    
    PortComparator* makeComparator();

    void addPort(QList<QSharedPointer<Port> >& list, QString const& name) const;

    void setElementInPort(QString const& element, QString const& value, QSharedPointer<Port> port);
    //-----------------------------------------------------------------------------
    // Function: getElements()
    //-----------------------------------------------------------------------------
    QStringList getElements() const;


    PortComparator* comparator_;
};

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::tst_PortComparator()
//-----------------------------------------------------------------------------
tst_PortComparator::tst_PortComparator()
{

}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::initTestCase()
//-----------------------------------------------------------------------------
void tst_PortComparator::initTestCase()
{
    comparator_ = makeComparator();
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_PortComparator::cleanupTestCase()
{
    delete comparator_;
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::makeComparator()
//-----------------------------------------------------------------------------
PortComparator* tst_PortComparator::makeComparator()
{
    return new PortComparator;
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testComparingTwoNullsIsEqual()
//-----------------------------------------------------------------------------
void tst_PortComparator::testComparingTwoNullsIsEqual()
{
    QVERIFY2(comparator_->compare(QSharedPointer<Port>(0), 
        QSharedPointer<Port>(0)), "Two nulls should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testComparisonToNullIsNotEqual()
//-----------------------------------------------------------------------------
void tst_PortComparator::testComparisonToNullIsNotEqual()
{
    QSharedPointer<Port> port(new Port());

    QVERIFY2(!comparator_->compare(QSharedPointer<Port>(0), port), 
        "Comparison to null should not be equal.");
    QVERIFY2(!comparator_->compare(port, QSharedPointer<Port>(0)), 
        "Comparison to null should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testComparingWithSelfIsEqual()
//-----------------------------------------------------------------------------
void tst_PortComparator::testComparingWithSelfIsEqual()
{
    QSharedPointer<Port> port(new Port());
    QVERIFY2(comparator_->compare(port, port), "Comparing with self should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testElementComparison()
//-----------------------------------------------------------------------------
void tst_PortComparator::testElementComparison()
{
    QStringList elements;
    elements << "name" << getElements();

    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    foreach(QString const& element, elements)
    {
        QSharedPointer<Port> reference(new Port());
        setElementInPort(element, referenceValue, reference);
        
        QSharedPointer<Port> subject(new Port());
        setElementInPort(element, subjectValue, subject);

        QVERIFY2(comparator_->compare(reference, subject) == expectedComparison,
            QString("%1 '%2' does not match '%3'").arg(element, referenceValue, subjectValue).toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testNameComparison_data()
//-----------------------------------------------------------------------------
void tst_PortComparator::testElementComparison_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testDirectionComparison()
//-----------------------------------------------------------------------------
void tst_PortComparator::testDirectionComparison()
{
    QFETCH(General::Direction, referenceDirection);
    QFETCH(General::Direction, subjectDirection);
    QFETCH(bool, expectedComparison);

    QSharedPointer<Port> reference(new Port());
    reference->setDirection(referenceDirection);

    QSharedPointer<Port> subject(new Port());
    subject->setDirection(subjectDirection);

    QVERIFY2(comparator_->compare(reference, subject) == expectedComparison,
        QString("Comparing direction '%1' to '%2' yields unexpected result").arg(General::direction2Str(referenceDirection),
        General::direction2Str(subjectDirection)).toLocal8Bit());
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testDirectionComparison_data()
//-----------------------------------------------------------------------------
void tst_PortComparator::testDirectionComparison_data()
{
    QTest::addColumn<General::Direction>("referenceDirection");
    QTest::addColumn<General::Direction>("subjectDirection");
    QTest::addColumn<bool>("expectedComparison");

    QTest::newRow("Both in") << General::IN << General::IN << true;
    QTest::newRow("Both out") << General::OUT << General::OUT << true;
    QTest::newRow("Both inout") << General::INOUT << General::INOUT << true;
    QTest::newRow("In and out") << General::IN << General::OUT << false;
    QTest::newRow("In and inout") << General::IN << General::INOUT << false;
    QTest::newRow("out and phantom") << General::OUT << General::DIRECTION_PHANTOM << false;
    QTest::newRow("out and inout") << General::OUT << General::INOUT << false;
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testLeftBoundComparison()
//-----------------------------------------------------------------------------
void tst_PortComparator::testLeftBoundComparison()
{
    QFETCH(int, referenceValue);
    QFETCH(int, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<Port> reference(new Port());
    reference->setLeftBound(referenceValue);

    QSharedPointer<Port> subject(new Port());
    subject->setLeftBound(subjectValue);

    QVERIFY2(comparator_->compare(reference, subject) == expectedComparison,
        QString("Comparing left bound '%1' to '%2' yields unexpected result").arg(referenceValue,
        subjectValue).toLocal8Bit());
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testLeftBoundComparison()
//-----------------------------------------------------------------------------
void tst_PortComparator::testLeftBoundComparison_data()
{
    QTest::addColumn<int>("referenceValue");
    QTest::addColumn<int>("subjectValue");
    QTest::addColumn<bool>("expectedComparison");

    QTest::newRow("Both zero") << 0 << 0 << true;
    QTest::newRow("Both negative") << -1 << -1 << true;
    QTest::newRow("Zero and one") << 0 << 1 << false;
    QTest::newRow("One and one") << 1 << 1 << true;
    QTest::newRow("One and negative one") << 1 << -1 << false;
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testRightBoundComparison()
//-----------------------------------------------------------------------------
void tst_PortComparator::testRightBoundComparison()
{
    QFETCH(int, referenceValue);
    QFETCH(int, subjectValue);
    QFETCH(bool, expectedComparison);

    QSharedPointer<Port> reference(new Port());
    reference->setRightBound(referenceValue);

    QSharedPointer<Port> subject(new Port());
    subject->setRightBound(subjectValue);

    QVERIFY2(comparator_->compare(reference, subject) == expectedComparison,
        QString("Comparing right bound '%1' to '%2' yields unexpected result").arg(referenceValue,
        subjectValue).toLocal8Bit());
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testRightBoundComparison_data()
//-----------------------------------------------------------------------------
void tst_PortComparator::testRightBoundComparison_data()
{
    testLeftBoundComparison_data();
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testComparingTwoEmptyListsIsEqual()
//-----------------------------------------------------------------------------
void tst_PortComparator::testComparingTwoEmptyListsIsEqual()
{
    QVERIFY2(comparator_->compare(QList<QSharedPointer<Port> >(), 
        QList<QSharedPointer<Port> >()), "Two empty lists should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testComparingWithSameListIsEqual()
//-----------------------------------------------------------------------------
void tst_PortComparator::testComparingWithSameListIsEqual()
{
    QList<QSharedPointer<Port> > list;

    QVERIFY2(comparator_->compare(list, list), "Comparing with self should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testPortInOneComponentIsNotEqual()
//-----------------------------------------------------------------------------
void tst_PortComparator::testPortInOneListIsNotEqual()
{
    QList<QSharedPointer<Port> > reference;
    QList<QSharedPointer<Port> > subject;
    
    QSharedPointer<Port> port(new Port());
    reference.append(port);

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Lists with different ports should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testSimilarPortsIsEqual()
//-----------------------------------------------------------------------------
void tst_PortComparator::testSimilarPortsIsEqual()
{
    QList<QSharedPointer<Port> > reference;
    addPort(reference, "clk");

    QList<QSharedPointer<Port> > subject;
    addPort(subject, "clk");

    QVERIFY2(comparator_->compare(reference, subject), "Lists with same port should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testDifferentPortsIsNotEqual()
//-----------------------------------------------------------------------------
void tst_PortComparator::testDifferentPortsIsNotEqual()
{
    QList<QSharedPointer<Port> > reference;
    addPort(reference, "clk");

    QList<QSharedPointer<Port> > subject;    
    addPort(subject, "bridge count");

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Lists with different ports should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testMultiplePortsWithOneDifferenceIsNotEqual()
//-----------------------------------------------------------------------------
void tst_PortComparator::testMultiplePortsWithOneDifferenceIsNotEqual()
{
    QList<QSharedPointer<Port> > reference;  
    addPort(reference, "addr");
    addPort(reference, "data");
    addPort(reference, "clk");

    QList<QSharedPointer<Port> > subject;  
    addPort(subject, "addr");    
    addPort(subject, "data");
    addPort(subject, "clk_freq");    

    QVERIFY2(!comparator_->compare(reference, subject), 
        "Lists with different ports should not be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testPortsInDifferentOrderIsEqual()
//-----------------------------------------------------------------------------
void tst_PortComparator::testPortsInDifferentOrderIsEqual()
{
    QList<QSharedPointer<Port> > reference;   
    addPort(reference, "addr");
    addPort(reference, "data");
    addPort(reference, "clk");

    QList<QSharedPointer<Port> > subject;  
    addPort(subject, "data");
    addPort(subject, "clk");    
    addPort(subject, "addr");    

    QVERIFY2(comparator_->compare(reference, subject), 
        "Lists with same ports in different order should be equal.");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testDiffOfTwoEmptyListsIsNoChange()
//-----------------------------------------------------------------------------
void tst_PortComparator::testDiffOfTwoEmptyListsIsNoChange()
{
    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(QList<QSharedPointer<Port> >(),
        QList<QSharedPointer<Port> >());

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "port", "", IPXactDiff::NO_CHANGE);   
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testDiffToSameComponentIsNoChange()
//-----------------------------------------------------------------------------
void tst_PortComparator::testDiffToSameComponentIsNoChange()
{
    QList<QSharedPointer<Port> > reference;   

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, reference);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "port", "", IPXactDiff::NO_CHANGE);   
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testDiffToSameComponentIsNoChange()
//-----------------------------------------------------------------------------
void tst_PortComparator::testDiffElementChange()
{
    QStringList elements = getElements();

    QFETCH(QString, referenceValue);
    QFETCH(QString, subjectValue);
    QFETCH(bool, expectedComparison);

    foreach(QString const& element, elements)
    {
        QSharedPointer<Port> reference(new Port());
        reference->setName("testPort");
        setElementInPort(element, referenceValue, reference);

        QSharedPointer<Port> subject(new Port());
        subject->setName("testPort");
        setElementInPort(element, subjectValue, subject);

        QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

        QCOMPARE(diff.count(), 1);
        if (expectedComparison)
        {
            verifyDiffElementAndType(diff.first(), "port", "", IPXactDiff::NO_CHANGE);   
        }
        else
        {
            verifyDiffElementAndType(diff.first(), "port", "testPort", IPXactDiff::MODIFICATION);
            verifyModificationIs(diff.first()->getChangeList().first(), element, referenceValue, subjectValue);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testDiffElementChange_data()
//-----------------------------------------------------------------------------
void tst_PortComparator::testDiffElementChange_data()
{
    createTestInputsForStringValues();
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testDiffPorts()
//-----------------------------------------------------------------------------
void tst_PortComparator::testDiffPorts()
{
    QList<QSharedPointer<Port> > reference;  
    QSharedPointer<Port> referencePort(new Port());
    referencePort->setName("clk");
    referencePort->setTypeName("std_logic");
    referencePort->setDefaultValue("1");
    reference.append(referencePort);

    QList<QSharedPointer<Port> > subject;  
    QSharedPointer<Port> subjectPort(new Port());
    subjectPort->setName("clk");
    subjectPort->setTypeName("int");
    subjectPort->setDefaultValue("200");
    subject.append(subjectPort);

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);
    verifyDiffElementAndType(diff.first(), "port", "clk", IPXactDiff::MODIFICATION);

    QList<IPXactDiff::Modification> modifications = diff.first()->getChangeList();
    QCOMPARE(modifications.count(), 2);

    IPXactDiff::Modification dataTypeChange = modifications.at(0);
    verifyModificationIs(dataTypeChange, "type name", "std_logic", "int");

    IPXactDiff::Modification valueTypeChange = modifications.at(1);
    verifyModificationIs(valueTypeChange, "default value", "1", "200");
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::testDiffPortsInDifferentOrderIsNoChange()
//-----------------------------------------------------------------------------
void tst_PortComparator::testDiffPortsInDifferentOrderIsNoChange()
{
    QList<QSharedPointer<Port> > reference;    
    addPort(reference, "clk");
    addPort(reference, "reset_n");
    addPort(reference, "data");

    QList<QSharedPointer<Port> > subject;  
    addPort(subject, "reset_n");
    addPort(subject, "data");    
    addPort(subject, "clk");    

    QList<QSharedPointer<IPXactDiff> > diff = comparator_->diff(reference, subject);

    QCOMPARE(diff.count(), 1);    
    verifyDiffElementAndType(diff.first(), "port", "", IPXactDiff::NO_CHANGE);
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::addPort()
//-----------------------------------------------------------------------------
void tst_PortComparator::addPort(QList<QSharedPointer<Port> >& list, QString const& name) const
{
    QSharedPointer<Port> newPort(new Port());
    newPort->setName(name);

    list.append(newPort);
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::setElementInPort()
//-----------------------------------------------------------------------------
void tst_PortComparator::setElementInPort(QString const& element, QString const& value, QSharedPointer<Port> port)
{
    if (element == "name")
    {
        port->setName(value);
    }    
    else if (element == "port access type")
    {
        port->setPortAccessType(value);
    }  
    else if (element == "port access handle")
    {
        QFAIL(QString("No setter for element %1").arg(element).toLocal8Bit());
    }  
    else if (element == "type name")
    {
        port->setTypeName(value);
    }  
    else if (element == "type definitions")
    {
        port->setTypeDefinition("testType", value);
    }      
    else if (element == "default value")
    {
        port->setDefaultValue(value);
    }  
    else
    {
        QFAIL(QString("Attempting to set unspecified element %1").arg(element).toLocal8Bit());
    }
}

//-----------------------------------------------------------------------------
// Function: tst_PortComparator::getElements()
//-----------------------------------------------------------------------------
QStringList tst_PortComparator::getElements() const
{
    QStringList elements;
    elements << "port access type" << "type name" << "type definitions" << "default value";
    return elements;
}

QTEST_APPLESS_MAIN(tst_PortComparator)

#include "tst_PortComparator.moc"
    
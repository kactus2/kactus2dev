//-----------------------------------------------------------------------------
// File: tst_ConstraintSetReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Unit test for class ConstraintSetReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ConstraintSetReader.h>

#include <QtTest>
#include <QDomNode>

class tst_ConstraintSetReader : public QObject
{
    Q_OBJECT

public:
    tst_ConstraintSetReader();

private slots:

    void testReadConstraintSet();
};

//-----------------------------------------------------------------------------
// Function: tst_ConstraintSetReader::tst_ConstraintSetReader()
//-----------------------------------------------------------------------------
tst_ConstraintSetReader::tst_ConstraintSetReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ConstraintSetReader::testReadConstraintSet()
//-----------------------------------------------------------------------------
void tst_ConstraintSetReader::testReadConstraintSet()
{
    QString documentContent(
        "<ipxact:constraintSet constraintSetId=\"an-id\">"
            "<ipxact:name>testSet</ipxact:name>"
            "<ipxact:displayName>display</ipxact:displayName>"
            "<ipxact:description>description</ipxact:description>"
            "<ipxact:vector>"
                "<ipxact:left>32</ipxact:left>"
                "<ipxact:right>0</ipxact:right>"
            "</ipxact:vector>"
            "<ipxact:driveConstraint>"
                "<ipxact:cellSpecification cellStrength=\"median\">"
                    "<ipxact:cellFunction>inv</ipxact:cellFunction>"
                "</ipxact:cellSpecification>"
            "</ipxact:driveConstraint>"
            "<ipxact:loadConstraint>"
                "<ipxact:cellSpecification cellStrength=\"low\">"
                    "<ipxact:cellClass>sequential</ipxact:cellClass>"
                "</ipxact:cellSpecification>"
                "<ipxact:count>1</ipxact:count>"
            "</ipxact:loadConstraint>"
            "<ipxact:timingConstraint clockEdge=\"rise\" delayType=\"min\" clockName=\"a_clock\"/>"
            "<ipxact:timingConstraint clockEdge=\"fall\" delayType=\"max\" clockName=\"clock2\"/>"
        "</ipxact:constraintSet>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode setNode = document.firstChildElement("ipxact:constraintSet");

    QSharedPointer<ConstraintSet> testSet = ConstraintSetReader::createConstraintSetFrom(setNode, Document::Revision::Std14);

    QCOMPARE(testSet->name(), QString("testSet"));
    QCOMPARE(testSet->displayName(), QString("display"));
    QCOMPARE(testSet->description(), QString("description"));
    QCOMPARE(testSet->getVector().getLeft(), QString("32"));
    QCOMPARE(testSet->getVector().getRight(), QString("0"));
    QCOMPARE(testSet->getDriveConstraint()->getCellStrength(), CellSpecification::CellStrengthType::MEDIAN);
    QCOMPARE(testSet->getDriveConstraint()->getCellFunction(), QString("inv"));
    QCOMPARE(testSet->getLoadConstraint()->getCellStrength(), CellSpecification::CellStrengthType::LOW);
    QCOMPARE(testSet->getLoadConstraint()->getCellClass(), CellSpecification::CellClassType::SEQUENTIAL);
    QCOMPARE(testSet->getLoadConstraintCount(), QString("1"));
    QCOMPARE(testSet->getTimingConstraints()->size(), 2);
    QCOMPARE(testSet->getTimingConstraints()->first()->clockEdge_, QString("rise"));
    QCOMPARE(testSet->getTimingConstraints()->first()->delayType_, QString("min"));
    QCOMPARE(testSet->getTimingConstraints()->first()->clockName_, QString("a_clock"));
}


QTEST_APPLESS_MAIN(tst_ConstraintSetReader)

#include "tst_ConstraintSetReader.moc"

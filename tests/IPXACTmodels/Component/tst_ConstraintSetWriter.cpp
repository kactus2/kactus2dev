//-----------------------------------------------------------------------------
// File: tst_ConstraintSetWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.09.2015
//
// Description:
// Unit test for ConstraintSetWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/ConstraintSetWriter.h>

#include <QtTest>
#include <QXmlStreamWriter>

class tst_ConstraintSetWriter : public QObject
{
    Q_OBJECT

public:
    tst_ConstraintSetWriter();

private slots:

    void testWriteConstraintSet();
};

//-----------------------------------------------------------------------------
// Function: tst_ConstraintSetWriter::tst_ConstraintSetWriter()
//-----------------------------------------------------------------------------
tst_ConstraintSetWriter::tst_ConstraintSetWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_ConstraintSetWriter::testReadConstraintSet()
//-----------------------------------------------------------------------------
void tst_ConstraintSetWriter::testWriteConstraintSet()
{
    QString output;
	QXmlStreamWriter xmlStreamWriter(&output);

    Vector vec("32", "0");

    auto driveConstraint = QSharedPointer<CellSpecification>(new CellSpecification);
    driveConstraint->setCellStrength(CellSpecification::CellStrengthType::MEDIAN);
    driveConstraint->setCellFunction(QString("inv"));

    auto loadConstraint = QSharedPointer<CellSpecification>(new CellSpecification);
    loadConstraint->setCellStrength(CellSpecification::CellStrengthType::LOW);
    loadConstraint->setCellClass(CellSpecification::CellClassType::SEQUENTIAL);

    auto timingConstraint1 = QSharedPointer<ConstraintSet::TimingConstraint>(new ConstraintSet::TimingConstraint());
    auto timingConstraint2 = QSharedPointer<ConstraintSet::TimingConstraint>(new ConstraintSet::TimingConstraint());

    timingConstraint1->clockEdge_ = "rise";
    timingConstraint1->delayType_ = "min";
    timingConstraint1->clockName_ = "a_clock";

    timingConstraint2->clockEdge_ = "fall";
    timingConstraint2->delayType_ = "max";
    timingConstraint2->clockName_ = "clock2";

    auto constraintSet = QSharedPointer<ConstraintSet>(new ConstraintSet("testSet"));
    constraintSet->setConstraintSetId("an-id");
    constraintSet->setDisplayName("display");
    constraintSet->setDescription("description");
    constraintSet->setDriveConstraint(driveConstraint);
    constraintSet->setLoadConstraint(loadConstraint);
    constraintSet->setLoadConstraintCount(QString("1"));
    constraintSet->setVector(vec);
    constraintSet->getTimingConstraints()->append(timingConstraint1);
    constraintSet->getTimingConstraints()->append(timingConstraint2);

    QString expectedOutput(
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

    ConstraintSetWriter::writeConstraintSet(xmlStreamWriter, constraintSet, Document::Revision::Std14);
    QFile file("output.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    file.write(output.toUtf8());
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_ConstraintSetWriter)

#include "tst_ConstraintSetWriter.moc"

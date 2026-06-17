//-----------------------------------------------------------------------------
// File: ConstraintSet.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 11.06.2026
//
// Description:
// Describes the ipxact:constraintSet element.
//-----------------------------------------------------------------------------

#pragma once

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/CellSpecification.h>
#include <IPXACTmodels/common/Vector.h>


class ConstraintSet : public NameGroup
{
public:

    struct TimingConstraint
    {
        QString value_;
        QString clockEdge_;
        QString delayType_;
        QString clockName_;
    };
    
    explicit ConstraintSet(QString const& name = QString());

    ConstraintSet(ConstraintSet const& other);
    ConstraintSet& operator=(ConstraintSet const& other);

    ~ConstraintSet() = default;

    QString getConstraintSetId() const;
    void setConstraintSetId(QString const& newId);

    Vector getVector() const;
    void setVector(Vector const& vector);

    QSharedPointer<CellSpecification> getDriveConstraint() const;
    void setDriveConstraint(QSharedPointer<CellSpecification> driveConstraint);

    QSharedPointer<CellSpecification> getLoadConstraint() const;
    void setLoadConstraint(QSharedPointer<CellSpecification> loadConstraint);

    QString getLoadConstraintCount() const;
    void setLoadConstraintCount(QString const& loadConstraintCount);

    QSharedPointer<QList<QSharedPointer<TimingConstraint> > > getTimingConstraints() const;
    void setTimingConstraints(QSharedPointer<QList<QSharedPointer<TimingConstraint> > > timingConstraints);

private:

    QString constraintSetId_;

    Vector vector_;

    QSharedPointer<CellSpecification> driveConstraint_;

    QSharedPointer<CellSpecification> loadConstraint_;
    QString loadConstraintCount_;

    QSharedPointer<QList<QSharedPointer<TimingConstraint> > > timingConstraints_ =
        QSharedPointer<QList<QSharedPointer<TimingConstraint> > >(new QList<QSharedPointer<TimingConstraint> >());
};

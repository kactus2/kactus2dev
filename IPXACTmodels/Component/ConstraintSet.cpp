//-----------------------------------------------------------------------------
// File: ConstraintSet.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 11.06.2026
//
// Description:
// Describes the ipxact:constraintSet element.
//-----------------------------------------------------------------------------

#include "ConstraintSet.h"

#include <utilities/Copy.h>

ConstraintSet::ConstraintSet(QString const& name) :
    NameGroup(name),
    constraintSetId_(),
    vector_(),
    driveConstraint_(),
    loadConstraint_(),
    loadConstraintCount_()
{

}

ConstraintSet::ConstraintSet(ConstraintSet const& other) :
    NameGroup(other),
    vector_(other.vector_),
    constraintSetId_(other.constraintSetId_),
    loadConstraintCount_(other.loadConstraintCount_)
{
    if (other.driveConstraint_)
    {
        driveConstraint_ = QSharedPointer<CellSpecification>(new CellSpecification(*other.driveConstraint_));
    }

    if (other.loadConstraint_)
    {
        loadConstraint_ = QSharedPointer<CellSpecification>(new CellSpecification(*other.loadConstraint_));
    }

    Copy::copyList(other.timingConstraints_, timingConstraints_);
}

ConstraintSet& ConstraintSet::operator=(ConstraintSet const& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        vector_ = other.vector_;
        constraintSetId_ = other.constraintSetId_;
        loadConstraintCount_ = other.loadConstraintCount_;

        driveConstraint_.clear();
        if (other.driveConstraint_)
        {
            driveConstraint_ = QSharedPointer<CellSpecification>(new CellSpecification(*other.driveConstraint_));
        }

        loadConstraint_.clear();
        if (other.loadConstraint_)
        {
            loadConstraint_ = QSharedPointer<CellSpecification>(new CellSpecification(*other.loadConstraint_));
        }

        timingConstraints_.clear();
        Copy::copyList(other.timingConstraints_, timingConstraints_);
    }

    return *this;
}

QString ConstraintSet::getConstraintSetId() const
{
    return constraintSetId_;
}

void ConstraintSet::setConstraintSetId(QString const& newId)
{
    constraintSetId_ = newId;
}

Vector ConstraintSet::getVector() const
{
    return vector_;
}

void ConstraintSet::setVector(Vector const& vector)
{
    vector_ = vector;
}

QSharedPointer<CellSpecification> ConstraintSet::getDriveConstraint() const
{
    return driveConstraint_;
}

void ConstraintSet::setDriveConstraint(QSharedPointer<CellSpecification> driveConstraint)
{
    driveConstraint_ = driveConstraint;
}

QSharedPointer<CellSpecification> ConstraintSet::getLoadConstraint() const
{
    return loadConstraint_;
}

void ConstraintSet::setLoadConstraint(QSharedPointer<CellSpecification> loadConstraint)
{
    loadConstraint_ = loadConstraint;
}

QString ConstraintSet::getLoadConstraintCount() const
{
    return loadConstraintCount_;
}

void ConstraintSet::setLoadConstraintCount(QString const& loadConstraintCount)
{
    loadConstraintCount_ = loadConstraintCount;
}

QSharedPointer<QList<QSharedPointer<ConstraintSet::TimingConstraint> > > ConstraintSet::getTimingConstraints() const
{
    return timingConstraints_;
}

void ConstraintSet::setTimingConstraints(QSharedPointer<QList<QSharedPointer<ConstraintSet::TimingConstraint> > > timingConstraints)
{
    timingConstraints_ = timingConstraints;
}

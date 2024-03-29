//-----------------------------------------------------------------------------
// File: PartSelect.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.08.2015
//
// Description:
// Describes an ipxact:partSelect element.
//-----------------------------------------------------------------------------

#include "PartSelect.h"

//-----------------------------------------------------------------------------
// Function: PartSelect::PartSelect()
//-----------------------------------------------------------------------------
PartSelect::PartSelect(QString const& leftRange, QString const& rightRange):
range_(leftRange, rightRange)
{

}

//-----------------------------------------------------------------------------
// Function: PartSelect::PartSelect()
//-----------------------------------------------------------------------------
PartSelect::PartSelect(const PartSelect& other):
range_(other.range_)
{
    for (QString const& index : *other.indices_)
    {
        indices_->append(index);
    }
}

//-----------------------------------------------------------------------------
// Function: PartSelect::clone()
//-----------------------------------------------------------------------------
PartSelect* PartSelect::clone() const
{
    return new PartSelect(*this);
}

//-----------------------------------------------------------------------------
// Function: PartSelect::operator=()
//-----------------------------------------------------------------------------
PartSelect& PartSelect::operator=(const PartSelect& other)
{
    if (this != &other)
    {
        range_ = other.range_;

        indices_->clear();
        for (QString const& index : *other.indices_)
        {
            indices_->append(index);
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: PartSelect::getLeftRange()
//-----------------------------------------------------------------------------
QString PartSelect::getLeftRange() const
{
    return range_.getLeft();
}

//-----------------------------------------------------------------------------
// Function: PartSelect::getRightRange()
//-----------------------------------------------------------------------------
QString PartSelect::getRightRange() const
{
    return range_.getRight();
}

//-----------------------------------------------------------------------------
// Function: PartSelect::getRange()
//-----------------------------------------------------------------------------
Range PartSelect::getRange() const
{
    return range_;
}

//-----------------------------------------------------------------------------
// Function: PartSelect::setLeftRange()
//-----------------------------------------------------------------------------
void PartSelect::setLeftRange(QString const& newLeftRange)
{
    range_.setLeft(newLeftRange);
}

//-----------------------------------------------------------------------------
// Function: PartSelect::setRightRange()
//-----------------------------------------------------------------------------
void PartSelect::setRightRange(QString const& newRightRange)
{
    range_.setRight(newRightRange);
}

//-----------------------------------------------------------------------------
// Function: PartSelect::getIndices()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> PartSelect::getIndices() const
{
    return indices_;
}

//-----------------------------------------------------------------------------
// Function: PartSelect::setIndices()
//-----------------------------------------------------------------------------
void PartSelect::setIndices(QSharedPointer<QStringList> newIndices)
{
    indices_ = newIndices;
}

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
range_(leftRange, rightRange),
indices_(new QStringList())
{

}

//-----------------------------------------------------------------------------
// Function: PartSelect::PartSelect()
//-----------------------------------------------------------------------------
PartSelect::PartSelect(const PartSelect& other):
range_(other.getLeftRange(), other.getRightRange()),
indices_(new QStringList())
{
    foreach (QString index, *other.indices_)
    {
        indices_->append(index);
    }
}

//-----------------------------------------------------------------------------
// Function: PartSelect::~PartSelect()
//-----------------------------------------------------------------------------
PartSelect::~PartSelect()
{
    indices_.clear();
}

//-----------------------------------------------------------------------------
// Function: PartSelect::operator=()
//-----------------------------------------------------------------------------
PartSelect& PartSelect::operator=(const PartSelect& other)
{
    if (this != &other)
    {
        setLeftRange(other.getLeftRange());
        setRightRange(other.getRightRange());

        indices_->clear();
        foreach (QString index, *other.indices_)
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
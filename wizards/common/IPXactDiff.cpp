//-----------------------------------------------------------------------------
// File: IPXactDiff.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Class for capturing changes in IP-XACT elements.
//-----------------------------------------------------------------------------

#include "IPXactDiff.h"

//-----------------------------------------------------------------------------
// Function: IPXactDiff::IPXactDiff()
//-----------------------------------------------------------------------------
IPXactDiff::IPXactDiff(QString const& element, QString const& name):element_(element), name_(name), 
    changeType_(NO_CHANGE), modifications_()
{

}

//-----------------------------------------------------------------------------
// Function: IPXactDiff::~IPXactDiff()
//-----------------------------------------------------------------------------
IPXactDiff::~IPXactDiff()
{

}

//-----------------------------------------------------------------------------
// Function: IPXactDiff::id()
//-----------------------------------------------------------------------------
QString IPXactDiff::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: IPXactDiff::element()
//-----------------------------------------------------------------------------
QString IPXactDiff::element() const
{
    return element_;
}

//-----------------------------------------------------------------------------
// Function: IPXactDiff::type()
//-----------------------------------------------------------------------------
IPXactDiff::DiffType IPXactDiff::changeType() const
{
    return changeType_;
}

//-----------------------------------------------------------------------------
// Function: IPXactDiff::setType()
//-----------------------------------------------------------------------------
void IPXactDiff::setChangeType(DiffType type)
{
    changeType_ = type;
}

//-----------------------------------------------------------------------------
// Function: IPXactDiff::modifications()
//-----------------------------------------------------------------------------
QList<IPXactDiff::Modification> IPXactDiff::getChangeList() const
{
    return modifications_;
}

//-----------------------------------------------------------------------------
// Function: IPXactDiff::addModificationIfChanged()
//-----------------------------------------------------------------------------
void IPXactDiff::checkForChange(QString const& element, QString const& referenceValue, QString const& subjectValue)
{
    if (referenceValue != subjectValue)
    {
        Modification modification;
        modification.modifiedElement = element;
        modification.previousValue = referenceValue;
        modification.newValue = subjectValue;

        modifications_.append(modification);
    }
}

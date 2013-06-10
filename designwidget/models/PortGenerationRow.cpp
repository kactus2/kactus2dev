//-----------------------------------------------------------------------------
// File: PortGenerationRow.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 3.6.2011
//
// Description:
// One row in port generation table.
//-----------------------------------------------------------------------------

#include "PortGenerationRow.h"

#include <QString>

#include "models/generaldeclarations.h"

//-----------------------------------------------------------------------------
// Function: PortGenerationRow()
//-----------------------------------------------------------------------------
PortGenerationRow::PortGenerationRow(QString name, General::Direction direction,
    QString description, int size) : sourceName_(name), sourceDirection_(direction),
    sourceDescription_(description), size_(size), draftName_(), 
    draftDirection_(General::DIRECTION_INVALID), draftDescription_()
{
}

//-----------------------------------------------------------------------------
// Function: ~PortGenerationRow()
//-----------------------------------------------------------------------------
PortGenerationRow::~PortGenerationRow()
{
}

//-----------------------------------------------------------------------------
// Function: getSourceDescription()
//-----------------------------------------------------------------------------
QString PortGenerationRow::getSourceDescription() const
{
    return sourceDescription_;
}    

//-----------------------------------------------------------------------------
// Function: getSourceDirection()
//-----------------------------------------------------------------------------
General::Direction PortGenerationRow::getSourceDirection() const
{
    return sourceDirection_;
}

//-----------------------------------------------------------------------------
// Function: getSourceName()
//-----------------------------------------------------------------------------
QString PortGenerationRow::getSourceName() const
{
    return sourceName_;
}      

//-----------------------------------------------------------------------------
// Function: getSize()
//-----------------------------------------------------------------------------
int PortGenerationRow::getSize() const
{
    return size_;
}

//-----------------------------------------------------------------------------
// Function: setSize()
//-----------------------------------------------------------------------------
void PortGenerationRow::setSize(int const size)
{
    size_ = size;
}

//-----------------------------------------------------------------------------
// Function: getDraftName()
//-----------------------------------------------------------------------------
QString PortGenerationRow::getDraftName() const
{
    return draftName_;
}

//-----------------------------------------------------------------------------
// Function: setDraftName()
//-----------------------------------------------------------------------------
void PortGenerationRow::setDraftName(QString const name)
{
    draftName_ = name;
}

//-----------------------------------------------------------------------------
// Function: getDraftDirection()
//-----------------------------------------------------------------------------
General::Direction PortGenerationRow::getDraftDirection() const
{
    return draftDirection_;
}

//-----------------------------------------------------------------------------
// Function: setDraftDirection()
//-----------------------------------------------------------------------------
void PortGenerationRow::setDraftDirection(General::Direction const direction)
{
    draftDirection_ = direction;
}

//-----------------------------------------------------------------------------
// Function: getDraftDescription()
//-----------------------------------------------------------------------------
QString PortGenerationRow::getDraftDescription() const
{
    return draftDescription_;
}

//-----------------------------------------------------------------------------
// Function: setDraftDescription()
//-----------------------------------------------------------------------------
void PortGenerationRow::setDraftDescription(QString const description)
{
    draftDescription_ = description;
}

//-----------------------------------------------------------------------------
// Function: isValid()
//-----------------------------------------------------------------------------
bool PortGenerationRow::isValid() const
{
    if ( sourceName_.isEmpty() )
    {
        return false;
    }

    if ( sourceDirection_ == General::DIRECTION_PHANTOM || sourceDirection_ == General::DIRECTION_INVALID )
    {
        return false;
    }

    if ( size_ < 0 )
    {
        return false;
    }

    if ( draftName_.isEmpty() )
    {
        return false;
    }

    if ( draftDirection_ == General::DIRECTION_PHANTOM || draftDirection_ == General::DIRECTION_INVALID )
    {
        return false;
    }

    return true;
}
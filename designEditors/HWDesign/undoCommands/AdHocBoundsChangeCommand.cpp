//-----------------------------------------------------------------------------
// File: AdHocBoundsChangeCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.05.2017
//
// Description:
// Undo command for changing left and right bounds of an ad hoc port.
//-----------------------------------------------------------------------------

#include "AdHocBoundsChangeCommand.h"

#include <IPXACTmodels/common/PartSelect.h>
#include <IPXACTmodels/Design/PortReference.h>

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::AdHocBoundsChangeCommand()
//-----------------------------------------------------------------------------
AdHocBoundsChangeCommand::AdHocBoundsChangeCommand(QSharedPointer<PortReference> referencedPort,
    QString const& newLeftBound, QString const& newRightBound, QUndoCommand* parent):
QUndoCommand(parent),
referencedPort_(referencedPort),
oldPartSelect_(0),
newPartSelect_(0)
{
    setupPartSelectForPortReference(newLeftBound, newRightBound);
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::setupPartSelectForPortReference()
//-----------------------------------------------------------------------------
void AdHocBoundsChangeCommand::setupPartSelectForPortReference(QString const& newLeftValue,
    QString const& newRightValue)
{
    newPartSelect_ = QSharedPointer<PartSelect>(new PartSelect(newLeftValue, newRightValue));

    if (referencedPort_->getPartSelect())
    {
        oldPartSelect_ = referencedPort_->getPartSelect();
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::~AdHocBoundsChangeCommand()
//-----------------------------------------------------------------------------
AdHocBoundsChangeCommand::~AdHocBoundsChangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::undo()
//-----------------------------------------------------------------------------
void AdHocBoundsChangeCommand::undo()
{
    QUndoCommand::undo();

    setPartSelectForReferencedPort(oldPartSelect_);
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::redo()
//-----------------------------------------------------------------------------
void AdHocBoundsChangeCommand::redo()
{
    setPartSelectForReferencedPort(newPartSelect_);

    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::setPartSelectForReferencedPort()
//-----------------------------------------------------------------------------
void AdHocBoundsChangeCommand::setPartSelectForReferencedPort(QSharedPointer<PartSelect> partSelect)
{
    if (partSelect && (!partSelect->getLeftRange().isEmpty() || !partSelect->getRightRange().isEmpty()))
    {
        referencedPort_->setPartSelect(partSelect);
    }
    else
    {
        referencedPort_->setPartSelect(QSharedPointer<PartSelect>());
    }
}

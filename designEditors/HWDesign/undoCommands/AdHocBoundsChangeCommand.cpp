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
QObject(0),
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

    if (oldPartSelect_)
    {
        emit increaseReferences(oldPartSelect_->getLeftRange());
        emit increaseReferences(oldPartSelect_->getRightRange());
    }
    if (newPartSelect_)
    {
        emit decreaseReferences(newPartSelect_->getLeftRange());
        emit decreaseReferences(newPartSelect_->getRightRange());
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocBoundsChangeCommand::redo()
//-----------------------------------------------------------------------------
void AdHocBoundsChangeCommand::redo()
{
    setPartSelectForReferencedPort(newPartSelect_);

    if (newPartSelect_)
    {
        emit increaseReferences(newPartSelect_->getLeftRange());
        emit increaseReferences(newPartSelect_->getRightRange());
    }
    if (oldPartSelect_)
    {
        emit decreaseReferences(oldPartSelect_->getLeftRange());
        emit decreaseReferences(oldPartSelect_->getRightRange());
    }

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

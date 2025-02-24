//-----------------------------------------------------------------------------
// File: AdHocTieOffChangeCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.05.2016
//
// Description:
// Undo command for changing tie off in ad hoc editor.
//-----------------------------------------------------------------------------

#include "AdHocTieOffChangeCommand.h"

#include <editors/HWDesign/HWComponentItem.h>
#include <editors/HWDesign/AdHocItem.h>
#include <editors/HWDesign/ActivePortItem.h>
#include <editors/HWDesign/HierarchicalPortItem.h>
#include <editors/HWDesign/HWDesignDiagram.h>

#include <IPXACTmodels/Design/AdHocConnection.h>
#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::AdHocTieOffChangeCommand()
//-----------------------------------------------------------------------------
AdHocTieOffChangeCommand::AdHocTieOffChangeCommand(AdHocItem* portItem, QSharedPointer<AdHocConnection> connection,
    QString const& newTieOffValue, QString newParsedTieOff, QString newFormattedTieOff, int newBase,
    QString const& oldTieOffValue, QString oldParsedTieOff, QString oldFormattedTieOff, int oldBase,
    HWDesignDiagram* designDiagram, QUndoCommand* parent):
QObject(0),
AdHocTiedValueCommand(portItem, connection, designDiagram),
QUndoCommand(parent),
containingDiagram_(designDiagram),
oldTieOff_(oldTieOffValue),
parsedOldTieOff_(oldParsedTieOff),
formattedOldTieOff_(oldFormattedTieOff),
oldBase_(oldBase),
newTieOff_(newTieOffValue),
parsedNewTieOff_(newParsedTieOff),
formattedNewTieOff_(newFormattedTieOff),
newBase_(newBase),
valueFormatter_()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::~AdHocTieOffChangeCommand()
//-----------------------------------------------------------------------------
AdHocTieOffChangeCommand::~AdHocTieOffChangeCommand()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::undo()
//-----------------------------------------------------------------------------
void AdHocTieOffChangeCommand::undo()
{
    QUndoCommand::undo();

    QSharedPointer<AdHocConnection> tieOffConnection = getTiedValueConnection();
    if (tieOffConnection)
    {
        tieOffConnection->setTiedValue(oldTieOff_);

        changeTieOffSymbolsInConnectedPorts(oldTieOff_, parsedOldTieOff_, formattedOldTieOff_, oldBase_);

        addOrRemoveTiedValueConnection();

        emit increaseReferences(oldTieOff_);
        emit decreaseReferences(newTieOff_);
    }

    emit refreshEditors();
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::redo()
//-----------------------------------------------------------------------------
void AdHocTieOffChangeCommand::redo()
{
    QSharedPointer<AdHocConnection> tieOffConnection = getTiedValueConnection();
    if (tieOffConnection)
    {
        tieOffConnection->setTiedValue(newTieOff_);

        changeTieOffSymbolsInConnectedPorts(newTieOff_, parsedNewTieOff_, formattedNewTieOff_, newBase_);

        addOrRemoveTiedValueConnection();

        emit increaseReferences(newTieOff_);
        emit decreaseReferences(oldTieOff_);
    }

    emit refreshEditors();

    QUndoCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::changeTieOffSymbolsInConnectedPorts()
//-----------------------------------------------------------------------------
void AdHocTieOffChangeCommand::changeTieOffSymbolsInConnectedPorts(QString const& tieOffValue,
    QString const& parsedTieOff, QString const& formattedTieOff, int tieOffBase) const
{
    QSharedPointer<AdHocConnection> tieOffConnection = getTiedValueConnection();

    if (!tieOffConnection->getInternalPortReferences()->isEmpty())
    {
        for (auto const& internalReference : *tieOffConnection->getInternalPortReferences())
        {
            HWComponentItem* componentItem =
                containingDiagram_->getComponentItem(internalReference->getComponentRef());

            if (componentItem)
            {
                ActivePortItem* portItem = componentItem->getAdHocPort(internalReference->getPortRef());

                if (portItem)
                {
                    drawTieOffSymbol(portItem, tieOffValue, parsedTieOff, formattedTieOff, tieOffBase);
                }
            }
        }
    }

    if (!tieOffConnection->getExternalPortReferences()->isEmpty())
    {
        for (auto const& externalReference : *tieOffConnection->getExternalPortReferences())
        {
            HWConnectionEndpoint* endPoint =
                containingDiagram_->getDiagramAdHocPort(externalReference->getPortRef());
            if (endPoint)
            {
                AdHocItem* portItem = dynamic_cast<AdHocItem*>(endPoint);
                if (portItem)
                {
                    drawTieOffSymbol(portItem, tieOffValue, parsedTieOff, formattedTieOff, tieOffBase);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::drawTieOffSymbol()
//-----------------------------------------------------------------------------
void AdHocTieOffChangeCommand::drawTieOffSymbol(AdHocItem* portItem, QString const& tieOffValue,
    QString parsedTieOff, QString formattedTieOff, int tieOffBase) const
{
    bool canConvertTieOffToInt = true;
    parsedTieOff.toInt(&canConvertTieOffToInt);

    if (tieOffValue.isEmpty())
    {
        portItem->removeTieOffItem();
    }
    else
    {
        QString tieOffWithBase = valueFormatter_.format(parsedTieOff, tieOffBase);

        portItem->changeTieOffLabel(formattedTieOff, tieOffWithBase, canConvertTieOffToInt);
    }
}

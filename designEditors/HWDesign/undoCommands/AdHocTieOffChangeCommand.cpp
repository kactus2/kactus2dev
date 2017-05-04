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

#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/AdHocItem.h>
#include <designEditors/HWDesign/AdHocPortItem.h>
#include <designEditors/HWDesign/AdHocInterfaceItem.h>
#include <designEditors/HWDesign/HWDesignDiagram.h>

#include <IPXACTmodels/Design/AdHocConnection.h>
#include <IPXACTmodels/Design/Design.h>

//-----------------------------------------------------------------------------
// Function: AdHocTieOffChangeCommand::AdHocTieOffChangeCommand()
//-----------------------------------------------------------------------------
AdHocTieOffChangeCommand::AdHocTieOffChangeCommand(AdHocItem* portItem, QSharedPointer<AdHocConnection> connection,
    QString const& newTieOffValue, QString newParsedTieOff, QString newFormattedTieOff, int newBase,
    QString const& oldTieOffValue, QString oldParsedTieOff, QString oldFormattedTieOff, int oldBase,
    HWDesignDiagram* designDiagram, QUndoCommand* parent):
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
    }
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
    }

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
        foreach (QSharedPointer<PortReference> internalReference, *tieOffConnection->getInternalPortReferences())
        {
            HWComponentItem* componentItem =
                containingDiagram_->getComponentItem(internalReference->getComponentRef());

            if (componentItem)
            {
                AdHocPortItem* portItem = componentItem->getAdHocPort(internalReference->getPortRef());

                if (portItem)
                {
                    drawTieOffSymbol(portItem, tieOffValue, parsedTieOff, formattedTieOff, tieOffBase);
                }
            }
        }
    }

    if (!tieOffConnection->getExternalPortReferences()->isEmpty())
    {
        foreach (QSharedPointer<PortReference> externalReference, *tieOffConnection->getExternalPortReferences())
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

//-----------------------------------------------------------------------------
// File: HWColumnAddCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.10.2017
//
// Description:
// Undo command for adding HW graphics columns.
//-----------------------------------------------------------------------------

#include "HWColumnAddCommand.h"

#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>

#include <designEditors/common/DesignDiagram.h>
#include <designEditors/HWDesign/AdHocInterfaceItem.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/kactusExtensions/Kactus2Placeholder.h>

//-----------------------------------------------------------------------------
// Function: HWColumnAddCommand::HWColumnAddCommand()
//-----------------------------------------------------------------------------
HWColumnAddCommand::HWColumnAddCommand(GraphicsColumnLayout* layout, GraphicsColumn* column,
    DesignDiagram* diagram, QUndoCommand* parent):
GraphicsColumnAddCommand(layout, column, diagram, parent),
missingAdHocPortItems_(getMissingAdHocPortItems())
{

}

//-----------------------------------------------------------------------------
// Function: HWColumnAddCommand::getMissingAdHocPortItems()
//-----------------------------------------------------------------------------
QVector<AdHocInterfaceItem*> HWColumnAddCommand::getMissingAdHocPortItems() const
{
    QVector<AdHocInterfaceItem*> missingAdHocInterfaces;

    QVector<QSharedPointer<Port> > missingPorts = getMissingAdHocPorts();

    if (!missingPorts.isEmpty())
    {
        QSharedPointer<VendorExtension> adhocExtension = getDiagram()->getDesign()->getAdHocPortPositions();
        QSharedPointer<Kactus2Group> adhocGroup = adhocExtension.dynamicCast<Kactus2Group>();
        if (!adhocGroup)
        {
            adhocGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:adHocVisibilities"));
            getDiagram()->getDesign()->getVendorExtensions()->append(adhocGroup);
        }

        foreach (QSharedPointer<Port> port, missingPorts)
        {
            QSharedPointer<Kactus2Placeholder> newAdHocPosition = getAdHocPositionExtension(adhocGroup, port);

            AdHocInterfaceItem* newAdHocItem =
                new AdHocInterfaceItem(getDiagram()->getEditedComponent(), port, newAdHocPosition);

            missingAdHocInterfaces.append(newAdHocItem);
        }
    }

    return missingAdHocInterfaces;
}


//-----------------------------------------------------------------------------
// Function: HWColumnAddCommand::getMissingPorts()
//-----------------------------------------------------------------------------
QVector<QSharedPointer<Port> > HWColumnAddCommand::getMissingAdHocPorts() const
{
    QVector<QSharedPointer<Port> > missingPorts;

    foreach (QSharedPointer<Port> adhocPort, *getDiagram()->getEditedComponent()->getPorts())
    {
        if (adhocPort->isAdHocVisible() && !portExistsInDiagram(adhocPort))
        {
            missingPorts.append(adhocPort);
        }
    }

    return missingPorts;

}

//-----------------------------------------------------------------------------
// Function: HWColumnAddCommand::portExistsInDiagram()
//-----------------------------------------------------------------------------
bool HWColumnAddCommand::portExistsInDiagram(QSharedPointer<Port> adHocPort) const
{
    QString portName = adHocPort->name();

    if (getDiagram()->getDiagramAdHocPort(portName))
    {
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------
// Function: HWColumnAddCommand::getPositionExtension()
//-----------------------------------------------------------------------------
QSharedPointer<Kactus2Placeholder> HWColumnAddCommand::getAdHocPositionExtension(
    QSharedPointer<Kactus2Group> positionGroup, QSharedPointer<Port> adhocPort) const
{
    foreach (QSharedPointer<VendorExtension> extension, positionGroup->getByType("kactus2:adHocVisible"))
    {
        QSharedPointer<Kactus2Placeholder> adhocPosition = extension.dynamicCast<Kactus2Placeholder>();
        if (adhocPosition && adhocPosition->getAttributeValue(QString("portName")) == adhocPort->name())
        {
            return adhocPosition;
        }
    }

    QSharedPointer<Kactus2Placeholder> newAdHocPosition(new Kactus2Placeholder("kactus2:adHocVisible"));
    return newAdHocPosition;
}

//-----------------------------------------------------------------------------
// Function: HWColumnAddCommand::~HWColumnAddCommand()
//-----------------------------------------------------------------------------
HWColumnAddCommand::~HWColumnAddCommand()
{
    if (columnShouldBeDeleted())
    {
        while (!missingAdHocPortItems_.isEmpty())
        {
            AdHocInterfaceItem* firstAdHoc = missingAdHocPortItems_.takeFirst();
            delete firstAdHoc;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: HWColumnAddCommand::undo()
//-----------------------------------------------------------------------------
void HWColumnAddCommand::undo()
{
    if (getColumn()->getColumnDesc()->getAllowedItems() & ColumnTypes::INTERFACE &&
        !missingAdHocPortItems_.isEmpty())
    {
        removeMissingPortsFromDesign();
    }

    GraphicsColumnAddCommand::undo();
}

//-----------------------------------------------------------------------------
// Function: HWColumnAddCommand::removeMissingPortsFromDesign()
//-----------------------------------------------------------------------------
void HWColumnAddCommand::removeMissingPortsFromDesign()
{
    QSharedPointer<VendorExtension> adhocExtension = getDiagram()->getDesign()->getAdHocPortPositions();
    QSharedPointer<Kactus2Group> adhocGroup = adhocExtension.dynamicCast<Kactus2Group>();
    
    foreach (AdHocInterfaceItem* adHocItem, missingAdHocPortItems_)
    {
        getColumn()->removeItem(adHocItem);

        foreach(QSharedPointer<VendorExtension> extension, adhocGroup->getByType("kactus2:adHocVisible"))
        {
            QSharedPointer<Kactus2Placeholder> portExtension = extension.dynamicCast<Kactus2Placeholder>();
            if (portExtension->getAttributeValue("portName") == adHocItem->name())
            {
                adhocGroup->removeFromGroup(portExtension);
            }
        }
    }
    
    if (adhocGroup->getByType("kactus2:adHocVisible").isEmpty())
    {
        getDiagram()->getDesign()->getVendorExtensions()->removeAll(adhocExtension);
    }
}

//-----------------------------------------------------------------------------
// Function: HWColumnAddCommand::redo()
//-----------------------------------------------------------------------------
void HWColumnAddCommand::redo()
{
    if (getColumn()->getColumnDesc()->getAllowedItems() & ColumnTypes::INTERFACE &&
        !missingAdHocPortItems_.isEmpty())
    {
        addMissingPortsToNewColumn();
    }

    GraphicsColumnAddCommand::redo();
}

//-----------------------------------------------------------------------------
// Function: HWColumnAddCommand::addMissingPortsToNewColumn()
//-----------------------------------------------------------------------------
void HWColumnAddCommand::addMissingPortsToNewColumn()
{
    QSharedPointer<VendorExtension> adHocPortPositions = getDiagram()->getDesign()->getAdHocPortPositions();
    QSharedPointer<Kactus2Group> adhocGroup = adHocPortPositions.dynamicCast<Kactus2Group>();
    if (!adhocGroup)
    {
        adhocGroup = QSharedPointer<Kactus2Group>(new Kactus2Group("kactus2:adHocVisibilities"));
        getDiagram()->getDesign()->getVendorExtensions()->append(adhocGroup);
    }

    foreach (AdHocInterfaceItem* adhocItem, missingAdHocPortItems_)
    {
        getColumn()->addItem(adhocItem);

        QSharedPointer<Kactus2Placeholder> adhocPositionData = adhocItem->getDataGroup();
        adhocPositionData->setAttribute("x", QString::number(adhocItem->scenePos().x()));
        adhocPositionData->setAttribute("y", QString::number(adhocItem->scenePos().y()));
        adhocItem->setDataGroup(adhocPositionData);

        adhocGroup->addToGroup(adhocPositionData);
    }
}

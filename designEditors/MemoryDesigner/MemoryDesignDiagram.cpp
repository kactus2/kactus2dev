//-----------------------------------------------------------------------------
// File: MemoryDesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 14.9.2012
//
// Description:
// Implements the memory design diagram class.
//-----------------------------------------------------------------------------

#include "MemoryDesignDiagram.h"

#include "MemoryDesignWidget.h"
#include "MemoryItem.h"
#include "AddressSpaceItem.h"
#include "AddressSectionItem.h"
#include "MemoryColumn.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QMimeData>
#include <QMessageBox>
#include <QFileDialog>

#include <library/LibraryManager/libraryinterface.h>

#include <common/GenericEditProvider.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsColumnUndoCommands.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumn.h>

#include <designEditors/HWDesign/columnview/ColumnEditDialog.h>
#include <designEditors/common/diagramgrid.h>
#include <designEditors/common/DiagramUtil.h>
#include <designEditors/common/StickyNote/StickyNote.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/designconfiguration.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/model.h>
#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/addressspace.h>
#include <IPXACTmodels/businterface.h>
#include <IPXACTmodels/masterinterface.h>
#include <IPXACTmodels/slaveinterface.h>
#include <IPXACTmodels/channel.h>
#include <IPXACTmodels/mirroredslaveinterface.h>
#include <IPXACTmodels/Interconnection.h>
#include <IPXACTmodels/Interface.h>

#include <mainwindow/mainwindow.h>

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram()
//-----------------------------------------------------------------------------
MemoryDesignDiagram::MemoryDesignDiagram(LibraryInterface* lh, MainWindow* mainWnd,
                                         GenericEditProvider& editProvider,
                                         MemoryDesignWidget* parent)
    : DesignDiagram(lh, mainWnd, editProvider, parent),
      parent_(parent),
      layout_(),
      oldSelection_(0),
      resizingSubsection_(false),
      dualSubsectionResize_(false),
      subsectionResizeBottom_(0.0)
{
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(&editProvider, SIGNAL(modified()), this, SIGNAL(contentChanged()));
}

//-----------------------------------------------------------------------------
// Function: ~MemoryDesignDiagram()
//-----------------------------------------------------------------------------
MemoryDesignDiagram::~MemoryDesignDiagram()
{
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::clearScene()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::clearScene()
{
    layout_.clear();
    DesignDiagram::clearScene();
}

//-----------------------------------------------------------------------------
// Function: setDesign()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::loadDesign(QSharedPointer<Design> design)
{
    // Create the column layout.
    layout_ = QSharedPointer<GraphicsColumnLayout>(new GraphicsColumnLayout(this));
    layout_->setAutoReorganized(true);
    layout_->setAutoCreateColumnFunction(&MemoryDesignDiagram::createDefaultColumn);

    if (!design->getColumns().isEmpty())
    {
        QList<ColumnDesc> columns;
        columns.append(ColumnDesc("Available Memory", COLUMN_CONTENT_BUSES, 0, COLUMN_WIDTH));
        columns.append(ColumnDesc("Required Address Spaces", COLUMN_CONTENT_COMPONENTS, 0, COLUMN_WIDTH));
        design->setColumns(columns);
    }

    foreach(ColumnDesc const& desc, design->getColumns())
    {
        GraphicsColumn* column = new MemoryColumn(desc, layout_.data());
        layout_->addColumn(column, true);
    }

    // Create (HW) component instances.
    foreach (ComponentInstance const& instance, design->getComponentInstances())
    {
        QSharedPointer<LibraryComponent> libComponent = getLibraryInterface()->getModel(instance.getComponentRef());
        QSharedPointer<Component> component = libComponent.staticCast<Component>();

        if (!component)
        {
            emit errorMessage(tr("The component '%1' instantiated in the design '%2' "
                "was not found in the library").arg(
                instance.getComponentRef().getName()).arg(design->getVlnv()->getName()));

            // Create an unpackaged component so that we can still visualize the component instance.
            component = QSharedPointer<Component>(new Component(instance.getComponentRef()));
            component->setComponentImplementation(KactusAttribute::KTS_HW);
        }

        foreach (QSharedPointer<MemoryMap> map, component->getMemoryMaps())
        {
            MemoryItem* item = new MemoryItem(getLibraryInterface(), instance.getInstanceName(),
                                              component, map, 0);
            layout_->addItem(item);
        }

        foreach (QSharedPointer<AddressSpace> addressSpace, component->getAddressSpaces())
        {
            AddressSpaceItem* item = new AddressSpaceItem(getLibraryInterface(), instance.getInstanceName(),
                                                          component, addressSpace, 0);
            layout_->getColumns().at(1)->addItem(item);
        }
    }

    // Refresh the layout so that all components are placed in correct positions according to the stacking.
    layout_->updatePositions();

    design_ = design;
}

//-----------------------------------------------------------------------------
// Function: mousePressEvent()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    // If other than left button was pressed return back to select mode.
    if (event->button() != Qt::LeftButton)
    {
        setMode(MODE_SELECT);
        return;
    }

    if (getMode() == MODE_DRAFT)
    {
    }
    else if (getMode() == MODE_SELECT)
    {
        // Handle the mouse press.
        QGraphicsScene::mousePressEvent(event);
    }
    else if(getMode() == MODE_LABEL)
    {
        if (!isProtected())
        {
            createNote(event->scenePos());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: onSelected()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::onSelected(QGraphicsItem* newSelection)
{
    // Activate the correct views when something has been selected.
    if (newSelection)
    { 
        // Check if the selected item was a component.
        if (dynamic_cast<ComponentItem*>(newSelection) != 0)
        {
            ComponentItem* item = static_cast<ComponentItem*>(newSelection);
            emit componentSelected(item);
        }
        else
        {
            // Otherwise inform others that nothing is currently selected.
            emit clearItemSelection();
            emit helpUrlRequested("memorydesign/memorydesign.html");
        }
    }
    else
    {
        // Clear the selection.
        emit clearItemSelection();
        emit helpUrlRequested("memorydesign/memorydesign.html");
    }
}

//-----------------------------------------------------------------------------
// Function: mouseMoveEvent()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

//-----------------------------------------------------------------------------
// Function: mouseReleaseEvent()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    // Process the normal mouse release event.
    QGraphicsScene::mouseReleaseEvent(event);
}

//-----------------------------------------------------------------------------
// Function: onVerticalScroll()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::onVerticalScroll(qreal y)
{
    layout_->setOffsetY(y);
}

//-----------------------------------------------------------------------------
// Function: wheelEvent()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (event->modifiers() == Qt::CTRL)
    {
        MemoryDesignWidget* doc = static_cast<MemoryDesignWidget*>(parent());
        QGraphicsView* view = doc->getView();

        // Retrieve the center point in scene coordinates.
        //QPointF origCenterPos = view->mapToScene(view->rect().center());
        QPointF centerPos = itemsBoundingRect().center();

        // Set the zoom level and center the view.
        doc->setZoomLevel(doc->getZoomLevel() + event->delta() / 12);

        view->centerOn(centerPos);
        event->accept();
    }
}

//-----------------------------------------------------------------------------
// Function: createDesign()
//-----------------------------------------------------------------------------
QSharedPointer<Design> MemoryDesignDiagram::createDesign(VLNV const& vlnv) const
{
    QSharedPointer<Design> design(new Design(vlnv));
    // TODO:
    return design;
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::addColumn(ColumnDesc const& desc)
{
    GraphicsColumn* column = new MemoryColumn(desc, layout_.data());

    QSharedPointer<QUndoCommand> cmd(new GraphicsColumnAddCommand(layout_.data(), column));
    getEditProvider().addCommand(cmd);
    cmd->redo();
}

//-----------------------------------------------------------------------------
// Function: mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
// void MemoryDesignDiagram::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
// {
//     // Allow double click only when the mode is select.
//     if (getMode() != MODE_SELECT)
//     {
//         return;
//     }
// 
//     // Find the item under the cursor.
//     QGraphicsItem *item = itemAt(snapPointToGrid(event->scenePos()));
// 
//     if (item == 0)
//     {
//         return;
//     }
// 
// //     if (dynamic_cast<SystemComponentItem*>(item) != 0)
// //     {
// //     }
// //     else
//     if (item->type() == GraphicsColumn::Type)
//     {
//         if (!isProtected())
//         {
//             item->setSelected(true);
//             GraphicsColumn* column = qgraphicsitem_cast<GraphicsColumn*>(item);
// 
//             // Columns don't have adjustable content settings.
//             ColumnEditDialog dialog((QWidget*)parent(), false, column);
//             dialog.hideContentSettings();
//             
//             if (dialog.exec() == QDialog::Accepted)
//             {
//                 ColumnDesc desc(dialog.getName(), dialog.getContentType(), dialog.getAllowedItems(),
//                                 column->getColumnDesc().getWidth());
// 
//                 QSharedPointer<QUndoCommand> cmd(new GraphicsColumnChangeCommand(column, desc));
//                 getEditProvider().addCommand(cmd);
//             }
//         }
//     }
// }

//-----------------------------------------------------------------------------
// Function: getColumnLayout()
//-----------------------------------------------------------------------------
GraphicsColumnLayout* MemoryDesignDiagram::getColumnLayout()
{
    return layout_.data();
}

//-----------------------------------------------------------------------------
// Function: parent()
//-----------------------------------------------------------------------------
MemoryDesignWidget* MemoryDesignDiagram::parent() const
{
    return parent_;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::updateHierComponent()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::updateHierComponent()
{
}

//-----------------------------------------------------------------------------
// Function: onSelectionChanged()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::onSelectionChanged()
{
    // Retrieve the new selection.
    QGraphicsItem* newSelection = 0;

    if (!selectedItems().isEmpty())
    {
        newSelection = selectedItems().front();
    }

    onSelected(newSelection);

    // Save the current selection as the old selection.
    oldSelection_ = newSelection;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::beginResize()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::beginResizeSubsection(bool dual, qreal bottom)
{
    dualSubsectionResize_ = dual;
    subsectionResizeBottom_ = bottom;
    resizingSubsection_ = true;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::updateResize()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::updateResizeSubsection(qreal bottom)
{
    subsectionResizeBottom_ = bottom;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::endResize()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::endResizeSubsection()
{
    resizingSubsection_ = false;
    invalidate();
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::drawForeground()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::drawForeground(QPainter* painter, const QRectF& rect)
{
    painter->setWorldMatrixEnabled(true);

    if (resizingSubsection_)
    {
        QPen pen(Qt::black, 0);
        pen.setStyle(Qt::DashLine);
        painter->setPen(pen);

        painter->drawLine(rect.left(), subsectionResizeBottom_, rect.right(), subsectionResizeBottom_);

        if (dualSubsectionResize_)
        {
            painter->drawLine(rect.left(), subsectionResizeBottom_ + 10, rect.right(), subsectionResizeBottom_ + 10);
        }
    }

    drawMemoryDividers(painter, rect);

    // Draw guides for all selected items.
    foreach (QGraphicsItem* item, selectedItems())
    {
        MemoryBaseItem* baseItem = dynamic_cast<MemoryBaseItem*>(item);

        if (baseItem != 0)
        {
            baseItem->drawGuides(painter, rect);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::createDefaultColumn()
//-----------------------------------------------------------------------------
GraphicsColumn* MemoryDesignDiagram::createDefaultColumn(GraphicsColumnLayout* layout)
{
    ColumnDesc desc("Required Address Spaces", COLUMN_CONTENT_COMPONENTS, 0, COLUMN_WIDTH);
    return new MemoryColumn(desc, layout);
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::drawMemoryDividers()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::drawMemoryDividers(QPainter* painter, QRectF const& rect)
{
    // Draw section dividers to the address spaces based on the memory map positions.
    MemoryColumn* memoryColumn = 0;

    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        MemoryColumn* memColumn = static_cast<MemoryColumn*>(column);

        if (memColumn->getContentType() == COLUMN_CONTENT_BUSES)
        {
            memoryColumn = memColumn;
            break;
        }
    }

    foreach (QGraphicsItem* item, memoryColumn->getItems())
    {
        MemoryItem* memItem = static_cast<MemoryItem*>(item);

        foreach (AddressSectionItem* block, memItem->getSections())
        {
            int top = block->sceneBoundingRect().top();
            int bottom = block->sceneBoundingRect().bottom();
                
            if (top >= rect.top() && top < rect.bottom())
            {
                foreach (GraphicsColumn* column, layout_->getColumns())
                {
                    MemoryBaseItem* addrSpaceItem = static_cast<MemoryColumn*>(column)->findItemAt(top);
                    
                    if (addrSpaceItem != 0)
                    {
                        quint64 address = addrSpaceItem->convertAddress(block->getStartAddress(), memItem);
                        addrSpaceItem->drawStartAddressDivider(painter, rect, top, address);
                    }
                }
            }

            if (bottom >= rect.top() && bottom < rect.bottom())
            {
                foreach (GraphicsColumn* column, layout_->getColumns())
                {
                    MemoryBaseItem* addrSpaceItem = static_cast<MemoryColumn*>(column)->findItemAt(bottom);

                    if (addrSpaceItem != 0)
                    {
                        quint64 address = addrSpaceItem->convertAddress(block->getEndAddress(), memItem);
                        addrSpaceItem->drawEndAddressDivider(painter, rect, bottom, address);
                    }
                }
            }
        }
    }

    foreach (GraphicsColumn* column, layout_->getColumns())
    {
        // Check if this is an address space column.
        if (column->getContentType() == COLUMN_CONTENT_COMPONENTS)
        {
            MemoryColumn* memColumn = static_cast<MemoryColumn*>(column);

            foreach (QGraphicsItem* item, memColumn->getItems())
            {
                AddressSpaceItem* addrSpaceItem = static_cast<AddressSpaceItem*>(item);

                foreach (AddressSectionItem* segment, addrSpaceItem->getSections())
                {
                    int top = segment->sceneBoundingRect().top();
                    int bottom = segment->sceneBoundingRect().bottom();

                    if (top >= rect.top() && top < rect.bottom())
                    {
                        MemoryBaseItem* memItem = memoryColumn->findItemAt(top);

                        if (memItem != 0)
                        {
                            quint64 address = memItem->convertAddress(segment->getStartAddress(), addrSpaceItem);
                            memItem->drawStartAddressDivider(painter, rect, top, address);
                        }
                    }

                    if (bottom >= rect.top() && bottom < rect.bottom())
                    {
                        MemoryBaseItem* memItem = memoryColumn->findItemAt(bottom);

                        if (memItem != 0)
                        {
                            quint64 address = memItem->convertAddress(segment->getEndAddress(), addrSpaceItem);
                            memItem->drawEndAddressDivider(painter, rect, bottom, address);
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::isConnected()
//-----------------------------------------------------------------------------
bool MemoryDesignDiagram::isConnected(AddressSpaceItem const* addrSpaceItem, MemoryItem const* memoryItem,
                                      quint64* addressOffset) const
{
    // Find the route from the component containing the given address space to a component
    // containing the given memory map.
    foreach (QSharedPointer<BusInterface> busIf, addrSpaceItem->getComponent()->getBusInterfaces())
    {
        // Check if the bus interface has the correct address space as a reference.
        if (busIf->getInterfaceMode() == General::MASTER &&
            busIf->getMaster()->getAddressSpaceRef() == addrSpaceItem->getAddressSpace()->getName())
        {
            quint64 addressOffsetTemp = 0;

            if (findRoute(addrSpaceItem->getInstanceName(), busIf, memoryItem, addressOffsetTemp))
            {
                *addressOffset = addressOffsetTemp;
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::findRoute()
//-----------------------------------------------------------------------------
bool MemoryDesignDiagram::findRoute(QString const& instanceName, QSharedPointer<BusInterface const> busIf,
                                    MemoryItem const* memoryItem, quint64& addressOffset) const
{
    // Check all connections that start from the bus interface.
    foreach (Interconnection const& conn, design_->getInterconnections())
    {
        Interface const* interface = 0;

        QPair<Interface, Interface> interfaces = conn.getInterfaces();
        if (interfaces.first.references(instanceName, busIf->getName()) )
        {
            interface = &interfaces.second;
        }

        if (interfaces.second.references(instanceName, busIf->getName()))
        {
            interface = &interfaces.first;
        }

        // Check if we found a matching connection.
        if (interface != 0)
        {
            // Retrieve the component referenced by the connection.
            QString connectionComponentInstance = interface->getComponentRef();
            QSharedPointer<Component const> component = getComponentByInstanceName(connectionComponentInstance);
            
            if (component != 0)
            {
                // Retrieve the correct bus interface.
                QString connectionInterface = interface->getBusRef();
                QSharedPointer<BusInterface const> otherBusIf = component->getBusInterface(connectionInterface);

                if (otherBusIf != 0)
                {
                    // Master can connect to either slave or mirrored master.
                    if (busIf->getInterfaceMode() == General::MASTER)
                    {
                        // If the other bus interface is slave, we either have a connection to the correct
                        // component or then this was a wrong route.
                        if (otherBusIf->getInterfaceMode() == General::SLAVE &&
                            connectionComponentInstance == memoryItem->getInstanceName())
                        {
                            addressOffset = Utils::str2Uint(busIf->getMaster()->getBaseAddress());
                            return true;
                        }
                        // With mirrored master, the route continues through a channel.
                        else if (otherBusIf->getInterfaceMode() == General::MIRROREDMASTER)
                        {
                            foreach (QSharedPointer<Channel> channel, component->getChannels())
                            {
                                // Check if the channel contains the bus interface in question.
                                if (channel->getInterfaces().contains(connectionInterface))
                                {
                                    foreach (QString const& chIfName, channel->getInterfaces())
                                    {
                                        if (chIfName != connectionInterface)
                                        {
                                            // Retrieve the bus interface.
                                            QSharedPointer<BusInterface const> nextBusIf = component->getBusInterface(chIfName);

                                            if (nextBusIf != 0)
                                            {
                                                quint64 addressOffsetTemp = 0;

                                                if (findRoute(connectionComponentInstance, nextBusIf, memoryItem,
                                                              addressOffsetTemp))
                                                {
                                                    addressOffset = addressOffsetTemp + Utils::str2Uint(busIf->getMaster()->getBaseAddress());
                                                    return true;
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            return false;
                        }
                    }
                    // Mirrored slave can only connect to a slave.
                    else if (busIf->getInterfaceMode() == General::MIRROREDSLAVE &&
                             otherBusIf->getInterfaceMode() == General::SLAVE)
                    {
                        // Check if we ended up in the correct component.
                        if (connectionComponentInstance == memoryItem->getInstanceName())
                        {
                            addressOffset = Utils::str2Uint(busIf->getMirroredSlave()->getRemapAddress());
                            return true;
                        }

                        // Otherwise check if the route continues through bridges.
                        foreach (QSharedPointer<SlaveInterface::Bridge const> bridge, otherBusIf->getSlave()->getBridges())
                        {
                            QSharedPointer<BusInterface const> nextBusIf = component->getBusInterface(bridge->masterRef_);

                            if (nextBusIf != 0)
                            {
                                quint64 addressOffsetTemp = 0;

                                if (findRoute(connectionComponentInstance, nextBusIf, memoryItem,
                                    addressOffsetTemp))
                                {
                                    addressOffset = addressOffsetTemp + Utils::str2Uint(busIf->getMaster()->getBaseAddress());
                                    return true;
                                }
                            }
                        }
                    }

                    break;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::getComponentByInstanceName()
//-----------------------------------------------------------------------------
QSharedPointer<Component const> MemoryDesignDiagram::getComponentByInstanceName(QString const& componentRef) const
{
    foreach (ComponentInstance const& instance, design_->getComponentInstances())
    {
        if (instance.getInstanceName() == componentRef)
        {
            QSharedPointer<LibraryComponent const> libComp =
                getLibraryInterface()->getModelReadOnly(instance.getComponentRef());
            return libComp.dynamicCast<Component const>();
        }
    }

    return QSharedPointer<Component const>();
}

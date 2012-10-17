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
#include "MemoryColumn.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QMimeData>
#include <QMessageBox>
#include <QFileDialog>

#include <LibraryManager/libraryinterface.h>

#include <designwidget/columnview/ColumnEditDialog.h>

#include <common/diagramgrid.h>
#include <common/DiagramUtil.h>
#include <common/GenericEditProvider.h>
#include <common/dialogs/newObjectDialog/newobjectdialog.h>
#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/GraphicsColumnUndoCommands.h>
#include <common/graphicsItems/GraphicsConnection.h>
#include <common/graphicsItems/GraphicsColumn.h>

#include <models/component.h>
#include <models/designconfiguration.h>
#include <models/design.h>
#include <models/model.h>
#include <models/memorymap.h>
#include <models/addressspace.h>

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
        GraphicsColumn* column = new MemoryColumn(desc, layout_.data(), this);
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
            MemoryItem* item = new MemoryItem(getLibraryInterface(), component, map, 0);
            layout_->addItem(item);
        }

        foreach (QSharedPointer<AddressSpace> addressSpace, component->getAddressSpaces())
        {
            AddressSpaceItem* item = new AddressSpaceItem(getLibraryInterface(), component, addressSpace, 0);
            layout_->getColumns().at(1)->addItem(item);
        }
    }

    // Refresh the layout so that all components are placed in correct positions according to the stacking.
    layout_->updatePositions();
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

//    QList<ColumnDesc> columns;

    foreach (QGraphicsItem const* item, items())
    {
    }

//     foreach (GraphicsColumn* column, layout_->getColumns())
//     {
//         columns.append(column->getColumnDesc());
//     }
// 
//     design->setColumns(columns);   

    return design;
}

//-----------------------------------------------------------------------------
// Function: addColumn()
//-----------------------------------------------------------------------------
void MemoryDesignDiagram::addColumn(ColumnDesc const& desc)
{
    GraphicsColumn* column = new MemoryColumn(desc, layout_.data(), this);

    QSharedPointer<QUndoCommand> cmd(new GraphicsColumnAddCommand(layout_.data(), column));
    getEditProvider().addCommand(cmd);
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
}

//-----------------------------------------------------------------------------
// Function: MemoryDesignDiagram::createDefaultColumn()
//-----------------------------------------------------------------------------
GraphicsColumn* MemoryDesignDiagram::createDefaultColumn(GraphicsColumnLayout* layout, QGraphicsScene* scene)
{
    ColumnDesc desc("Required Address Spaces", COLUMN_CONTENT_COMPONENTS, 0, COLUMN_WIDTH);
    return new MemoryColumn(desc, layout, scene);
}

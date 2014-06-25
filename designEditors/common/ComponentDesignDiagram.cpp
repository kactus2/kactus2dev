//-----------------------------------------------------------------------------
// File: ComponentDesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.6.2014
//
// Description:
// Base class for all component designs.
//-----------------------------------------------------------------------------

#include "ComponentDesignDiagram.h"

#include <common/GenericEditProvider.h>

#include <common/graphicsItems/ComponentItem.h>
#include <common/graphicsItems/ConnectionEndpoint.h>
#include <common/graphicsItems/ConnectionUndoCommands.h>
#include <common/graphicsItems/CommonGraphicsUndoCommands.h>
#include <common/graphicsItems/GraphicsColumn.h>
#include <common/graphicsItems/GraphicsColumnLayout.h>
#include <common/graphicsItems/GraphicsConnection.h>

#include <designEditors/common/DiagramUtil.h>
#include <designEditors/common/DesignWidget.h>

#include <library/LibraryManager/libraryhandler.h>

#include <QGraphicsSceneMouseEvent>
#include <QString>
#include <QMessageBox>
#include <QCoreApplication>
#include <QApplication>
#include <QMenu>

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::ComponentDesignDiagram()
//-----------------------------------------------------------------------------
ComponentDesignDiagram::ComponentDesignDiagram(LibraryInterface* lh, GenericEditProvider& editProvider, DesignWidget* parent)
    : DesignDiagram(lh, editProvider, parent),
      tempConnection_(0),
      connectionStartPoint_(0), 
      tempPotentialEndingEndPoints_(),
      connectionEndPoint_(0),
      sourceComp_(0),
      previouslySelectedItems_(),
      selectAllAction_(tr("Select All"), this),
      copyAction_(tr("Copy"), this),
      pasteAction_(tr("Paste"), this),
      addAction_(tr("Add to Library"), this),
      openComponentAction_(tr("Open Component"), this),
      openDesignMenu_(tr("Open Design")),
      clickedPosition_()
{
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(&editProvider, SIGNAL(modified()), this, SIGNAL(contentChanged()));

	setupActions();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::~ComponentDesignDiagram()
//-----------------------------------------------------------------------------
ComponentDesignDiagram::~ComponentDesignDiagram()
{
    clearSelection();
    destroyConnections();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::clearScene()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::clearScene()
{
    clearSelection();
    destroyConnections();
    DesignDiagram::clearScene();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::setMode()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::setMode(DrawMode mode)
{
    if (getMode() != mode)
    {
        if (getMode() == MODE_CONNECT)
        {
            endConnect();
        }

        if (mode == MODE_SELECT)
        {
            hideOffPageConnections();
        }
    }

    DesignDiagram::setMode(mode);
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::singleSelection()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::singleSelection() const
{
    return selectedItems().count() == 1;
}

//-----------------------------------------------------------------------------
// Function: onSelectionChanged()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::onSelectionChanged()
{   
    ensureOneTypeSelected(previouslySelectedItems_);
    raiseSelectedConnectionToFront();

    // Retrieve the new selection.
    QList<QGraphicsItem*> nowSelectedItems = selectedItems();
    QGraphicsItem* selectedItem = 0;

    if (!nowSelectedItems.isEmpty())
    {
        selectedItem = nowSelectedItems.first();
    }

    // If the old selection was an off-page connector, hide its connections.
    // Also hide the previously selected connection if it was an off-page connection.
    foreach (QGraphicsItem* oldSelection, previouslySelectedItems_)
    {
        if (oldSelection->type() == offpageConnectorType())
        {
            ConnectionEndpoint* connector = dynamic_cast<ConnectionEndpoint*>(oldSelection);

            foreach (GraphicsConnection* connection, connector->getConnections())
            {
                if (connection != selectedItem)
                {
                    connection->hide();
                }
            }
        }
        else if (oldSelection->type() == connectionType() && !nowSelectedItems.contains(oldSelection))
        {
            GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(oldSelection);

            if (conn->endpoint1() != 0)
            {
                if (conn->endpoint1()->type() == offpageConnectorType())
                {
                    oldSelection->hide();
                }
                else
                {
                    oldSelection->setZValue(-1000);
                }
            }
        }
    }

    // If the new selection is an off-page connector, show its connections.
    if (selectedItem != 0 && selectedItem->type() == offpageConnectorType())
    {
        ConnectionEndpoint* connector = dynamic_cast<ConnectionEndpoint*>(selectedItem);

        foreach (GraphicsConnection* connection, connector->getConnections())
        {
            connection->show();
        }
    }

    // Save the current selection as the old selection.
    previouslySelectedItems_ = nowSelectedItems;
    
    onSelected(selectedItem);
    prepareContextMenuActions();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::selectionToFront()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::raiseSelectedConnectionToFront()
{
    if (selectedItems().isEmpty())
        return;

    QGraphicsItem* selectedItem = selectedItems().first();
    if (selectedItem->type() == connectionType())
    {
        selectedItem->setZValue(-900);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::onOpenComponentAction()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::onOpenComponentAction()
{
    if (singleSelection())
    {
        ComponentItem* component = dynamic_cast<ComponentItem *>(selectedItems().first());
        if (component)
        {
            openInComponentEditor(component);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::onOpenDesignAction()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::onOpenDesignAction(QAction* selectedAction)
{
    if (singleSelection())
    {
        ComponentItem* component = dynamic_cast<ComponentItem *>(selectedItems().first());
        if (component)
        {
            QString viewName = selectedAction->data().toString();

            openDesignForComponent(component, viewName);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::mousePressEvent()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // if other than left button was pressed return the mode back to select
	if (mouseEvent->button() != Qt::LeftButton)
    {
        endInteraction();
        if (getMode() == MODE_SELECT && mouseEvent->button() == Qt::RightButton)
        {
            setInteractionMode(CONTEXT_MENU);
        }

        setMode(MODE_SELECT);		
        return;
	}

    if (getMode() == MODE_SELECT)
    {
        // Check if the user pressed Alt over a component switch to replace component mode.
        if (!isProtected() && mouseEvent->modifiers() & Qt::AltModifier)
        {
            beginComponentReplaceDrag(mouseEvent->scenePos());
        }
        else
        {
            // Handle the mouse press and bring the new selection to front.
            QGraphicsScene::mousePressEvent(mouseEvent);
        }
    }
    else if (getMode() == MODE_CONNECT && !isProtected())
    {
        if (mouseEvent->modifiers() & Qt::ShiftModifier)
        {
            setInteractionMode(OFFPAGE);
        }
        else
        {
            setInteractionMode(NORMAL);
        }
               
        connectAt(mouseEvent->scenePos());
    }
    else if (getMode() == MODE_INTERFACE && !isProtected())
    {
        addInterfaceAt(mouseEvent->scenePos());
    }
    else if (getMode() == MODE_DRAFT && !isProtected())
    {
        draftAt(mouseEvent->scenePos());
    }
    else if (getMode() == MODE_TOGGLE_OFFPAGE && !isProtected())
    {
        toggleOffPageAt(mouseEvent->scenePos());
    }
    else if(getMode() == MODE_LABEL && !isProtected())
    {
         createNoteAt(mouseEvent->scenePos());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::mouseMoveEvent()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent)
{
    clickedPosition_ = QCursor::pos();

    // Check if the connect mode is active.
    if (getMode() == MODE_CONNECT || getMode() == MODE_TOGGLE_OFFPAGE)
    {
        updateConnectionDisplay(mouseEvent->scenePos());
    }
    else if (inReplaceMode())
    {
        updateComponentReplaceDragCursor(mouseEvent->scenePos());
    }
    else if (inAssociationMode())
    {
        updateAssociationLineDraw(mouseEvent->scenePos());
    }
    // Allow moving items only when a single item is selected.
    else if (singleSelection())
    {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::mouseReleaseEvent()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    // Check if we're replacing a component.
    if (inReplaceMode())
    {
        endComponentReplaceDrag(mouseEvent->scenePos());        
    }
    
    else if (canEndAssociation())
    {
        endAssociation(mouseEvent->scenePos());
        return;
    }

    // process the normal mouse release event
	QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
// Function: keyReleaseEvent()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::keyReleaseEvent(QKeyEvent *event)
{
    // Check if the user ended the off-page connection mode.
    if ((event->key() == Qt::Key_Shift) && inOffPageMode())
    {
        if (hasConnectionStartingPoint() && 
            connectionStartPoint_->getConnections().size() == 0 && 
            connectionStartPoint_->type() == offpageConnectorType())
        {
            connectionStartPoint_->hide();
        }

        endConnect();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::endConnect()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::endConnect()
{
    // Discard the connection that was being drawn.
    discardConnection();

    // Disable highlights from all end points.
    disableHighlights();

    // Disable off-page mode.
    setInteractionMode(NORMAL);
}

//-----------------------------------------------------------------------------
// Function: dragMoveEvent()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::dragMoveEvent(QGraphicsSceneDragDropEvent * event)
{
    updateDropAction(event);
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::createContextMenu()
//-----------------------------------------------------------------------------
QMenu* ComponentDesignDiagram::createContextMenu(QPointF const& pos)
{
    QMenu* menu = 0;

    if (contextMenuEnabled())
    {
        QGraphicsItem* item = getBaseItemOf(itemAt(pos, QTransform()));

        if (!selectedItems().contains(item))
        {
            clearSelection();
            
            if (item)
            {
                item->setSelected(true);
            }
        }
        
        prepareContextMenuActions();

        menu = new QMenu(getParent());
        menu->addAction(&selectAllAction_);
        menu->addSeparator();
        menu->addAction(&addAction_);
        menu->addAction(&openComponentAction_);
        menu->addMenu(&openDesignMenu_);
        menu->addSeparator();
        menu->addAction(&copyAction_);
        menu->addAction(&pasteAction_);

        ComponentItem* compItem = dynamic_cast<ComponentItem*>(item);
        if (compItem)
        {
            updateOpenDesignMenuFor(compItem);
        }
    }

    return menu;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::updateOpenDesignMenuFor()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::updateOpenDesignMenuFor(ComponentItem* compItem)
{
    QString activeViewName = getActiveViewOf(compItem);
    QStringList views = hierarchicalViewsOf(compItem);

    foreach(QString viewName, views)
    {
        QString actionText = viewName;
        if (viewName == activeViewName)
        {
            actionText.append(" ");
            actionText.append(tr("(active)"));
        }
        QAction* action = openDesignMenu_.addAction(actionText);
        action->setData(viewName);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::prepareContextMenuActions()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::prepareContextMenuActions()
{
    addAction_.setEnabled(addToLibraryActionEnabled());
    openComponentAction_.setEnabled(openComponentActionEnabled());

    openDesignMenu_.clear();    
    openDesignMenu_.setEnabled(openDesignEnabled());

    copyAction_.setEnabled(copyActionEnabled());
    pasteAction_.setEnabled(pasteActionEnabled());
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::addToLibraryActionEnabled()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::addToLibraryActionEnabled() const
{
    return !isProtected() && singleSelection() && draftSelected();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::openDesignEnabled()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::openDesignEnabled() const
{
    return singleSelection() && !draftSelected() && isHierarchicalComponent(selectedItems().first());
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::draftSelected()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::draftSelected() const
{
    return !selectedItems().empty() && 
        selectedItems().first()->type() == componentType() &&
        !dynamic_cast<ComponentItem*>(selectedItems().first())->componentModel()->getVlnv()->isValid();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::openComponentItem()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::openComponentItem(ComponentItem* comp)
{
    QStringList hierViews = hierarchicalViewsOf(comp);

    // if configuration is used and it contains an active view for the instance
 
    if (!getActiveViewOf(comp).isEmpty()) 
    {
        openComponentByActiveView(comp);
    }
    // If the component does not contain any hierarchical views, open the component editor.
    else if (hierViews.size() == 0)
    {
        if (comp->componentModel()->hasViews())
        {
            emit noticeMessage(tr("No active view was selected for instance %1, "
                "opening component editor.").arg(comp->name()));
        }

        openInComponentEditor(comp);
    }
    // Open the first design if there is one or multiple hierarchical view.
    else
    {
        emit noticeMessage(tr("No active view was selected for instance %1, "
            "opening the only hierarchical view of the component.").arg(comp->name()));

        openDesignForComponent(comp, hierViews.first());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::openInComponentEditor()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::openInComponentEditor(ComponentItem* comp)
{
    emit openComponent(*comp->componentModel()->getVlnv());
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::getActiveViewOf()
//-----------------------------------------------------------------------------
QString ComponentDesignDiagram::getActiveViewOf(ComponentItem* compItem) const
{
    QString activeViewName;

    QSharedPointer<DesignConfiguration> designConf = getDesignConfiguration();
    if (designConf && designConf->hasActiveView(compItem->name())) 
    {
        activeViewName = designConf->getActiveView(compItem->name());
    }

    return activeViewName;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::activeCursorPosition()
//-----------------------------------------------------------------------------
QPoint ComponentDesignDiagram::contextMenuPosition() const
{
    return getParent()->mapFromGlobal(clickedPosition_);
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::setupActions()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::setupActions()
{
    getParent()->addAction(&selectAllAction_);
    selectAllAction_.setShortcut(QKeySequence::SelectAll);
    connect(&selectAllAction_, SIGNAL(triggered()),this, SLOT(selectAll()), Qt::UniqueConnection);

    getParent()->addAction(&copyAction_);
    copyAction_.setShortcut(QKeySequence::Copy);
    connect(&copyAction_, SIGNAL(triggered()),this, SLOT(onCopyAction()), Qt::UniqueConnection);

    getParent()->addAction(&pasteAction_);
    pasteAction_.setShortcut(QKeySequence::Paste);
    connect(&pasteAction_, SIGNAL(triggered()),this, SLOT(onPasteAction()), Qt::UniqueConnection);	

    getParent()->addAction(&addAction_);
    connect(&addAction_, SIGNAL(triggered()), this, SLOT(onAddToLibraryAction()), Qt::UniqueConnection);

    getParent()->addAction(&openComponentAction_);
    connect(&openComponentAction_, SIGNAL(triggered()), this, SLOT(onOpenComponentAction()), Qt::UniqueConnection);

    connect(&openDesignMenu_, SIGNAL(triggered(QAction*)),
        this, SLOT(onOpenDesignAction(QAction*)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: destroyConnections()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::destroyConnections()
{
    foreach (QGraphicsItem* item, items()) 
    {
        // if the item is an interconnection
        if (item->type() == connectionType()) 
        {
            removeItem(item);
            delete item;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::connectAt()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::connectAt(QPointF const& cursorPosition)
{
    // no items are selected if the mode is connect
    clearSelection();

    if (!creatingConnection())
    {
        setConnectionStaringPoint(cursorPosition);

        if (hasConnectionStartingPoint())
        {
            beginCreateConnection(cursorPosition);
        }
    }
    else
    {
        endConnectionTo(cursorPosition);

        // In off page mode immediately continue with a new connection.
        if (inOffPageMode() && hasConnectionStartingPoint())
        {
            beginCreateConnection(cursorPosition);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::creatingConnection()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::creatingConnection() const
{
    return tempConnection_ != 0;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::setConnectionStaringPoint()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::setConnectionStaringPoint(QPointF const& cursorPosition)
{
    // No need to change the starting point in off page mode.
    if (inOffPageMode() && connectionStartPoint_ != 0)
    {
        return;
    }

    // Try to snap to a connection end point.
    ConnectionEndpoint* endpoint = DiagramUtil::snapToItem<ConnectionEndpoint>(cursorPosition, this, GridSize);

    if (endpoint == 0 || !endpoint->isVisible())
    {
        connectionStartPoint_ = 0;
        return;
    }

    if (inOffPageMode())
    {
        clearHighlightedEndpoint();

        if (endpoint->type() != offpageConnectorType())
        {
            endpoint = endpoint->getOffPageConnector();
            endpoint->show();
        }
    }

    connectionStartPoint_ = endpoint;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::hasConnectionStartingPoint()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::hasConnectionStartingPoint() const
{
    return connectionStartPoint_ != 0;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::beginCreateConnection()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::beginCreateConnection(QPointF const& startingPoint)
{
    Q_ASSERT(connectionStartPoint_ != 0);

    // Create the connection.
    tempConnection_ = createConnection(connectionStartPoint_, snapPointToGrid(startingPoint));

    addItem(tempConnection_);

    // Determine all potential end points to which the starting end point could be connected
    // and highlight them.
    highlightConnectableEndpoints();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::highlightConnectableEndpoints()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::highlightConnectableEndpoints()
{
    foreach(QGraphicsItem* item, items())
    {
        ConnectionEndpoint* endpoint = dynamic_cast<ConnectionEndpoint*>(item);

        if (isPossibleEndpointForCurrentConnection(endpoint))
        {
            endpoint->setHighlight(ConnectionEndpoint::HIGHLIGHT_ALLOWED);
            tempPotentialEndingEndPoints_.append(endpoint);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::isPossibleEndpointForCurrentConnection()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::isPossibleEndpointForCurrentConnection(ConnectionEndpoint* endpoint)
{
    return endpoint != 0 && endpoint->isVisible() && endpoint != connectionStartPoint_ &&
        endpoint->getOffPageConnector() != connectionStartPoint_ &&
        endpoint->canConnect(connectionStartPoint_) && connectionStartPoint_->canConnect(endpoint);
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::endConnectionTo()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::endConnectionTo(QPointF const& point)
{
    // Disable highlights from all potential end points.
    clearPotentialEndpoints();

    ConnectionEndpoint* endpoint = DiagramUtil::snapToItem<ConnectionEndpoint>(point, this, GridSize);

    // Check if there is an end point close enough to the cursor and the
    // end points can be connected together.
    if (isPossibleEndpointForCurrentConnection(endpoint))
    {
        // Check if the connection should be converted to an off-page connection.
        bool firstOffPage = connectionStartPoint_->type() == offpageConnectorType();
        bool secondOffPage = endpoint->type() == offpageConnectorType();

        if (inOffPageMode() ||
            ((firstOffPage || secondOffPage) && connectionStartPoint_->type() != endpoint->type()))
        {
            if (!firstOffPage)
            {
                connectionStartPoint_ = connectionStartPoint_->getOffPageConnector();
            }

            if (!secondOffPage)
            {
                endpoint = endpoint->getOffPageConnector();
            }

            discardConnection();

            GraphicsConnection* newTempConnection = createConnection(connectionStartPoint_, endpoint);

            addItem(newTempConnection);
            tempConnection_ = newTempConnection;
        }

        if (tempConnection_->connectEnds())
        {
            tempConnection_->fixOverlap();

            QSharedPointer<QUndoCommand> cmd = createAddCommandForConnection(tempConnection_);
            getEditProvider().addCommand(cmd);

            tempConnection_ = 0;
        }
        else
        {
            discardConnection();
            connectionStartPoint_ = 0;
        }

        if (!inOffPageMode())
        {
            connectionStartPoint_ = 0;
        }

    }
    // Delete the temporary connection.
    else
    {        
        discardConnection();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::hasConnectionEndingPoint()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::hasConnectionEndingPoint() const
{
    return connectionEndPoint_ != 0;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::discardConnection()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::discardConnection()
{
    if (tempConnection_)
    {
        removeItem(tempConnection_);
        delete tempConnection_;
        tempConnection_ = 0;
    }    
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::disableHighlights()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::disableHighlights()
{
    // If the starting point is set, disable highlight from it too.
    if (connectionStartPoint_ != 0)
    {
        connectionStartPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        connectionStartPoint_ = 0;
    }

    clearHighlightedEndpoint();

    clearPotentialEndpoints();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::clearHighlightedEndPoint()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::clearHighlightedEndpoint()
{
    if (hasConnectionEndingPoint())
    {
        connectionEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        connectionEndPoint_ = 0;
    }
}
//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::clearPotentialEndpoints()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::clearPotentialEndpoints()
{
    foreach (ConnectionEndpoint* endpoint, tempPotentialEndingEndPoints_)
    {
        endpoint->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
    }
    tempPotentialEndingEndPoints_.clear();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::addInterfaceAt()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::addInterfaceAt(QPointF const& position)
{
    // Find the column under the cursor.
    GraphicsColumn* column = getLayout()->findColumnAt(position);

    // Add a new diagram interface to the column it it is allowed.
    if (column != 0 && column->getColumnDesc().getAllowedItems() & CIT_INTERFACE)
    {
        addTopLevelInterface(column, position);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::toggleOffPageAt()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::toggleOffPageAt(QPointF const& position)
{
    // Try to snap to a connection end point.
    ConnectionEndpoint* endpoint = DiagramUtil::snapToItem<ConnectionEndpoint>(position, this, GridSize);

    QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

    if (endpoint != 0 && endpoint->isVisible())
    {
        QList<GraphicsConnection*> connections = endpoint->getConnections();
        if (connections.size() > 0)
        {
            hideOffPageConnections();

            foreach (GraphicsConnection* conn, connections)
            {
                toggleConnectionStyle(conn, cmd.data());
            }
        }
    }
    else
    {
        QGraphicsItem* item = itemAt(position, QTransform());

        GraphicsConnection* connection = dynamic_cast<GraphicsConnection*>(item);
        if (connection)
        {
            toggleConnectionStyle(connection, cmd.data());
        }
    }

    if (cmd->childCount() > 0)
    {
        getEditProvider().addCommand(cmd);
    }
}

//-----------------------------------------------------------------------------
// Function: toggleConnectionStyle()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::toggleConnectionStyle(GraphicsConnection* conn, QUndoCommand* parentCmd)
{
    Q_ASSERT(parentCmd != 0);
    emit clearItemSelection();

    conn->toggleOffPage();
    new ConnectionToggleOffPageCommand(conn, parentCmd);
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::hideOffPageConnections()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::hideOffPageConnections()
{
    foreach (QGraphicsItem* item, items())
    {
        GraphicsConnection* conn = dynamic_cast<GraphicsConnection*>(item);

        if (conn != 0 && conn->endpoint1()->type() == offpageConnectorType())
        {
            conn->hide();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::beginComponentReplaceDrag()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::beginComponentReplaceDrag(QPointF const& startpoint)
{
    ComponentItem* sourceComp =  getTopmostComponent(startpoint);

    if (sourceComp)
    {
        sourceComp_ = sourceComp;
        QApplication::setOverrideCursor(Qt::ForbiddenCursor);
        setInteractionMode(REPLACE);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::updateComponentReplaceDragCursor()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::updateComponentReplaceDragCursor(QPointF const& cursorPosition)
{
    ComponentItem* destComp = getTopmostComponent(cursorPosition);

    if (destComp != 0 && destComp != sourceComp_)
    {
        QApplication::changeOverrideCursor(Qt::ClosedHandCursor);
    }
    else
    {
        QApplication::changeOverrideCursor(Qt::ForbiddenCursor);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::endComponentReplaceDrag()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::endComponentReplaceDrag(QPointF const& endpoint)
{
    setInteractionMode(NORMAL);
    QApplication::restoreOverrideCursor();

    ComponentItem* destComp = 0;
    QList<QGraphicsItem*> itemList = items(endpoint);

    if (!itemList.empty())
    {
        destComp = dynamic_cast<ComponentItem*>(itemList.back());
    }

    if (destComp == 0 || destComp == sourceComp_)
    {
        return;
    }

    QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
        tr("Component instance '%1' is about to be switched in place "
        "with '%2'. Continue and replace?").arg(destComp->name(), sourceComp_->name()),
        QMessageBox::Yes | QMessageBox::No, getParent());

    if (msgBox.exec() == QMessageBox::Yes)
    {
        replace(destComp, sourceComp_);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::updateConnectionDisplay()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::updateConnectionDisplay(QPointF const& cursorPosition)
{
    if (creatingConnection())
    {
        highlightPotentialEndpointUnderCursor(cursorPosition);
        updateConnectionDraw(cursorPosition);
    }
    else
    {
        highlightEndpointUnderCursor(cursorPosition);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::updateConnectionDraw()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::updateConnectionDraw(QPointF const& cursorPosition)
{
    Q_ASSERT(tempConnection_->route().size() != 0);

    GraphicsConnection* updatedConnection = 0;

    if (hasConnectionEndingPoint())
    {
        updatedConnection = createConnection(connectionStartPoint_, connectionEndPoint_);
    }
    else
    {
        updatedConnection = createConnection(connectionStartPoint_, snapPointToGrid(cursorPosition));
    }

    discardConnection();

    addItem(updatedConnection);
    tempConnection_ = updatedConnection;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::highlightEndpointUnderCursor()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::highlightEndpointUnderCursor(QPointF const& cursorPosition)
{
    clearHighlightedEndpoint();

    // Find out if there is an end point currently under the cursor.
    ConnectionEndpoint* endpoint = DiagramUtil::snapToItem<ConnectionEndpoint>(cursorPosition, this, GridSize);
    if (endpoint != 0 && endpoint->isVisible())
    {
        connectionEndPoint_ = endpoint;
        connectionEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_HOVER);

        if (connectionEndPoint_->type() == offpageConnectorType())
        {
            hideOffPageConnections();

            foreach (GraphicsConnection* conn, connectionEndPoint_->getConnections())
            {
                conn->show();
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::updateConnectionHighlight()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::highlightPotentialEndpointUnderCursor(QPointF const& cursorPosition)
{
    disableCurrentHighlight();

    // Find out if there is an end point currently under the cursor.
    ConnectionEndpoint* endpoint = DiagramUtil::snapToItem<ConnectionEndpoint>(cursorPosition, this, GridSize);
    if (endpoint != 0 && tempPotentialEndingEndPoints_.contains(endpoint))
    {
        // Highlight the end point.          
        connectionEndPoint_ = endpoint;
        connectionEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_HOVER);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::disableCurrentHighlight()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::disableCurrentHighlight()
{
    if (hasConnectionEndingPoint())
    {
        if (hasConnectionStartingPoint() && connectionEndPoint_ != connectionStartPoint_ &&
            tempPotentialEndingEndPoints_.contains(connectionEndPoint_))
        {
            connectionEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_ALLOWED);
        }
        else
        {
            connectionEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        }

        connectionEndPoint_ = 0;
    }
}

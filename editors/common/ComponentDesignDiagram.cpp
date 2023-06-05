//-----------------------------------------------------------------------------
// File: ComponentDesignDiagram.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
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

#include <editors/common/DiagramUtil.h>
#include <editors/common/DesignWidget.h>
#include <editors/common/ComponentItemAutoConnector/AutoConnectorItem.h>
#include <editors/common/ComponentItemAutoConnector/ComponentItemAutoConnector.h>

#include <KactusAPI/include/LibraryHandler.h>

#include <QGraphicsSceneMouseEvent>
#include <QString>
#include <QMessageBox>
#include <QCoreApplication>
#include <QApplication>
#include <QMenu>

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::ComponentDesignDiagram()
//-----------------------------------------------------------------------------
ComponentDesignDiagram::ComponentDesignDiagram(LibraryInterface* lh, QSharedPointer<IEditProvider> editProvider, 
    DesignWidget* parent):
    DesignDiagram(lh, editProvider, parent),
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
    deleteAction_(tr("Delete"), this),
    openDesignMenu_(tr("Open Design")),
    openAutoConnector_(tr("Connect to ..."), this),
    clickedPosition_(),
    lastMousePress_(Qt::NoButton),
    lastSelectedItemIsAtRightEdge_(false)
{
    connect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(editProvider.data(), SIGNAL(modified()), this, SIGNAL(contentChanged()));

	setupActions();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::~ComponentDesignDiagram()
//-----------------------------------------------------------------------------
ComponentDesignDiagram::~ComponentDesignDiagram()
{
    disconnect(this, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
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
    QGraphicsItem* selectedItem = nullptr;

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

            if (conn->endpoint1() != nullptr)
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
    if (selectedItem != nullptr && selectedItem->type() == offpageConnectorType())
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
// Function: ComponentDesignDiagram::onOpenAutoConnector()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::onOpenAutoConnector()
{
    ComponentItemAutoConnector* autoConnector = createAutoConnector(contextMenuItem_);
    if (autoConnector && autoConnector->exec() == QDialog::Accepted)
    {
        QVector<QPair<AutoConnectorItem*, AutoConnectorItem*> > autoConnections =
            autoConnector->getConnectedItems();

        if (!autoConnections.isEmpty())
        {
            for (auto connectionItem : autoConnections)
            {
                ConnectionEndpoint* startPointItem = getEndPointForItem(connectionItem.first);
                ConnectionEndpoint* endPointItem = getEndPointForItem(connectionItem.second);

                if (startPointItem && endPointItem)
                {
                    createConnectionBetweenEndPoints(startPointItem, endPointItem);
                }
            }

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::getVisibleNameForComponentItem()
//-----------------------------------------------------------------------------
QString ComponentDesignDiagram::getVisibleNameForComponentItem(ComponentItem* item) const
{
    if (!item->displayName().isEmpty())
    {
        return item->displayName();
    }
    else
    {
        return item->name();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::mousePressEvent()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    lastMousePress_ = mouseEvent->button();

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

            if (singleSelection())
            {
                auto const selectedItem = selectedItems().first();
                auto const firstView = views().first();
                if (selectedItem && firstView)
                {
                    QRectF itemRectangle = selectedItem->sceneBoundingRect();
                    QRectF viewScene = firstView->mapToScene(firstView->rect()).boundingRect();

                    if (itemRectangle.right() > viewScene.right())
                    {
                        lastSelectedItemIsAtRightEdge_ = true;
                    }
                }
            }
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
        updateAssociationCursor(mouseEvent->scenePos());
        updateAssociationLineDraw(mouseEvent->scenePos());
    }
    // Allow moving items only when a single item is selected.
    else if (singleSelection())
    {
        if (lastMousePress_ == Qt::LeftButton)
        {
            ensureMovedItemVisibility(mouseEvent);
        }

        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::getMovedItemRectangle()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::ensureMovedItemVisibility(QGraphicsSceneMouseEvent* mouseEvent)
{
    auto const selectedItem = selectedItems().first();
    auto const firstView = views().first();
    if (selectedItem && firstView)
    {
        QRectF itemRectangle = selectedItem->sceneBoundingRect();
        QRectF viewScene = firstView->mapToScene(firstView->rect()).boundingRect();
        if (itemRectangle.height() > viewScene.height())
        {
            QPointF mousePosition = mouseEvent->scenePos();
            qreal newY = mousePosition.y();
            qreal newHeight = viewScene.height() / 2;

            qreal availableAbove = mousePosition.y() - itemRectangle.y();
            if (availableAbove >= newHeight / 2)
            {
                newY = newY - newHeight / 2;
            }
            else
            {
                newY = newY - availableAbove;
            }

            itemRectangle.setY(newY);
            itemRectangle.setHeight(newHeight);
        }

        int sideMargin = 50;

        if (lastSelectedItemIsAtRightEdge_)
        {
            sideMargin = -50;
        }

        firstView->ensureVisible(itemRectangle, sideMargin);
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

    lastMousePress_ = Qt::NoButton;
    lastSelectedItemIsAtRightEdge_ = false;
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
            connectionStartPoint_->getConnections().isEmpty() && 
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
    QMenu* menu = nullptr;

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
        if (addAutoConnectorActionToContextMenu())
        {
            menu->addAction(&openAutoConnector_);
        }
        menu->addSeparator();
        menu->addAction(&copyAction_);
        menu->addAction(&pasteAction_);
        menu->addAction(&deleteAction_);

        openAutoConnector_.setText("Connect to ...");

        ComponentItem* compItem = dynamic_cast<ComponentItem*>(item);
        contextMenuItem_ = compItem;
        if (compItem)
        {
            setupAutoconnectText(compItem);
            updateOpenDesignMenuFor(compItem);
        }
    }

    return menu;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::addAutoConnectorActionToContextMenu()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::addAutoConnectorActionToContextMenu() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::updateOpenDesignMenuFor()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::updateOpenDesignMenuFor(ComponentItem* compItem)
{
    QString activeViewName = getActiveViewOf(compItem);
    QStringList views = hierarchicalViewsOf(compItem);

    for (auto const& viewName : views)
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
// Function: ComponentDesignDiagram::setupAutoconnectText()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::setupAutoconnectText(ComponentItem* componentItem)
{
    if (openAutoConnector_.isEnabled())
    {
        QString targetName = "";

        if (selectedItems().count() == 1)
        {
            targetName = getEditedComponent()->getVlnv().getName();
        }
        else
        {
            ComponentItem* connectionTarget;
            if (selectedItems().first() == componentItem)
            {
                connectionTarget = dynamic_cast<ComponentItem*>(selectedItems().last());
            }
            else
            {
                connectionTarget = dynamic_cast<ComponentItem*>(selectedItems().first());
            }

            if (connectionTarget)
            {
                targetName = connectionTarget->displayName();
                if (targetName.isEmpty())
                {
                    targetName = connectionTarget->name();
                }
            }
        }

        QString actionText = openAutoConnector_.text();
        actionText.replace("...", targetName);
        openAutoConnector_.setText(actionText);
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
    openAutoConnector_.setEnabled(autoConnectorEnabled());
    copyAction_.setEnabled(copyActionEnabled());
    pasteAction_.setEnabled(pasteActionEnabled());
    deleteAction_.setEnabled(deleteActionEnabled());
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::deleteActionEnabled()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::deleteActionEnabled() const
{
    return !isProtected() && !selectedItems().isEmpty() && selectedItemIsCorrectType();
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
// Function: ComponentDesignDiagram::autoConnectorEnabled()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::autoConnectorEnabled() const
{
    int itemCount = selectedItems().count();

    return !isProtected() && (itemCount == 1 || itemCount == 2 ) &&
        selectedItems().first()->type() == componentType() && selectedItems().last()->type() == componentType();
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::draftSelected()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::draftSelected() const
{
    return !selectedItems().empty() && 
        selectedItems().first()->type() == componentType() &&
        !dynamic_cast<ComponentItem*>(selectedItems().first())->componentModel()->getVlnv().isValid();
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
    else if (hierViews.isEmpty())
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
    emit openComponent(comp->componentModel()->getVlnv());
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::getActiveViewOf()
//-----------------------------------------------------------------------------
QString ComponentDesignDiagram::getActiveViewOf(ComponentItem* compItem) const
{
    QString activeViewName;
    auto instanceName = compItem->name();



    if (QSharedPointer<DesignConfiguration> designConf = getDesignConfiguration();  designConf != nullptr)
    {

        activeViewName = designConf->getActiveView(compItem->name());
    }

    return activeViewName;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::getActiveViewForEndPoint()
//-----------------------------------------------------------------------------
QString ComponentDesignDiagram::getActiveViewForEndPoint(ConnectionEndpoint* endPoint) const
{
    if (endPoint->isHierarchical())
    {
        return getTopView();
    }
    else
    {
        return getActiveViewOf(endPoint->encompassingComp());
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::contextMenuPosition()
//-----------------------------------------------------------------------------
QPoint ComponentDesignDiagram::contextMenuPosition() const
{
    return getParent()->mapFromGlobal(clickedPosition_);
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::findSceneMappedCursorPosition()
//-----------------------------------------------------------------------------
QPointF ComponentDesignDiagram::findCursorPositionMappedToScene() const
{
    return views().first()->mapToScene(contextMenuPosition());
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::setupActions()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::setupActions()
{
    getParent()->addAction(&selectAllAction_);
    selectAllAction_.setShortcut(QKeySequence::SelectAll);
    selectAllAction_.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(&selectAllAction_, SIGNAL(triggered()),this, SLOT(selectAll()), Qt::UniqueConnection);

    getParent()->addAction(&copyAction_);
    copyAction_.setShortcut(QKeySequence::Copy);
    copyAction_.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(&copyAction_, SIGNAL(triggered()),this, SLOT(onCopyAction()), Qt::UniqueConnection);

    getParent()->addAction(&pasteAction_);
    pasteAction_.setShortcut(QKeySequence::Paste);
    pasteAction_.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(&pasteAction_, SIGNAL(triggered()),this, SLOT(onPasteAction()), Qt::UniqueConnection);	

    getParent()->addAction(&addAction_);
    connect(&addAction_, SIGNAL(triggered()), this, SLOT(onAddToLibraryAction()), Qt::UniqueConnection);

    getParent()->addAction(&openComponentAction_);
    connect(&openComponentAction_, SIGNAL(triggered()), this, SLOT(onOpenComponentAction()), Qt::UniqueConnection);

    getParent()->addAction(&deleteAction_);
    deleteAction_.setShortcut(QKeySequence::Delete);
    deleteAction_.setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(&deleteAction_, SIGNAL(triggered()), this, SIGNAL(deleteSelectedItems()), Qt::UniqueConnection);

    connect(&openDesignMenu_, SIGNAL(triggered(QAction*)),
        this, SLOT(onOpenDesignAction(QAction*)), Qt::UniqueConnection);

    connect(&openAutoConnector_, SIGNAL(triggered()), this, SLOT(onOpenAutoConnector()), Qt::AutoConnection);
}

//-----------------------------------------------------------------------------
// Function: destroyConnections()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::destroyConnections()
{
    QList<GraphicsConnection*> connectionList;
    for (QGraphicsItem* item : items())
    {
        GraphicsConnection* graphicsConnection = dynamic_cast<GraphicsConnection*>(item);
        if (graphicsConnection)
        {
            connectionList.append(graphicsConnection);
        }
    }

    for (GraphicsConnection * graphicsConnection : connectionList)
    {
        removeItem(graphicsConnection);
        delete graphicsConnection;
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
        setConnectionStartingPoint(cursorPosition);

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
    return tempConnection_ != nullptr;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::setConnectionStartingPoint()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::setConnectionStartingPoint(QPointF const& cursorPosition)
{
    // No need to change the starting point in off page mode.
    if (inOffPageMode() && connectionStartPoint_ != nullptr)
    {
        return;
    }

    // Try to snap to a connection end point.
    ConnectionEndpoint* endpoint = DiagramUtil::snapToItem<ConnectionEndpoint>(cursorPosition, this, GridSize);

    if (endpoint == nullptr || !endpoint->isVisible())
    {
        connectionStartPoint_ = nullptr;
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
    return connectionStartPoint_ != nullptr;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::beginCreateConnection()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::beginCreateConnection(QPointF const& startingPoint)
{
    Q_ASSERT(connectionStartPoint_ != nullptr);

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
    for (QGraphicsItem* item : items())
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
bool ComponentDesignDiagram::isPossibleEndpointForCurrentConnection(ConnectionEndpoint* endpoint) const
{
    return endpoint != nullptr && endpoint->isVisible() && endpoint != connectionStartPoint_ &&
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

        if (tempConnection_->endpoint1()->canConnect(tempConnection_->endpoint2()) &&
            tempConnection_->endpoint2()->canConnect(tempConnection_->endpoint1()))
        {
            QSharedPointer<QUndoCommand> cmd = createAddCommandForConnection(tempConnection_);
            cmd->redo();

            tempConnection_->fixOverlap();
            if (tempConnection_->endpoint1() && tempConnection_->endpoint2())
            {
                getEditProvider()->addCommand(cmd); 
            }
            else
            {
                discardConnection();
                connectionStartPoint_ = nullptr;
            }
            tempConnection_ = nullptr;
        }
        else
        {
            discardConnection();
            connectionStartPoint_ = nullptr;
        }

        if (!inOffPageMode())
        {
            connectionStartPoint_ = nullptr;
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
    return connectionEndPoint_ != nullptr;
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
        tempConnection_ = nullptr;
    }    
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::disableHighlights()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::disableHighlights()
{
    // If the starting point is set, disable highlight from it too.
    if (connectionStartPoint_ != nullptr)
    {
        connectionStartPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_OFF);
        connectionStartPoint_ = nullptr;
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
        connectionEndPoint_ = nullptr;
    }
}
//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::clearPotentialEndpoints()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::clearPotentialEndpoints()
{
    for (ConnectionEndpoint* endpoint : tempPotentialEndingEndPoints_)
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
    if (column != nullptr && column->getColumnDesc()->getAllowedItems() & ColumnTypes::INTERFACE)
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
    ConnectionEndpoint const* endpoint = DiagramUtil::snapToItem<ConnectionEndpoint>(position, this, GridSize);

    QSharedPointer<QUndoCommand> cmd(new QUndoCommand());

    if (endpoint != nullptr && endpoint->isVisible())
    {
        QList<GraphicsConnection*> connections = endpoint->getConnections();
        if (connections.isEmpty() == false)
        {
            hideOffPageConnections();

            for (GraphicsConnection* conn : connections)
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
        getEditProvider()->addCommand(cmd);
    }
}

//-----------------------------------------------------------------------------
// Function: toggleConnectionStyle()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::toggleConnectionStyle(GraphicsConnection* conn, QUndoCommand* parentCmd)
{
    Q_ASSERT(parentCmd != nullptr);
    emit clearItemSelection();

    conn->toggleOffPage();
    new ConnectionToggleOffPageCommand(conn, parentCmd);
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::hideOffPageConnections()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::hideOffPageConnections()
{
    for (QGraphicsItem* item : items())
    {
        GraphicsConnection* connection = dynamic_cast<GraphicsConnection*>(item);

        if (connection && connection->endpoint1() && connection->endpoint1()->type() == offpageConnectorType())
        {
            connection->hide();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::beginComponentReplaceDrag()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::beginComponentReplaceDrag(QPointF const& startpoint)
{
    ComponentItem* sourceComp = getTopmostComponent(startpoint);

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

    if (destComp != nullptr && destComp != sourceComp_ && componentItemIsAllowedInColumnAtPosition(cursorPosition))
    {
        QApplication::changeOverrideCursor(Qt::ClosedHandCursor);
    }
    else
    {
        QApplication::changeOverrideCursor(Qt::ForbiddenCursor);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::componentItemIsAllowedInColumn()
//-----------------------------------------------------------------------------
bool ComponentDesignDiagram::componentItemIsAllowedInColumnAtPosition(QPointF const& cursorPosition) const
{
    QSharedPointer<Component> draggedComponent = sourceComp_->componentModel();
    if (draggedComponent)
    {
        ColumnTypes::ColumnItemType itemType = ColumnTypes::COMPONENT;

        if (draggedComponent->isBridge())
        {
            itemType = ColumnTypes::BRIDGE;
        }
        else if (draggedComponent->isChannel())
        {
            itemType = ColumnTypes::CHANNEL;
        }
        else if (draggedComponent->isBus())
        {
            itemType = ColumnTypes::INTERFACE;
        }

        GraphicsColumn const* column = getLayout()->findColumnAt(cursorPosition);

        return column->getColumnDesc()->getAllowedItems() & itemType;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentDesignDiagram::endComponentReplaceDrag()
//-----------------------------------------------------------------------------
void ComponentDesignDiagram::endComponentReplaceDrag(QPointF const& endpoint)
{
    setInteractionMode(NORMAL);
    QApplication::restoreOverrideCursor();
    
    if (componentItemIsAllowedInColumnAtPosition(endpoint))
    {
        ComponentItem* destinationComponent = getTopmostComponent(endpoint);

        if (destinationComponent == nullptr || destinationComponent == sourceComp_)
        {
            return;
        }

        QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
            tr("Component instance '%1' is about to be switched in place with '%2'. Continue and replace?").
            arg(destinationComponent->name(), sourceComp_->name()),
            QMessageBox::Yes | QMessageBox::No, getParent());

        if (msgBox.exec() == QMessageBox::Yes)
        {
            replace(destinationComponent, sourceComp_);
        }
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
    Q_ASSERT(tempConnection_->route().isEmpty() == false);

    GraphicsConnection* updatedConnection = nullptr;

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
    if (endpoint != nullptr && endpoint->isVisible())
    {
        connectionEndPoint_ = endpoint;
        connectionEndPoint_->setHighlight(ConnectionEndpoint::HIGHLIGHT_HOVER);

        if (connectionEndPoint_->type() == offpageConnectorType())
        {
            hideOffPageConnections();

            for (GraphicsConnection* conn : connectionEndPoint_->getConnections())
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
    if (endpoint != nullptr && tempPotentialEndingEndPoints_.contains(endpoint))
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

        connectionEndPoint_ = nullptr;
    }
}

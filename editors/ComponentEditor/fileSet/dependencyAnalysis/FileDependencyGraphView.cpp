//-----------------------------------------------------------------------------
// File: FileDependencyGraphWidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 23.01.2013
//
// Description:
// Widget for showing the table-style file dependency graph.
//-----------------------------------------------------------------------------

#include "FileDependencyGraphView.h"

#include "FileDependencyColumns.h"
#include "FileDependencyItem.h"
#include "FileDependencyModel.h"
#include "FileDependencySortFilter.h"

#include <IPXACTmodels/kactusExtensions/FileDependency.h>

#include <QHeaderView>
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QDebug>
#include <QMenu>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphView::FileDependencyGraphView(QWidget* parent)
    : QTreeView(parent),
      sortFilter_(new FileDependencySortFilter(this)),
      model_(0),
      columns_(),
      maxVisibleGraphColumns_(0),
      scrollIndex_(0),
      selectedDependency_(0),
      drawingDependency_(false),
      filters_(FILTER_DEFAULT),
      manualDependencyStartItem_(0),
      manualDependencyEndItem_(0),
      multiManualCreation_(false)
{
    setUniformRowHeights(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);

    connect(header(), SIGNAL(sectionResized(int, int, int)), this, SLOT(onSectionResized()), Qt::UniqueConnection);

    connect(this, SIGNAL(collapsed(QModelIndex const&)), this, SLOT(onDependenciesReset()), Qt::UniqueConnection);
    connect(this, SIGNAL(expanded(QModelIndex const&)), this, SLOT(onDependenciesReset()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::~FileDependencyGraphWidget()
//-----------------------------------------------------------------------------
FileDependencyGraphView::~FileDependencyGraphView()
{
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::setModel()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::setModel(QAbstractItemModel* model)
{
    // Disconnect the previous model.
    if (model_ != 0)
    {
        model_->disconnect(this);
    }

    FileDependencyModel* depModel = dynamic_cast<FileDependencyModel*>(model);
    if (depModel != 0)
    {
        connect(depModel, SIGNAL(dependencyAdded(FileDependency*)),
                this, SLOT(onDependencyAdded(FileDependency*)), Qt::UniqueConnection);
        connect(depModel, SIGNAL(dependencyChanged(FileDependency*)),
                this, SLOT(onDependencyChanged(FileDependency*)), Qt::UniqueConnection);
        connect(depModel, SIGNAL(dependencyRemoved(FileDependency*)),
                this, SLOT(onDependencyRemoved(FileDependency*)), Qt::UniqueConnection);
        connect(depModel, SIGNAL(dependenciesReset()),
                this, SLOT(onDependenciesReset()), Qt::UniqueConnection);

        connect(this, SIGNAL(dependencyChanged(FileDependency*)),
                depModel, SIGNAL(dependencyChanged(FileDependency*)), Qt::UniqueConnection);

        model_ = depModel;
    }
    else
    {
        model_ = 0;
    }

    sortFilter_->setSourceModel(model_);
    QTreeView::setModel(sortFilter_);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::setFilters()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::setFilters(DependencyFilters filters)
{
    // Save filters and apply.
    filters_ = filters;
    sortFilter_->setFilters(filters_);
    onDependenciesReset();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::getFilters()
//-----------------------------------------------------------------------------
FileDependencyGraphView::DependencyFilters FileDependencyGraphView::getFilters() const
{
    return filters_;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::setDependenciesEditable()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::setDependenciesEditable(bool editable)
{
    setColumnHidden(FileDependencyColumns::CREATE_DEPENDENCY, !editable);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::setGraphColumnScrollIndex()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::setGraphColumnScrollIndex(int index)
{
    scrollIndex_ = index;
    viewport()->repaint();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::paintEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::paintEvent(QPaintEvent* event)
{
    QTreeView::paintEvent(event);

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    painter.save();

    drawDependencyGraph(painter, event->rect());

    if (drawingDependency_)
    {
        drawManualCreationArrow(painter);
    }

    painter.restore();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::mousePressEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton && manualDependencyStartItem_)
    {
        endDependencyDraw();
        viewport()->repaint();
    }

    int column = columnAt(event->x());

    // Check if the user pressed over the dependencies column.
    if (column == FileDependencyColumns::DEPENDENCIES)
    {
        selectDependencyUnderCursor(event);        
    }
    // Check if click is on the path.
    else if (column == FileDependencyColumns::PATH)
    {
        createContextMenuForDependency(event);
    }
    // Otherwise check if the user pressed over the manual creation column.
    else if (column == FileDependencyColumns::CREATE_DEPENDENCY)
    {
        createDependency(event);
    }
    else
    {
        QTreeView::mousePressEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::mouseMoveEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::mouseMoveEvent(QMouseEvent* event)
{
    // If creating manual dependencies
    if(manualDependencyStartItem_ && drawingDependency_)
    {
        QModelIndex currentPoint = sortFilter_->mapToSource(indexAt(event->pos()));
        if (currentPoint.isValid())
        {
            FileDependencyItem* currentDependencyItem = 
                static_cast<FileDependencyItem*>(currentPoint.internalPointer());
            manualDependencyEndItem_ = currentDependencyItem;
            viewport()->repaint();
        }
    }
    else
    {
        QTreeView::mouseMoveEvent(event);

        FileDependency* hovered = findDependencyAt(event->pos());
        if (hovered != 0)
        {
            setToolTip(hovered->getDescription());
        }
        else
        {
            setToolTip("");
        }

        viewport()->repaint();
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::keyPressEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::keyPressEvent(QKeyEvent* event)
{
    // When delete is pressed, attempt to delete the selected dependency.
    if (event->key() == Qt::Key_Delete && selectedDependency_)
    {
        // Check that the selected dependency is a manual one and that it's not locked.
        if (selectedDependency_->isManual() && !selectedDependency_->isLocked())
        {
            model_->removeDependency(selectedDependency_);
            selectedDependency_ = 0;
            emit selectionChanged(0);
        }
    }
    else if (event->key() == Qt::Key_Shift)
    {
        multiManualCreation_ = true;
    }
    else
    {
        QTreeView::keyPressEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Functi: FileDependencyGraphView::keyReleaseEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::keyReleaseEvent(QKeyEvent* event)
{
    // Stop creating multiple dependencies when shift is released.
    if (event->key() == Qt::Key_Shift)
    {
        multiManualCreation_ = false;
        manualDependencyStartItem_ = 0;
        manualDependencyEndItem_ = 0;
        drawingDependency_ = false;
        viewport()->repaint();
    }
    else if (event->key() == Qt::Key_Escape)
    {
        manualDependencyStartItem_ = 0;
        manualDependencyEndItem_ = 0;
        drawingDependency_ = false;
        viewport()->repaint();
    }
    // If key release isn't handled here, send to parent class.
    else
    {
        QTreeView::keyReleaseEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::resizeEvent()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::resizeEvent(QResizeEvent *event)
{
    QTreeView::resizeEvent(event);
    onSectionResized();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::rowsInserted()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::rowsInserted(QModelIndex const& parent, int start, int end)
{
    QTreeView::rowsInserted(parent, start, end);

    disconnect(this, SIGNAL(expanded(QModelIndex const&)), this, SLOT(onDependenciesReset()));
    setExpanded(parent, true);
    connect(this, SIGNAL(expanded(QModelIndex const&)), this, SLOT(onDependenciesReset()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::reset()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::reset()
{
    emit graphColumnScollMaximumChanged(0);
    emit selectionChanged(0);

    QTreeView::reset();

    disconnect(this, SIGNAL(expanded(QModelIndex const&)), this, SLOT(onDependenciesReset()));
    expandAll();
    connect(this, SIGNAL(expanded(QModelIndex const&)), this, SLOT(onDependenciesReset()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::onDependencyAdded()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onDependencyAdded(FileDependency* dependency, bool immediateRepaint)
{
    FileDependencyItem* fromItem = model_->findFileItem(dependency->getFile1());
    FileDependencyItem* toItem = model_->findFileItem(dependency->getFile2());

    Q_ASSERT(fromItem != 0);
    Q_ASSERT(toItem != 0);

    // Check if the dependency should not be visible.
    if (!filterDependency(dependency))
    {
        return;
    }

    if (fromItem != 0 && toItem != 0)
    {
        // Determine the y coordinates for the dependency.
        int fromY = 0;
        int toY = 0;

        getVisualRowY(sortFilter_->mapFromSource(model_->getItemIndex(fromItem, 0)), fromY);
        getVisualRowY(sortFilter_->mapFromSource(model_->getItemIndex(toItem, 0)), toY);

        // The user may be scrolling the view so scroll bar position must be taken into account.
        int offset = verticalOffset();
        fromY += offset;
        toY += offset;

        GraphDependency graphDep(dependency, fromY, toY);

        // Find the first column from the left which has space for the dependency.
        GraphColumn* selectedColumn = 0;
        int x = GRAPH_MARGIN - scrollIndex_ * GRAPH_SPACING;

        foreach (GraphColumn* column, columns_)
        {
            if (hasSpace(column, graphDep))
            {
                selectedColumn = column;
                break;
            }

            x += GRAPH_SPACING;
        }

        // If no free column was found, create a new one.
        if (selectedColumn == 0)
        {            
            selectedColumn = new GraphColumn();
            columns_.append(selectedColumn);            

            onSectionResized();
        }
        
        selectedColumn->dependencies.append(graphDep);

        // Repaint only the region of the new dependency.
        if (immediateRepaint)
        {
            repaintArrowRegion(x, fromY, toY);
        }
    }
}


//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onDependencyChanged()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onDependencyChanged(FileDependency* dependency)
{
    repaintDependency(dependency);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onDependencyRemoved()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onDependencyRemoved(FileDependency* dependency)
{
    int x = GRAPH_MARGIN - scrollIndex_ * GRAPH_SPACING;

    foreach (GraphColumn* column, columns_)
    {        
        foreach (GraphDependency const& dep, column->dependencies)
        {           
            if (dep.dependency == dependency)
            {
                // Repaint only the area of the dependency.
                int fromY = 0;
                int toY = 0;
                bool visible = getCoordinates(dep, fromY, toY);

                column->dependencies.removeOne(dep);

                if (visible)
                {
                    repaintArrowRegion(x, fromY, toY);
                }

                return;
            }
        }

        x += GRAPH_SPACING;
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::onDependenciesReset()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onDependenciesReset()
{
    selectedDependency_ = 0;
    emit selectionChanged(0);

    columns_.clear();
    emit graphColumnScollMaximumChanged(0);

    foreach (QSharedPointer<FileDependency> dependency, model_->getDependencies())
    {
        onDependencyAdded(dependency.data(), false);
    }

    viewport()->repaint();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onSectionResized()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onSectionResized()
{
    int width = columnWidth(FileDependencyColumns::DEPENDENCIES);
    width -= 2 * GRAPH_MARGIN;

    if (width <= 0)
    {
        maxVisibleGraphColumns_ = 0;
    }
    else
    {
        maxVisibleGraphColumns_ = 1 + width / GRAPH_SPACING;
    }

    emit graphColumnScollMaximumChanged(qMax<int>(0, columns_.size() - maxVisibleGraphColumns_));
    emit dependencyColumnPositionChanged(columnViewportPosition(FileDependencyColumns::DEPENDENCIES));
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onAddLocation()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onAddLocation()
{
    // Open a file dialog to define the location.
    QString selectedDirectory = QFileDialog::getExistingDirectory(this, tr("Choose Source Directory"));
    if(selectedDirectory.isEmpty())
    {
        return;
    }

    selectedDirectory = QFileInfo(selectedDirectory).filePath();
    model_->defineLocation(contextMenuItem_, selectedDirectory);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onLocationReset()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onLocationReset()
{
    model_->resetLocation(contextMenuItem_);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onMenuReverse()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onMenuReverse()
{
    selectedDependency_->reverse();
    emit dependencyChanged(selectedDependency_);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onMenuBidirectional()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onMenuBidirectional()
{
    selectedDependency_->setBidirectional(!selectedDependency_->isBidirectional());
    emit dependencyChanged(selectedDependency_);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onMenuLock()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onMenuLock()
{
    selectedDependency_->setLocked(!selectedDependency_->isLocked());
    onDependencyChanged(selectedDependency_);
    emit dependencyChanged(selectedDependency_);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::onMenuDelete()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::onMenuDelete()
{
    model_->removeDependency(selectedDependency_);
    selectedDependency_ = 0;
    emit selectionChanged(0);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::selectDependencyUnderCursor()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::selectDependencyUnderCursor(QMouseEvent* event)
{
    if ((event->button() == Qt::LeftButton || event->button() == Qt::RightButton) && !manualDependencyStartItem_)
    {
        FileDependency* pressedDependency = findDependencyAt(event->pos());

        if (pressedDependency != selectedDependency_)
        {
            changeDependencySelection(pressedDependency);
        }

        if (event->button() == Qt::RightButton && selectedDependency_ != 0)
        {
            createContextMenu(event->globalPos());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::createContextMenuForDependency()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::createContextMenuForDependency(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        QModelIndex selectedIndex = sortFilter_->mapToSource(indexAt(event->pos()));
        if (selectedIndex.isValid())
        {
            FileDependencyItem* selectedItem = static_cast<FileDependencyItem*>(selectedIndex.internalPointer());

            if (selectedItem->getType() == FileDependencyItem::ITEM_TYPE_FILE && selectedItem->isExternal())
            {
                QMenu contextMenu;               

                if (selectedItem->getParent()->getType() == FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION)
                {
                    QAction* locationAction = contextMenu.addAction("Define Location...");
                    connect(locationAction, SIGNAL(triggered()), this, SLOT(onAddLocation()));
                }
                else
                {
                    QAction* changeAction = contextMenu.addAction("Change Location...");
                    connect(changeAction, SIGNAL(triggered()), this, SLOT(onAddLocation()));

                    QAction* resetAction = contextMenu.addAction("Reset");
                    connect(resetAction, SIGNAL(triggered()), this, SLOT(onLocationReset()));
                }

                contextMenu.exec(event->globalPos());
            }
        }
    }
    else
    {
        QTreeView::mousePressEvent(event);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::createDependency()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::createDependency(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        // Starting to create manual dependency
        if (!manualDependencyStartItem_)
        {
            // Remove possible selection
            if (selectedDependency_)
            {
                clearDependencySelection();
            }

            QModelIndex startPoint = sortFilter_->mapToSource(indexAt(event->pos()));

            if (startPoint.isValid())
            {
                manualDependencyStartItem_ = static_cast<FileDependencyItem*>(startPoint.internalPointer());

                if (manualDependencyStartItem_->getType() != FileDependencyItem::ITEM_TYPE_FILE)
                {
                    manualDependencyStartItem_ = 0;
                }
                else if (manualDependencyStartItem_->getParent()->getType() == FileDependencyItem::ITEM_TYPE_EXTERNAL_LOCATION ||
                    manualDependencyStartItem_->getParent()->getType() == FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION)
                {
                    manualDependencyStartItem_ = 0;
                    emit warningMessage(tr("Cannot create manual dependency from external or unknown file location."));
                }
                else
                {
                    startDependencyDraw();
                    emit warningMessage("");
                }
            }
        }
        // Ending manual dependency creation.
        else
        {
            if (manualDependencyEndItem_->getParent()->getType() == FileDependencyItem::ITEM_TYPE_EXTERNAL_LOCATION ||
                manualDependencyEndItem_->getParent()->getType() == FileDependencyItem::ITEM_TYPE_UNKNOWN_LOCATION)
            {
                emit warningMessage(tr("Cannot create manual dependency to external or unknown file location."));
            }
            else if (manualDependencyStartItem_ &&
                manualDependencyEndItem_->getType() == FileDependencyItem::ITEM_TYPE_FILE)
            {
                FileDependency* existingDependency = model_->findDependency(manualDependencyStartItem_->getPath(),
                    manualDependencyEndItem_->getPath());

                if (existingDependency)
                {
                    emit warningMessage(tr("Manually created dependency was discarded because it already exists."));
                }
                else
                {
                    // Check for a reversed dependency and whether the new dependency cannot be combined with it.
                    FileDependency* reversedDependency = model_->findDependency(manualDependencyEndItem_->getPath(),
                        manualDependencyStartItem_->getPath());

                    if (reversedDependency == 0 || !reversedDependency->isManual())
                    {
                        FileDependency* createdDependency = createManualDependency();                           
                        model_->addDependency(QSharedPointer<FileDependency>(createdDependency));

                        // Selecting created manual dependency.
                        changeDependencySelection(createdDependency);
                        emit warningMessage("");
                    }
                    else
                    {
                        reversedDependency->setBidirectional(true);
                        changeDependencySelection(reversedDependency);
                        emit warningMessage(tr("An existing manual dependency was changed to bidirectional."));
                    }
                }

                // If shift-key is hold down not ending manual creation yet.
                if (multiManualCreation_)
                {
                    startDependencyDraw();
                }
                else
                {
                    endDependencyDraw();
                }

                viewport()->repaint();
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawDependencies()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::drawDependencyGraph(QPainter& painter, QRect const& rect)
{
    int columnOffset = columnViewportPosition(FileDependencyColumns::DEPENDENCIES);
    int x = GRAPH_MARGIN;

    int lastColumnToDraw = qMin(columns_.size(), scrollIndex_ + maxVisibleGraphColumns_);
    for (int i = scrollIndex_; i < lastColumnToDraw; ++i)
    {
        foreach (GraphDependency const& dep, columns_[i]->dependencies)
        {
            int fromY = 0;
            int toY = 0;

            if (getCoordinates(dep, fromY, toY) &&
                ((filters_ & FILTER_DIFFERENCE) || dep.dependency->getStatus() != FileDependency::STATUS_REMOVED))
            {
                // Cull arrows that are not inside the view rectangle.
                QRect arrowRect(columnOffset + x - ARROW_WIDTH, qMin(fromY, toY) - POINTER_OFFSET,
                    2 * ARROW_WIDTH, qAbs(toY - fromY) + 2 * POINTER_OFFSET);

                if (arrowRect.intersects(rect))
                {
                    // Choose color for the arrow based on the dependency information and the selected dependency.
                    QColor color = Qt::black;

                    if (dep.dependency == selectedDependency_)
                    {
                        color = Qt::blue;
                    }
                    else if (dep.dependency->isManual())
                    {
                        color = Qt::magenta;
                    }
                    else if (filters_ & FILTER_DIFFERENCE)
                    {
                        if (dep.dependency->getStatus() == FileDependency::STATUS_ADDED)
                        {
                            color = QColor(0, 222, 0);
                        }
                        else if (dep.dependency->getStatus() == FileDependency::STATUS_REMOVED)
                        {
                            color = Qt::red;
                        }
                    }

                    drawArrow(painter, columnOffset + x, fromY, toY, color, dep.dependency->isBidirectional());
                }
            }
        }

        x += GRAPH_SPACING;
    }

    // Draw coverage of the dependencies that are out of sight.
    painter.setPen(QPen(QColor(0, 158, 255), 2));
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Left side coverage.
    x = columnOffset + 2;

    for (int i = 0; i < scrollIndex_; ++i)
    {
        foreach (GraphDependency const& dep, columns_[i]->dependencies)
        {
            int fromY = 0;
            int toY = 0;

            if (getCoordinates(dep, fromY, toY))
            {
                painter.drawLine(x, fromY, x, toY);
            }
        }
    }

    // Right side coverage.
    x = columnOffset + columnWidth(FileDependencyColumns::DEPENDENCIES) - 2;

    for (int i = scrollIndex_ + maxVisibleGraphColumns_; i < columns_.size(); ++i)
    {
        foreach (GraphDependency const& dep, columns_[i]->dependencies)
        {
            int fromY = 0;
            int toY = 0;

            if (getCoordinates(dep, fromY, toY))
            {
                painter.drawLine(x, fromY, x, toY);
            }
        }
    }

    painter.setRenderHint(QPainter::Antialiasing);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::getCoordinates()
//-----------------------------------------------------------------------------
bool FileDependencyGraphView::getCoordinates(GraphDependency const &dep, int& fromY, int& toY) const
{
    FileDependencyItem* fromItem = model_->findFileItem(dep.dependency->getFile1());
    FileDependencyItem* toItem = model_->findFileItem(dep.dependency->getFile2());

    Q_ASSERT(fromItem != 0);
    Q_ASSERT(toItem != 0);

    // Determine the y coordinates for the dependency.
    // If the item is not visible, reroute the dependency to the missing item's parent.
    if (!getVisualRowY(sortFilter_->mapFromSource(model_->getItemIndex(fromItem, 0)), fromY))
    {
        fromItem = fromItem->getParent();

        if (!getVisualRowY(sortFilter_->mapFromSource(model_->getItemIndex(fromItem, 0)), fromY))
        {
            return false;
        }
    }

    if (!getVisualRowY(sortFilter_->mapFromSource(model_->getItemIndex(toItem, 0)), toY))
    {
        toItem = toItem->getParent();

        if (!getVisualRowY(sortFilter_->mapFromSource(model_->getItemIndex(toItem, 0)), toY))
        {
            return false;
        }
    }

    // The items can be same when both items are not visible and have a same parent.
    // In that case, the dependency is invisible.
    if (fromItem == toItem)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawManualCreationArrow()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::drawManualCreationArrow(QPainter& painter)
{
    int x = columnViewportPosition(FileDependencyColumns::CREATE_DEPENDENCY) + 
        columnWidth(FileDependencyColumns::CREATE_DEPENDENCY) / 2;
    int startY = 0;
    int currentY = 0;

    getVisualRowY(sortFilter_->mapFromSource(model_->getItemIndex(manualDependencyStartItem_, 0)), startY);
    getVisualRowY(sortFilter_->mapFromSource(model_->getItemIndex(manualDependencyEndItem_, 0)), currentY);
    drawArrow(painter, x, startY, currentY, Qt::blue, false);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawArrow()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::drawArrow(QPainter& painter, int x, int fromY, int toY,
    QColor const& color, bool bidirectional)
{
    painter.setPen(QPen(color, 2));
    painter.setBrush(QBrush(color));
    painter.drawLine(x, fromY, x, toY);

    if (bidirectional)
    {
        drawArrowHeadUp(painter, x, qMin(fromY, toY));
        drawArrowHeadDown(painter, x, qMax(fromY, toY));
    }
    else
    {
        // Draw dot at the starting point.
        painter.drawPie(x - DOT_RADIUS, fromY - DOT_RADIUS, DOT_DIAMETER, DOT_DIAMETER, 0, 5760);

        if (fromY < toY)
        {
            drawArrowHeadDown(painter, x, toY);          
        }
        else
        {
            drawArrowHeadUp(painter, x, toY);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::drawArrowHeadUp()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::drawArrowHeadUp(QPainter& painter, int x, int y)
{
    QPoint points[3] = {
        QPoint(x - ARROW_WIDTH, y - POINTER_OFFSET + ARROW_HEIGHT),
        QPoint(x + ARROW_WIDTH, y - POINTER_OFFSET + ARROW_HEIGHT),
        QPoint(x, y - POINTER_OFFSET)
    };

    painter.drawPolygon(points, 3);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::drawArrowHeadDown()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::drawArrowHeadDown(QPainter& painter, int x, int y)
{
    QPoint points[3] = {
        QPoint(x - ARROW_WIDTH, y + POINTER_OFFSET - ARROW_HEIGHT),
        QPoint(x + ARROW_WIDTH, y + POINTER_OFFSET - ARROW_HEIGHT),
        QPoint(x, y + POINTER_OFFSET)
    };

    painter.drawPolygon(points, 3);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::getRowY()
//-----------------------------------------------------------------------------
bool FileDependencyGraphView::getVisualRowY(QModelIndex const& index, int& y) const
{
    QRect rect = visualRect(index);
    
    if (rect.isNull())
    {
        return false;
    }

    y = rect.center().y();
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::hasSpace()
//-----------------------------------------------------------------------------
bool FileDependencyGraphView::hasSpace(GraphColumn const* column, GraphDependency const& dependency) const
{
    int minY = qMin(dependency.fromY, dependency.toY) - SAFE_MARGIN;
    int maxY = qMax(dependency.fromY, dependency.toY) + SAFE_MARGIN;

    foreach (GraphDependency const& dep, column->dependencies)
    {
        if (!(maxY < qMin(dep.fromY, dep.toY) || minY > qMax(dep.fromY, dep.toY)))
        {
            return false;
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
// Function: FileDependencyGraphWidget::drawRow()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::drawRow(QPainter* painter, QStyleOptionViewItem const& option,
                                      QModelIndex const& index) const
{
    QTreeView::drawRow(painter, option, index);

    // Draw a line below the row.
    QRect rowRect = visualRect(index);

    painter->save();
    painter->setPen(QPen(QColor(200, 200, 200), 0));
    painter->drawLine(option.rect.left(), rowRect.bottom(), option.rect.right(), rowRect.bottom());
    painter->restore();
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::findDependencyAt()
//-----------------------------------------------------------------------------
FileDependency* FileDependencyGraphView::findDependencyAt(QPoint const& point) const
{
    int x = columnViewportPosition(FileDependencyColumns::DEPENDENCIES) + GRAPH_MARGIN;

    for (int i = scrollIndex_; i < columns_.size(); i++)
    {
        // Check if the column contains the x coordinate.
        if (qAbs(x - point.x()) <= ARROW_WIDTH + SELECTION_MARGIN)
        {
            // Go through all dependencies in the column until we find a match.
            foreach (GraphDependency const& dep, columns_[i]->dependencies)
            {
                int fromY = 0;
                int toY = 0;

                if (getCoordinates(dep, fromY, toY))
                {
                    // Interval intersection test.
                    if (qMin(fromY, toY) - SELECTION_MARGIN <= point.y() &&
                        point.y() <= qMax(fromY, toY) + SELECTION_MARGIN)
                    {
                        return dep.dependency;
                    }
                }
            }

            break;
        }

        x += GRAPH_SPACING;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::filterDependency()
//-----------------------------------------------------------------------------
bool FileDependencyGraphView::filterDependency(FileDependency const* dependency) const
{
    bool typeFilter = ((filters_ & FILTER_AUTOMATIC) && !dependency->isManual()) ||
                      ((filters_ & FILTER_MANUAL) && dependency->isManual());

    bool wayFilter = ((filters_ & FILTER_TWO_WAY) && dependency->isBidirectional()) ||
                     ((filters_ & FILTER_ONE_WAY) && !dependency->isBidirectional());

    bool extFilter = ((filters_ & FILTER_INTERNAL) && !model_->findFileItem(dependency->getFile2())->isExternal()) ||
                     ((filters_ & FILTER_EXTERNAL) && model_->findFileItem(dependency->getFile2())->isExternal());

    bool changesFilter = !(filters_ & FILTER_DIFFERENCE) ||
                         dependency->getStatus() != FileDependency::STATUS_UNCHANGED;

    bool existFilter = (filters_ & FILTER_DIFFERENCE) || dependency->getStatus() != FileDependency::STATUS_REMOVED;

    return (typeFilter && wayFilter && extFilter && changesFilter && existFilter);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::repaintDependency()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::repaintDependency(FileDependency const* dependency)
{
    // Repaint only the area of the dependency.
    int x = GRAPH_MARGIN - scrollIndex_ * GRAPH_SPACING;

    foreach (GraphColumn const* column, columns_)
    {
        foreach (GraphDependency const& dep, column->dependencies)
        {
            if (dep.dependency == dependency)
            {
                int fromY = 0;
                int toY = 0;

                if (getCoordinates(dep, fromY, toY))
                {
                    repaintArrowRegion(x, fromY, toY);
                }

                break;
            }
        }

        x += GRAPH_SPACING;
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::repaintArrowRegion()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::repaintArrowRegion(int x, int fromY, int toY)
{
    int columnOffset = columnViewportPosition(FileDependencyColumns::DEPENDENCIES);
    viewport()->repaint(QRect(columnOffset + x - ARROW_WIDTH - SAFE_MARGIN,
        qMin(fromY, toY) - POINTER_OFFSET - SAFE_MARGIN,
        2 * (ARROW_WIDTH + SAFE_MARGIN),
        qAbs(toY - fromY) + 2 * (POINTER_OFFSET + SAFE_MARGIN)));

}
//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::createContextMenu()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::createContextMenu(const QPoint& position)
{
    if (!selectedDependency_->isManual())
    {
        return;
    }

    QMenu contextMenu;

    QAction* reverseAction = contextMenu.addAction("Reverse");
    if (selectedDependency_->isLocked() || selectedDependency_->isBidirectional())
    {
        reverseAction->setEnabled(false);
    }
    connect(reverseAction, SIGNAL(triggered()), this, SLOT(onMenuReverse()));

    QAction* directionChangeAction;
    if (selectedDependency_->isBidirectional())
    {
        directionChangeAction = contextMenu.addAction("Make unidirectional");
    }
    else
    {
        directionChangeAction = contextMenu.addAction("Make bidirectional");
    }
    if (selectedDependency_->isLocked())
    {
        directionChangeAction->setEnabled(false);
        directionChangeAction->setCheckable(true);
        directionChangeAction->setChecked(selectedDependency_->isBidirectional());
    }
    connect(directionChangeAction, SIGNAL(triggered()), this, SLOT(onMenuBidirectional()));

    contextMenu.addSeparator();

    QAction* lockAction;
    if (selectedDependency_->isLocked())
    {
        lockAction = contextMenu.addAction("Unlock");
    }
    else
    {
        lockAction = contextMenu.addAction("Lock");
    }
    connect(lockAction, SIGNAL(triggered()), this, SLOT(onMenuLock()));

    contextMenu.addSeparator();

    QAction* deleteAction = contextMenu.addAction("Delete");
    if (selectedDependency_->isLocked())
    {
        deleteAction->setEnabled(false);
    }
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(onMenuDelete()));
    
    // Open the context menu.
    contextMenu.exec(position);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::changeDependencySelection()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::changeDependencySelection(FileDependency* newSelection)
{
    FileDependency* oldDependency = selectedDependency_;
    selectedDependency_ = newSelection;

    repaintDependency(oldDependency);
    repaintDependency(selectedDependency_);                

    emit selectionChanged(selectedDependency_);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::clearDependencySelection()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::clearDependencySelection()
{
    FileDependency* oldDependency = selectedDependency_;
    selectedDependency_ = 0;

    repaintDependency(oldDependency);            
    emit selectionChanged(selectedDependency_);
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::startDependencyDraw()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::startDependencyDraw()
{
    manualDependencyEndItem_ = manualDependencyStartItem_;
    drawingDependency_ = true;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::endDependencyDraw()
//-----------------------------------------------------------------------------
void FileDependencyGraphView::endDependencyDraw()
{
    manualDependencyStartItem_ = 0;
    manualDependencyEndItem_ = 0;
    drawingDependency_ = false;
}

//-----------------------------------------------------------------------------
// Function: FileDependencyGraphView::createManualDependency()
//-----------------------------------------------------------------------------
FileDependency* FileDependencyGraphView::createManualDependency()
{
    FileDependency* createdDependency = new FileDependency();
    createdDependency->setFile1(manualDependencyStartItem_->getPath());
    createdDependency->setFile2(manualDependencyEndItem_->getPath());
    createdDependency->setManual(true);
    createdDependency->setStatus(FileDependency::STATUS_ADDED);
    return createdDependency;
}

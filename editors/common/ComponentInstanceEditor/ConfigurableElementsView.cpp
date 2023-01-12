//-----------------------------------------------------------------------------
// File: ConfigurableElementsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.03.2015
//
// Description:
// A view for configurable element tree model.
//-----------------------------------------------------------------------------

#include "ConfigurableElementsView.h"

#include <common/KactusColors.h>

#include <QMenu>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::ConfigurableElementsView()
//-----------------------------------------------------------------------------
ConfigurableElementsView::ConfigurableElementsView(QWidget* parent):
EditableTableView(parent)
{
    setToolTip(QString());
    removeAction_.setText(tr("Remove configurable element"));
    setSelectionMode(QAbstractItemView::ContiguousSelection);
    setItemsDraggable(false);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::contextMenuEvent()
//-----------------------------------------------------------------------------
void ConfigurableElementsView::contextMenuEvent(QContextMenuEvent* event)
{
    QModelIndex contextMenuIndex = indexAt(event->pos());
    if (!contextMenuIndex.isValid())
    {
        event->accept();
        return;
    }

    QMenu contextMenu;
    if (contextMenuIndex.isValid())
    {
        contextMenu.addAction(&clearAction_);
        contextMenu.addAction(&copyAction_);
        contextMenu.addAction(&pasteAction_);
    }

    if (model()->data(contextMenuIndex, Qt::UserRole).toBool())
    {
        removeAction_.setEnabled(true);
        contextMenu.addAction(&removeAction_);
    }

    contextMenu.exec(event->globalPos());
    event->accept();
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::onClearAction()
//-----------------------------------------------------------------------------
void ConfigurableElementsView::onClearAction()
{
    QModelIndex selectedIndex = currentIndex();
    if (selectedIndex.isValid())
    {
        QString oldValue = model()->data(selectedIndex, Qt::EditRole).toString();
        QString newValue("");

        emit createElementChangeCommand(oldValue, newValue, selectedIndex, model());
    }
}

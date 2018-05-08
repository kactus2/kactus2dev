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
EditableTreeView(false, QString(""), QString(""), QString(tr("Remove configurable element")),
    QString(tr("Remove all unknown configurable elements")) ,parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::~ConfigurableElementsView()
//-----------------------------------------------------------------------------
ConfigurableElementsView::~ConfigurableElementsView()
{

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

    if (model()->data(contextMenuIndex, Qt::UserRole).toBool())
    {
        getRemoveAction()->setEnabled(true);

        QMenu contextMenu;
        contextMenu.addAction(getRemoveAction());

        contextMenu.exec(event->globalPos());
        event->accept();
    }
    else if (!contextMenuIndex.parent().isValid() &&
        model()->data(contextMenuIndex, Qt::ForegroundRole) == KactusColors::ERROR)
    {
        getRemoveAllSubItemsAction()->setEnabled(true);

        QMenu contextMenu;
        contextMenu.addAction(getRemoveAllSubItemsAction());

        contextMenu.exec(event->globalPos());
        event->accept();
    }
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementsView::onClearAction()
//-----------------------------------------------------------------------------
void ConfigurableElementsView::onClearAction()
{
    QModelIndex selectedIndex = currentIndex();
    if (selectedIndex.parent().isValid())
    {
        QString oldValue = model()->data(selectedIndex, Qt::EditRole).toString();
        QString newValue("");

        emit createElementChangeCommand(oldValue, newValue, selectedIndex, model());
    }
}

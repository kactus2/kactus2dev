//-----------------------------------------------------------------------------
// File: PortWireTypeView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 30.11.2017
//
// Description:
// View for port wire type definitions.
//-----------------------------------------------------------------------------

#include "PortWireTypeView.h"

#include <editors/ComponentEditor/ports/PortWireTypeColumns.h>

#include <QMenu>

//-----------------------------------------------------------------------------
// Function: PortWireTypeView::PortWireTypeView()
//-----------------------------------------------------------------------------
PortWireTypeView::PortWireTypeView(QWidget* parent):
EditableTreeView(true, QString(tr("Add type")), QString(tr("Add view reference")), QString(tr("Remove row")),
    QString(tr("Remove all view references")), parent)
{
    setSelectionMode(QAbstractItemView::ContiguousSelection);
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeView::~PortWireTypeView()
//-----------------------------------------------------------------------------
PortWireTypeView::~PortWireTypeView()
{

}

//-----------------------------------------------------------------------------
// Function: PortWireTypeView::contextMenuEvent()
//-----------------------------------------------------------------------------
void PortWireTypeView::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu contextMenu;
    contextMenu.addAction(getAddItemAction());

    QModelIndex contextMenuIndex = indexAt(event->pos());
    if (contextMenuIndex.isValid())
    {
        if (!contextMenuIndex.parent().isValid())
        {
            contextMenu.addAction(getAddSubItemAction());
        }

        contextMenu.addSeparator();

        contextMenu.addAction(getClearAction());
        contextMenu.addAction(getRemoveAction());

        if (!contextMenuIndex.parent().isValid())
        {
            contextMenu.addAction(getRemoveAllSubItemsAction());
        }
    }

    contextMenu.exec(event->globalPos());
    event->accept();
}

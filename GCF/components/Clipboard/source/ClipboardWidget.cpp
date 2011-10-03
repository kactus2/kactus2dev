/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "ClipboardWidget.h"
#include "ui_ClipboardWidget.h"
#include "IClipboard.h"

#include <QItemSelectionModel>
#include <QItemSelection>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>

struct GCF::Components::ClipboardWidgetData
{
    ClipboardWidgetData() : clipboard(0) { }

    Ui::ClipboardWidget ui;
    IClipboard* clipboard;

    ClipboardData clipboardData(const QModelIndex& index);
};

GCF::Components::ClipboardWidget::ClipboardWidget(IClipboard* clipboard, QWidget* parent)
:QWidget(parent)
{
    d = new GCF::Components::ClipboardWidgetData;
    d->clipboard = clipboard;
    d->ui.setupUi(this);

    d->ui.listView->setModel( d->clipboard->activeClipboardModel() );
	d->ui.listView->installEventFilter(this);
	selectionChanged(QItemSelection());

    connect(d->ui.listView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(selectionChanged(QItemSelection)));
}

GCF::Components::ClipboardWidget::~ClipboardWidget()
{
    delete d;
}

QObject* GCF::Components::ClipboardWidget::containerObject()
{
	return this;
}

IClipboard* GCF::Components::ClipboardWidget::clipboard() const
{
    return d->clipboard;
}

void GCF::Components::ClipboardWidget::on_cmdRemoveSelected_clicked()
{
    // Remove the active item in the clipbaord
    QModelIndex ci = d->ui.listView->currentIndex();
    if( !ci.isValid() )
        return;

    int index = ci.row();
	d->clipboard->removeDataAt(index);
}

void GCF::Components::ClipboardWidget::on_cmdClearAll_clicked()
{
    // Clear all items from the clipboard
    d->clipboard->removeAll();
}

void GCF::Components::ClipboardWidget::on_listView_clicked(const QModelIndex& index)
{
    ClipboardData data = d->clipboardData(index);
    QString context = d->clipboard->activeContextName();
    emit dataClicked(context, data, index.row());
}

void GCF::Components::ClipboardWidget::on_listView_doubleClicked(const QModelIndex& index)
{
    ClipboardData data = d->clipboardData(index);
    QString context = d->clipboard->activeContextName();
    emit dataDoubleClicked(context, data, index.row());
}

void GCF::Components::ClipboardWidget::selectionChanged(const QItemSelection& newSelection)
{
    d->ui.cmdRemoveSelected->setEnabled( newSelection.count() );
    d->ui.cmdClearAll->setEnabled( d->clipboard->dataCount() );
}

bool GCF::Components::ClipboardWidget::eventFilter(QObject* obj, QEvent* event)
{
    if(obj != d->ui.listView && obj != d->ui.listView->viewport())
        return false;

    if(event->type() != QEvent::ContextMenu)
        return false;

    QContextMenuEvent* ce = (QContextMenuEvent*)event;
	QModelIndex index = d->ui.listView->indexAt(ce->pos());
    if(!index.isValid())
        return false;

	d->ui.listView->selectionModel()->select(index, QItemSelectionModel::Rows);
	d->ui.listView->update();

    // We wont let the list view handle the context menu event.
    // We will handle it ourselves now.

    // First fetch the data associated with index
    ClipboardData data = d->clipboardData(index);
    QString context = d->clipboard->activeContextName();

    // Construct a context menu
    QMenu menu;

    // Allow listeners to add items into the menu
    emit dataRightClicked(context, data, index.row(), &menu);

    // Now insert our own actions
    if(menu.actions().count())
        menu.addSeparator();
    QAction* removeAction = menu.addAction("Remove");

    // Show the context menu
    QAction* result = menu.exec(QCursor::pos());
    if(result == removeAction)
		d->clipboard->removeDataAt(index.row());

    return true;
}

ClipboardData GCF::Components::ClipboardWidgetData::clipboardData(const QModelIndex& index)
{
    if( !index.isValid() )
        return ClipboardData();

    int row = index.row();
    ClipboardData data = clipboard->data(row);
    return data;
}



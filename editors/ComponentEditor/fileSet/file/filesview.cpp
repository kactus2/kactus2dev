//-----------------------------------------------------------------------------
// File: filesview.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 4.6.2012
//
// Description:
// The view to display the files contained in a file set.
//-----------------------------------------------------------------------------

#include "filesview.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>

#include <QFileDialog>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QStringList>


//-----------------------------------------------------------------------------
// Function: filesview::FilesView()
//-----------------------------------------------------------------------------
FilesView::FilesView(LibraryInterface* handler, QSharedPointer<Component> component, QWidget *parent):
EditableTableView(parent),
handler_(handler),
component_(component),
addByBrowsingAction_(new QAction(tr("Add file(s)..."), this))
{
    connect(addByBrowsingAction_, SIGNAL(triggered()), this, SIGNAL(addItemByBrowsing()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: filesview::~FilesView()
//-----------------------------------------------------------------------------
FilesView::~FilesView()
{

}

//-----------------------------------------------------------------------------
// Function: FilesView::contextMenuEvent()
//-----------------------------------------------------------------------------
void FilesView::contextMenuEvent(QContextMenuEvent* event)
{
    pressedPoint_ = event->pos();

    QModelIndex index = indexAt(pressedPoint_);

    QMenu menu(this);

    menu.addAction(addByBrowsingAction_);
    menu.addSeparator();
    menu.addAction(&addAction_);

    // if at least one valid item is selected
    if (index.isValid())
    {
        menu.addAction(&removeAction_);
        menu.addAction(&clearAction_);
        menu.addAction(&copyAction_);
        menu.addAction(&pasteAction_);
    }

    menu.exec(event->globalPos());

    event->accept();
}

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

#include <QSortFilterProxyModel>
#include <QStringList>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: filesview::FilesView()
//-----------------------------------------------------------------------------
FilesView::FilesView(LibraryInterface* handler, QSharedPointer<Component> component, QWidget *parent):
EditableTableView(parent),
handler_(handler),
component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: filesview::~FilesView()
//-----------------------------------------------------------------------------
FilesView::~FilesView()
{

}

//-----------------------------------------------------------------------------
// Function: filesview::mouseDoubleClickEvent()
//-----------------------------------------------------------------------------
void FilesView::mouseDoubleClickEvent( QMouseEvent* event )
{
	// if there is no item on the clicked position then a new item should be added
	QModelIndex index = indexAt(event->pos());
	if (!index.isValid())
    {
		// ask user to select the files to add
		QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files to add."),
            handler_->getPath(component_->getVlnv()));
		
		if (files.isEmpty())
        {
			event->accept();
			return;
		}

		foreach (QString file, files)
        {
			emit addItem(index, file);
		}

		event->accept();
		return;
	}

	QTableView::mouseDoubleClickEvent(event);
}

//-----------------------------------------------------------------------------
// Function: filesview::onAddAction()
//-----------------------------------------------------------------------------
void FilesView::onAddAction()
{
	QModelIndexList indexes = selectedIndexes();
	QModelIndex posToAdd;

	QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

	if (!indexes.isEmpty())
    {
		qSort(indexes);
		posToAdd = indexes.first();
	}

	// if proxy is used then map the index to source indexes
	if (sortProxy)
    {
		posToAdd = sortProxy->mapToSource(posToAdd);
	}

	QStringList files = QFileDialog::getOpenFileNames(this, tr("Select files to add."),
        handler_->getPath(component_->getVlnv()));

	if (files.isEmpty())
    {
		return;
	}

	foreach (QString file, files)
    {
		emit addItem(posToAdd, file);
	}
}

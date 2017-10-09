//-----------------------------------------------------------------------------
// File: ParametersView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.10.2017
//
// Description:
// View used to display parameters.
//-----------------------------------------------------------------------------

#include "ParametersView.h"

#include <editors/ComponentEditor/parameters/ParameterColumns.h>

#include <QMenu>
#include <QSortFilterProxyModel>

//-----------------------------------------------------------------------------
// Function: ParametersView::ParametersView()
//-----------------------------------------------------------------------------
ParametersView::ParametersView(QWidget* parent):
EditableTableView(parent),
recalculateReferencesAction_(tr("Recalculate references"), this)
{
    setupActions();
}

//-----------------------------------------------------------------------------
// Function: ParametersView::~ParametersView()
//-----------------------------------------------------------------------------
ParametersView::~ParametersView()
{

}

//-----------------------------------------------------------------------------
// Function: ParametersView::contextMenuEvent()
//-----------------------------------------------------------------------------
void ParametersView::contextMenuEvent(QContextMenuEvent* event)
{
	pressedPoint_ = event->pos();

	QModelIndex index = indexAt(pressedPoint_);

	QMenu menu(this);

    addBasicActionsForContextMenu(menu, index);
    addElementCopyActionForContextMenu(menu, index);
    addRecalculateReferencesActionForContextMenu(menu);
    addImportExportActionsForContextMenu(menu);

	menu.exec(event->globalPos());

	event->accept();
}

//-----------------------------------------------------------------------------
// Function: ParametersView::addRecalculateReferencesActionForContextMenu()
//-----------------------------------------------------------------------------
void ParametersView::addRecalculateReferencesActionForContextMenu(QMenu& menu)
{
    QModelIndexList indexList = selectedIndexes();
    foreach (QModelIndex currentIndex, indexList)
    {
        if (!currentIndex.isValid() || currentIndex.column() != ParameterColumns::USAGE_COUNT)
        {
            return;
        }
    }

    menu.addSeparator();
    menu.addAction(&recalculateReferencesAction_);
}

//-----------------------------------------------------------------------------
// Function: ParametersView::setupActions()
//-----------------------------------------------------------------------------
void ParametersView::setupActions()
{
    EditableTableView::setupActions();

    addAction(&recalculateReferencesAction_);
    recalculateReferencesAction_.setToolTip(tr("Recalculate the references made to the parameter"));
    recalculateReferencesAction_.setStatusTip(tr("Recalculate the references made to the parameter"));
    connect(&recalculateReferencesAction_, SIGNAL(triggered()),
        this, SLOT(onRecalculateReferencesAction()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ParametersView::onRecalculateReferencesAction()
//-----------------------------------------------------------------------------
void ParametersView::onRecalculateReferencesAction()
{
    QModelIndexList indexes = selectedIndexes();
    QModelIndexList sortedIndexes;
    QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

    for (int i = 0; i < indexes.size(); ++i)
    {
        QModelIndex newIndex = sortProxy->mapToSource(indexes.at(i));
        sortedIndexes.append(newIndex);
    }

    emit(recalculateReferenceToIndexes(sortedIndexes));
}

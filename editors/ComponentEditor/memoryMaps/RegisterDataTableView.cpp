#include "RegisterDataTableView.h"
#include <QSortFilterProxyModel>
#include <QMenu>
RegisterDataTableView::RegisterDataTableView(QWidget* parent):
    EditableTableView(parent),
    addRFAction_(tr("Add register file"), this)
{
     addAction_.setText(tr("Add register"));
     addAction_.setToolTip(tr("Add a new register to table"));
     addAction_.setStatusTip(tr("Add a new register to table"));
     disconnect(&addAction_, SIGNAL(triggered()),this, SLOT(onAddAction()) );
     connect(&addAction_, SIGNAL(triggered()), this, SLOT(onAddRegisterAction()), Qt::UniqueConnection);


     addAction(&addRFAction_);
     addRFAction_.setToolTip(tr("Add a new register file to table"));
     addRFAction_.setStatusTip(tr("Add a new register file to table"));
     addRFAction_.setShortcut(Qt::CTRL + Qt::Key_Enter);
     addRFAction_.setShortcutContext(Qt::WidgetShortcut);
     connect(&addRFAction_, SIGNAL(triggered()), this, SLOT(onAddRegisterFileAction()), Qt::UniqueConnection);

}



void RegisterDataTableView::contextMenuEvent(QContextMenuEvent* event)
{
    pressedPoint_ = event->pos();

    QModelIndex index = indexAt(pressedPoint_);

    QMenu menu(this);
    addBasicActionsForContextMenu(menu, index);
    menu.insertAction(&addAction_, &addRFAction_);
    addElementCopyActionForContextMenu(menu, index);
    addImportExportActionsForContextMenu(menu);

    menu.exec(event->globalPos());

    event->accept();
}


void RegisterDataTableView::onAddRegisterAction()
{
    QModelIndexList indexes = selectedIndexes();
    QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());
    QModelIndex posToAdd=getPosToAdd(indexes, sortProxy);

    int rowsToAdd = qMax(1, countRows(selectedIndexes()));
    for (int i = 0; i < rowsToAdd; ++i)
    {
        emit addRegister(posToAdd);
    }
    finalizeAddition(posToAdd, sortProxy);
}

void RegisterDataTableView::onAddRegisterFileAction()
{
    QModelIndexList indexes = selectedIndexes();
    QSortFilterProxyModel* sortProxy = dynamic_cast<QSortFilterProxyModel*>(model());

    QModelIndex posToAdd=getPosToAdd(indexes, sortProxy);
    int rowsToAdd = qMax(1, countRows(indexes));
    for (int i = 0; i < rowsToAdd; ++i)
    {
        emit addRegisterFile(posToAdd);
    }
    finalizeAddition(posToAdd,sortProxy);
}


QModelIndex RegisterDataTableView::getPosToAdd(QModelIndexList &indexes, QSortFilterProxyModel* sortProxy){

    QModelIndex posToAdd;


    if (!indexes.isEmpty())
    {
        qSort(indexes);
        posToAdd = indexes.first();

        if (sortProxy)
        {
            posToAdd = sortProxy->mapToSource(posToAdd);
        }
    }
    return posToAdd;
}

void RegisterDataTableView::finalizeAddition(QModelIndex posToAdd,QSortFilterProxyModel* sortProxy){

    QModelIndex firstCreatedIndex;
    int nameColumn = NAME_COLUMN;
    if (posToAdd.isValid() && (posToAdd.sibling(posToAdd.row(), NAME_COLUMN).flags() & Qt::ItemIsEditable) == 0)
    {
        nameColumn++;
    }

    if (posToAdd.isValid())
    {
        firstCreatedIndex = posToAdd.sibling(posToAdd.row(), nameColumn);
        if (sortProxy)
        {
            firstCreatedIndex = sortProxy->mapFromSource(firstCreatedIndex);
        }
    }
    else
    {
        firstCreatedIndex = model()->index(model()->rowCount() - 1, nameColumn);
        if (sortProxy)
        {
            QModelIndex indexOnLastRow = sortProxy->sourceModel()->index(model()->rowCount() - 1, nameColumn);
            firstCreatedIndex = sortProxy->mapFromSource(indexOnLastRow);
        }
    }

    clearSelection();
    setCurrentIndex(firstCreatedIndex);
}




//-----------------------------------------------------------------------------
// File: TransactionalPortsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 05.03.2024
//
// Description:
// View for transactional ports editor.
//-----------------------------------------------------------------------------

#include "TransactionalPortsView.h"

#include "TransactionalPortColumns.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: TransactionalPortsView::onCopyAction()
//-----------------------------------------------------------------------------
void TransactionalPortsView::onCopyAction()
{
    if (!currentIndex().isValid() || currentIndex().column() != TransactionalPortColumns::TYPE_NAME ||
        selectedIndexes().size() != 1)
    {
        PortsView::onCopyAction();
        return;
    }

    QMimeData* mimeData = new QMimeData();
    mimeData->setImageData(currentIndex().data(Qt::EditRole));

    QApplication::clipboard()->setMimeData(mimeData);
}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsView::onPasteAction()
//-----------------------------------------------------------------------------
void TransactionalPortsView::onPasteAction()
{
    if (!currentIndex().isValid() || currentIndex().column() != TransactionalPortColumns::TYPE_NAME ||
        selectedIndexes().size() != 1)
    {
        PortsView::onPasteAction();
        return;
    }

    auto pasteData = QApplication::clipboard()->mimeData()->imageData();
    model()->setData(currentIndex(), pasteData, Qt::EditRole);
}

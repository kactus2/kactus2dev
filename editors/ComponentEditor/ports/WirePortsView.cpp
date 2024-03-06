//-----------------------------------------------------------------------------
// File: WirePortsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 16.02.2024
//
// Description:
// View for wire ports editor.
//-----------------------------------------------------------------------------

#include "WirePortsView.h"

#include "WirePortColumns.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: WirePortsView::onCopyAction()
//-----------------------------------------------------------------------------
void WirePortsView::onCopyAction()
{
    if (!currentIndex().isValid() || currentIndex().column() != WirePortColumns::TYPE_NAME ||
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
// Function: WirePortsView::onPasteAction()
//-----------------------------------------------------------------------------
void WirePortsView::onPasteAction()
{
    if (!currentIndex().isValid() || currentIndex().column() != WirePortColumns::TYPE_NAME ||
        selectedIndexes().size() != 1)
    {
        PortsView::onPasteAction();
        return;
    }

    auto pasteData = QApplication::clipboard()->mimeData()->imageData();
    model()->setData(currentIndex(), pasteData, Qt::EditRole);
}

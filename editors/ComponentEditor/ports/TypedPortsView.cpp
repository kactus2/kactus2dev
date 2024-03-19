//-----------------------------------------------------------------------------
// File: TypedPortsView.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 16.02.2024
//
// Description:
// View for typed ports editors.
//-----------------------------------------------------------------------------

#include "TypedPortsView.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: TypedPortsView::TypedPortsView()
//-----------------------------------------------------------------------------
TypedPortsView::TypedPortsView(int typeColumn, int nameColumn, BusInterfaceInterface* busInterface, QWidget* parent) :
    PortsView(nameColumn, busInterface, parent),
    typeColumn_(typeColumn)
{

}

//-----------------------------------------------------------------------------
// Function: TypedPortsView::onCopyAction()
//-----------------------------------------------------------------------------
void TypedPortsView::onCopyAction()
{
    if (!currentIndex().isValid() || currentIndex().column() != typeColumn_ ||
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
// Function: TypedPortsView::onPasteAction()
//-----------------------------------------------------------------------------
void TypedPortsView::onPasteAction()
{
    if (!currentIndex().isValid() || currentIndex().column() != typeColumn_ ||
        selectedIndexes().size() != 1)
    {
        PortsView::onPasteAction();
        return;
    }

    auto pasteData = QApplication::clipboard()->mimeData()->imageData();
    model()->setData(currentIndex(), pasteData, Qt::EditRole);
}

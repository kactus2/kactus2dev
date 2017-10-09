//-----------------------------------------------------------------------------
// File: ParameterizableItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 06.10.2017
//
// Description:
// Component editor item for handling parameter signals.
//-----------------------------------------------------------------------------

#include "ParameterizableItem.h"

#include <editors/ComponentEditor/itemeditor.h>

//-----------------------------------------------------------------------------
// Function: ParameterizableItem::ParameterizableItem()
//-----------------------------------------------------------------------------
ParameterizableItem::ParameterizableItem(ComponentEditorTreeModel* model, LibraryInterface* libHandler,
    QSharedPointer<Component> component, ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterizableItem::ParameterizableItem()
//-----------------------------------------------------------------------------
ParameterizableItem::ParameterizableItem(LibraryInterface* libHandler, QSharedPointer<Component> component,
    ComponentEditorTreeModel* parent):
ComponentEditorItem(libHandler, component, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterizableItem::~ParameterizableItem()
//-----------------------------------------------------------------------------
ParameterizableItem::~ParameterizableItem()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterizableItem::connectItemEditorToReferenceCounter()
//-----------------------------------------------------------------------------
void ParameterizableItem::connectItemEditorToReferenceCounter()
{
    ComponentEditorItem::connectItemEditorToReferenceCounter();

    connect(editor_, SIGNAL(recalculateReferencesToParameters(QVector<QSharedPointer<Parameter> >)),
        referenceCounter_.data(), SLOT(recalculateReferencesToParameters(QVector<QSharedPointer<Parameter> >)),
        Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ParameterizableItem::connectReferenceTree()
//-----------------------------------------------------------------------------
void ParameterizableItem::connectReferenceTree()
{
    connect(editor_, SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);
}
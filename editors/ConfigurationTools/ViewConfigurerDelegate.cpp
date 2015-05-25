//-----------------------------------------------------------------------------
// File: ViewConfigurerDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2015
//
// Description:
// Delegate for view configurer.
//-----------------------------------------------------------------------------

#include "ViewConfigurerDelegate.h"
#include "ViewConfigurerColumns.h"

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>

#include <library/LibraryManager/libraryhandler.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/vlnv.h>

#include <QPainter>
#include <QPen>

//-----------------------------------------------------------------------------
// Function: ViewConfigurerDelegate::ViewConfigurerDelegate()
//-----------------------------------------------------------------------------
ViewConfigurerDelegate::ViewConfigurerDelegate(LibraryHandler* libraryHandler, QObject* parent):
QStyledItemDelegate(parent),
libraryHandler_(libraryHandler)
{

}

//-----------------------------------------------------------------------------
// Function: ViewConfigurerDelegate::~ViewConfigurerDelegate()
//-----------------------------------------------------------------------------
ViewConfigurerDelegate::~ViewConfigurerDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: ViewConfigurerDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ViewConfigurerDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& /*option*/,
    QModelIndex const& index) const
{
    if (index.column() == ViewConfigurerColumns::INSTANCE_VIEW)
    {
        return createViewSelector(index, parent);
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurerDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ViewConfigurerDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == ViewConfigurerColumns::INSTANCE_VIEW)
    {
        QString text = index.model()->data(index, Qt::DisplayRole).toString();

        ReferenceSelector* viewCombo = qobject_cast<ReferenceSelector*>(editor);

        if (text == ViewConfigurerColumns::NOVIEWTEXT)
        {
            text = "";
        }

        viewCombo->selectItem(text);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurerDelegate::setModelData()
//-----------------------------------------------------------------------------
void ViewConfigurerDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const
{
    if (index.column() == ViewConfigurerColumns::INSTANCE_VIEW)
    {
        ReferenceSelector* viewCombo = qobject_cast<ReferenceSelector*>(editor);
        QString text = viewCombo->currentText();

        if (text.isEmpty())
        {
            text = ViewConfigurerColumns::NOVIEWTEXT;
        }

        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurerDelegate::createViewSelector()
//-----------------------------------------------------------------------------
QWidget* ViewConfigurerDelegate::createViewSelector(QModelIndex const& currentIndex, QWidget* parent) const
{
    QModelIndex vlnvIndex = currentIndex.sibling(currentIndex.row(), ViewConfigurerColumns::ITEM_VLNV);
    QString itemVlnv = vlnvIndex.data(Qt::DisplayRole).toString();

    ReferenceSelector* viewCombo = new ReferenceSelector(parent);

    if (itemVlnv.count(":") == 3)
    {
        VLNV componentVLNV (VLNV::COMPONENT, itemVlnv);

        QSharedPointer<const Component> indexedComponent = libraryHandler_->getModelReadOnly(componentVLNV).
            dynamicCast<const Component>();

        QStringList viewNames = indexedComponent->getViewNames();
        viewCombo->refresh(viewNames);
    }

    return viewCombo;
}

//-----------------------------------------------------------------------------
// Function: ViewConfigurerDelegate::paint()
//-----------------------------------------------------------------------------
void ViewConfigurerDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index)
    const
{
    QStyledItemDelegate::paint(painter, option, index);

    QPen oldPen = painter->pen();
    QPen newPen(Qt::lightGray);
    newPen.setWidth(1);
    painter->setPen(newPen);

    painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());

    painter->setPen(oldPen);
}

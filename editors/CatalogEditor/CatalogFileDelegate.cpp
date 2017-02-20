//-----------------------------------------------------------------------------
// File: CatalogFileDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.02.2017
//
// Description:
// Delegate to provide editors to edit catalog files.
//-----------------------------------------------------------------------------

#include "CatalogFileDelegate.h"

#include "CatalogFileColumns.h"

#include <common/widgets/assistedLineEdit/AssistedLineEdit.h>
#include <common/widgets/vlnvEditor/VLNVContentMatcher.h>

#include <QPen>

//-----------------------------------------------------------------------------
// Function: CatalogFileDelegate::CatalogFileDelegate()
//-----------------------------------------------------------------------------
CatalogFileDelegate::CatalogFileDelegate(LibraryInterface* library, QObject *parent):
QStyledItemDelegate(parent),    
    library_(library),
    matcher_(new VLNVContentMatcher(library)),
    dataTree_(new VLNVDataTree())

{
    
}

//-----------------------------------------------------------------------------
// Function: CatalogFileDelegate::~CatalogFileDelegate()
//-----------------------------------------------------------------------------
CatalogFileDelegate::~CatalogFileDelegate()
{
    delete matcher_;
    delete dataTree_;
}

//-----------------------------------------------------------------------------
// Function: CatalogFileDelegate::sizeHint()
//-----------------------------------------------------------------------------
QSize CatalogFileDelegate::sizeHint(QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    return QStyledItemDelegate::sizeHint(option, index) + QSize(0, 4);
}

//-----------------------------------------------------------------------------
// Function: CatalogFileDelegate::paint()
//-----------------------------------------------------------------------------
void CatalogFileDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    painter->save();

    QPen newPen(Qt::lightGray);
    newPen.setWidth(1);
    painter->setPen(newPen);

    painter->drawLine(option.rect.topRight(), option.rect.bottomRight());

    painter->restore();
}

//-----------------------------------------------------------------------------
// Function: CatalogFileDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget * CatalogFileDelegate::createEditor(QWidget *parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() < CatalogFileColumns::PATH)
    {
        updateSuggestedItems(index);

        AssistedLineEdit* editor = new AssistedLineEdit(parent->window(), parent);
        editor->setContentMatcher(matcher_);
        return editor;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: CatalogFileDelegate::updateSuggestedItems()
//-----------------------------------------------------------------------------
void CatalogFileDelegate::updateSuggestedItems(QModelIndex const& index) const
{
    QList<VLNV::IPXactType> types;
    VLNV::IPXactType type = CatalogFileColumns::CATEGORY_TYPES[index.parent().row()];
    types.append(type);

    dataTree_->clear();
    dataTree_->parse(library_, types);

    VLNVDataNode const* dataNode = dataTree_;
    for (int column = CatalogFileColumns::VENDOR; column < index.column(); column++)
    {
        if (dataNode)
        {
            QString previousLevel = index.sibling(index.row(), column).data(Qt::DisplayRole).toString();
            dataNode = dataNode->findChild(previousLevel);
        }
    }

    matcher_->setDataNode(dataNode);
}

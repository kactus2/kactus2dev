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

#include <QPen>
#include <QIcon>
#include <QApplication>
#include <QMouseEvent>

//-----------------------------------------------------------------------------
// Function: CatalogFileDelegate::CatalogFileDelegate()
//-----------------------------------------------------------------------------
CatalogFileDelegate::CatalogFileDelegate(QObject *parent):
QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: CatalogFileDelegate::~CatalogFileDelegate()
//-----------------------------------------------------------------------------
CatalogFileDelegate::~CatalogFileDelegate()
{

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
    QStyleOptionViewItemV4 viewItemOption(option);

    QStyledItemDelegate::paint(painter, viewItemOption, index);

    painter->save();

    QPen newPen(QStringLiteral("gainsboro"));
    newPen.setWidth(1);
    painter->setPen(newPen);

    painter->drawLine(option.rect.topRight(), option.rect.bottomRight());
    painter->drawLine(option.rect.bottomLeft(), option.rect.bottomRight());

    painter->restore();
}

//-----------------------------------------------------------------------------
// File: FileDependencyDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 15.02.2013
//
// Description:
// Delegate used in visualizing the file dependency model.
//-----------------------------------------------------------------------------

#include "FileDependencyDelegate.h"

#include "FileDependencyModel.h"

#include <QPainter>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::FileDependencyDelegate()
//-----------------------------------------------------------------------------
FileDependencyDelegate::FileDependencyDelegate(QObject* parent /*= 0*/)
    : QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::~FileDependencyDelegate()
//-----------------------------------------------------------------------------
FileDependencyDelegate::~FileDependencyDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::paint()
//-----------------------------------------------------------------------------
void FileDependencyDelegate::paint(QPainter *painter, QStyleOptionViewItem const& option,
                                   QModelIndex const& index) const
{
    QStyleOptionViewItemV4 viewItemOption(option);

//     if (index.column() == FILE_DEPENDENCY_COLUMN_STATUS)
//     {
//         // Draw the background as usual.
//         drawBackground(painter, option, index);
// 
//         // Draw the icon centered.
//         QIcon icon = qvariant_cast<QIcon>(index.model()->data(index, Qt::DecorationRole));
//         const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
// 
//         QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
//                                             QSize(option.decorationSize.width() + 5, option.decorationSize.height()),
//                                             QRect(option.rect.x() + textMargin, option.rect.y(),
//                                             option.rect.width() - (2 * textMargin), option.rect.height()));
// 
//         drawDecoration(painter, viewItemOption, viewItemOption.rect, icon.pixmap(option.decorationSize));
// 
//         // Draw the focus.
//         drawFocus(painter, option, option.rect);
//     }
//     else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: FileDependencyDelegate::sizeHint()
//-----------------------------------------------------------------------------
QSize FileDependencyDelegate::sizeHint(QStyleOptionViewItem const& option,
                                       QModelIndex const& index) const
{
    return QStyledItemDelegate::sizeHint(option, index) + QSize(0, 4);
}

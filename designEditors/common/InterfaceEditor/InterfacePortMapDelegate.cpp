//-----------------------------------------------------------------------------
// File: InterfacePortMapDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.05.2016
//
// Description:
// Delegate for the visualizing port maps of a bus interface.
//-----------------------------------------------------------------------------

#include "InterfacePortMapDelegate.h"
#include "InterfacePortMapColumns.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: InterfacePortMapDelegate::InterfacePortMapDelegate()
//-----------------------------------------------------------------------------
InterfacePortMapDelegate::InterfacePortMapDelegate(QObject *parent):
QStyledItemDelegate(parent),
portMapGroupModify_(false),
portMapGroupState_(Qt::Unchecked)
{

}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapDelegate::~InterfacePortMapDelegate()
//-----------------------------------------------------------------------------
InterfacePortMapDelegate::~InterfacePortMapDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapDelegate::paint()
//-----------------------------------------------------------------------------
void InterfacePortMapDelegate::paint(QPainter *painter, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    QStyleOptionViewItemV4 viewItemOption(option);

    if (index.column() == InterfacePortMapColumns::INTERFACE_EXCLUDE)
    {
        QVariant colourVariant = index.data(Qt::BackgroundRole);
        QColor backGroudColor = colourVariant.value<QColor>();

        painter->fillRect(option.rect, backGroudColor);

        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
            QSize(option.decorationSize.width() + 5, option.decorationSize.height()),
            QRect(option.rect.x() + textMargin, option.rect.y(),
            option.rect.width() - (2 * textMargin), option.rect.height()));

        viewItemOption.rect = newRect;
    }

    QStyledItemDelegate::paint(painter, viewItemOption, index);
}

//-----------------------------------------------------------------------------
// Function: InterfacePortMapDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool InterfacePortMapDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
    QStyleOptionViewItem const& option, QModelIndex const& index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // Always reset the ad-hoc group modify flag.
    if (event->type() == QEvent::MouseButtonRelease)
    {
        portMapGroupModify_ = false;
    }

    // Make sure that the item is checkable.
    Qt::ItemFlags flags = model->flags(index);

    if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
    {
        return false;
    }

    // Make sure that we have a check state.
    QVariant value = index.data(Qt::CheckStateRole);

    if (!value.isValid())
    {
        return false;
    }

    Qt::CheckState newState = static_cast<Qt::CheckState>(value.toInt());

    // Handle the mouse button events.
    if (event->type() == QEvent::MouseButtonPress)
    {
        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
            option.decorationSize,
            QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
            option.rect.width() - (2 * textMargin),
            option.rect.height()));

        if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
        {
            return false;
        }

        newState = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked ? Qt::Unchecked : Qt::Checked);
        portMapGroupModify_ = true;
        portMapGroupState_ = newState;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        if (!portMapGroupModify_ || static_cast<Qt::CheckState>(value.toInt()) == portMapGroupState_)
        {
            return false;
        }

        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;

        QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
            option.decorationSize,
            QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
            option.rect.width() - (2 * textMargin),
            option.rect.height()));

        if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
        {
            return false;
        }

        newState = portMapGroupState_;
    }
    else if (event->type() == QEvent::KeyPress)
    {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space &&
            static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return model->setData(index, newState, Qt::CheckStateRole);
}

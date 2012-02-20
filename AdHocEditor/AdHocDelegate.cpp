//-----------------------------------------------------------------------------
// File: AdHocDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.2.2012
//
// Description:
// Delegate for the table visualizing ad-hoc visibility for component ports.
//-----------------------------------------------------------------------------

#include "AdHocDelegate.h"

#include <QLabel>
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: AdHocDelegate::AdHocDelegate()
//-----------------------------------------------------------------------------
AdHocDelegate::AdHocDelegate(QObject *parent /*= 0*/) : QStyledItemDelegate(parent), adhocGroupModify_(false),
                                                        adhocGroupState_(Qt::Unchecked)
{

}

//-----------------------------------------------------------------------------
// Function: AdHocDelegate::~AdHocDelegate()
//-----------------------------------------------------------------------------
AdHocDelegate::~AdHocDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AdHocDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
                                     QModelIndex const& index) const
{
    // if the column is the one specified for direction items 
    switch (index.column())
    {
    case ADHOC_COL_NAME:
    case ADHOC_COL_DIRECTION:
        {
            QLabel* label = new QLabel(parent);
            return label;
        }

    default:
        {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AdHocDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    switch (index.column())
    {
    case ADHOC_COL_NAME:
    case ADHOC_COL_DIRECTION:
        {
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            QLabel* label = qobject_cast<QLabel*>(editor);
            label->setText(text);
            break;
        }

    default:
        {
            QStyledItemDelegate::setEditorData(editor, index);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocDelegate::setModelData()
//-----------------------------------------------------------------------------
void AdHocDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                 QModelIndex const& index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

//-----------------------------------------------------------------------------
// Function: AdHocDelegate::paint()
//-----------------------------------------------------------------------------
void AdHocDelegate::paint(QPainter *painter, QStyleOptionViewItem const& option,
                          QModelIndex const& index) const
{
    QStyleOptionViewItemV4 viewItemOption(option);

    if (index.column() == ADHOC_COL_VISIBILITY)
    {
        painter->fillRect(option.rect, Qt::white);

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
// Function: AdHocDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool AdHocDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
                                QStyleOptionViewItem const& option, QModelIndex const& index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // Always reset the ad-hoc group modify flag.
    if (event->type() == QEvent::MouseButtonRelease)
    {
        adhocGroupModify_ = false;
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

    Qt::CheckState newState;

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
        adhocGroupModify_ = true;
        adhocGroupState_ = newState;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        if (!adhocGroupModify_ || static_cast<Qt::CheckState>(value.toInt()) == adhocGroupState_)
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

        newState = adhocGroupState_;
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

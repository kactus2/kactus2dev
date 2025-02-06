//-----------------------------------------------------------------------------
// File: SVDCPUDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.05.2021
//
// Description:
// The delegate that provides editors to edit SVD CPU details.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreeCPUDelegate.h"

#include <Plugins/LinuxDeviceTree/CPUSelection/LinuxDeviceTreeCPUColumns.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

//-----------------------------------------------------------------------------
// Function: SVDCPUDelegate::LinuxDeviceTreeCPUDelegate()
//-----------------------------------------------------------------------------
LinuxDeviceTreeCPUDelegate::LinuxDeviceTreeCPUDelegate(QObject* parent):
QStyledItemDelegate(parent),
booleanModify_(false),
booleanState_(Qt::Unchecked)
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreeCPUDelegate::editorEvent(QEvent *event, QAbstractItemModel* model,
    QStyleOptionViewItem const& option, QModelIndex const& index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // Always reset the ad-hoc group modify flag.
    if (event->type() == QEvent::MouseButtonRelease)
    {
        booleanModify_ = false;
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
        booleanModify_ = true;
        booleanState_ = newState;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        if (!booleanModify_ || static_cast<Qt::CheckState>(value.toInt()) == booleanState_)
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
        
        newState = booleanState_;
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

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreeCPUDelegate::paint()
//-----------------------------------------------------------------------------
void LinuxDeviceTreeCPUDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
	QStyleOptionViewItem viewItemOption(option);

    if (index.column() == LinuxDeviceTreeCPUColumns::CREATEDEVICETREE)
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

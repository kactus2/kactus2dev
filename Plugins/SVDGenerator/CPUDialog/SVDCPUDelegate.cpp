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

#include "SVDCPUDelegate.h"

#include <Plugins/SVDGenerator/CPUDialog/SVDCPUColumns.h>
#include <Plugins/SVDGenerator/ConnectivityGraphUtilities.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: SVDCPUDelegate::SVDCPUDelegate()
//-----------------------------------------------------------------------------
SVDCPUDelegate::SVDCPUDelegate(QObject* parent):
QStyledItemDelegate(parent),
booleanModify_(false),
booleanState_(Qt::Unchecked)
{

}

//-----------------------------------------------------------------------------
// Function: SVDCPUDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* SVDCPUDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == SVDCPUColumns::ENDIAN)
    {
        QComboBox* editor(new QComboBox(parent));
        return editor;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDelegate::setEditorData()
//-----------------------------------------------------------------------------
void SVDCPUDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == SVDCPUColumns::ENDIAN)
    {
        QComboBox* endianEditor = dynamic_cast<QComboBox*>(editor);
        if (endianEditor)
        {
            QString currentEndian = index.data(Qt::DisplayRole).toString();

            endianEditor->addItem("");
            endianEditor->addItem("little");
            endianEditor->addItem("big");
            endianEditor->addItem("selectable");
            endianEditor->addItem("other");

            endianEditor->setCurrentIndex(endianEditor->findText(currentEndian));
        }
    }
    else if (index.column() != SVDCPUColumns::REVISION ||
        (index.column() == SVDCPUColumns::REVISION &&
            index.data().toString() != ConnectivityGraphUtilities::REVISION_FORMAT))
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: SVDCPUDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool SVDCPUDelegate::editorEvent(QEvent *event, QAbstractItemModel* model, QStyleOptionViewItem const& option,
    QModelIndex const& index)
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
// Function: SVDCPUDelegate::paint()
//-----------------------------------------------------------------------------
void SVDCPUDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
	QStyleOptionViewItem viewItemOption(option);

    if (index.column() == SVDCPUColumns::CREATESVD || index.column() == SVDCPUColumns::MPUPRESENT ||
        index.column() == SVDCPUColumns::FPUPRESENT || index.column() == SVDCPUColumns::VENDORSYSTICKCONFIG)
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

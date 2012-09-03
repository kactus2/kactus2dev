//-----------------------------------------------------------------------------
// File: AddressDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.08.2012
//
// Description:
// The delegate that provides widgets for the address editor.
//-----------------------------------------------------------------------------

#include "AddressDelegate.h"

#include <QLineEdit>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

//-----------------------------------------------------------------------------
// Function: AddressDelegate::AddressDelegate()
//-----------------------------------------------------------------------------
AddressDelegate::AddressDelegate(QObject* parent /*= 0*/)
    : QStyledItemDelegate(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AddressDelegate::~AddressDelegate()
//-----------------------------------------------------------------------------
AddressDelegate::~AddressDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: AddressDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* AddressDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
    switch (index.column())
    {
    case ADDRESS_COL_START_ADDRESS:
        {
            QLineEdit* line = new QLineEdit(parent);
//             connect(line, SIGNAL(editingFinished()),
//                 this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return line;
        }

//     case ADHOC_COL_NAME:
//     case ADHOC_COL_DIRECTION:
//         {
//             QLabel* label = new QLabel(parent);
//             return label;
//         }

    default:
        {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressDelegate::setEditorData()
//-----------------------------------------------------------------------------
void AddressDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    switch (index.column())
    {
    case ADDRESS_COL_START_ADDRESS:
        {
            QLineEdit* line = qobject_cast<QLineEdit*>(editor);
            Q_ASSERT_X(line, "AddressDelegate::setEditorData", "Type conversion failed for QLineEdit");

            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            line->setText(text);
            return;
        }

//     case ADHOC_COL_NAME:
//     case ADHOC_COL_DIRECTION:
//         {
//             QString text = index.model()->data(index, Qt::DisplayRole).toString();
//             QLabel* label = qobject_cast<QLabel*>(editor);
//             label->setText(text);
//             break;
//         }

    default:
        {
            QStyledItemDelegate::setEditorData(editor, index);
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressDelegate::setModelData()
//-----------------------------------------------------------------------------
void AddressDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    switch (index.column())
    {
    case ADDRESS_COL_START_ADDRESS:
        {
            QLineEdit* line = qobject_cast<QLineEdit*>(editor);
            Q_ASSERT_X(line, "AddressDelegate::setEditorData", "Type conversion failed for QLineEdit");

            QString text = line->text();
            model->setData(index, text, Qt::EditRole);
            return;
        }

    default:
        {
            QStyledItemDelegate::setModelData(editor, model, index);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressDelegate::paint()
//-----------------------------------------------------------------------------
void AddressDelegate::paint(QPainter *painter, QStyleOptionViewItem const& option,
                            QModelIndex const& index) const
{
    QStyleOptionViewItemV4 viewItemOption(option);

    if (index.column() == ADDRESS_COL_LOCKED)
    {
        //painter->fillRect(option.rect, Qt::white);

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
// Function: AddressDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool AddressDelegate::editorEvent(QEvent* event, QAbstractItemModel* model,
                                  QStyleOptionViewItem const& option, QModelIndex const& index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // Always reset the ad-hoc group modify flag.
    if (event->type() == QEvent::MouseButtonRelease)
    {
        groupModify_ = false;
    }

    // Make sure that the item is checkable.
    Qt::ItemFlags flags = model->flags(index);

    if (!(flags & Qt::ItemIsEnabled) || index.column() != ADDRESS_COL_LOCKED)
    {
        return false;
    }

    // Make sure that we have a check state.
    QVariant value = index.data(Qt::UserRole);

    if (!value.isValid())
    {
        return false;
    }

    bool newState;

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

        newState = !value.toBool();
        groupModify_ = true;
        groupState_ = newState;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        if (!groupModify_ || value.toBool() == groupState_)
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

        newState = groupState_;
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

    return model->setData(index, newState, Qt::UserRole);
}

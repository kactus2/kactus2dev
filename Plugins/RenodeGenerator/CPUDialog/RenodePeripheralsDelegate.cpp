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

#include "RenodePeripheralsDelegate.h"

#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QComboBox>
#include <QDir>

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::RenodePeripheralsDelegate()
//-----------------------------------------------------------------------------
RenodePeripheralsDelegate::RenodePeripheralsDelegate(QObject* parent /* = 0 */):
QStyledItemDelegate(parent),
booleanModify_(false),
booleanState_(Qt::Unchecked),
currentFolder_("")
{

}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::onFolderChanged()
//-----------------------------------------------------------------------------
void RenodePeripheralsDelegate::onFolderChanged(QString const& newFolder)
{
    currentFolder_ = newFolder;
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* RenodePeripheralsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == PeripheralColumns::CLASS)
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
// Function: RenodePeripheralsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void RenodePeripheralsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == PeripheralColumns::CLASS)
    {
        QComboBox* classEditor = dynamic_cast<QComboBox*>(editor);
        classEditor->setEditable(true);
        if (classEditor)
        {
            QString filePath = QDir::currentPath() + "/Plugins/RenodeGenerator/peripherals.txt";
            QFile peripheralClassFile(filePath);
            if (peripheralClassFile.open(QIODevice::ReadOnly))
            {
                QTextStream fileStream(&peripheralClassFile);
                while (!fileStream.atEnd())
                {
                    classEditor->addItem(fileStream.readLine());
                }

                peripheralClassFile.close();
            }

            QString currentClass = index.data(Qt::DisplayRole).toString();
            classEditor->setCurrentIndex(classEditor->findText(currentClass));
        }
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::setModelData()
//-----------------------------------------------------------------------------
void RenodePeripheralsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    QStyledItemDelegate::setModelData(editor, model, index);
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsDelegate::editorEvent()
//-----------------------------------------------------------------------------
bool RenodePeripheralsDelegate::editorEvent(QEvent *event, QAbstractItemModel* model, QStyleOptionViewItem const& option,
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
// Function: RenodePeripheralsDelegate::paint()
//-----------------------------------------------------------------------------
void RenodePeripheralsDelegate::paint(QPainter* painter, QStyleOptionViewItem const& option, QModelIndex const& index) const
{
	QStyleOptionViewItem viewItemOption(option);

    if (index.column() == PeripheralColumns::INITABLE)
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

//-----------------------------------------------------------------------------
// File: ComPropertyDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.4.2012
//
// Description:
// Delegate for editing COM properties.
//-----------------------------------------------------------------------------

#include "ComPropertyDelegate.h"

#include <QComboBox>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: ComPropertyDelegate::ComPropertyDelegate()
//-----------------------------------------------------------------------------
ComPropertyDelegate::ComPropertyDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: ComPropertyDelegate::~ComPropertyDelegate()
//-----------------------------------------------------------------------------
ComPropertyDelegate::~ComPropertyDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: ComPropertyDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ComPropertyDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
                                             QModelIndex const& index) const
{
    switch (index.column())
    {
    case PROPERTY_COL_NAME:
    case PROPERTY_COL_DEFAULT:
    case PROPERTY_COL_DESC:
        {
            QLineEdit* line = new QLineEdit(parent);
            connect(line, SIGNAL(editingFinished()),
                    this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return line;
        }

    case PROPERTY_COL_TYPE:
        {
            QComboBox* box = new QComboBox(parent);
            box->setEditable(true);
            box->setInsertPolicy(QComboBox::InsertAlphabetically);

            box->addItem("integer");
            box->addItem("ip_address");
            box->addItem("string");
            return box;
        }

    case PROPERTY_COL_REQUIRED:
        {
            QComboBox* box = new QComboBox(parent);
            box->addItem("false");
            box->addItem("true");
            return box;
        }

    default:
        {
            return QStyledItemDelegate::createEditor(parent, option, index);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComPropertyDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ComPropertyDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    switch (index.column())
    {
    case PROPERTY_COL_TYPE:
    case PROPERTY_COL_REQUIRED:
        {
            QComboBox* box = qobject_cast<QComboBox*>(editor);
            Q_ASSERT_X(box, "ComPropertyDelegate::setEditorData", "Type conversion failed for QComboBox");

            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            box->setCurrentIndex(box->findText(text));
            return;
        }

    case PROPERTY_COL_NAME:
    case PROPERTY_COL_DEFAULT:
    case PROPERTY_COL_DESC:
        {
            QLineEdit* line = qobject_cast<QLineEdit*>(editor);
            Q_ASSERT_X(line, "ComPropertyDelegate::setEditorData", "Type conversion failed for QLineEdit");

            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            line->setText(text);
            return;
        }

    default:
        {
            QStyledItemDelegate::setEditorData(editor, index);
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComPropertyDelegate::setModelData()
//-----------------------------------------------------------------------------
void ComPropertyDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    switch (index.column())
    {
    case PROPERTY_COL_TYPE:
    case PROPERTY_COL_REQUIRED:
        {
            QComboBox* box = qobject_cast<QComboBox*>(editor);
            Q_ASSERT_X(box, "ComPropertyDelegate::setEditorData", "Type conversion failed for QComboBox");

            QString text = box->currentText();
            model->setData(index, text, Qt::EditRole);
            return;
        }

    case PROPERTY_COL_NAME:
    case PROPERTY_COL_DEFAULT:
    case PROPERTY_COL_DESC:
        {
            QLineEdit* line = qobject_cast<QLineEdit*>(editor);
            Q_ASSERT_X(line, "ComPropertyDelegate::setEditorData", "Type conversion failed for QLineEdit");

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
// Function: ComPropertyDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void ComPropertyDelegate::commitAndCloseEditor()
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());

    if (lineEdit != 0)
    {
        emit commitData(lineEdit);
        emit closeEditor(lineEdit);
    }
}

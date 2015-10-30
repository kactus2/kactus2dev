//-----------------------------------------------------------------------------
// File: PropertyValueDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 16.4.2012
//
// Description:
// Delegate for property values.
//-----------------------------------------------------------------------------

#include "PropertyValueDelegate.h"

#include <IPXACTmodels/kactusExtensions/ComProperty.h>

#include <QComboBox>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: PropertyValueDelegate::PropertyValueDelegate()
//-----------------------------------------------------------------------------
PropertyValueDelegate::PropertyValueDelegate(QObject* parent):
QStyledItemDelegate(parent),
allowedProperties_()
{

}

//-----------------------------------------------------------------------------
// Function: PropertyValueDelegate::~PropertyValueDelegate()
//-----------------------------------------------------------------------------
PropertyValueDelegate::~PropertyValueDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: PropertyValueDelegate::setAllowedProperties()
//-----------------------------------------------------------------------------
void PropertyValueDelegate::setAllowedProperties(QList< QSharedPointer<ComProperty> > properties)
{
    allowedProperties_ = properties;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PropertyValueDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == 0)
    {
        QComboBox* box = new QComboBox(parent);

        foreach (QSharedPointer<ComProperty const> comProperty, allowedProperties_)
        {
            if (!comProperty->isRequired())
            {
                box->addItem(comProperty->name());
            }
        }

        return box;
    }

    else if (index.column() == 1)
    {
        QLineEdit* line = new QLineEdit(parent);
        connect(line, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return line;
    }

    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PropertyValueDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == 0)
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        Q_ASSERT_X(box, "PropertyValueDelegate::setEditorData", "Type conversion failed for QComboBox");

        QString text = index.data(Qt::DisplayRole).toString();
        box->setCurrentIndex(box->findText(text));
        return;
    }

    else if (index.column() == 1)
    {
        QLineEdit* line = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(line, "PropertyValueDelegate::setEditorData", "Type conversion failed for QLineEdit");

        QString text = index.data(Qt::DisplayRole).toString();
        line->setText(text);
        return;
    }

    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
        return;
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueDelegate::setModelData()
//-----------------------------------------------------------------------------
void PropertyValueDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, 
    QModelIndex const& index) const
{
    if (index.column() == 0)
    {
        QComboBox* box = qobject_cast<QComboBox*>(editor);
        Q_ASSERT_X(box, "PropertyValueDelegate::setEditorData", "Type conversion failed for QComboBox");

        QString text = box->currentText();
        model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == 1)
    {
        QLineEdit* line = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT_X(line, "PropertyValueDelegate::setEditorData", "Type conversion failed for QLineEdit");

        QString text = line->text();
        model->setData(index, text, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void PropertyValueDelegate::commitAndCloseEditor()
{
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());

    if (lineEdit != 0)
    {
        emit commitData(lineEdit);
        emit closeEditor(lineEdit);
    }
}

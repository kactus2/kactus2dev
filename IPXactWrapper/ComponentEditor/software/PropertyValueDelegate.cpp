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

#include <models/ComProperty.h>

#include <QComboBox>
#include <QLineEdit>

//-----------------------------------------------------------------------------
// Function: PropertyValueDelegate::PropertyValueDelegate()
//-----------------------------------------------------------------------------
PropertyValueDelegate::PropertyValueDelegate(QObject* parent)
    : QStyledItemDelegate(parent),
      m_allowedProperties(0)
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
void PropertyValueDelegate::setAllowedProperties(QList< QSharedPointer<ComProperty> > const* properties)
{
    m_allowedProperties = properties;
}

//-----------------------------------------------------------------------------
// Function: PropertyValueDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PropertyValueDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
                                             QModelIndex const& index) const
{
    switch (index.column())
    {
        // If selecting the name of the property.
        case 0:
            {
                QComboBox* box = new QComboBox(parent);
                // set box to be editable
                //box->setEditable(true);

                if (m_allowedProperties != 0)
                {
                    // Fill in with allowed values from the list of properties.
                    foreach (QSharedPointer<ComProperty const> prop, *m_allowedProperties)
                    {
                        if (!prop->isRequired())
                        {
                            box->addItem(prop->getName());
                        }
                    }
                }
                
                return box;
            }

        // If editing the value of a property.
        case 1:
            {
                QLineEdit* line = new QLineEdit(parent);
                connect(line, SIGNAL(editingFinished()),
                    this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
                return line;
            }

        default:
            {
                return QStyledItemDelegate::createEditor(parent, option, index);
            }
    }
}

//-----------------------------------------------------------------------------
// Function: PropertyValueDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PropertyValueDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    switch (index.column())
    {
    // If the name of the property.
    case 0:
        {

            QComboBox* box = qobject_cast<QComboBox*>(editor);
            Q_ASSERT_X(box, "PropertyValueDelegate::setEditorData", "Type conversion failed for QComboBox");

            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            box->setCurrentIndex(box->findText(text));
            return;
        }

    // If the value of the property.
    case 1:
        {
            QLineEdit* line = qobject_cast<QLineEdit*>(editor);
            Q_ASSERT_X(line, "PropertyValueDelegate::setEditorData", "Type conversion failed for QLineEdit");

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
// Function: PropertyValueDelegate::setModelData()
//-----------------------------------------------------------------------------
void PropertyValueDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    switch (index.column())
    {
    // If the name of the property.
    case 0:
        {
            QComboBox* box = qobject_cast<QComboBox*>(editor);
            Q_ASSERT_X(box, "PropertyValueDelegate::setEditorData", "Type conversion failed for QComboBox");

            QString text = box->currentText();
            model->setData(index, text, Qt::EditRole);
            return;
        }

    // If the value of the property.
    case 1:
        {
            QLineEdit* line = qobject_cast<QLineEdit*>(editor);
            Q_ASSERT_X(line, "PropertyValueDelegate::setEditorData", "Type conversion failed for QLineEdit");

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

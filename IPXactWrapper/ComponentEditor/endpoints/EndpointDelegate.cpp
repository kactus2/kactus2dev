//-----------------------------------------------------------------------------
// File: EndpointDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.9.2011
//
// Description:
// Endpoint delegate for the endpoint editor.
//-----------------------------------------------------------------------------

#include "EndpointDelegate.h"

#include <QComboBox>
#include <QLineEdit>

EndpointDelegate::EndpointDelegate(QObject *parent /*= 0*/ ) : QStyledItemDelegate(parent)
{
}

EndpointDelegate::~EndpointDelegate() {
}

QWidget* EndpointDelegate::createEditor(QWidget* parent, 
                                        const QStyleOptionViewItem& option, 
                                        const QModelIndex& index ) const
{
    switch (index.column())
    {
    case 0:
    case 3:
        {
            // use line edit to edit all other columns
            QLineEdit* editor = new QLineEdit(parent);
            connect(editor, SIGNAL(editingFinished()),
                    this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return editor;
        }

    case 1:
        {
            QComboBox* combo = new QComboBox(parent);
            combo->addItem(QString("message"));
            combo->addItem(QString("packet"));
            combo->addItem(QString("scalar"));
            connect(combo, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return combo;
        }

    case 2:
        {
            QComboBox* combo = new QComboBox(parent);
            combo->addItem(QString("in"));
            combo->addItem(QString("out"));
            connect(combo, SIGNAL(currentIndexChanged(int)),
                this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
            return combo;
        }

    default: {
        return QStyledItemDelegate::createEditor(parent, option, index);
             }
    }
}

void EndpointDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const
{
    switch (index.column())
    {
    case 0:
    case 3:
        {
            // use the line edit for other columns
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
            lineEdit->setText(text);
            break;
        }

    case 1:
    case 2:
        {
            QString text = index.model()->data(index, Qt::DisplayRole).toString();
            QComboBox* combo = qobject_cast<QComboBox*>(editor);

            int comboIndex = combo->findText(text);
            combo->setCurrentIndex(comboIndex);
            break;
        }
    }
}

void EndpointDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                    const QModelIndex& index) const
{
    switch (index.column())
    {
    case 0:
    case 3:
        {
            // get the text from line edit for other columns
            QLineEdit* lineEdit = qobject_cast<QLineEdit*>(editor);
            QString text = lineEdit->text();
            model->setData(index, text, Qt::EditRole);
            break;
        }

    case 1:
    case 2:
        {
            QComboBox* combo = qobject_cast<QComboBox*>(editor);
            QString text = combo->currentText();
            model->setData(index, text, Qt::EditRole);
            break;
        }
    }
}

void EndpointDelegate::commitAndCloseEditor() {

    // try to get pointer to editor in both cases
    QComboBox* combo = qobject_cast<QComboBox*>(sender());
    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());

    // if the editor was combo box
    if (combo) {
        emit commitData(combo);
        emit closeEditor(combo);
    }

    // if editor was line edit
    else {
        emit commitData(lineEdit);
        emit closeEditor(lineEdit);
    }
}

//-----------------------------------------------------------------------------
// File: apiinterfacesdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.06.2012
//
// Description:
// The delegate that provides editors to edit the API definitions.
//-----------------------------------------------------------------------------

#include "apiinterfacesdelegate.h"

#include "ApiInterfaceColumns.h"

#include <QLineEdit>
#include <QComboBox>

//-----------------------------------------------------------------------------
// Function: ApiInterfacesDelegate::ApiInterfacesDelegate()
//-----------------------------------------------------------------------------
ApiInterfacesDelegate::ApiInterfacesDelegate(QObject *parent): QStyledItemDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesDelegate::~ApiInterfacesDelegate()
//-----------------------------------------------------------------------------
ApiInterfacesDelegate::~ApiInterfacesDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ApiInterfacesDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, 
    QModelIndex const& index) const
{
    if (index.column() == ApiInterfaceColumns::NAME || index.column() == ApiInterfaceColumns::DISPLAY_NAME ||
        index.column() == ApiInterfaceColumns::DESCRIPTION)
    {
        QLineEdit* edit = new QLineEdit(parent);
        connect(edit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return edit;
    }
    else if (index.column() == ApiInterfaceColumns::API_DEFINITION)
    {
        Q_ASSERT(false);
        return 0;
    }
    else if (index.column() == ApiInterfaceColumns::DEPENDENCY)
    {
        QComboBox* combo = new QComboBox(parent);
        combo->addItem(tr("requester"));
        combo->addItem(tr("provider"));
        combo->setCurrentIndex(0);
        combo->setEditable(false);
        return combo;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ApiInterfacesDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == ApiInterfaceColumns::NAME || index.column() == ApiInterfaceColumns::DISPLAY_NAME ||
        index.column() == ApiInterfaceColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        edit->setText(index.data(Qt::DisplayRole).toString());			
    }
    else if (index.column() == ApiInterfaceColumns::API_DEFINITION)
    {
        Q_ASSERT(false);
    }
    else if (index.column() == ApiInterfaceColumns::DEPENDENCY)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        Q_ASSERT(combo);

        QString selectedText = index.data(Qt::DisplayRole).toString();
        int indexToSelect = combo->findText(selectedText);
        combo->setCurrentIndex(indexToSelect);
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);					 
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesDelegate::setModelData()
//-----------------------------------------------------------------------------
void ApiInterfacesDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, 
    QModelIndex const& index) const
{
    if (index.column() == ApiInterfaceColumns::NAME || index.column() == ApiInterfaceColumns::DISPLAY_NAME ||
        index.column() == ApiInterfaceColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        model->setData(index, edit->text(), Qt::EditRole);
    }
    else if (index.column() == ApiInterfaceColumns::API_DEFINITION)
    {
        Q_ASSERT(false);
    }
    else if (index.column() == ApiInterfaceColumns::DEPENDENCY)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        Q_ASSERT(combo);

        model->setData(index, combo->currentText(), Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiInterfacesDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void ApiInterfacesDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}

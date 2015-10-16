//-----------------------------------------------------------------------------
// File: cominterfacesdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 28.06.2012
//
// Description:
// Delegate class for component COM interfaces.
//-----------------------------------------------------------------------------

#include "cominterfacesdelegate.h"

#include "ComInterfaceColumns.h"

#include <QLineEdit>
#include <QComboBox>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ComInterfacesDelegate::ComInterfacesDelegate()
//-----------------------------------------------------------------------------
ComInterfacesDelegate::ComInterfacesDelegate(QObject *parent):
QStyledItemDelegate(parent)
{
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesDelegate::~ComInterfacesDelegate()
//-----------------------------------------------------------------------------
ComInterfacesDelegate::~ComInterfacesDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ComInterfacesDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option,
    QModelIndex const& index) const
{
    if (index.column() == ComInterfaceColumns::NAME ||
        index.column() == ComInterfaceColumns::DESCRIPTION)
    {
        QLineEdit* edit = new QLineEdit(parent);
        connect(edit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return edit;
    }
    else if (index.column() == ComInterfaceColumns::COM_DEFINITION)
    {
        Q_ASSERT(false);
        return 0;
    }
    else if (index.column() == ComInterfaceColumns::TRANSFER_TYPE)
    {
        QComboBox* combo = new QComboBox(parent);
        return combo;
    }
    else if (index.column() == ComInterfaceColumns::DIRECTION)
    {
        QComboBox* combo = new QComboBox(parent);
        combo->addItem(tr("in"));
        combo->addItem(tr("out"));
        combo->addItem(tr("inout"));
        combo->setCurrentIndex(0);
        return combo;
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }	
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ComInterfacesDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == ComInterfaceColumns::NAME ||
        index.column() == ComInterfaceColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        const QString text = index.data(Qt::DisplayRole).toString();
        edit->setText(text);
    }
    else if (index.column() == ComInterfaceColumns::COM_DEFINITION)
    {
        Q_ASSERT(false);

    }
    else if (index.column() == ComInterfaceColumns::TRANSFER_TYPE)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        Q_ASSERT(combo);

        // remove the previous items
        combo->clear();

        // add the possible options for transfer types.
        QStringList types = index.data(ComInterfaceColumns::TRANSFER_TYPE_OPTIONS).toStringList();
        combo->addItem("");
        combo->addItems(types);

        // select the right option
        QString selected = index.data(Qt::DisplayRole).toString();
        combo->setCurrentIndex(combo->findText(selected));
    }
    else if (index.column() == ComInterfaceColumns::DIRECTION)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        Q_ASSERT(combo);

        QString direction = index.data(Qt::DisplayRole).toString();
        combo->setCurrentIndex(combo->findText(direction));
    }
    else
    {
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesDelegate::setModelData()
//-----------------------------------------------------------------------------
void ComInterfacesDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, 
    QModelIndex const& index ) const
{
    if (index.column() == ComInterfaceColumns::NAME ||
        index.column() == ComInterfaceColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        QString text = edit->text();
        model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == ComInterfaceColumns::COM_DEFINITION)
    {
        Q_ASSERT(false);
    }
    else if (index.column() == ComInterfaceColumns::TRANSFER_TYPE)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        Q_ASSERT(combo);

        QString type = combo->currentText();
        model->setData(index, type, Qt::EditRole);
    }
    else if (index.column() == ComInterfaceColumns::DIRECTION)
    {
        QComboBox* combo = qobject_cast<QComboBox*>(editor);
        Q_ASSERT(combo);

        QString dir = combo->currentText();
        model->setData(index, dir, Qt::EditRole);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ComInterfacesDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void ComInterfacesDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}

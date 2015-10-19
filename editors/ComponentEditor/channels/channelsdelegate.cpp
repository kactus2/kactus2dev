//-----------------------------------------------------------------------------
// File: channelsdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The delegate that provides editors to edit the channels of a component.
//-----------------------------------------------------------------------------

#include "channelsdelegate.h"
#include "ChannelColumns.h"

#include <common/widgets/EnumCollectionEditor/EnumCollectionEditor.h>

#include <IPXACTmodels/Component/Component.h>

#include <QLineEdit>
#include <QString>

//-----------------------------------------------------------------------------
// Function: ChannelsDelegate::ChannelsDelegate()
//-----------------------------------------------------------------------------
ChannelsDelegate::ChannelsDelegate(QSharedPointer<Component> component, QObject* parent):
QStyledItemDelegate(parent),
    component_(component)
{
    Q_ASSERT(component_);
}

//-----------------------------------------------------------------------------
// Function: ChannelsDelegate::~ChannelsDelegate()
//-----------------------------------------------------------------------------
ChannelsDelegate::~ChannelsDelegate()
{
}

//-----------------------------------------------------------------------------
// Function: ChannelsDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* ChannelsDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
    QModelIndex const& index) const
{
    if (index.column() == ChannelColumns::NAME_COLUMN ||
        index.column() == ChannelColumns::DISPLAY_NAME_COLUMN ||
        index.column() == ChannelColumns::DESCRIPTION_COLUMN)
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
        return lineEdit;
    }
    else if (index.column() == ChannelColumns::INTERFACE_COLUMN) 
    {
        return new EnumCollectionEditor(parent);
    }
    else
    {
        return QStyledItemDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ChannelsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ChannelsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == ChannelColumns::NAME_COLUMN ||
        index.column() == ChannelColumns::DISPLAY_NAME_COLUMN ||
        index.column() == ChannelColumns::DESCRIPTION_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        const QString text = index.data(Qt::DisplayRole).toString();
        edit->setText(text);
    }

    else if (index.column() == ChannelColumns::INTERFACE_COLUMN) 
    {
        EnumCollectionEditor* collectionEditor = static_cast<EnumCollectionEditor*>(editor);
        Q_ASSERT(collectionEditor != 0);

        QStringList selectedInterfaces =
            index.data(ChannelColumns::USER_DISPLAY_ROLE).toStringList();

        foreach (QString const& name, component_->getBusInterfaceNames())
        {
            collectionEditor->addItem(name, selectedInterfaces.contains(name));
        }
    }

    else
    {
        QStyledItemDelegate::setEditorData(editor, index);

    }
}

//-----------------------------------------------------------------------------
// Function: ChannelsDelegate::setModelData()
//-----------------------------------------------------------------------------
void ChannelsDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, QModelIndex const& index ) const
{
    if (index.column() == ChannelColumns::NAME_COLUMN ||
        index.column() == ChannelColumns::DISPLAY_NAME_COLUMN ||
        index.column() == ChannelColumns::DESCRIPTION_COLUMN)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        QString text = edit->text();
        model->setData(index, text, Qt::EditRole);
    }
    else if (index.column() == ChannelColumns::INTERFACE_COLUMN) 
    {
        EnumCollectionEditor* collectionEditor = static_cast<EnumCollectionEditor*>(editor);
        Q_ASSERT(collectionEditor != 0);

        QStringList interfaces = collectionEditor->getSelectedItems();
        model->setData(index, interfaces, ChannelColumns::USER_EDIT_ROLE);
    }
    else
    {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ChannelsDelegate::commitAndCloseEditor()
//-----------------------------------------------------------------------------
void ChannelsDelegate::commitAndCloseEditor()
{
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}

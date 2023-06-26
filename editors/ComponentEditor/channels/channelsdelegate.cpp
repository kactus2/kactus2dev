//-----------------------------------------------------------------------------
// File: channelsdelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
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
EnumerationEditorConstructorDelegate(parent),
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
    if (index.column() == ChannelColumns::NAME ||
        index.column() == ChannelColumns::DISPLAY_NAME ||
        index.column() == ChannelColumns::DESCRIPTION)
    {
        QLineEdit* lineEdit = new QLineEdit(parent);
        connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);

        return lineEdit;
    }
    else
    {
        return EnumerationEditorConstructorDelegate::createEditor(parent, option, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ChannelsDelegate::setEditorData()
//-----------------------------------------------------------------------------
void ChannelsDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    if (index.column() == ChannelColumns::NAME ||
        index.column() == ChannelColumns::DISPLAY_NAME ||
        index.column() == ChannelColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        edit->setText(index.data(Qt::DisplayRole).toString());
    }
    else
    {
        EnumerationEditorConstructorDelegate::setEditorData(editor, index);
    }
}

//-----------------------------------------------------------------------------
// Function: ChannelsDelegate::setModelData()
//-----------------------------------------------------------------------------
void ChannelsDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    if (index.column() == ChannelColumns::NAME ||
        index.column() == ChannelColumns::DISPLAY_NAME ||
        index.column() == ChannelColumns::DESCRIPTION)
    {
        QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
        Q_ASSERT(edit);

        model->setData(index, edit->text(), Qt::EditRole);
    }
    else
    {
        EnumerationEditorConstructorDelegate::setModelData(editor, model, index);
    }
}

//-----------------------------------------------------------------------------
// Function: channelsdelegate::isEnumerationEditorColumn()
//-----------------------------------------------------------------------------
bool ChannelsDelegate::isEnumerationEditorColumn(QModelIndex const& index) const
{
    if (index.column() == ChannelColumns::INTERFACES)
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: channelsdelegate::getCurrentSelection()
//-----------------------------------------------------------------------------
QStringList ChannelsDelegate::getCurrentSelection(QModelIndex const& index) const
{
    return index.data(ChannelColumns::USER_DISPLAY_ROLE).toStringList();
}

//-----------------------------------------------------------------------------
// Function: channelsdelegate::getAvailableItems()
//-----------------------------------------------------------------------------
QStringList ChannelsDelegate::getAvailableItems() const
{
    return component_->getBusInterfaceNames();
}

//-----------------------------------------------------------------------------
// Function: channelsdelegate::setEnumerationDataToModel()
//-----------------------------------------------------------------------------
void ChannelsDelegate::setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, QStringList const& selectedItems) const
{
    model->setData(index, selectedItems, ChannelColumns::USER_EDIT_ROLE);
}

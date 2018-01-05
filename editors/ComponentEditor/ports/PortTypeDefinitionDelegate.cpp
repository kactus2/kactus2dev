//-----------------------------------------------------------------------------
// File: PortTypeDefinitionDelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.11.2017
//
// Description:
// The delegate providing editors for port type definition.
//-----------------------------------------------------------------------------

#include "PortTypeDefinitionDelegate.h"

#include <common/widgets/assistedLineEdit/AssistedLineEdit.h>
#include <common/widgets/assistedLineEdit/BasicLineContentMatcher.h>

//-----------------------------------------------------------------------------
// Function: PortTypeDefinitionDelegate::PortTypeDefinitionDelegate()
//-----------------------------------------------------------------------------
PortTypeDefinitionDelegate::PortTypeDefinitionDelegate(QObject* parent):
QStyledItemDelegate(parent),
matcher_(new BasicLineContentMatcher())
{

}

//-----------------------------------------------------------------------------
// Function: PortTypeDefinitionDelegate::~PortTypeDefinitionDelegate()
//-----------------------------------------------------------------------------
PortTypeDefinitionDelegate::~PortTypeDefinitionDelegate()
{

}

//-----------------------------------------------------------------------------
// Function: PortTypeDefinitionDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* PortTypeDefinitionDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const&, QModelIndex const&)
    const
{
    QStringList suggestedDefinitions;
    suggestedDefinitions.append(QString("IEEE.std_logic_1164.all"));
    suggestedDefinitions.append(QString("IEEE.numeric_std.all"));
    matcher_->setItems(suggestedDefinitions);

    AssistedLineEdit* editor = new AssistedLineEdit(parent->window(), parent);
    editor->setContentMatcher(matcher_);

    return editor;
}

//-----------------------------------------------------------------------------
// Function: PortTypeDefinitionDelegate::setEditorData()
//-----------------------------------------------------------------------------
void PortTypeDefinitionDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    AssistedLineEdit* lineEditor = qobject_cast<AssistedLineEdit*>(editor);
    if (lineEditor)
    {
        QString text = index.data(Qt::DisplayRole).toString();
        lineEditor->setText(text);
    }
}

//-----------------------------------------------------------------------------
// Function: PortTypeDefinitionDelegate::setModelData()
//-----------------------------------------------------------------------------
void PortTypeDefinitionDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index)
    const
{
    AssistedLineEdit* assistedEditor = qobject_cast<AssistedLineEdit*>(editor);
    if (assistedEditor)
    {
        model->setData(index, assistedEditor->text(), Qt::EditRole);
    }
}

/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: groupmanagerdelegate.cpp
 *		Project: Kactus 2
 */

#include "groupmanagerdelegate.h"

#include <common/widgets/assistedLineEdit/AssistedLineEdit.h>
#include <common/widgets/assistedLineEdit/BasicLineContentMatcher.h>

#include <QComboBox>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: GroupManagerDelegate::GroupManagerDelegate()
//-----------------------------------------------------------------------------
GroupManagerDelegate::GroupManagerDelegate(QObject *parent):
QStyledItemDelegate(parent),
    matcher_(new BasicLineContentMatcher())
{
    QStringList suggestedItems;
    suggestedItems.append(tr("diagnostics"));
    suggestedItems.append(tr("documentation"));
    suggestedItems.append(tr("generatedFiles"));
    suggestedItems.append(tr("projectFiles"));
    suggestedItems.append(tr("simulation"));
    suggestedItems.append(tr("sourceFiles"));

    matcher_->setItems(suggestedItems);
}

//-----------------------------------------------------------------------------
// Function: GroupManagerDelegate::~GroupManagerDelegate()
//-----------------------------------------------------------------------------
GroupManagerDelegate::~GroupManagerDelegate() 
{
    delete matcher_;
}

//-----------------------------------------------------------------------------
// Function: GroupManagerDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* GroupManagerDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& /*option*/,
    QModelIndex const& /*index*/) const
{
    AssistedLineEdit* editor = new AssistedLineEdit(parent->window(), parent);
    editor->setContentMatcher(matcher_);

    return editor;
}

//-----------------------------------------------------------------------------
// Function: GroupManagerDelegate::setEditorData()
//-----------------------------------------------------------------------------
void GroupManagerDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
    QString text = index.model()->data(index, Qt::EditRole).toString();

    AssistedLineEdit* assistedEditor = qobject_cast<AssistedLineEdit*>(editor);
    Q_ASSERT(assistedEditor);

    assistedEditor->setText(text);
}

//-----------------------------------------------------------------------------
// Function: GroupManagerDelegate::setModelData()
//-----------------------------------------------------------------------------
void GroupManagerDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const
{
    AssistedLineEdit* assistedEditor = qobject_cast<AssistedLineEdit*>(editor);
    Q_ASSERT(assistedEditor);

    model->setData(index, assistedEditor->text(), Qt::EditRole);
}

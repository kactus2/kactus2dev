//-----------------------------------------------------------------------------
// File: filetypeeditordelegate.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 07.12.2011
//
// Description:
// Provides editor to select a type for a file.
//-----------------------------------------------------------------------------

#include "filetypeeditordelegate.h"

#include <common/widgets/assistedLineEdit/AssistedLineEdit.h>

#include <IPXACTmodels/common/FileType.h>

#include <QStringList>
#include <QSettings>

//-----------------------------------------------------------------------------
// Function: FileTypeEditorDelegate::FileTypeEditorDelegate()
//-----------------------------------------------------------------------------
FileTypeEditorDelegate::FileTypeEditorDelegate(Document::Revision docRevision, QObject *parent):
QStyledItemDelegate(parent),
    revision_(docRevision)
{

}

//-----------------------------------------------------------------------------
// Function: FileTypeEditorDelegate::~FileTypeEditorDelegate()
//-----------------------------------------------------------------------------
FileTypeEditorDelegate::~FileTypeEditorDelegate()
{
    delete matcher_;
}

//-----------------------------------------------------------------------------
// Function: FileTypeEditorDelegate::createEditor()
//-----------------------------------------------------------------------------
QWidget* FileTypeEditorDelegate::createEditor(QWidget* parent, QStyleOptionViewItem const& /*option*/,
	QModelIndex const& /*index*/ ) const
{
    AssistedLineEdit* editor = new AssistedLineEdit(parent->window(), parent);

    updateSuggestedItems();
    editor->setContentMatcher(matcher_);
    
	return editor;
}

//-----------------------------------------------------------------------------
// Function: FileTypeEditorDelegate::setEditorData()
//-----------------------------------------------------------------------------
void FileTypeEditorDelegate::setEditorData(QWidget* editor, QModelIndex const& index) const
{
	QString text = index.model()->data(index, Qt::EditRole).toString();

	AssistedLineEdit* assistedEditor = qobject_cast<AssistedLineEdit*>(editor);
	Q_ASSERT(assistedEditor);

	assistedEditor->setText(text);
}

//-----------------------------------------------------------------------------
// Function: FileTypeEditorDelegate::setModelData()
//-----------------------------------------------------------------------------
void FileTypeEditorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
    QModelIndex const& index) const
{
	AssistedLineEdit* assistedEditor = qobject_cast<AssistedLineEdit*>(editor);
	Q_ASSERT(assistedEditor);

	model->setData(index, assistedEditor->text(), Qt::EditRole);
}

//-----------------------------------------------------------------------------
// Function: FileTypeEditorDelegate::updateSuggestedItems()
//-----------------------------------------------------------------------------
void FileTypeEditorDelegate::updateSuggestedItems() const
{
    QSettings settings;
    settings.beginGroup("FileTypes");
    QStringList typeNames = settings.childGroups();
    settings.endGroup();

    typeNames.append(FileTypes::FILE_TYPES);
    if (revision_ == Document::Revision::Std22)
    {
        typeNames.append(FileTypes::FILE_TYPES_2022);
    }
    typeNames.removeDuplicates();

    matcher_->setItems(typeNames);
}

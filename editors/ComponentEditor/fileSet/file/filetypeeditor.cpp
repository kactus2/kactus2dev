//-----------------------------------------------------------------------------
// File: filetypeeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileTypeEditor is used to specify a file type for a file.
//-----------------------------------------------------------------------------

#include "filetypeeditor.h"
#include "filetypeeditordelegate.h"

#include <common/dialogs/comboSelector/comboselector.h>

#include <IPXACTmodels/Component/File.h>

//-----------------------------------------------------------------------------
// Function: filetypeeditor::FileTypeEditor()
//-----------------------------------------------------------------------------
FileTypeEditor::FileTypeEditor(QWidget *parent, QSharedPointer<File> file):
ListManager(tr("Specified file types"), parent), 
file_(file)
{
	Q_ASSERT_X(file_, "FileTypeEditor constructor", "Null File-pointer given as parameter");
}

//-----------------------------------------------------------------------------
// Function: filetypeeditor::~FileTypeEditor()
//-----------------------------------------------------------------------------
FileTypeEditor::~FileTypeEditor()
{

}

//-----------------------------------------------------------------------------
// Function: filetypeeditor::apply()
//-----------------------------------------------------------------------------
void FileTypeEditor::apply()
{
	// get all items from the model
	QStringList items = model_->items();

	// remove all previous file types and userFileTypes from the model
	file_->clearFileTypes();
    foreach (QString fileType, items)
    {
        file_->getFileTypes()->append(fileType);
    }
}

//-----------------------------------------------------------------------------
// Function: filetypeeditor::()
//-----------------------------------------------------------------------------
void FileTypeEditor::restore()
{
    QStringList fileTypes;
    foreach (QString item, *file_->getFileTypes())
    {
        fileTypes.append(item);
    }

	model_->setItems(fileTypes);
}

//-----------------------------------------------------------------------------
// Function: filetypeeditor::isValid()
//-----------------------------------------------------------------------------
bool FileTypeEditor::isValid() const
{
	// at least one file type has to be specified
	if (model_->rowCount() <= 0)
    {
		return false;
	}

	return !model_->items().contains("");
}

//-----------------------------------------------------------------------------
// Function: filetypeeditor::initialize()
//-----------------------------------------------------------------------------
void FileTypeEditor::initialize( const QStringList& items /*= QStringList()*/ )
{
	ListManager::initialize(items);

	view_->setProperty("mandatoryField", true);

	view_->setItemDelegate(new FileTypeEditorDelegate(this));

	restore();
}

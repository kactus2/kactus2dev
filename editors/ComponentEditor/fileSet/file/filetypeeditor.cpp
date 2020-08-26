//-----------------------------------------------------------------------------
// File: filetypeeditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileTypeEditor is used to specify a file type for a file.
//-----------------------------------------------------------------------------

#include "filetypeeditor.h"
#include "filetypeeditordelegate.h"

#include <common/dialogs/comboSelector/comboselector.h>

#include <editors/ComponentEditor/fileSet/interfaces/FileInterface.h>

#include <IPXACTmodels/Component/File.h>

//-----------------------------------------------------------------------------
// Function: filetypeeditor::FileTypeEditor()
//-----------------------------------------------------------------------------
FileTypeEditor::FileTypeEditor(QWidget *parent, std::string const& fileName, FileInterface* fileInterface):
ListManager(tr("File types"), parent), 
fileName_(fileName),
fileInterface_(fileInterface)
{
    Q_ASSERT_X(fileInterface_, "FileTypeEditor constructor", "Null File interface-pointer given as parameter");
}

//-----------------------------------------------------------------------------
// Function: filetypeeditor::apply()
//-----------------------------------------------------------------------------
void FileTypeEditor::apply()
{
	// get all items from the model
	QStringList items = model_->items();

	// remove all previous file types and userFileTypes from the model
    fileInterface_->clearFileTypes(fileName_);

    foreach (QString fileType, items)
    {
        fileInterface_->addFileType(fileName_, fileType.toStdString());
    }
}

//-----------------------------------------------------------------------------
// Function: filetypeeditor::()
//-----------------------------------------------------------------------------
void FileTypeEditor::restore()
{
    QStringList fileTypes;
    std::vector<std::string> types = fileInterface_->getFileTypes(fileName_);
    for (auto singleType : types)
    {
        fileTypes.append(QString::fromStdString(singleType));
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

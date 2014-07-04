/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesetitem.h"
#include "componenteditorfileitem.h"
#include "componenteditortreemodel.h"
#include <editors/ComponentEditor/fileSet/fileseteditor.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::ComponentEditorFileSetItem()
//-----------------------------------------------------------------------------
ComponentEditorFileSetItem::ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet,
													   ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
fileSet_(fileSet),
files_(fileSet->getFiles()) 
{
	Q_ASSERT(fileSet);

    int childCount = files_.size();
    for (int i = 0; i < childCount; i++)
    {
        createChild(i);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::~ComponentEditorFileSetItem()
//-----------------------------------------------------------------------------
ComponentEditorFileSetItem::~ComponentEditorFileSetItem() {
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorFileSetItem::text() const {
	return fileSet_->getName();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorFileSetItem::isValid() const {
	// check that the file set is valid
	if (!fileSet_->isValid(true)) {
		return false;
	}

	// check that the dependent directories exist
	QString xmlPath = libHandler_->getPath(*component_->getVlnv());
	QStringList dependentDirs = fileSet_->getDependencies();

	// check each directory
	foreach (QString relDirPath, dependentDirs) {
		QString absPath = General::getAbsolutePath(xmlPath, relDirPath);
		QFileInfo dirInfo(absPath);

		// if the directory does not exist
		if (!dirInfo.exists()) {
			return false;
		}
	}

	// check that all files are valid
	foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_) {
		if (!childItem->isValid()) {
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorFileSetItem::editor() {
	if (!editor_) {
		 editor_ = new FileSetEditor(libHandler_, component_, fileSet_, NULL);
		 editor_->setProtection(locked_);
		 connect(editor_, SIGNAL(contentChanged()),
			 this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		 connect(editor_, SIGNAL(childAdded(int)),
			 this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		 connect(editor_, SIGNAL(childRemoved(int)),
			 this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		 connect(editor_, SIGNAL(childMoved(int, int)),
			 this, SLOT(onMoveChild(int, int)), Qt::UniqueConnection);
		 connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			 this, SIGNAL(helpUrlRequested(QString const&)));

         connect(editor_, SIGNAL(childRemoved(int)),
             this, SIGNAL(childRemoved(int)), Qt::UniqueConnection);
	}
	return editor_;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorFileSetItem::getTooltip() const
{
	return tr("Contains a list of files and their build commands");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::createChild()
//-----------------------------------------------------------------------------
void ComponentEditorFileSetItem::createChild(int index)
{
	QSharedPointer<ComponentEditorFileItem> fileItem(new ComponentEditorFileItem(
		files_.at(index), model_, libHandler_, component_, this));

    connect(fileItem.data(), SIGNAL(openCSource(QString const&, QSharedPointer<Component>)),
            model_, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)), Qt::UniqueConnection);

	fileItem->setLocked(locked_);
	childItems_.insert(index, fileItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::onFileAdded()
//-----------------------------------------------------------------------------
void ComponentEditorFileSetItem::onFileAdded(File* file)
{
    if (files_.at(files_.size() - 1) != file)
    {
        Q_ASSERT(false);
    }

    onAddChild(files_.size() - 1);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::getFileSet()
//-----------------------------------------------------------------------------
FileSet const* ComponentEditorFileSetItem::getFileSet() const
{
    return fileSet_.data();
}

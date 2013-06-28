/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesetitem.h"
#include "componenteditorfileitem.h"
#include "componenteditortreemodel.h"
#include <IPXactWrapper/ComponentEditor/fileSet/fileseteditor.h>

#include <QFont>
#include <QApplication>

ComponentEditorFileSetItem::ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet,
													   ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
fileSet_(fileSet),
files_(fileSet->getFiles()) {

	Q_ASSERT(fileSet);

	foreach (QSharedPointer<File> file, files_) {
		QSharedPointer<ComponentEditorFileItem> fileItem(new ComponentEditorFileItem(
			                                             file, model, libHandler, component, this));

        connect(fileItem.data(), SIGNAL(openCSource(QString const&, QSharedPointer<Component>)),
                model, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)), Qt::UniqueConnection);

		childItems_.append(fileItem);
	}
}

ComponentEditorFileSetItem::~ComponentEditorFileSetItem() {
}

QString ComponentEditorFileSetItem::text() const {
	return fileSet_->getName();
}

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

ItemEditor* ComponentEditorFileSetItem::editor() {
	if (!editor_) {
		 editor_ = new FileSetEditor(libHandler_, component_, fileSet_, NULL);
		 editor_->setDisabled(locked_);
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

QFont ComponentEditorFileSetItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorFileSetItem::getTooltip() const {
	return tr("Contains a list of files and their build commands");
}

void ComponentEditorFileSetItem::createChild( int index ) {

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
    Q_ASSERT(files_.at(files_.size() - 1) == file);
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

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::updateFileItems()
//-----------------------------------------------------------------------------
void ComponentEditorFileSetItem::updateFileItems()
{
//     childItems_.clear();
// 
//     foreach (QSharedPointer<File> file, files_) {
//         QSharedPointer<ComponentEditorFileItem> fileItem(new ComponentEditorFileItem(
//             file, model_, libHandler_, component_, this));
// 		  fileItem->setLocked(locked_);
// 
//         connect(fileItem.data(), SIGNAL(openCSource(QString const&, QSharedPointer<Component>)),
//                 model_, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)), Qt::UniqueConnection);
// 
//         childItems_.append(fileItem);
//     }
// 
//     emit contentChanged(this);

    //onEditorChanged();
}

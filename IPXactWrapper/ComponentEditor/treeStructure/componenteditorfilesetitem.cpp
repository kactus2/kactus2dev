/* 
 *  	Created on: 10.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfilesetitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfilesetitem.h"
#include "componenteditorfileitem.h"
#include "componenteditortreemodel.h"

#include <QFont>
#include <QApplication>

ComponentEditorFileSetItem::ComponentEditorFileSetItem(QSharedPointer<FileSet> fileSet,
													   ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
fileSet_(fileSet),
files_(fileSet->getFiles()),
editor_(libHandler, component, fileSet, NULL) {

	Q_ASSERT(fileSet);

	foreach (QSharedPointer<File> file, files_) {
		QSharedPointer<ComponentEditorFileItem> fileItem(new ComponentEditorFileItem(
			                                             file, model, libHandler, component, this));

        connect(fileItem.data(), SIGNAL(openCSource(QString const&, QSharedPointer<Component>)),
                model, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)), Qt::UniqueConnection);

		childItems_.append(fileItem);
	}

	editor_.hide();

	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childAdded(int)),
		this, SLOT(onAddChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childRemoved(int)),
		this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(childMoved(int, int)),
		this, SLOT(onMoveChild(int, int)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
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

	// check that all files are valid
	foreach (QSharedPointer<ComponentEditorItem> childItem, childItems_) {
		if (!childItem->isValid()) {
			return false;
		}
	}

	return true;
}

ItemEditor* ComponentEditorFileSetItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorFileSetItem::editor() const {
	return &editor_;
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
	childItems_.insert(index, fileItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileSetItem::onFileAdded()
//-----------------------------------------------------------------------------
void ComponentEditorFileSetItem::onFileAdded(File* file)
{
    editor_.onFileAdded(file);
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
    childItems_.clear();

    foreach (QSharedPointer<File> file, files_) {
        QSharedPointer<ComponentEditorFileItem> fileItem(new ComponentEditorFileItem(
            file, model_, libHandler_, component_, this));

        connect(fileItem.data(), SIGNAL(openCSource(QString const&, QSharedPointer<Component>)),
                model_, SIGNAL(openCSource(QString const&, QSharedPointer<Component>)), Qt::UniqueConnection);

        childItems_.append(fileItem);
    }
}

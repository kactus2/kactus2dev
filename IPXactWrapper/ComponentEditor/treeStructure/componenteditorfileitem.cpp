/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfileitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfileitem.h"

#include <models/component.h>
#include <models/generaldeclarations.h>

#include <QFont>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>

ComponentEditorFileItem::ComponentEditorFileItem(QSharedPointer<File> file,
												 ComponentEditorTreeModel* model,
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
file_(file),
editor_(libHandler, component, file) {
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
}

ComponentEditorFileItem::~ComponentEditorFileItem() {
}

QString ComponentEditorFileItem::text() const {
	return file_->getName();
}

bool ComponentEditorFileItem::isValid() const {
	// if the file is not valid
	if (!file_->isValid(true)) {
		return false;
	}
	
	// check that the file exists in the file system
	
	// get the path to the xml file
	QString basePath = libHandler_->getPath(*component_->getVlnv());
	// get the file's relative path
	QString relPath = file_->getName();

	// create the absolute file path to the file
	QString absolutePath = General::getAbsolutePath(basePath, relPath);

	// if the file does not exist then this is false item
	QFileInfo fileInfo(absolutePath);
	if (!fileInfo.exists()) {
		return false;
	}

	// file was valid and existed
	return true;
}

ItemEditor* ComponentEditorFileItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorFileItem::editor() const {
	return &editor_;
}

QFont ComponentEditorFileItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorFileItem::getTooltip() const {
	return tr("Specifies a single file associated with the file set");
}

bool ComponentEditorFileItem::canBeOpened() const {
	return true;
}

void ComponentEditorFileItem::openItem(bool builtinEditor) {

	// create the necessary paths
	const QString relPath = file_->getName();
	const QString xmlPath = libHandler_->getPath(*component_->getVlnv());
	const QString absolutePath = General::getAbsolutePath(xmlPath, relPath);

    if (builtinEditor &&
        (QFileInfo(absolutePath).completeSuffix().toLower() == "c" ||
         QFileInfo(absolutePath).completeSuffix().toLower() == "h"))
    {
        emit openCSource(absolutePath, component_);
    }
    else
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(absolutePath));
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorFileItem::hasBuiltinEditor()
//-----------------------------------------------------------------------------
bool ComponentEditorFileItem::hasBuiltinEditor() const
{
    const QString relPath = file_->getName();
    const QString xmlPath = libHandler_->getPath(*component_->getVlnv());
    const QString absolutePath = General::getAbsolutePath(xmlPath, relPath);
    return QFileInfo(absolutePath).completeSuffix().toLower() == "c";
}

void ComponentEditorFileItem::onEditorChanged() {
	
	// on file also the grand parent must be updated
	if (parent() && parent()->parent()) {
		emit contentChanged(parent()->parent());
	}

	// call the base class to update this and parent
	ComponentEditorItem::onEditorChanged();
}

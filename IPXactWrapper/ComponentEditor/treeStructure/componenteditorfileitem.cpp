/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorfileitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorfileitem.h"

#include <IPXactWrapper/ComponentEditor/fileSet/file/fileeditor.h>
#include <models/component.h>
#include <models/generaldeclarations.h>
#include <common/utils.h>

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
urlTester_(new QRegExpValidator(Utils::URL_IDENTIFY_REG_EXP, this)),
urlValidator_(new QRegExpValidator(Utils::URL_VALIDITY_REG_EXP, this)) {
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

	QString filePath = file_->getName();
	int pos = 0;
	
	// check if the path is actually a URL to (external) location
	if (urlTester_->validate(filePath, pos) == QValidator::Acceptable) { 
		pos = 0;

		// if the URL is not valid
		if (urlValidator_->validate(filePath, pos) != QValidator::Acceptable) {
			return false;
		}

		// the URL was valid
		return true;
	}

	// the path was not URL so interpret is as file path
	else {

		// get the path to the xml file
		QString basePath = libHandler_->getPath(*component_->getVlnv());

		QString absPath;

		// if the path is relative then create absolute path
		QFileInfo originalInfo(filePath);
		if (originalInfo.isRelative()) {
			absPath = General::getAbsolutePath(basePath, filePath);
		}
		// if the reference is directly absolute
		else {
			absPath = filePath;
		}

		// check if the file exists in the file system
		QFileInfo fileInfo(absPath);
		return fileInfo.exists();
	}
}

ItemEditor* ComponentEditorFileItem::editor() {

	if (!editor_) {
		editor_ = new FileEditor(libHandler_, component_, file_);
		editor_->setDisabled(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
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

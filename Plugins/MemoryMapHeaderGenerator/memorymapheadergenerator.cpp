/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.cpp
 * 	Project:	Kactus 2
*/

#include "memorymapheadergenerator.h"
#include <LibraryManager/libraryinterface.h>
#include <PluginSystem/IPluginUtility.h>
#include <common/dialogs/fileSaveDialog/filesavedialog.h>
#include "localheadersavemodel.h"
#include <models/memorymap.h>

#include <QtPlugin>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QCoreApplication>

#include <QDebug>

MemoryMapHeaderGenerator::MemoryMapHeaderGenerator():
utility_(NULL) {
}

MemoryMapHeaderGenerator::~MemoryMapHeaderGenerator() {
}

QString const& MemoryMapHeaderGenerator::getName() const {
	static QString name(tr("Memory Map Header Generator"));
	return name;
}

QString const& MemoryMapHeaderGenerator::getVersion() const {
	static QString version(tr("1.0"));
	return version;
}

QString const& MemoryMapHeaderGenerator::getDescription() const {
	static QString description(tr("Generates C-headers for memory maps of a component"));
	return description;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* MemoryMapHeaderGenerator::getSettingsWidget()
{
    return new PluginSettingsWidget();
}


//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon MemoryMapHeaderGenerator::getIcon() const
{
    return QIcon(":icons/headerGenerator24.png");
}

bool MemoryMapHeaderGenerator::checkGeneratorSupport( QSharedPointer<LibraryComponent const> libComp ) const {
	
	// make sure the object is a component
	QSharedPointer<Component const> comp = libComp.dynamicCast<Component const>();
	if (!comp) {
		return false;
	}

	return comp->hasLocalMemoryMaps();
}

void MemoryMapHeaderGenerator::runGenerator( IPluginUtility* utility, QSharedPointer<LibraryComponent> libComp ) {
	utility_ = utility;

	QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
	Q_ASSERT(comp);

	// the model which displays the local memory maps and their header paths
	LocalHeaderSaveModel model(utility_->getLibraryInterface(), this);
	model.setComponent(comp);

	FileSaveDialog dialog(utility_->getParentWidget());
	dialog.setModel(&model);

	int result = dialog.exec();

	// if user clicked cancel
	if (result == QDialog::Rejected) {
		return;
	}

	// get list of the selected paths and generate each header
	const QList<LocalHeaderSaveModel::SaveFileOptions*>& options = model.getHeaderOptions();
	foreach (LocalHeaderSaveModel::SaveFileOptions* headerOpt, options) {

		QFile file(headerOpt->fileInfo_.absoluteFilePath());

		// open the file and erase all old contents if any exists
		
		// if file could not be opened
		if (!file.open(QFile::Truncate | QFile::WriteOnly)) {

			QString message(tr("File: %1 could not be opened for writing.").arg(headerOpt->fileInfo_.absoluteFilePath()));
			QMessageBox::critical(utility_->getParentWidget(), QCoreApplication::applicationName(), message);
			break;
		}
		// now file has been opened for writing

		// open file stream to write to
		QTextStream vhdlStream(&file);

		// write header comments of the file
		

		// close the file after writing
		file.close();
	}
}

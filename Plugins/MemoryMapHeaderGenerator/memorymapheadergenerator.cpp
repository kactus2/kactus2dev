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

#include <QtPlugin>

#include <QDebug>

MemoryMapHeaderGenerator::MemoryMapHeaderGenerator():
utility_(NULL) {
}

MemoryMapHeaderGenerator::~MemoryMapHeaderGenerator() {
}

QString const& MemoryMapHeaderGenerator::getName() const {
	static QString name(tr("Memory map header generator"));
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
// Function: MemoryMapHeaderGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon MemoryMapHeaderGenerator::getIcon() const
{
    return QIcon(":icons/generation_memHeader2.png");
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

	FileSaveDialog dialog(utility_->getParentWidget());

	int result = dialog.exec();

	if (result == QDialog::Rejected) {
		qDebug() << "Dialog rejected";
		return;
	}

	qDebug() << "Dialog accepted";
}

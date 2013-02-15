/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.cpp
 * 	Project:	Kactus 2
*/

#include "memorymapheadergenerator.h"
#include <LibraryManager/libraryinterface.h>
#include <PluginSystem/IPluginUtility.h>

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

bool MemoryMapHeaderGenerator::checkGeneratorSupport( QSharedPointer<LibraryComponent> libComp ) const {
	
	// make sure the object is a component
	QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
	if (!comp) {
		return false;
	}

	return comp->hasLocalMemoryMaps();
	//return true;
}

void MemoryMapHeaderGenerator::runGenerator( IPluginUtility* utility, QSharedPointer<LibraryComponent> libComp ) {
	qDebug() << "Header generator is run";
}

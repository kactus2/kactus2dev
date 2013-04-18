/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.cpp
 * 	Project:	Kactus 2
*/

#include "alterabspgenerator.h"
#include <LibraryManager/libraryinterface.h>
#include <PluginSystem/IPluginUtility.h>

#include <QtPlugin>

#include <QDebug>

AlteraBSPGenerator::AlteraBSPGenerator():
utility_(NULL) {
}

AlteraBSPGenerator::~AlteraBSPGenerator() {
}

QString const& AlteraBSPGenerator::getName() const {
	static QString name(tr("Altera BSP Generator"));
	return name;
}

QString const& AlteraBSPGenerator::getVersion() const {
	static QString version(tr("1.0"));
	return version;
}

QString const& AlteraBSPGenerator::getDescription() const {
	static QString description(tr("Generates board support package for CPU component."));
	return description;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* AlteraBSPGenerator::getSettingsWidget()
{
    return new PluginSettingsWidget();
}


//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon AlteraBSPGenerator::getIcon() const
{
    return QIcon(":icons/BSPicon24.png");
}

bool AlteraBSPGenerator::checkGeneratorSupport( QSharedPointer<LibraryComponent const> libComp,
	QSharedPointer<LibraryComponent const> libDesConf /*= QSharedPointer<LibraryComponent const>()*/,
	QSharedPointer<LibraryComponent const> libDes /*= QSharedPointer<LibraryComponent const>()*/ ) const {
	
	return true;
}

void AlteraBSPGenerator::runGenerator( IPluginUtility* utility,
	QSharedPointer<LibraryComponent> libComp,
	QSharedPointer<LibraryComponent> libDesConf /*= QSharedPointer<LibraryComponent>()*/,
	QSharedPointer<LibraryComponent> libDes /*= QSharedPointer<LibraryComponent>()*/ ) {

	utility_ = utility;
	Q_ASSERT(utility_);

	
	qDebug() << "Altera BSP generator";
}

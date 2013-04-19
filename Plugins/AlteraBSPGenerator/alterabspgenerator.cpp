/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.cpp
 * 	Project:	Kactus 2
*/

#include "alterabspgenerator.h"
#include <LibraryManager/libraryinterface.h>
#include <PluginSystem/IPluginUtility.h>
#include <models/component.h>
#include <common/KactusAttribute.h>
#include "alterabspgeneratordialog.h"

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
	
	// BSP package can only be run on component editor 
	if (libDesConf || libDes) {
		return false;
	}

	QSharedPointer<const Component> comp = libComp.dynamicCast<const Component>();
	
	switch (comp->getComponentImplementation()) {
	
		// HW component must be CPU and contain at least one SW view which specifies the BSP command
	case KactusAttribute::KTS_HW: {
		return comp->hasSWViews() && comp->isCpu();
											}
	// only HW components can contain BSP
	default: {
		return false;
				}
	}

	// generator can be run if component contains at least one sw view
	return comp->hasSWViews();
}

void AlteraBSPGenerator::runGenerator( IPluginUtility* utility,
	QSharedPointer<LibraryComponent> libComp,
	QSharedPointer<LibraryComponent> libDesConf /*= QSharedPointer<LibraryComponent>()*/,
	QSharedPointer<LibraryComponent> libDes /*= QSharedPointer<LibraryComponent>()*/ ) {

	utility_ = utility;
	Q_ASSERT(utility_);

	Q_ASSERT(libComp);
	Q_ASSERT(!libDesConf);
	Q_ASSERT(!libDes);

	QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
	Q_ASSERT(comp);
	
	AlteraBSPGeneratorDialog dialog(comp, utility_->getParentWidget());

	int result = dialog.exec();
	if (result == QDialog::Rejected) {
		qDebug() << "BSP generator canceled";
		return;
	}

	qDebug() << "BSP generator accepted";
}

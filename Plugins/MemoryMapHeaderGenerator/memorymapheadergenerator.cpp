/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	memorymapheadergenerator.cpp
 * 	Project:	Kactus 2
*/

#include "memorymapheadergenerator.h"

#include <Plugins/PluginSystem/IPluginUtility.h>

#include <Plugins/MemoryMapHeaderGenerator/LocalMemoryMapHeaderWriter.h>
#include <Plugins/MemoryMapHeaderGenerator/GlobalMemoryMapHeaderWriter.h>
#include <Plugins/MemoryMapHeaderGenerator/SystemMemoryMapHeaderWriter.h>

#include <QtPlugin>
#include <QCoreApplication>
#include <QMessageBox>
//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::MemoryMapHeaderGenerator()
//-----------------------------------------------------------------------------
MemoryMapHeaderGenerator::MemoryMapHeaderGenerator():
utility_(NULL),
design_(),
localSaveOptions_(),
globalSaveOptions_()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::~MemoryMapHeaderGenerator()
//-----------------------------------------------------------------------------
MemoryMapHeaderGenerator::~MemoryMapHeaderGenerator()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getName()
//-----------------------------------------------------------------------------
QString MemoryMapHeaderGenerator::getName() const
{
	return tr("Memory Map Header Generator");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getVersion()
//-----------------------------------------------------------------------------
QString MemoryMapHeaderGenerator::getVersion() const
{
	return tr("1.0");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getDescription()
//-----------------------------------------------------------------------------
QString MemoryMapHeaderGenerator::getDescription() const
{
	return tr("Generates C-headers for memory maps of a component");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getVendor()
//-----------------------------------------------------------------------------
QString MemoryMapHeaderGenerator::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getLicence()
//-----------------------------------------------------------------------------
QString MemoryMapHeaderGenerator::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getLicenceHolder()
//-----------------------------------------------------------------------------
QString MemoryMapHeaderGenerator::getLicenceHolder() const
{
    return tr("Public");
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

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool MemoryMapHeaderGenerator::checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp,
    QSharedPointer<LibraryComponent const> libDesConf, QSharedPointer<LibraryComponent const> /*libDes*/) const
{
	QSharedPointer<Component const> comp = libComp.dynamicCast<Component const>();
	if (!comp)
    {
		return false;
	}

	// if there is no design then header is generated for local memory maps
	if (!libDesConf)
    {
		return comp->hasLocalMemoryMaps();
	}

	// make sure the second parameter is for a design configuration object
	QSharedPointer<DesignConfiguration const> designConf = libDesConf.dynamicCast<DesignConfiguration const>();
	// the design configuration must be for HW or system
	if (designConf)
    {
		return comp->getComponentImplementation() == KactusAttribute::HW;
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::runGenerator()
//-----------------------------------------------------------------------------
void MemoryMapHeaderGenerator::runGenerator( IPluginUtility* utility, QSharedPointer<LibraryComponent> libComp,
    QSharedPointer<LibraryComponent> libDesConf /*= QSharedPointer<LibraryComponent>()*/,
    QSharedPointer<LibraryComponent> libDes /*= QSharedPointer<LibraryComponent>()*/ )
{
	utility_ = utility;

    utility_->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

	QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
	Q_ASSERT(comp);

	QSharedPointer<Design> design = libDes.dynamicCast<Design>();

	// if there is no design object then create headers for local memory maps
	if (!design)
    {
        LocalMemoryMapHeaderWriter localWriter(utility_, comp, this);
        localWriter.writeMemoryMapHeader(localSaveOptions_);
	}
	// if there is a design configuration
	else if (libDesConf)
    {
		Q_ASSERT(design);

		// the component knows the implementation of the view
		KactusAttribute::Implementation implementation = comp->getViewType(*libDesConf->getVlnv());

		QSharedPointer<DesignConfiguration> desConf = libDesConf.dynamicCast<DesignConfiguration>();
		Q_ASSERT(desConf);

		// if the generator is run on a hierarchical HW component
		if (implementation == KactusAttribute::HW)
        {
            GlobalMemoryMapHeaderWriter globalWriter(utility_, design, desConf, this);
            globalWriter.writeMemoryMapHeader(comp, globalSaveOptions_);
		}

		// the generator is run on a system component
		else
        {
            SystemMemoryMapHeaderWriter systemWriter(utility, this);
            systemWriter.writeMemoryMapHeader(comp, desConf, design);
		}
	}

	// if there is a design but no design configuration
	else
    {
		Q_ASSERT(design);

		KactusAttribute::Implementation implementation = comp->getViewType(*design->getVlnv());

		// if the generator is run on a hierarchical HW component
		if (implementation == KactusAttribute::HW)
        {
            GlobalMemoryMapHeaderWriter globalWriter(utility_, design, QSharedPointer<DesignConfiguration> (),
                this);
            globalWriter.writeMemoryMapHeader(comp, globalSaveOptions_);
		}

		// the generator is run on a system component without the configuration
		else
        {
			QMessageBox::warning(utility->getParentWidget(), QCoreApplication::applicationName(),
				tr("A system design opened without configuration.\nSystem design must always have a configuration."));
			return;
		}
	}
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> MemoryMapHeaderGenerator::getProgramRequirements()
{
	return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: memorymapheadergenerator::setLocalSaveFileOptions()
//-----------------------------------------------------------------------------
void MemoryMapHeaderGenerator::setLocalSaveFileOptions(QSharedPointer<MemoryMap> localMemoryMap,
    QFileInfo fileInfo, QString swViewName)
{
    LocalHeaderSaveModel::SaveFileOptions* newSaveOptions (new LocalHeaderSaveModel::SaveFileOptions);
    newSaveOptions->localMemMap_ = localMemoryMap;
    newSaveOptions->fileInfo_ = fileInfo;
    newSaveOptions->swView_ = swViewName;

    localSaveOptions_.clear();
    localSaveOptions_.append(newSaveOptions);
}

//-----------------------------------------------------------------------------
// Function: memorymapheadergenerator::setGlobalSaveFileOptions()
//-----------------------------------------------------------------------------
void MemoryMapHeaderGenerator::setGlobalSaveFileOptions(QSharedPointer<Component> component,
    QString const& instanceName, QString const& instanceId, QString const& masterInterfaceName, QFileInfo fileInfo)
{
    GlobalHeaderSaveModel::SaveFileOptions* newSaveOptions (new GlobalHeaderSaveModel::SaveFileOptions);
    newSaveOptions->comp_ = *component->getVlnv();
    newSaveOptions->instance_ = instanceName;
    newSaveOptions->instanceId_ = instanceId;
    newSaveOptions->interface_ = masterInterfaceName;
    newSaveOptions->fileInfo_ = fileInfo;

    globalSaveOptions_.clear();
    globalSaveOptions_.append(newSaveOptions);
}

//-----------------------------------------------------------------------------
// Function: memorymapheadergenerator::setSystemSaveFileOptions()
//-----------------------------------------------------------------------------
void MemoryMapHeaderGenerator::setSystemSaveFileOptions()
{

}

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
QWidget* MemoryMapHeaderGenerator::getSettingsWidget()
{
    return new QWidget();
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
bool MemoryMapHeaderGenerator::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConfiguration) const
{
	// if there is no design then header is generated for local memory maps
	if (!designConfiguration)
    {
		return component->hasLocalMemoryMaps();
	}

	// the design configuration must be for HW or system
	return designConfiguration->getImplementation() == KactusAttribute::HW;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::runGenerator()
//-----------------------------------------------------------------------------
void MemoryMapHeaderGenerator::runGenerator(IPluginUtility* utility, 
    QSharedPointer<Component> component,
    QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
	utility_ = utility;

    utility_->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

	Q_ASSERT(component);

	// if there is no design object then create headers for local memory maps
	if (!design)
    {
        LocalMemoryMapHeaderWriter localWriter(utility_, component, this);
        localWriter.writeMemoryMapHeader(localSaveOptions_);
	}
	// if there is a design configuration
	else if (designConfiguration)
    {
		Q_ASSERT(design);

		// the component knows the implementation of the view
		KactusAttribute::Implementation implementation = designConfiguration->getImplementation();

		Q_ASSERT(designConfiguration);

		// if the generator is run on a hierarchical HW component
		if (implementation == KactusAttribute::HW)
        {
            GlobalMemoryMapHeaderWriter globalWriter(utility_, design, designConfiguration, this);
            globalWriter.writeMemoryMapHeader(component, globalSaveOptions_);
		}

		// the generator is run on a system component
		else
        {
            SystemMemoryMapHeaderWriter systemWriter(utility_, this);
            systemWriter.writeMemoryMapHeader(component, designConfiguration, design);
		}
	}

	// if there is a design but no design configuration
	else
    {
		Q_ASSERT(design);

		KactusAttribute::Implementation implementation = design->getImplementation();

		// if the generator is run on a hierarchical HW component
		if (implementation == KactusAttribute::HW)
        {
            GlobalMemoryMapHeaderWriter globalWriter(utility_, design, QSharedPointer<DesignConfiguration> (),
                this);
            globalWriter.writeMemoryMapHeader(component, globalSaveOptions_);
		}

		// the generator is run on a system component without the configuration
		else
        {
			QMessageBox::warning(utility_->getParentWidget(), QCoreApplication::applicationName(),
				tr("A system design opened without configuration.\n"
                "System design must always have a configuration."));
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
    newSaveOptions->comp_ = component->getVlnv();
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

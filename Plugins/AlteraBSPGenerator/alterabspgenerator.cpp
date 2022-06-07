/* 
 *	Created on: 15.2.2013
 *	Author:		Antti Kamppi
 * 	File name:	alterabspgenerator.cpp
 * 	Project:	Kactus 2
*/

#include "alterabspgenerator.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <KactusAPI/include/IPluginUtility.h>

#include <IPXACTmodels/Component/Component.h>

#include "alterabspgeneratordialog.h"

#include <QDir>

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
AlteraBSPGenerator::AlteraBSPGenerator():
utility_(nullptr)
{
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getName()
//-----------------------------------------------------------------------------
QString AlteraBSPGenerator::getName() const
{
	static QString name(tr("Altera BSP Generator"));
	return name;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getVersion()
//-----------------------------------------------------------------------------
QString AlteraBSPGenerator::getVersion() const
{
	static QString version(tr("1.0"));
	return version;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getDescription()
//-----------------------------------------------------------------------------
QString AlteraBSPGenerator::getDescription() const
{
    static QString description(tr("Generates board support package for CPU component."));
    return description;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getVendor()
//-----------------------------------------------------------------------------
QString AlteraBSPGenerator::getVendor() const
{
    static QString vendor(tr("Tampere University (tuni.fi)"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getLicence()
//-----------------------------------------------------------------------------
QString AlteraBSPGenerator::getLicence() const
{
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getLicenceHolder()
//-----------------------------------------------------------------------------
QString AlteraBSPGenerator::getLicenceHolder() const
{
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* AlteraBSPGenerator::getSettingsWidget()
{
    return nullptr;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon AlteraBSPGenerator::getIcon() const
{
    return QIcon(":icons/BSPicon24.png");
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool AlteraBSPGenerator::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConfiguration) const
{
	// BSP package can only be run on component editor 
	if (design || designConfiguration)
    {
		return false;
	}

    // Must have a component.
    if (!component)
    {
        return false;
    }
	
	// HW component must be CPU and contain at least one view.
	if (component->getImplementation() == KactusAttribute::HW)
    {
		return component->isCpu() && component->getViews()->size() > 0;
    }

	// By default, always return false.
	return false;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::runGenerator()
//-----------------------------------------------------------------------------
void AlteraBSPGenerator::runGenerator(IPluginUtility* utility, 
    QSharedPointer<Component> component,
    QSharedPointer<Design> /*design*/,
    QSharedPointer<DesignConfiguration> /*designConfiguration*/)
{
	utility_ = utility;
	Q_ASSERT(utility_);

	Q_ASSERT(component);

	
	AlteraBSPGeneratorDialog dialog(utility_->getLibraryInterface(), component, utility_->getParentWidget());

	int result = dialog.exec();

	// if user did not want to add the generated files to the component metadata.
	if (result == QDialog::Rejected)
    {
		return;
	}

	// the path to the component's xml file for creating the relative paths.
	QString xmlPath = utility_->getLibraryInterface()->getPath(component->getVlnv());

	// contains the file type settings for adding files
	QSettings settings;

	// if nothing is changed then no saving is necessary
	bool modified = false;

	// package the generated files to component metadata
	const QList<AlteraBSPGeneratorDialog::GenerationOptions>& genOptions = dialog.getCreatedDirs();
	foreach (const AlteraBSPGeneratorDialog::GenerationOptions& opt, genOptions) {

		modified = true;

		QString fileSetName = QString("%1_bsp").arg(opt.swViewName_);

		// find the file set and if none exists then create one
		QSharedPointer<FileSet> fileSet = component->getFileSet(fileSetName);
		fileSet->setGroups("generatedFiles");
		fileSet->setDescription(tr("Contains the BSP files generated by Altera tools for SW view %1").arg(opt.swViewName_));

		// find the contents of the directory
		QDir genDir(opt.dirPath_);

		// search the file system for generated files
		foreach (QFileInfo const& entry,
            genDir.entryInfoList(QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files | QDir::Dirs))
        {
			addEntry(entry, xmlPath, fileSet, settings);
		}

        QSharedPointer<View> swView = component->getModel()->findView(opt.swViewName_);
        Q_ASSERT(swView);

        QSharedPointer<ComponentInstantiation> insta =
            component->getModel()->findComponentInstantiation(swView->getComponentInstantiationRef());

        if (insta)
        {
            insta->getFileSetReferences()->append(fileSetName);
        }
	}

	if (modified)
    {
		// save the changes to the file sets and SW views
		utility_->getLibraryInterface()->writeModelToFile(component);
	}
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::addEntry()
//-----------------------------------------------------------------------------
void AlteraBSPGenerator::addEntry(const QFileInfo &entry,
	const QString& xmlPath,
	QSharedPointer<FileSet> fileSet,
	QSettings& settings) {

	// Non-existing entries are not packaged
	if (!entry.exists()) {
		return;
	}

	QString relPath = General::getRelativePath(xmlPath, entry.absoluteFilePath());
	Q_ASSERT(!relPath.isEmpty());

	if (entry.isFile()) {
		fileSet->addFile(relPath, settings);
	}
	else if (entry.isDir()) {
		fileSet->addDependency(relPath);
	}
}

//-----------------------------------------------------------------------------
// Function: MemoryMapHeaderGenerator::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> AlteraBSPGenerator::getProgramRequirements() {
	QList<IPlugin::ExternalProgramRequirement> list;

	// windows uses the batch file to run cygwin
#ifdef Q_OS_WIN32
	IPlugin::ExternalProgramRequirement batchFile;
	batchFile.name_ = AlteraBSPGeneratorDialog::VARIABLE_NAMES[AlteraBSPGeneratorDialog::WIN_PATH];
	batchFile.filters_ = tr("Batch files (*.bat)");
	batchFile.description_ = tr("The batch file which starts the Nios II command shell for windows. "
		"Usually named as \"Nios II Command Shell.bat\".");
	list.append(batchFile);

	// others run the shell script
#else
	IPlugin::ExternalProgramRequirement shellFile;
	shellFile.name_ = AlteraBSPGeneratorDialog::VARIABLE_NAMES[AlteraBSPGeneratorDialog::LINUX_PATH];
	shellFile.filters_ = tr("Shell scripts (*.sh)");
	shellFile.description_ = tr("The shell script which sets the environment variables for "
		"Nios II Command shell. Usually named as \"nios2_command_shell.sh\"");
	list.append(shellFile);
#endif

	return list;
}

//-----------------------------------------------------------------------------
// File: MakefileGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.9.2014
//
// Description:
// Makefile generator plugin.
//-----------------------------------------------------------------------------

#include "MakefileGenerator.h"
#include "MakefileGeneratorPlugin.h"
#include "MakeParametersDialog.h"
#include "MakeConfiguration.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QDir>

#include <IPXACTmodels/kactusExtensions/SystemView.h>

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::MakefileGeneratorPlugin()
//-----------------------------------------------------------------------------
MakefileGeneratorPlugin::MakefileGeneratorPlugin() : QObject(0)
{

}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::MakefileGeneratorPlugin()
//-----------------------------------------------------------------------------
MakefileGeneratorPlugin::~MakefileGeneratorPlugin()
{

}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getName()
//----------------------------------------------------------------------------
QString MakefileGeneratorPlugin::getName() const
{
    return "Makefile Generator";
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString MakefileGeneratorPlugin::getVersion() const
{
    return "1.2";
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString MakefileGeneratorPlugin::getDescription() const
{
    return "Generates C/C++ makefiles for a system design.";
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString MakefileGeneratorPlugin::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString MakefileGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString MakefileGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* MakefileGeneratorPlugin::getSettingsWidget()
{
    return new QWidget();
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon MakefileGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/MakefileGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool MakefileGeneratorPlugin::checkGeneratorSupport(
    QSharedPointer<Component const> component,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConfiguration) const
{
    return (design != 0 && designConfiguration != 0 && designConfiguration->getDesignConfigImplementation()
        == KactusAttribute::SYSTEM);
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void MakefileGeneratorPlugin::runGenerator(IPluginUtility* utility, 
    QSharedPointer<Component> component,
    QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    // First state we are running. Tell the version.
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

	// Library interface is utilized.
    LibraryInterface* library = utility->getLibraryInterface();

	// Find the name of the system view pointing to the design configuration.
	QString sysViewName;

	foreach (QSharedPointer<SystemView> view, component->getSystemViews())
	{
		if (view->getHierarchyRef() == designConfiguration->getVlnv())
		{
			sysViewName = view->name();
			break;
		}
    }

    // System view must exist.
    if (sysViewName.isEmpty())
    {
        utility->printError("The design configuration did not match any system view. VLNV: " + 
            designConfiguration->getVlnv().toString());
        return;
    }

	// Parse the design for buildable software stacks.
    SWStackParser stackParser(library, component, design, designConfiguration);
    stackParser.parse(sysViewName);

    // Parse the stacks for buildable objects.
    MakefileParser makeParser(library, stackParser);
    makeParser.parse(component);

    QString targetPath = QFileInfo(library->getPath(design->getVlnv())).absolutePath() + "/sw_" + sysViewName;

    QDir path;
    if (!path.mkpath(targetPath))
    {
        utility->printError("Could not create path for the makefiles: " + targetPath);
        return;
    }

	// Show the dialog.
    QSharedPointer<MakeConfiguration> configuration(new MakeConfiguration(&stackParser));
    configuration->getFileOuput()->setOutputPath(targetPath);
	MakeParametersDialog dialog(configuration, makeParser.getParsedData(), utility->getParentWidget());

	// Return, if user did not want to proceed after seeing it.
	if (dialog.exec() == QDialog::Rejected)
	{
		utility->printError("Makefile generation rejected by user.");
		return;
    }

	// Generate files from parsed data.
    MakefileGenerator generator(makeParser, utility, stackParser.getGeneralFileSet());

	// Also the directory of the top component is needed for the generated files.
	QString topDir = QFileInfo(library->getPath(component->getVlnv())).absolutePath(); 
    int exe_count = generator.generate(configuration->getFileOuput()->getOutputPath(), topDir);

	// Did we actually generate anything?
	if (exe_count > 0)
	{
		// Top component and the design may have been affected by changes -> save.
		library->writeModelToFile(component);
		library->writeModelToFile(design);

		// Inform that the generation is complete.
		utility->printInfo( tr("Makefile generation complete. Created a makefile for %1 executables.").arg(exe_count) );
	}
	else
	{
		// Inform that there was nothing to generate.
		utility->printError( "No makefiles generated.");
	}
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> MakefileGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}
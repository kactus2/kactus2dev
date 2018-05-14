//-----------------------------------------------------------------------------
// File: LinuxDeviceTreePlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Copyright: Tampere University of Technology
// Author: Mikko Teuho
// Date: 08.05.2018
//
// Description:
// Generator plugin for Linux Device Tree.
//-----------------------------------------------------------------------------

#include "LinuxDeviceTreePlugin.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::LinuxDeviceTreePlugin()
//-----------------------------------------------------------------------------
LinuxDeviceTreePlugin::LinuxDeviceTreePlugin()
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::~LinuxDeviceTreePlugin()
//-----------------------------------------------------------------------------
LinuxDeviceTreePlugin::~LinuxDeviceTreePlugin()
{

}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getName()
//----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getName() const
{
    return tr("Linux Device Tree Generator");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getVersion()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getVersion() const
{
    return "0.1";
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getDescription()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getDescription() const
{
    return QString("Creates a Linux Device Tree from the selected document");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getVendor()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getLicence()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString LinuxDeviceTreePlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* LinuxDeviceTreePlugin::getSettingsWidget()
{
    return new QWidget();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getSettingsModel()
//-----------------------------------------------------------------------------
PluginSettingsModel* LinuxDeviceTreePlugin::getSettingsModel()
{
    return NULL;
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon LinuxDeviceTreePlugin::getIcon() const
{
    return QIcon(":icons/LinuxDeviceTreeGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool LinuxDeviceTreePlugin::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> design, QSharedPointer<DesignConfiguration const> designConfiguration) const
{
    return (design != 0 && designConfiguration != 0);
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::runGenerator()
//-----------------------------------------------------------------------------
void LinuxDeviceTreePlugin::runGenerator(IPluginUtility* utility, QSharedPointer<Component> component,
    QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration)
{
    // This time, a design is required
/*
    if (!design)
    {
        utility->printError( "Sample generator got a null design." );
    }
*/

    // This is needed as a member variable to be used more conveniently.
    utility_ = utility;

/*
    // Parse the matching file sets.
    SampleParser sparser;
    sparser.parse(utility->getLibraryInterface(), design);

    // Print info about results.
    utility->printInfo(QObject::tr("Found %1 matching file sets.").arg(sparser.getParsedData()->count()));

    // Initialize writer, provide the parsed data, catch its error signals.
    SampleWriter swriter(sparser.getParsedData());
    connect(&swriter, SIGNAL(reportError(const QString&)), 
        this, SLOT(onErrorReport(const QString&)), Qt::UniqueConnection);

    // Get the path to the top component: The new file will be in the same path.
    QString componentXmlPath = utility->getLibraryInterface()->getPath(component->getVlnv());
    QFileInfo pathInfo(componentXmlPath);

    // Generate.
    swriter.write(component, pathInfo.absolutePath());

    // Inform when done.
    utility->printInfo( "Sample generation complete." );

    // Top component may have been affected by changes -> save.
    utility->getLibraryInterface()->writeModelToFile(component);
*/
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> LinuxDeviceTreePlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: LinuxDeviceTreePlugin:::onErrorReport()
//-----------------------------------------------------------------------------
void LinuxDeviceTreePlugin::onErrorReport(const QString& report)
{
    utility_->printError(report);
}

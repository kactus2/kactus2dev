//-----------------------------------------------------------------------------
// File: PluginMock.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.04.2018
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#include "PluginMock.h"

#include <KactusAPI/include/IPluginUtility.h>

//-----------------------------------------------------------------------------
// Function: PluginMock::PluginMock()
//-----------------------------------------------------------------------------
PluginMock::PluginMock()
{

}

//-----------------------------------------------------------------------------
// Function: PluginMock::~PluginMock()
//-----------------------------------------------------------------------------
PluginMock::~PluginMock()
{

}

//-----------------------------------------------------------------------------
// Function: PluginMock::getName()
//-----------------------------------------------------------------------------
QString PluginMock::getName() const
{
    return QStringLiteral("PluginMock");
}

//-----------------------------------------------------------------------------
// Function: PluginMock::getVersion()
//-----------------------------------------------------------------------------
QString PluginMock::getVersion() const
{
    return QStringLiteral("1.0");
}

//-----------------------------------------------------------------------------
// Function: PluginMock::getDescription()
//-----------------------------------------------------------------------------
QString PluginMock::getDescription() const
{
    return QStringLiteral("Mock object for plugins.");
}

//-----------------------------------------------------------------------------
// Function: PluginMock::getVendor()
//-----------------------------------------------------------------------------
QString PluginMock::getVendor() const
{
    return QStringLiteral("tut.fi");
}

//-----------------------------------------------------------------------------
// Function: PluginMock::getLicense()
//-----------------------------------------------------------------------------
QString PluginMock::getLicense() const
{
    return QStringLiteral("LGLP2");
}

//-----------------------------------------------------------------------------
// Function: PluginMock::getLicenseHolder()
//-----------------------------------------------------------------------------
QString PluginMock::getLicenseHolder() const
{
    return QStringLiteral("Public");
}

//-----------------------------------------------------------------------------
// Function: PluginMock::getSettingsWidget()546582
//-----------------------------------------------------------------------------
QWidget* PluginMock::getSettingsWidget()
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: PluginMock::getSettingsModel()
//-----------------------------------------------------------------------------
PluginSettingsModel* PluginMock::getSettingsModel()
{
    return 0;
}

//-----------------------------------------------------------------------------
// Function: PluginMock::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> PluginMock::getProgramRequirements()
{
    QList<IPlugin::ExternalProgramRequirement> noRequirements;
    return noRequirements;
}

//-----------------------------------------------------------------------------
// Function: PluginMock::getCommand()
//-----------------------------------------------------------------------------
QString PluginMock::getCommand() const
{
    return QStringLiteral("mock");
}

//-----------------------------------------------------------------------------
// Function: PluginMock::process()
//-----------------------------------------------------------------------------
void PluginMock::process(QStringList const& arguments, IPluginUtility* utility)
{
    utility->printInfo(arguments.join(QLatin1Char(' ')));
}

//-----------------------------------------------------------------------------
// Function: PluginMock::getOutputFormat()
//-----------------------------------------------------------------------------
QString PluginMock::getOutputFormat() const
{
    return QString();
}

//-----------------------------------------------------------------------------
// Function: PluginMock::runGenerator()
//-----------------------------------------------------------------------------
void PluginMock::runGenerator(IPluginUtility* utility, QSharedPointer<Component> component, QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration, QString const& viewName, QString const& outputDirectory)
{
}

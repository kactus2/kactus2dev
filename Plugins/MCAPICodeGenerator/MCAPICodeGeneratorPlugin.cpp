//-----------------------------------------------------------------------------
// File: MCAPICodeGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 13.10.2014
//
// Description:
// MCAPI code generator plugin.
//-----------------------------------------------------------------------------

#include "MCAPICodeGeneratorPlugin.h"
#include "MCAPICodeGenerator.h"
#include "MCAPIHeaderGenerator.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>

#include <library/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::MCAPICodeGeneratorPlugin()
//-----------------------------------------------------------------------------
MCAPICodeGeneratorPlugin::MCAPICodeGeneratorPlugin()
{
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::~MCAPICodeGeneratorPlugin()
//-----------------------------------------------------------------------------
MCAPICodeGeneratorPlugin::~MCAPICodeGeneratorPlugin()
{
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::getName()
//----------------------------------------------------------------------------
QString MCAPICodeGeneratorPlugin::getName() const
{
    return "MCAPI Code Generator";
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString MCAPICodeGeneratorPlugin::getVersion() const
{
    return "1.1";
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString MCAPICodeGeneratorPlugin::getDescription() const
{
    return "Generates MCAPI code templates based on the metadata.";
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString MCAPICodeGeneratorPlugin::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString MCAPICodeGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString MCAPICodeGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
QWidget* MCAPICodeGeneratorPlugin::getSettingsWidget()
{
    return new QWidget();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon MCAPICodeGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/McapiGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool MCAPICodeGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConfiguration) const
{
    return (component != 0 && component->getImplementation() == KactusAttribute::SW) ||
        ( design != 0 && designConfiguration != 0 && designConfiguration->getImplementation() == KactusAttribute::SYSTEM );
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void MCAPICodeGeneratorPlugin::runGenerator(IPluginUtility* utility, 
    QSharedPointer<Component> component,
    QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    if (component != 0 && component->getImplementation() == KactusAttribute::SW)
    {
        MCAPIParser parser( utility );
        parser.parseMCAPIForComponent(component);
        MCAPICodeGenerator generator( parser, utility );
        QString dir = QFileInfo(utility->getLibraryInterface()->getPath(component->getVlnv())).absolutePath(); 
        generator.generateMCAPIForComponent(dir, component);
    }
    else if (design != 0 && designConfiguration != 0 &&
        designConfiguration->getImplementation() == KactusAttribute::SYSTEM )
    {
        MCAPIParser parser( utility );
        parser.parseTopLevel(design, component, designConfiguration);

        QStringList replacedFiles = parser.getReplacedFiles();

        // Ask verification from the user, if any file is being replaced,
        if ( replacedFiles.size() > 0 )
        {
            // Details will be the list of files being replaced.
            QString detailMsg;

            foreach ( QString file, replacedFiles )
            {
                detailMsg += file + "\n";
            }

            QMessageBox msgBox( QMessageBox::Warning, QCoreApplication::applicationName(),
                "Some files will be WRITTEN OVER in the generation. Proceed?",
                QMessageBox::Yes | QMessageBox::No, utility->getParentWidget());
            msgBox.setDetailedText(detailMsg);

            // If user did not want to replace the files.
            if (msgBox.exec() == QMessageBox::No) {
                return;
            }
        }

        MCAPIHeaderGenerator generator( parser, utility );
        VLNV topVLNV = component->getVlnv();
        QString topDir = QFileInfo(utility->getLibraryInterface()->getPath(topVLNV)).absolutePath();
        generator.generateTopLevel(component, designConfiguration, topDir);
        utility->getLibraryInterface()->writeModelToFile(design);
    }

    utility->getLibraryInterface()->writeModelToFile(component);

    utility->printInfo( "MCAPI generation complete.");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> MCAPICodeGeneratorPlugin::getProgramRequirements() {
    return QList<IPlugin::ExternalProgramRequirement>();
}
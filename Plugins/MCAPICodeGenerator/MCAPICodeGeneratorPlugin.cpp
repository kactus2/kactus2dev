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
#include "MCAPIParser.h"

#include <QMessageBox>
#include <QCoreApplication>

#include <IPXACTmodels/component.h>

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/PluginSystem/IPluginUtility.h>

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
PluginSettingsWidget* MCAPICodeGeneratorPlugin::getSettingsWidget()
{
    return new PluginSettingsWidget();
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
bool MCAPICodeGeneratorPlugin::checkGeneratorSupport( QSharedPointer<LibraryComponent const> libComp,
    QSharedPointer<LibraryComponent const> libDesConf,
    QSharedPointer<LibraryComponent const> libDes ) const
{
    QSharedPointer<Component const> comp = libComp.dynamicCast<Component const>();
    QSharedPointer<DesignConfiguration const> desgConf = libDesConf.dynamicCast<DesignConfiguration const>();

    return (comp != 0 && comp->getComponentImplementation() == KactusAttribute::SW) ||
        ( libDes != 0 && desgConf != 0 && desgConf->getDesignConfigImplementation() == KactusAttribute::SYSTEM );
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void MCAPICodeGeneratorPlugin::runGenerator( IPluginUtility* utility, 
    QSharedPointer<LibraryComponent> libComp,
    QSharedPointer<LibraryComponent> libDesConf,
    QSharedPointer<LibraryComponent> libDes)
{
    QSharedPointer<Design> design = libDes.dynamicCast<Design>();
    QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
    QSharedPointer<DesignConfiguration const> desgConf = libDesConf.dynamicCast<DesignConfiguration const>();

    if ( comp != 0 && comp->getComponentImplementation() == KactusAttribute::SW )
    {
        MCAPIParser parser( utility );
        parser.parseMCAPIForComponent(comp);
        MCAPICodeGenerator generator( parser, utility );
        QString dir = QFileInfo(utility->getLibraryInterface()->getPath(*libComp->getVlnv())).absolutePath(); 
        generator.generateMCAPIForComponent(dir, comp);
    }
    else if ( libDes != 0 && desgConf != 0 &&
        desgConf->getDesignConfigImplementation() == KactusAttribute::SYSTEM )
    {
        MCAPIParser parser( utility );
        parser.parseTopLevel(design, comp, desgConf);

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
        VLNV* topVLNV = comp->getVlnv();
        QString topDir = QFileInfo(utility->getLibraryInterface()->getPath(*topVLNV)).absolutePath();
        generator.generateTopLevel(design, comp, desgConf, topDir);
        utility->getLibraryInterface()->writeModelToFile(design);
    }

    utility->getLibraryInterface()->writeModelToFile(libComp);

    utility->printInfo( "MCAPI generation complete.");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> MCAPICodeGeneratorPlugin::getProgramRequirements() {
    return QList<IPlugin::ExternalProgramRequirement>();
}
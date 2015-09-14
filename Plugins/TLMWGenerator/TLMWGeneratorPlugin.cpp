//-----------------------------------------------------------------------------
// File: TLMWGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 13.7.2015
//
// Description:
// TLMW generator plugin.
//-----------------------------------------------------------------------------

#include "TLMWGeneratorPlugin.h"
#include "TLMWHeaderGenerator.h"
#include "TLMWParser.h"

#include <QMessageBox>
#include <QCoreApplication>

#include <IPXACTmodels/component.h>

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/PluginSystem/IPluginUtility.h>

//-----------------------------------------------------------------------------
// Function: TLMWCGeneratorPlugin::TLMWGeneratorPlugin()
//-----------------------------------------------------------------------------
TLMWGeneratorPlugin::TLMWGeneratorPlugin()
{
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::~TLMWGeneratorPlugin()
//-----------------------------------------------------------------------------
TLMWGeneratorPlugin::~TLMWGeneratorPlugin()
{
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::getName()
//----------------------------------------------------------------------------
QString TLMWGeneratorPlugin::getName() const
{
    return "TLMW Generator";
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString TLMWGeneratorPlugin::getVersion() const
{
    return "1.0";
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString TLMWGeneratorPlugin::getDescription() const
{
    return "Generates TLMW header files based on the metadata.";
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString TLMWGeneratorPlugin::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: TLMWCodeGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString TLMWGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString TLMWGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* TLMWGeneratorPlugin::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon TLMWGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/TLMWGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool TLMWGeneratorPlugin::checkGeneratorSupport( QSharedPointer<LibraryComponent const> libComp,
    QSharedPointer<LibraryComponent const> libDesConf,
    QSharedPointer<LibraryComponent const> libDes ) const
{
    QSharedPointer<Component const> comp = libComp.dynamicCast<Component const>();
    QSharedPointer<DesignConfiguration const> desgConf = libDesConf.dynamicCast<DesignConfiguration const>();

    return ( libDes != 0 && desgConf != 0 && desgConf->getDesignConfigImplementation() == KactusAttribute::SYSTEM );
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void TLMWGeneratorPlugin::runGenerator( IPluginUtility* utility, 
    QSharedPointer<LibraryComponent> libComp,
    QSharedPointer<LibraryComponent> libDesConf,
    QSharedPointer<LibraryComponent> libDes)
{
    QSharedPointer<Design> design = libDes.dynamicCast<Design>();
    QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
    QSharedPointer<DesignConfiguration const> desgConf = libDesConf.dynamicCast<DesignConfiguration const>();

	if ( libDes != 0 && desgConf != 0 &&
        desgConf->getDesignConfigImplementation() == KactusAttribute::SYSTEM )
    {
        TLMWParser parser( utility );
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

        TLMWHeaderGenerator generator( parser, utility );
        VLNV* topVLNV = comp->getVlnv();
        QString topDir = QFileInfo(utility->getLibraryInterface()->getPath(*topVLNV)).absolutePath();
        generator.generateTopLevel(design, comp, desgConf, topDir);
        utility->getLibraryInterface()->writeModelToFile(design);
    }

    utility->getLibraryInterface()->writeModelToFile(libComp);

    utility->printInfo( "TLMW generation complete.");
}

//-----------------------------------------------------------------------------
// Function: TLMWGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> TLMWGeneratorPlugin::getProgramRequirements() {
    return QList<IPlugin::ExternalProgramRequirement>();
}
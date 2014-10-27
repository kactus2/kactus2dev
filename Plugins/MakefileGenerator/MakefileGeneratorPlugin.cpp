//-----------------------------------------------------------------------------
// File: MakefileGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 15.9.2014
//
// Description:
// Makefile generator plugin.
//-----------------------------------------------------------------------------

#include "MakefileGenerator.h"
#include "MakefileParser.h"
#include "MakefileGeneratorPlugin.h"

#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <QMessageBox>
#include <QCoreApplication>

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
QString const& MakefileGeneratorPlugin::getName() const
{
    static QString name("Makefile Generator");
    return name;
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString const& MakefileGeneratorPlugin::getVersion() const
{
    static QString version("0.1");
    return version;
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString const& MakefileGeneratorPlugin::getDescription() const
{
    static QString desc("Generates Makefile for the design.");
    return desc;
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString const& MakefileGeneratorPlugin::getVendor() const {
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString const& MakefileGeneratorPlugin::getLicence() const {
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& MakefileGeneratorPlugin::getLicenceHolder() const {
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* MakefileGeneratorPlugin::getSettingsWidget()
{
    return new PluginSettingsWidget();
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
bool MakefileGeneratorPlugin::checkGeneratorSupport( QSharedPointer<LibraryComponent const> libComp,
    QSharedPointer<LibraryComponent const> libDesConf,
    QSharedPointer<LibraryComponent const> libDes ) const {

    QSharedPointer<DesignConfiguration const> desgConf = libDesConf.dynamicCast<DesignConfiguration const>();

    return ( libDes != 0 && desgConf != 0 && desgConf->getDesignConfigImplementation() == KactusAttribute::KTS_SYS );
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void MakefileGeneratorPlugin::runGenerator( IPluginUtility* utility, 
    QSharedPointer<LibraryComponent> libComp,
    QSharedPointer<LibraryComponent> libDesConf,
    QSharedPointer<LibraryComponent> libDes)
{
    QSharedPointer<const Design> design = libDes.dynamicCast<Design const>();
    QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
    QSharedPointer<DesignConfiguration const> desgConf = libDesConf.dynamicCast<DesignConfiguration const>();
    
    QString targetDir = QFileInfo(utility->getLibraryInterface()->getPath(*libDes->getVlnv())).absolutePath(); 
    QString topDir = QFileInfo(utility->getLibraryInterface()->getPath(*libComp->getVlnv())).absolutePath(); 

    MakefileParser parser;
    parser.parse( utility->getLibraryInterface(), comp, desgConf, design, targetDir );

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

    MakefileGenerator generator( parser );
    generator.generate(targetDir, topDir);

    utility->getLibraryInterface()->writeModelToFile(libComp);

    utility->printInfo( "Makefile generation complete.");
}

//-----------------------------------------------------------------------------
// Function: MakefileGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> MakefileGeneratorPlugin::getProgramRequirements() {
    return QList<IPlugin::ExternalProgramRequirement>();
}
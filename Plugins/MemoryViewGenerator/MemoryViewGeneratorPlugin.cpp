//-----------------------------------------------------------------------------
// File: MemoryViewGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.04.2016
//
// Description:
// Plugin for creating a CSV listing of all slave memories within a design hierarchy.
//-----------------------------------------------------------------------------

#include "MemoryViewGeneratorPlugin.h"

#include <Plugins/PluginSystem/IPluginUtility.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>

#include "MemoryViewGenerator.h"

#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::MemoryViewGeneratorPlugin()
//-----------------------------------------------------------------------------
MemoryViewGeneratorPlugin::MemoryViewGeneratorPlugin()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::~MemoryViewGeneratorPlugin()
//-----------------------------------------------------------------------------
MemoryViewGeneratorPlugin::~MemoryViewGeneratorPlugin()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::getName()
//-----------------------------------------------------------------------------
QString MemoryViewGeneratorPlugin::getName() const
{
    return tr("Memory View Generator");
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString MemoryViewGeneratorPlugin::getVersion() const
{
    return tr("0.1");
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString MemoryViewGeneratorPlugin::getDescription() const
{
    return tr("Creates a CSV listing of all slave memories within a design hierarchy.");
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString MemoryViewGeneratorPlugin::getVendor() const
{
    return tr("TUT");
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString MemoryViewGeneratorPlugin::getLicence() const
{
    return tr("GPL2");
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString MemoryViewGeneratorPlugin::getLicenceHolder() const
{
    return tr("Public");
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* MemoryViewGeneratorPlugin::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> MemoryViewGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon MemoryViewGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/memory.png");
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool MemoryViewGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Document const> /*libComp*/,
    QSharedPointer<Document const> libDesConf, 
    QSharedPointer<Document const> libDes) const
{
    return libDesConf || libDes;
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void MemoryViewGeneratorPlugin::runGenerator(IPluginUtility* utility, QSharedPointer<Document> libComp, 
    QSharedPointer<Document> libDesConf,
    QSharedPointer<Document> libDes)
{
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));
    
    QString xmlFilePath = utility->getLibraryInterface()->getDirectoryPath(libComp->getVlnv());

    QString targetFile = QFileDialog::getSaveFileName(utility->getParentWidget(), tr("Select target file"), 
        xmlFilePath, tr("Comma separated values (*.csv)"));

    if (targetFile.isEmpty())
    {
        utility->printInfo(tr("Generation aborted."));
        return;
    }

    MemoryViewGenerator generator(utility->getLibraryInterface());   
    generator.generate(libComp.dynamicCast<Component>(), "", targetFile);

    saveToFileset(targetFile, libComp, utility);

    utility->printInfo(tr("Generation complete."));
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::saveToFileset()
//-----------------------------------------------------------------------------
void MemoryViewGeneratorPlugin::saveToFileset(QString const& targetFile, QSharedPointer<Document> component, 
    IPluginUtility* utility)
{
    QString xmlFilePath = utility->getLibraryInterface()->getDirectoryPath(component->getVlnv());

    QSharedPointer<Component> topComponent = component.dynamicCast<Component>();

    QString filesetName = "memoryListings";
    QSharedPointer<FileSet> targetFileset = topComponent->getFileSet(filesetName);
    if (!targetFileset)
    {
        targetFileset = QSharedPointer<FileSet>(new FileSet(filesetName));
        topComponent->getFileSets()->append(targetFileset);
    }

    if (targetFileset->getFile(targetFile).isNull())
    {
        QString relativeFilePath = General::getRelativePath(xmlFilePath, targetFile);

        QSharedPointer<File> file(new File(relativeFilePath));
        file->getFileTypes()->append(QStringLiteral("unknown"));

        targetFileset->addFile(file);

        utility->printInfo(tr("Added file %1 to top component file set %2.").arg(targetFile, filesetName));

        utility->getLibraryInterface()->writeModelToFile(component);
    }
}

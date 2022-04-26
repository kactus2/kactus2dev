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

#include <Plugins/common/HDLParser/HDLParserCommon.h>
#include <Plugins/PluginSystem/IPluginUtility.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GenerationControl.h>

#include <library/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>

#include "MemoryViewGenerator.h"

#include <QDateTime>
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
    return tr("0.2");
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
    return tr("Tampere University (tuni.fi)");
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
QWidget* MemoryViewGeneratorPlugin::getSettingsWidget()
{
    return nullptr;
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
bool MemoryViewGeneratorPlugin::checkGeneratorSupport(QSharedPointer<Component const> component,
    QSharedPointer<Design const> design,
    QSharedPointer<DesignConfiguration const> designConfiguration) const
{
    if (!component || component->getImplementation() != KactusAttribute::HW)
    {
        return false;
    }

    return design || designConfiguration;
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void MemoryViewGeneratorPlugin::runGenerator(IPluginUtility* utility, 
    QSharedPointer<Component> component,
    QSharedPointer<Design> design,
    QSharedPointer<DesignConfiguration> designConfiguration)
{
    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));
    
    QString xmlFilePath = utility->getLibraryInterface()->getDirectoryPath(component->getVlnv());

    QString targetFile = QFileDialog::getSaveFileName(utility->getParentWidget(), tr("Select target file"), 
        xmlFilePath, tr("Comma separated values (*.csv)"));

    if (!targetFile.isEmpty())
    {
        GenerationTuple controlTuple;
        controlTuple.component = component;
        controlTuple.design = design;
        controlTuple.designConfiguration = designConfiguration;

        QSharedPointer<QList<QSharedPointer<View> > > views = GenerationControl::findPossibleViews(controlTuple);
        QString activeView;
        if (!views->isEmpty())
        {
            activeView = views->first()->name();
        }

        MemoryViewGenerator generator(utility->getLibraryInterface());
        generator.generate(component, activeView, targetFile);

        saveToFileset(targetFile, component, utility);

        utility->printInfo(tr("Generation complete."));
    }
    else
    {
        utility->printInfo(tr("Generation aborted."));
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryViewGeneratorPlugin::saveToFileset()
//-----------------------------------------------------------------------------
void MemoryViewGeneratorPlugin::saveToFileset(QString const& targetFile, QSharedPointer<Component> component, 
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

    QString relativeFilePath = General::getRelativePath(xmlFilePath, targetFile);

    QSharedPointer<File> file;
    foreach (QSharedPointer<File> filesetFile, *targetFileset->getFiles())
    {            
        if (filesetFile->name().compare(relativeFilePath) == 0)
        {
            file = filesetFile;
        }
    }

    if (!file)
    {
        file = QSharedPointer<File>(new File(relativeFilePath));
        file->getFileTypes()->append(QStringLiteral("unknown"));

        targetFileset->addFile(file);
        utility->printInfo(tr("Added file %1 to top component file set %2.").arg(targetFile, filesetName));
    }

    file->setDescription(tr("Generated on %1 by Kactus2 %2 plugin version %3.").arg(
        QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss"), getName(), getVersion()));

    utility->getLibraryInterface()->writeModelToFile(component);
}

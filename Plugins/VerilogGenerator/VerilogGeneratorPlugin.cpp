//-----------------------------------------------------------------------------
// File: VerilogGeneratorPlugin.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.08.2014
//
// Description:
// Plugin for structural verilog generation.
//-----------------------------------------------------------------------------

#include "veriloggeneratorplugin.h"

#include "VerilogGenerator/VerilogGenerator.h"

#include <Plugins/PluginSystem/IPluginUtility.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/fileset.h>

#include <QDateTime>
#include <QFileDialog>
#include <QMessageBox>


//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::()
//-----------------------------------------------------------------------------
VerilogGeneratorPlugin::VerilogGeneratorPlugin(): QObject(0), utility_(0), topComponent_(0), outputFile_()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::()
//-----------------------------------------------------------------------------
VerilogGeneratorPlugin::~VerilogGeneratorPlugin()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getName()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getName() const
{
    static QString name(tr("Verilog Generator"));
    return name;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getVersion()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getVersion() const
{
    static QString version(tr("1.0"));
    return version;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getDescription()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getDescription() const
{
    static QString description(tr("Generates a top-level Verilog module for a HW design or component."));
    return description;
}
//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getVendor()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getVendor() const
{
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicence()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getLicence() const
{
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& VerilogGeneratorPlugin::getLicenceHolder() const
{
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* VerilogGeneratorPlugin::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> VerilogGeneratorPlugin::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::getIcon()
//-----------------------------------------------------------------------------
QIcon VerilogGeneratorPlugin::getIcon() const
{
    return QIcon(":icons/verilogGenerator.png");
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp,
    QSharedPointer<LibraryComponent const> libDesConf,
    QSharedPointer<LibraryComponent const> libDes) const
{
    QSharedPointer<const Component> targetComponent = libComp.dynamicCast<const Component>();
    
    return targetComponent && targetComponent->getComponentImplementation() == KactusAttribute::KTS_HW;    
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::runGenerator()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::runGenerator(IPluginUtility* utility, 
    QSharedPointer<LibraryComponent> libComp,
    QSharedPointer<LibraryComponent> libDesConf,
    QSharedPointer<LibraryComponent> libDes)
{
    utility_ = utility;
    topComponent_ = libComp.dynamicCast<Component>();

    utility->printInfo(tr("Running %1 %2.").arg(getName(), getVersion()));

    VerilogGenerator generator(utility->getLibraryInterface());
    generator.parse(topComponent_, libDes.dynamicCast<Design>());

    selectOutputFile();

    if (outputFileSelected())
    {
        utility_->printInfo(tr("Generation started %1.").arg(QDateTime::currentDateTime().toString(Qt::LocalDate)));

        generator.generate(outputFile_);

        utility_->printInfo(tr("Finished writing file %1.").arg(outputFile_));

        if (fileShouldBeAddedToFileset())
        {          
            addGeneratedFileToFileSet();
        }

        utility_->printInfo(tr("Generation complete."));
    }    
    else
    {
        utility_->printInfo(tr("Generation aborted."));
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::selectOutputFile()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::selectOutputFile()
{
    QString suggestedFile = "";

    if (outputFileSelected())
    {
        suggestedFile  = outputFile_;
    }
    else
    {
        QString topComponentPath = utility_->getLibraryInterface()->getPath(*topComponent_->getVlnv());
        QString xmlDir =  QFileInfo(topComponentPath).canonicalPath();
        suggestedFile = xmlDir + "/" + topComponent_->getVlnv()->getName() + ".v";
    }

    outputFile_ = QFileDialog::getSaveFileName(utility_->getParentWidget(),
        tr("Select output file for verilog generation"), suggestedFile, tr("Verilog files (*.v)"));
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::outputFileSelected()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::outputFileSelected() const
{
    return !outputFile_.isEmpty();
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::fileShouldBeAddedToFileset()
//-----------------------------------------------------------------------------
bool VerilogGeneratorPlugin::fileShouldBeAddedToFileset() const
{
    bool addToFileset = false;

    QString filePath = relativePathFromXmlToFile(outputFile_);

    if (!topComponent_->hasFile(filePath))
    {
        QMessageBox::StandardButton button = QMessageBox::question(utility_->getParentWidget(), 
            tr("Save generated file to metadata?"),
            tr("Would you like to save the generated file to IP-XACT metadata?"), 
            QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    
        addToFileset = (button == QMessageBox::Yes);
    }

    return addToFileset;
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::relativePathForFile()
//-----------------------------------------------------------------------------
QString VerilogGeneratorPlugin::relativePathFromXmlToFile(QString const& filePath) const
{
    QString xmlPath = utility_->getLibraryInterface()->getPath(*topComponent_->getVlnv());
    return General::getRelativePath(xmlPath, filePath);
}

//-----------------------------------------------------------------------------
// Function: VerilogGeneratorPlugin::addFileToFileSet()
//-----------------------------------------------------------------------------
void VerilogGeneratorPlugin::addGeneratedFileToFileSet() const
{
    QString filePath = relativePathFromXmlToFile(outputFile_);

    QSettings settings;
    QSharedPointer<FileSet> fileSet = topComponent_->getFileSet("verilogSource");
    fileSet->addFile(filePath, settings);
    
    bool saveSucceeded = utility_->getLibraryInterface()->writeModelToFile(topComponent_);
    if (saveSucceeded)
    {
        utility_->printInfo(tr("Added file %1 to fileset %2.").arg(outputFile_, fileSet->getName()));
    }    
    else
    {
        QString component = topComponent_->getVlnv()->toString();
        QString savePath = utility_->getLibraryInterface()->getPath(*topComponent_->getVlnv());
        utility_->printError(tr("Could not write component %1 to file %2.").arg(component, savePath));
    }
}

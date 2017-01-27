//-----------------------------------------------------------------------------
// File: GeneratorConfiguration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2016
//
// Description:
// Container class for generator configuration.
//-----------------------------------------------------------------------------

#include "GeneratorConfiguration.h"

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <Plugins/common/HDLParser/HDLComponentParser.h>
#include <Plugins/common/HDLParser/MetaDesign.h>

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::GeneratorConfiguration(LibraryInterface* library, GenerationTuple input,
    QSharedPointer<ViewSelection> viewSelection, bool isDesign) :
	library_(library), input_(input), viewSelection_(viewSelection), isDesign_(isDesign),
    fileOutput_(new FileOuput), generateInterfaces_(false), generateMemory_(false)
{
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::~GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::~GeneratorConfiguration()
{
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::validSelections()
//-----------------------------------------------------------------------------
bool GeneratorConfiguration::validSelections(QString &warning)
{
    // Must have a file set as well.
    if (!viewSelection_->validSelections(warning))
    {
        return false;
    }

    // Must have valid file output.
    if (!fileOutput_->validSelections(warning))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::parseDocuments()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::parseDocuments()
{
    // Clear the existing list of files.
    fileOutput_->getFiles()->clear();

    VerilogWriterFactory vFac(library_, generateInterfaces_, generateMemory_, "foo", "bar");

    if (isDesign_)
    {
        // Parse the design hierarchy.
        QList<QSharedPointer<MetaDesign> > designs =
            MetaDesign::parseHierarchy(library_, input_, viewSelection_->getView());

        // Go through the parsed designs.
        foreach(QSharedPointer<MetaDesign> design, designs)
        {
            QSharedPointer<GenerationFile> gFile = vFac.prepareDesign(design);
            gFile->write();

            fileOutput_->getFiles()->append(gFile);
        }
    }
    else
    {
        QSharedPointer<HDLComponentParser> componentParser
            (new HDLComponentParser(library_, input_.component, viewSelection_->getView()));

        QSharedPointer<GenerationFile> gFile = vFac.prepareComponent(fileOutput_->getOutputPath(), componentParser);
        gFile->write();

        fileOutput_->getFiles()->append(gFile);
    }

    // Emit the signal.
    emit outputFilesChanged();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getViewSelection()
//-----------------------------------------------------------------------------
QSharedPointer<ViewSelection> GeneratorConfiguration::getViewSelection() const
{
    return viewSelection_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getFileOuput()
//-----------------------------------------------------------------------------
QSharedPointer<FileOuput> GeneratorConfiguration::getFileOuput() const
{
    return fileOutput_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setInterfaceGeneration()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setInterfaceGeneration(bool shouldGenerate)
{
    generateInterfaces_ = shouldGenerate;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getInterfaceGeneration()
//-----------------------------------------------------------------------------
bool GeneratorConfiguration::getInterfaceGeneration() const
{
    return generateInterfaces_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setMemoryGeneration()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setMemoryGeneration(bool shouldGenerate)
{
    generateMemory_ = shouldGenerate;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getMemoryGeneration()
//-----------------------------------------------------------------------------
bool GeneratorConfiguration::getMemoryGeneration() const
{
    return generateMemory_;
}
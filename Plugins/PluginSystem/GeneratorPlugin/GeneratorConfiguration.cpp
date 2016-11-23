//-----------------------------------------------------------------------------
// File: GeneratorConfiguration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 23.02.2015
//
// Description:
// Container class for generator configuration.
//-----------------------------------------------------------------------------

#include "GeneratorConfiguration.h"

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::GeneratorConfiguration(QSharedPointer<ViewSelection> viewSelection,
    HDLComponentParser* componentParser, HDLDesignParser* designParser) :
	viewSelection_(viewSelection), componentParser_(componentParser), designParser_(designParser),
    fileOutput_(new FileOuput), generateInterface_(false), generateMemory_(false)
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
    // If the file is not saved to a file set, no selections needed.
    if (viewSelection_->getSaveToFileset())
    {
        // Must have a file set as well.
        if (viewSelection_->getFileSetName().isEmpty())
        {
            warning = QLatin1String("<b>Define the file set.</b>");
            return false;
        }
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
    // Clear the existing list of file names and VLNVs.
    fileOutput_->getFileNames()->clear();
    fileOutput_->getVLNVs()->clear();

    // Parse the top level component.
    componentParser_->parseComponent(viewSelection_->getView());

    if (designParser_)
    {
        // Parse the designs hierarchy.
        designParser_->parseDesign(componentParser_->getParsedComponent(), viewSelection_->getView());

        // Go through the parsed designs.
        foreach(QSharedPointer<GenerationDesign> design, designParser_->getParsedDesigns())
        {
            // Point to the parsed file name.
            QString* fileName = &design->topComponent_->fileName_;
            // Add the file suffix to it.
            *fileName = design->topComponent_->moduleName_ + ".v";
            // Inform the names to the model.
            fileOutput_->getFileNames()->append(fileName);

            // Append the VLNV to the list.
            fileOutput_->getVLNVs()->append(design->topComponent_->component->getVlnv().toString());
        }
    }
    else
    {
        // Point to the parsed file name.
        QString* fileName = &componentParser_->getParsedComponent()->fileName_;
        // Add the file suffix to it.
        *fileName = componentParser_->getParsedComponent()->moduleName_ + ".v";
        // Inform the names to the model.
        fileOutput_->getFileNames()->append(fileName);

        // Append the VLNV to the list.
        fileOutput_->getVLNVs()->append(componentParser_->getParsedComponent()->component->getVlnv().toString());
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
    generateInterface_ = shouldGenerate;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getInterfaceGeneration()
//-----------------------------------------------------------------------------
bool GeneratorConfiguration::getInterfaceGeneration() const
{
    return generateInterface_;
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
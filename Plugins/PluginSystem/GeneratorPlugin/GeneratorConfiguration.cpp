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
    HDLComponentParser* componentParser, HDLDesignParser* designParser,
    VerilogGenerator* generator) :
	viewSelection_(viewSelection), componentParser_(componentParser), designParser_(designParser),
    generator_(generator),
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
            fileOutput_->getVLNVs()->append(design->topComponent_->component_->getVlnv().toString());
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
        fileOutput_->getVLNVs()->append(componentParser_->getParsedComponent()->component_->getVlnv().toString());
    }

    // Emit the signal.
    emit outputFilesChanged();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getPreview()
//-----------------------------------------------------------------------------
QString GeneratorConfiguration::getPreview()
{
    return "";

    // Prepare the generator.
    /*if (designGeneration)
    {
        QList<QSharedPointer<GenerationDesign> > designs = designParser->getParsedDesigns();
        foreach (QSharedPointer<GenerationDesign> design, designs)
        {
            // Design generation gets parsed designs as a parameter, and the output path.
            generator.prepareDesign(configuration->getFileOuput()->getOutputPath(), design);
        }
    }
    else
    {
        // Component generation gets the parsed component as a parameter, and the output path.
        if (!generator.prepareComponent(configuration->getFileOuput()->getOutputPath(), componentParser->getParsedComponent()))
        {
            // If it says no-go, abort the generation.
            return "CANNOT PARSE";
        }
    }

    VerilogDocument document = generator_->getDocuments()->first();
    QFile outputFile(document->filePath_); 
    if (!outputFile.open(QIODevice::WriteOnly))
    {
        emit reportError(tr("Could not open output file for writing: %1").arg(document->filePath_));
        return;
    }

    QTextStream outputStream(&outputFile);

    generate(outputStream, document);

    outputFile.close();*/
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
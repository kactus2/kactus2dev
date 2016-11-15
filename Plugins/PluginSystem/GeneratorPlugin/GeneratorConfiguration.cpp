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

#include <QDir>

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

    // Must have path for the files. 
    if (fileOutput_->getOutputPath().isEmpty() || !QDir(fileOutput_->getOutputPath()).exists())
    {
        warning = QLatin1String("<b>The output directory must exist!</b>");
        return false;
    }

   return true;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::parseDocuments()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::parseDocuments()
{
    fileOutput_->getFileNames()->clear();

    componentParser_->parseComponent(viewSelection_->getView());

    QStringList vlnvs;

    if (designParser_)
    {
        designParser_->parseDesign(componentParser_->getParsedComponent(), viewSelection_->getView());

        foreach(QSharedPointer<GenerationDesign> design, designParser_->getParsedDesigns())
        {
            QString* fileName = &design->topComponent_->fileName_;
            *fileName = design->topComponent_->moduleName_ + ".v";
            fileOutput_->getFileNames()->append(fileName);

            vlnvs.append(design->topComponent_->component->getVlnv().toString());
        }
    }
    else
    {
        QString* fileName = &componentParser_->getParsedComponent()->fileName_;
        *fileName = componentParser_->getParsedComponent()->moduleName_ + ".v";
        fileOutput_->getFileNames()->append(fileName);

        vlnvs.append(componentParser_->getParsedComponent()->component->getVlnv().toString());
    }

    emit outputFilesChanged(vlnvs);
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
// Function: GeneratorConfiguration::getInterfaceGeneration()
//-----------------------------------------------------------------------------
bool GeneratorConfiguration::getInterfaceGeneration() const
{
    return generateInterface_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setInterfaceGeneration()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setInterfaceGeneration(bool shouldGenerate)
{
    generateInterface_ = shouldGenerate;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getMemoryGeneration()
//-----------------------------------------------------------------------------
bool GeneratorConfiguration::getMemoryGeneration() const
{
    return generateMemory_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setMemoryGeneration()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setMemoryGeneration(bool shouldGenerate)
{
    generateMemory_ = shouldGenerate;
}
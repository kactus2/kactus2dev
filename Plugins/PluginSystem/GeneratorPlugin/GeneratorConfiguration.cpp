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
    outputPath_(), fileNames_(new QList<QString*>), generateInterface_(false), generateMemory_(false)
{
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::~GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::~GeneratorConfiguration()
{
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::parseDocuments()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::parseDocuments()
{
    fileNames_->clear();

    componentParser_->parseComponent(viewSelection_->getView());

    QStringList vlnvs;

    if (designParser_)
    {
        designParser_->parseDesign(componentParser_->getParsedComponent(), viewSelection_->getView());

        foreach(QSharedPointer<GenerationDesign> design, designParser_->getParsedDesigns())
        {
            QString* fileName = &design->topComponent_->fileName_;
            *fileName = design->topComponent_->moduleName_ + ".v";
            fileNames_->append(fileName);

            vlnvs.append(design->topComponent_->component->getVlnv().toString());
        }
    }
    else
    {
        QString* fileName = &componentParser_->getParsedComponent()->fileName_;
        *fileName = componentParser_->getParsedComponent()->moduleName_ + ".v";
        fileNames_->append(fileName);

        vlnvs.append(componentParser_->getParsedComponent()->component->getVlnv().toString());
    }

    emit outputFilesChanged(vlnvs);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getFileNames()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QString*> > GeneratorConfiguration::getFileNames()
{
    return fileNames_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getViewSelection()
//-----------------------------------------------------------------------------
QSharedPointer<ViewSelection> GeneratorConfiguration::getViewSelection() const
{
    return viewSelection_;
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

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setOutputPath()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setOutputPath(QString const& path)
{
    outputPath_ = path;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getOutputPath()
//-----------------------------------------------------------------------------
QString GeneratorConfiguration::getOutputPath() const
{
    return outputPath_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setOutputFileName()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setOutputFileName(QString newName, int index)
{
    if (index < 0 || index >= fileNames_->size())
    {
        return;
    }

    QString* modpath = fileNames_->at(index);
    *modpath = newName;
}
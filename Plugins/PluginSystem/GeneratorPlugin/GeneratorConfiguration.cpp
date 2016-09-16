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
    QSharedPointer<HDLComponentParser> componentParser,
    QSharedPointer<HDLDesignParser> designParser) :
	viewSelection_(viewSelection), componentParser_(componentParser), designParser_(designParser),
    outputPath_(), outputPaths_(new QStringList), generateInterface_(false)
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
    outputPaths_->clear();

    componentParser_->parseComponent(viewSelection_->getView());

    if (designParser_)
    {
        designParser_->parseDesign(componentParser_->getParsedComponent(), viewSelection_->getView());

        foreach(QSharedPointer<GenerationDesign> design, designParser_->getParsedDesigns())
        {
            QString path = outputPath_ + "/" + design->topComponent_->component->getVlnv().getName();
            outputPaths_->append(path);
            design->topComponent_->path_ = path;
        }
    }
    else
    {
        QString path = outputPath_ + "/" + componentParser_->getParsedComponent()->component->getVlnv().getName();
        outputPaths_->append(path);
        componentParser_->getParsedComponent()->path_ = path;
    }

    emit outputFilesChanged();
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getDocumentNames()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> GeneratorConfiguration::getDocumentNames()
{
    return outputPaths_;
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
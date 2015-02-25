//-----------------------------------------------------------------------------
// File: GeneratorConfiguration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.02.2015
//
// Description:
// Container class for generator configuration.
//-----------------------------------------------------------------------------

#include "GeneratorConfiguration.h"

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::GeneratorConfiguration() : outputPath_(), activeViewName_(), saveToFileset_(false)
{

}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::~GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::~GeneratorConfiguration()
{

}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setActiveView()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setActiveView(QString const& viewName)
{
    activeViewName_ = viewName;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getActiveViewName()
//-----------------------------------------------------------------------------
QString GeneratorConfiguration::getActiveViewName() const
{
    return activeViewName_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getAddToFileset()
//-----------------------------------------------------------------------------
bool GeneratorConfiguration::getSaveToFileset() const
{
    return saveToFileset_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setAddToFileset()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setSaveToFileset(bool shouldSave)
{
   saveToFileset_ = shouldSave;
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

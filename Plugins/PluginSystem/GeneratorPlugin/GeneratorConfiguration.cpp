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

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::GeneratorConfiguration() : outputPath_(), activeView_(), saveToFileset_(false)
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
void GeneratorConfiguration::setActiveView(QSharedPointer<View> view)
{
    activeView_ = view;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getActiveView()
//-----------------------------------------------------------------------------
QSharedPointer<View> GeneratorConfiguration::getActiveView() const
{
    return activeView_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setActiveView()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setInstantiation(QSharedPointer<ComponentInstantiation> instantiation)
{
	instantiation_ = instantiation;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getActiveView()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> GeneratorConfiguration::getInstantiation() const
{
	return instantiation_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setFileSetRef()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setFileSetRef(QString const& fileSetRef)
{
	fileSetRef_ = fileSetRef;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getFileSetRef()
//-----------------------------------------------------------------------------
QString GeneratorConfiguration::getFileSetRef() const
{
	return fileSetRef_;
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

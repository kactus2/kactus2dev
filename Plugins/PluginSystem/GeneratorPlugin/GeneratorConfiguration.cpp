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
#include <IPXACTmodels/Component/FileSet.h>

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::GeneratorConfiguration(
	QSharedPointer<QList<QSharedPointer<View> > > views, 
	QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations, 
	QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets) :
	outputPath_(), view_(), saveToFileset_(false)
{
	// Set names as the of views as items, but also track the view objects.
	foreach (QSharedPointer<View> currentView, *views)
	{
		views_[currentView->name()] = currentView;
	}

	// Set names as the of instantiations as items, but also track the instantiation objects.
	foreach (QSharedPointer<ComponentInstantiation> cimp, *instantiations)
	{
		instantiations_[cimp->name()] = cimp;
	}

	foreach (QSharedPointer<FileSet> fileSet, *fileSets)
	{
		fileSets_[fileSet->name()] = fileSet;
	}
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::~GeneratorConfiguration()
//-----------------------------------------------------------------------------
GeneratorConfiguration::~GeneratorConfiguration()
{
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::viewNames()
//-----------------------------------------------------------------------------
QStringList GeneratorConfiguration::viewNames() const
{
	return QStringList(views_.keys());
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::instantiationNames()
//-----------------------------------------------------------------------------
QStringList GeneratorConfiguration::instantiationNames() const
{
	return QStringList(instantiations_.keys());
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::fileSetNames()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> GeneratorConfiguration::fileSetNames() const
{
	if (instantiation_)
	{
		return instantiation_->getFileSetReferences();
	}

	return QSharedPointer<QStringList>(new QStringList);
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setActiveView()
//-----------------------------------------------------------------------------
void GeneratorConfiguration::setView(QSharedPointer<View> view)
{
	view_ = view;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setActiveView()
//-----------------------------------------------------------------------------
QSharedPointer<View> GeneratorConfiguration::setView(QString viewName)
{
    view_ = views_[viewName];

	return view_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getActiveView()
//-----------------------------------------------------------------------------
QSharedPointer<View> GeneratorConfiguration::getView() const
{
    return view_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setInstantiation()
//-----------------------------------------------------------------------------
bool GeneratorConfiguration::setInstantiation(QString name)
{
	instantiationName_ = name;

	instantiation_ = instantiations_.value(name);

	if (instantiation_)
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> GeneratorConfiguration::getInstantiation() const
{
	return instantiation_;
}
//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getInstantiationName()
//-----------------------------------------------------------------------------
QString GeneratorConfiguration::getInstantiationName() const
{
	return instantiationName_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::setFileSetRef()
//-----------------------------------------------------------------------------
bool GeneratorConfiguration::setFileSet(QString const& fileSetRef)
{
	fileSet_ = fileSets_[fileSetRef];
	fileSetRef_ = fileSetRef;

	if (fileSet_)
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> GeneratorConfiguration::getFileSet() const
{
	return fileSet_;
}

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getFileSetRef()
//-----------------------------------------------------------------------------
QString GeneratorConfiguration::getFileSetName() const
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

//-----------------------------------------------------------------------------
// Function: GeneratorConfiguration::getCurrentLanguage()
//-----------------------------------------------------------------------------
QString GeneratorConfiguration::getCurrentLanguage() const
{
	if (instantiation_)
	{
		return instantiation_->getLanguage();
	}

	return "";
}
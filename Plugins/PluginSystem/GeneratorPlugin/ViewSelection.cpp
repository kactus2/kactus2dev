//-----------------------------------------------------------------------------
// File: ViewSelection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 13.09.2016
//
// Description:
// Container class for view selection.
//-----------------------------------------------------------------------------

#include "ViewSelection.h"

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>

//-----------------------------------------------------------------------------
// Function: ViewSelection::ViewSelection()
//-----------------------------------------------------------------------------
ViewSelection::ViewSelection( QString targetLanguage,
	QSharedPointer<QList<QSharedPointer<View> > > views, 
	QSharedPointer<QList<QSharedPointer<ComponentInstantiation> > > instantiations, 
	QSharedPointer<QList<QSharedPointer<FileSet> > > fileSets) :
	targetLanguage_(targetLanguage), view_(), saveToFileset_(false)
{
	// Track the views by name.
	foreach (QSharedPointer<View> currentView, *views)
	{
		views_[currentView->name()] = currentView;
	}

	// Track the instantiations by name.
	foreach (QSharedPointer<ComponentInstantiation> cimp, *instantiations)
	{
		instantiations_[cimp->name()] = cimp;
	}

	// Track the file sets by name.
	foreach (QSharedPointer<FileSet> fileSet, *fileSets)
	{
		fileSets_[fileSet->name()] = fileSet;
	}
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::~ViewSelection()
//-----------------------------------------------------------------------------
ViewSelection::~ViewSelection()
{
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::viewNames()
//-----------------------------------------------------------------------------
QStringList ViewSelection::viewNames() const
{
	return QStringList(views_.keys());
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::instantiationNames()
//-----------------------------------------------------------------------------
QStringList ViewSelection::instantiationNames() const
{
	return QStringList(instantiations_.keys());
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::fileSetNames()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> ViewSelection::fileSetNames() const
{
	if (instantiation_)
	{
		return instantiation_->getFileSetReferences();
	}

	return QSharedPointer<QStringList>(new QStringList);
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::getDefaultView()
//-----------------------------------------------------------------------------
QString ViewSelection::getDefaultView()
{
   foreach(QSharedPointer<View> view, views_)
   {
       QSharedPointer<ComponentInstantiation> cimp = instantiations_[view->getComponentInstantiationRef()];

       if (cimp && cimp->getLanguage() == targetLanguage_)
       {
            return view->name();
       }
   }

   return "";
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::setView()
//-----------------------------------------------------------------------------
void ViewSelection::setView(QSharedPointer<View> view)
{
	view_ = view;
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::setView()
//-----------------------------------------------------------------------------
void ViewSelection::setView(QString viewName)
{
    view_ = views_[viewName];

    instantiation_ = instantiations_.value(view_->getComponentInstantiationRef());
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::getActiveView()
//-----------------------------------------------------------------------------
QSharedPointer<View> ViewSelection::getView() const
{
    return view_;
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::getInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> ViewSelection::getInstantiation() const
{
	return instantiation_;
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::getInstantiationName()
//-----------------------------------------------------------------------------
QString ViewSelection::getInstantiationName() const
{
	return view_->getComponentInstantiationRef();
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::setFileSetRef()
//-----------------------------------------------------------------------------
bool ViewSelection::setFileSet(QString const& fileSetRef)
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
// Function: ViewSelection::getFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> ViewSelection::getFileSet() const
{
	return fileSet_;
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::getFileSetRef()
//-----------------------------------------------------------------------------
QString ViewSelection::getFileSetName() const
{
	return fileSetRef_;
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::getAddToFileset()
//-----------------------------------------------------------------------------
bool ViewSelection::getSaveToFileset() const
{
    return saveToFileset_;
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::setAddToFileset()
//-----------------------------------------------------------------------------
void ViewSelection::setSaveToFileset(bool shouldSave)
{
   saveToFileset_ = shouldSave;
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::targetLanguage()
//-----------------------------------------------------------------------------
QString ViewSelection::getTargetLanguage() const
{
    return targetLanguage_;
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::getCurrentLanguage()
//-----------------------------------------------------------------------------
QString ViewSelection::getCurrentLanguage() const
{
	if (instantiation_)
	{
		return instantiation_->getLanguage();
	}

	return "";
}
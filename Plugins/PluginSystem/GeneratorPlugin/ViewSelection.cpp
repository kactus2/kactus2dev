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
ViewSelection::ViewSelection(QString const& targetLanguage, 
	QString const& defaultViewName, 
	QString const& defaultFileSetName,
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

    // Try to find the default view from the mapping.
    view_ = views_.value(defaultViewName);

    if (!view_)
    {
        // If not found, try to find a view that has the target language in its instantiation.
        foreach(QSharedPointer<View> view, views_)
        {
            QSharedPointer<ComponentInstantiation> cimp = instantiations_.value(view->getComponentInstantiationRef());

            if (cimp && cimp->getLanguage().toLower() == targetLanguage_.toLower())
            {
                // If this is a match, pick both view and instantiation.
                view_ = view;
                instantiation_ = cimp;
                break;
            }
        }
    }

    // If none found, just pick the first one.
    if (!view_ && views_.size() > 0)
    {
        view_ = views_.first();
    }

    // If view is found, but not the instantiation, try to pick instantiation referenced by the view.
    if (view_ && !instantiation_)
    {
        instantiation_ = instantiations_.value(view_->getComponentInstantiationRef());
    }

    // Also try find a default file set.
    fileSet_ = fileSets_.value(defaultFileSetName);

    // If instantiation exists and it has file set references, but none of them is the default,
    // pick the first one.
    if (instantiation_ && instantiation_->getFileSetReferences()->size() > 0 &&
        (!fileSet_ || !instantiation_->getFileSetReferences()->contains(fileSet_->name())))
    {
        fileSet_ = fileSets_.value(instantiation_->getFileSetReferences()->first());
    }
    
    // If no other match, pick the first file set on the mapping.
    if (!fileSet_ && fileSets_.size() > 0)
    {
        fileSet_ = fileSets_.first();
    }

    // If a file set was found, take its name as the reference.
    if (fileSet_)
    {
        fileSetRef_ = fileSet_->name();
    }
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::~ViewSelection()
//-----------------------------------------------------------------------------
ViewSelection::~ViewSelection()
{
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::validSelections()
//-----------------------------------------------------------------------------
bool ViewSelection::validSelections(QString &warning)
{
    // Must have a file set.
    if (fileSetRef_.isEmpty())
    {
        warning = QLatin1String("<b>Define the file set.</b>");
        return false;
    }

    return true;
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
QStringList ViewSelection::fileSetNames() const
{
	return QStringList(fileSets_.keys());
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::setView()
//-----------------------------------------------------------------------------
void ViewSelection::setView(QString const& viewName)
{
    view_ = views_.value(viewName);

    if (view_)
    {
        instantiation_ = instantiations_.value(view_->getComponentInstantiationRef());

        if (instantiation_ && instantiation_->getFileSetReferences()->size() > 0)
        {
            QSharedPointer<FileSet> newFileSet = fileSets_.value(instantiation_->getFileSetReferences()->first());

            if (newFileSet)
            {
                fileSet_ = newFileSet;
                fileSetRef_ = newFileSet->name();
            }
        }
    }
    else
    {
        instantiation_ = QSharedPointer<ComponentInstantiation>();
    }
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::getActiveView()
//-----------------------------------------------------------------------------
QSharedPointer<View> ViewSelection::getView() const
{
    return view_;
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::getActiveViewName()
//-----------------------------------------------------------------------------
QString ViewSelection::getViewName() const
{
    if (!view_)
    {
        return "";
    }

    return view_->name();
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
    if (!view_)
    {
        return "";
    }

	return view_->getComponentInstantiationRef();
}

//-----------------------------------------------------------------------------
// Function: ViewSelection::setFileSetRef()
//-----------------------------------------------------------------------------
bool ViewSelection::setFileSet(QString const& fileSetRef)
{
	fileSet_ = fileSets_.value(fileSetRef);
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
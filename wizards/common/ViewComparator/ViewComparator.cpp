//-----------------------------------------------------------------------------
// File: ViewComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.10.2014
//
// Description:
// Comparator for finding differences in views.
//-----------------------------------------------------------------------------

#include "ViewComparator.h"

//-----------------------------------------------------------------------------
// Function: ViewComparator::ViewComparator()
//-----------------------------------------------------------------------------
ViewComparator::ViewComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ViewComparator::~ViewComparator()
//-----------------------------------------------------------------------------
ViewComparator::~ViewComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ViewComparator::compare()
//-----------------------------------------------------------------------------
bool ViewComparator::compare(QSharedPointer<const View> first, QSharedPointer<const View> second) const
{
    return IPXactElementComparator::compare(first, second);
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::compareFields()
//-----------------------------------------------------------------------------
bool ViewComparator::compareFields(QSharedPointer<const View> first, QSharedPointer<const View> second) const
{
    return first->name() == second->name() &&
        first->isHierarchical() == second->isHierarchical() &&  
        compareLists(first->getEnvIdentifiers(), second->getEnvIdentifiers()) && 
        first->getComponentInstantiationRef() == second->getComponentInstantiationRef() &&
        first->getDesignInstantiationRef() == second->getDesignInstantiationRef() &&
        first->getDesignConfigurationInstantiationRef() == second->getDesignConfigurationInstantiationRef();
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::compare()
//-----------------------------------------------------------------------------
bool ViewComparator::compare(QList<QSharedPointer<View> > const first, 
    QList<QSharedPointer<View> > const second) const
{
    return ListComparator::compare(first, second);
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ViewComparator::diff(QSharedPointer<const View> reference,
    QSharedPointer<const View> subject) const
{
    return IPXactElementComparator::diff(reference, subject);
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::diffFields()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ViewComparator::diffFields(QSharedPointer<const View> reference, 
    QSharedPointer<const View> subject) const
{
    QSharedPointer<IPXactDiff> add(new IPXactDiff(elementType(), reference->name()));
    add->setChangeType(IPXactDiff::MODIFICATION);

	QString refIds;
	QString subIds;

	foreach (QSharedPointer<View::EnvironmentIdentifier> identifier, *reference->getEnvIdentifiers())
	{
		refIds += identifier->toString() + ", ";
	}

	foreach (QSharedPointer<View::EnvironmentIdentifier> identifier, *subject->getEnvIdentifiers())
	{
		subIds += identifier->toString() + ", ";
	}

    add->checkForChange("environment identifiers", refIds, subIds );
    add->checkForChange("component instantiation reference", reference->getComponentInstantiationRef(),
        subject->getComponentInstantiationRef());

    add->checkForChange("hierarchy", viewHierarchyType(reference), viewHierarchyType(subject));

    add->checkForChange("design instantiation reference", reference->getDesignInstantiationRef(),
        subject->getDesignInstantiationRef());
    add->checkForChange("design configuration instantiation reference",
        reference->getDesignConfigurationInstantiationRef(), subject->getDesignConfigurationInstantiationRef());    

    QList<QSharedPointer<IPXactDiff> > list;
    list.append(add);
    return list;
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ViewComparator::diff(QList<QSharedPointer<View> > const references, 
    QList<QSharedPointer<View> > const subjects) const
{
    return ListComparator::diff(references, subjects);
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::compareLists()
//-----------------------------------------------------------------------------
bool ViewComparator::compareLists(QSharedPointer<QList<QSharedPointer<View::EnvironmentIdentifier> > > first,
	QSharedPointer<QList<QSharedPointer<View::EnvironmentIdentifier> > > second) const
{
    if (first->size() != second->size())
    {
        return false;
    }

	for (int i = 0; i < first->size(); ++i)
	{
		QSharedPointer<View::EnvironmentIdentifier> item1 = first->at(i);
		QSharedPointer<View::EnvironmentIdentifier> item2 = first->at(i);

		if ( item1->language != item2->language ||
			item1->tool != item2->tool ||
			item1->vendorSpecific != item2->vendorSpecific )
		{
			return false;
		}
	}

    return true;
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::hierarchyType()
//-----------------------------------------------------------------------------
QString ViewComparator::viewHierarchyType(QSharedPointer<const View> view) const
{
    if (view->isHierarchical())
    {
        return QObject::tr("hierarchical");
    }	
    else
    {
        return QObject::tr("non-hierarchical");
    }    
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::elementType()
//-----------------------------------------------------------------------------
QString ViewComparator::elementType() const
{
    return QObject::tr("view");
}

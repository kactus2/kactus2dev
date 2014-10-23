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
// Function: ViewComparator::compareFields()
//-----------------------------------------------------------------------------
bool ViewComparator::compareFields(QSharedPointer<const View> first, QSharedPointer<const View> second) const
{
    return first->isHierarchical() == second->isHierarchical() &&  
        first->getHierarchyRef() == second->getHierarchyRef() &&
        first->getLanguage() == second->getLanguage() &&
        first->getModelName() == second->getModelName() &&
        compareLists(first->getEnvIdentifiers(), second->getEnvIdentifiers()) &&
        compareLists(first->getFileSetRefs(), second->getFileSetRefs());
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::compare()
//-----------------------------------------------------------------------------
bool ViewComparator::compare(QList<QSharedPointer<View> > first, QList<QSharedPointer<View> > second) const
{
    if (first.count() != second.count())
    {
        return false;
    }

    QMap<QString, QSharedPointer<View> > sortedViews =  mapByName(second);

    foreach (QSharedPointer<View> firstView, first)
    {
        if (!sortedViews.contains(firstView->getName()))
        {
            return false;
        }

        QSharedPointer<View> secondView = sortedViews.value(firstView->getName());
        if (!IPXactElementComparator::compare(firstView, secondView))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::diffFields()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ViewComparator::diffFields(QSharedPointer<const View> reference, 
    QSharedPointer<const View> subject) const
{
    QSharedPointer<IPXactDiff> add(new IPXactDiff("view", reference->getName()));
    add->setChangeType(IPXactDiff::MODIFICATION);

    add->checkForChange("environment identifiers", reference->getEnvIdentifiers().join(", "), 
        subject->getEnvIdentifiers().join(", "));
    add->checkForChange("language", reference->getLanguage(), subject->getLanguage());
    add->checkForChange("model name", reference->getModelName(), subject->getModelName());

    add->checkForChange("hierarchy", viewHierarchyType(reference), viewHierarchyType(subject));

    add->checkForChange("hierarchy ref", reference->getHierarchyRef().toString(), 
        subject->getHierarchyRef().toString());
    add->checkForChange("file set ref", reference->getFileSetRefs().join(", "), 
        subject->getFileSetRefs().join(", "));

    QList<QSharedPointer<IPXactDiff> > list;
    list.append(add);
    return list;
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ViewComparator::diff(QList<QSharedPointer<View> > references, 
    QList<QSharedPointer<View> > subjects) const
{
    QList<QSharedPointer<IPXactDiff> > diffResult;

    QMap<QString, QSharedPointer<View> > subjectMap = mapByName(subjects);

    foreach(QSharedPointer<const View> reference, references)
    {    
        if (subjectMap.contains(reference->getName()))
        {
            QSharedPointer<const View> subject = subjectMap.value(reference->getName());
            if (!IPXactElementComparator::compare(reference, subject))
            {
                diffResult.append(IPXactElementComparator::diff(reference, subject));
            }
        }
        else
        {
            QSharedPointer<IPXactDiff> remove(new IPXactDiff(elementType(), reference->getName()));
            remove->setChangeType(IPXactDiff::REMOVE);
            diffResult.append(remove);
        }
    }

    QMap<QString, QSharedPointer<View> > referenceMap = mapByName(references);

    foreach(QSharedPointer<const View> reference, subjects)
    {    
        if (!referenceMap.contains(reference->getName()))
        {
            QSharedPointer<IPXactDiff> add(new IPXactDiff(elementType(), reference->getName()));
            add->setChangeType(IPXactDiff::ADD);
            diffResult.append(add);
        }
    }

    if (diffResult.isEmpty())
    {
        QSharedPointer<IPXactDiff> noChange(new IPXactDiff(elementType()));
        noChange->setChangeType(IPXactDiff::NO_CHANGE);
        diffResult.append(noChange);
    }

    return diffResult;
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::mapByName()
//-----------------------------------------------------------------------------
QMap<QString, QSharedPointer<View> > ViewComparator::mapByName(QList<QSharedPointer<View> > list) const
{
    QMap<QString, QSharedPointer<View> > result;

    foreach (QSharedPointer<View> view, list)
    {
        result.insert(view->getName(), view);
    }

    return result;
}

//-----------------------------------------------------------------------------
// Function: ViewComparator::compareLists()
//-----------------------------------------------------------------------------
bool ViewComparator::compareLists(QStringList firstList, QStringList secondList) const
{
    if (firstList.count() != secondList.count())
    {
        return false;
    }

    foreach(QString const& item, firstList)
    {
        if (!secondList.contains(item))
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

//-----------------------------------------------------------------------------
// File: ModelParameterComparator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Comparator for finding differences in lists of model parameters.
//-----------------------------------------------------------------------------

#include "ModelParameterComparator.h"

#include <IPXACTmodels/modelparameter.h>

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::()
//-----------------------------------------------------------------------------
ModelParameterComparator::ModelParameterComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::()
//-----------------------------------------------------------------------------
ModelParameterComparator::~ModelParameterComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::compare()
//-----------------------------------------------------------------------------
bool ModelParameterComparator::compare(QSharedPointer<const ModelParameter> first, 
    QSharedPointer<const ModelParameter> second) const
{
    if (first == second)
    {
        return true;
    }
    
    if (first.isNull() || second.isNull())
    {
        return false;
    }


    if (first->getValue() != second->getValue())
    {
        return false;
    }

    if (first->getDataType() != second->getDataType())
    {
        return false;
    }

    if (first->getUsageType() != second->getUsageType())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::compare()
//-----------------------------------------------------------------------------
bool ModelParameterComparator::compare(QList<QSharedPointer<ModelParameter> > const references,
    QList<QSharedPointer<ModelParameter> > const subjects)
{
    if (references.count() != subjects.count())
    {
        return false;
    }

    QMap<QString, QSharedPointer<const ModelParameter> > sortedSubjects = mapById(subjects);

    foreach(QSharedPointer<const ModelParameter> reference, references)
    {    
        if (!hasElementForId(sortedSubjects, reference->getName()))
        {
            return false;
        }

        if (!compare(reference, elementForId(sortedSubjects, reference->getName())))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ModelParameterComparator::diff(QSharedPointer<const ModelParameter> reference, 
    QSharedPointer<const ModelParameter> subject) const
{
    QList<QSharedPointer<IPXactDiff> > diffResult;

    QSharedPointer<IPXactDiff> modification(new IPXactDiff("model parameter", reference->getName()));
    modification->setChangeType(IPXactDiff::MODIFICATION);

    modification->checkForChange("value", reference->getValue(), subject->getValue());
    modification->checkForChange("data type", reference->getDataType(), subject->getDataType());
    modification->checkForChange("usage type", reference->getUsageType(), subject->getUsageType());
    
    diffResult.append(modification);
    return diffResult;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > 
    ModelParameterComparator::diff(QList<QSharedPointer<ModelParameter> > references, 
    QList<QSharedPointer<ModelParameter> > subjects)
{
   QList<QSharedPointer<IPXactDiff> > diffResult;

   if (compare(references, subjects))
   {
       QSharedPointer<IPXactDiff> noChange(new IPXactDiff("model parameter"));
       noChange->setChangeType(IPXactDiff::NO_CHANGE);
       diffResult.append(noChange);
       return diffResult;
   }

   QMap<QString, QSharedPointer<const ModelParameter> > sortedOthers = mapById(subjects);

   foreach(QSharedPointer<const ModelParameter> reference, references)
   {    
       if (hasElementForId(sortedOthers, reference->getName()))
       {
           QSharedPointer<const ModelParameter> subject = elementForId(sortedOthers, reference->getName());

           if (!compare(reference, subject))
           {
               diffResult.append(diff(reference, subject));
           }
       }
       else
       {
           QSharedPointer<IPXactDiff> remove(new IPXactDiff("model parameter", reference->getName()));
           remove->setChangeType(IPXactDiff::REMOVE);
           diffResult.append(remove);
       }
   }

   QMap<QString, QSharedPointer<const ModelParameter> > sortedReference = mapById(references);

   foreach(QSharedPointer<const ModelParameter> other, subjects)
   {    
       if (!hasElementForId(sortedReference, other->getName()))
       {
           QSharedPointer<IPXactDiff> add(new IPXactDiff("model parameter", other->getName()));
           add->setChangeType(IPXactDiff::ADD);
           diffResult.append(add);
       }
   }

   return diffResult;
}


//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::mapById()
//-----------------------------------------------------------------------------
QMap<QString, QSharedPointer<const ModelParameter> > ModelParameterComparator::mapById(QList<QSharedPointer<ModelParameter> > const list)
{
    QMap<QString, QSharedPointer<const ModelParameter> > sortedOthers;

    foreach(QSharedPointer<ModelParameter> modelParameter, list)
    {
        sortedOthers.insert(modelParameter->getName(), modelParameter);
    }

    return sortedOthers;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::hasElementForId()
//-----------------------------------------------------------------------------
bool ModelParameterComparator::hasElementForId(QMap<QString, QSharedPointer<const ModelParameter> > mappedElements, QString const& id)
{
    return mappedElements.contains(id);
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::elementForId()
//-----------------------------------------------------------------------------
QSharedPointer<const ModelParameter> ModelParameterComparator::elementForId(QMap<QString, 
    QSharedPointer<const ModelParameter> > const& mappedElements, QString const& id)
{
    return mappedElements.value(id);
}

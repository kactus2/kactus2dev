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
bool ModelParameterComparator::compare(QSharedPointer<ModelParameter> first, 
    QSharedPointer<ModelParameter> second) const
{
    return IPXactElementComparator::compare(first, second);
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::compareFields()
//-----------------------------------------------------------------------------
bool ModelParameterComparator::compareFields(QSharedPointer<const ModelParameter> first, 
    QSharedPointer<const ModelParameter> second) const
{
    return first->getValue() == second->getValue() &&
        first->getDataType() == second->getDataType() &&
        first->getUsageType() == second->getUsageType();
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

    QMap<QString, QSharedPointer<const ModelParameter> > sortedSubjects = mapByName(subjects);

    foreach(QSharedPointer<const ModelParameter> reference, references)
    {    
        if (!sortedSubjects.contains(reference->getName()))
        {
            return false;
        }

        if (!IPXactElementComparator::compare(reference, sortedSubjects.value(reference->getName())))
        {
            return false;
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::diff()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > 
    ModelParameterComparator::diff(QList<QSharedPointer<ModelParameter> > references, 
    QList<QSharedPointer<ModelParameter> > subjects)
{
   QList<QSharedPointer<IPXactDiff> > diffResult;

   QMap<QString, QSharedPointer<const ModelParameter> > subjectMap = mapByName(subjects);

   foreach(QSharedPointer<const ModelParameter> reference, references)
   {    
       if (subjectMap.contains(reference->getName()))
       {
           QSharedPointer<const ModelParameter> subject = subjectMap.value(reference->getName());
           if (!IPXactElementComparator::compare(reference, subject))
           {
               diffResult.append(IPXactElementComparator::diff(reference, subject));
           }
       }
       else
       {
           QSharedPointer<IPXactDiff> remove(new IPXactDiff("model parameter", reference->getName()));
           remove->setChangeType(IPXactDiff::REMOVE);
           diffResult.append(remove);
       }
   }

   QMap<QString, QSharedPointer<const ModelParameter> > referenceMap = mapByName(references);

   foreach(QSharedPointer<const ModelParameter> other, subjects)
   {    
       if (!referenceMap.contains(other->getName()))
       {
           QSharedPointer<IPXactDiff> add(new IPXactDiff("model parameter", other->getName()));
           add->setChangeType(IPXactDiff::ADD);
           diffResult.append(add);
       }
   }

   if (diffResult.isEmpty())
   {
       QSharedPointer<IPXactDiff> noChange(new IPXactDiff("model parameter"));
       noChange->setChangeType(IPXactDiff::NO_CHANGE);
       diffResult.append(noChange);
   }

   return diffResult;
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::diffFields()
//-----------------------------------------------------------------------------
QList<QSharedPointer<IPXactDiff> > ModelParameterComparator::diffFields(QSharedPointer<const ModelParameter> reference, 
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
// Function: ModelParameterComparator::elementType()
//-----------------------------------------------------------------------------
QString ModelParameterComparator::elementType() const
{
    return "model parameter";
}

//-----------------------------------------------------------------------------
// Function: ModelParameterComparator::mapById()
//-----------------------------------------------------------------------------
QMap<QString, QSharedPointer<const ModelParameter> > 
    ModelParameterComparator::mapByName(QList<QSharedPointer<ModelParameter> > const list)
{
    QMap<QString, QSharedPointer<const ModelParameter> > mappedResult;

    foreach(QSharedPointer<ModelParameter> modelParameter, list)
    {
        mappedResult.insert(modelParameter->getName(), modelParameter);
    }

    return mappedResult;
}

//-----------------------------------------------------------------------------
// File: ListComparator.inl
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.10.2014
//
// Description:
// Base class for all IP-XACT comparators comparing lists of elements.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Function: ListComparator<T>::~ListComparator()
//-----------------------------------------------------------------------------
template <class T>
ListComparator<T>::~ListComparator()
{

}

//-----------------------------------------------------------------------------
// Function: ListComparator<T>::compare()
//-----------------------------------------------------------------------------
template <class T>
bool ListComparator<T>::compare(QList<QSharedPointer<T> > const first,
    QList<QSharedPointer<T> > const second) const
{
    if (first.count() != second.count())
    {
        return false;
    }

    QMap<QString, QSharedPointer<const T> > const sortedSubjects = mapByName(second);

    foreach(QSharedPointer<const T> reference, first)
    {    
        if (!sortedSubjects.contains(reference->getName()))
        {
            return false;
        }

        if (!IPXactElementComparator<T>::compare(reference, sortedSubjects.value(reference->getName())))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ListComparator<T>::diff()
//-----------------------------------------------------------------------------
template <class T>
QList<QSharedPointer<IPXactDiff> > ListComparator<T>::diff(QList<QSharedPointer<T> > const references, 
    QList<QSharedPointer<T> > const subjects) const
{
    QList<QSharedPointer<IPXactDiff> > diffResult;

    QMap<QString, QSharedPointer<const T> > subjectMap = mapByName(subjects);

    foreach(QSharedPointer<const T> reference, references)
    {    
        if (subjectMap.contains(reference->getName()))
        {
            QSharedPointer<const T> subject = subjectMap.value(reference->getName());
            if (!IPXactElementComparator<T>::compare(reference, subject))
            {
                diffResult.append(IPXactElementComparator<T>::diff(reference, subject));
            }
        }
        else
        {
            QSharedPointer<IPXactDiff> remove(new IPXactDiff(elementType(), 
                reference->getName()));
            remove->setChangeType(IPXactDiff::REMOVE);
            diffResult.append(remove);
        }
    }

    QMap<QString, QSharedPointer<const T> > referenceMap = mapByName(references);

    foreach(QSharedPointer<const T> subject, subjects)
    {    
        if (!referenceMap.contains(subject->getName()))
        {
            QSharedPointer<IPXactDiff> add(new IPXactDiff(elementType(), subject->getName()));
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
// Function: ListComparator<T>::mapByName()
//-----------------------------------------------------------------------------
template <class T> 
QMap<QString, QSharedPointer<const T> > ListComparator<T>::mapByName(QList<QSharedPointer<T> > const list) const
{
    QMap<QString, QSharedPointer<const T> > mappedResult;

    foreach(QSharedPointer<const T> element, list)
    {
        mappedResult.insert(element->getName(), element);
    }

    return mappedResult;
}

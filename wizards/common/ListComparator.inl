
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
bool ListComparator<T>::compare(QList<QSharedPointer<T> > const first, QList<QSharedPointer<T> > const second) const
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

        if (!IPXactElementComparator::compare(reference, sortedSubjects.value(reference->getName())))
        {
            return false;
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
// Function: ListComparator::diff()
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

    QMap<QString, QSharedPointer<const T> > referenceMap = mapByName(references);

    foreach(QSharedPointer<const T> other, subjects)
    {    
        if (!referenceMap.contains(other->getName()))
        {
            QSharedPointer<IPXactDiff> add(new IPXactDiff(elementType(), other->getName()));
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
// Function: TComparator::mapById()
//-----------------------------------------------------------------------------
template <class T> 
QMap<QString, QSharedPointer<const T> > ListComparator<T>::mapByName(QList<QSharedPointer<T> > const list) const
{
    QMap<QString, QSharedPointer<const T> > mappedResult;

    foreach(QSharedPointer<const T> modelParameter, list)
    {
        mappedResult.insert(modelParameter->getName(), modelParameter);
    }

    return mappedResult;
}
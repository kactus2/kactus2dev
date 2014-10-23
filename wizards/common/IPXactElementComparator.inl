//-----------------------------------------------------------------------------
// File: IPXactElementComparator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 17.10.2014
//
// Description:
// Base class for all IP-XACT element comparators.
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Function: IPXactElementComparator<T>::~IPXactElementComparator()
//-----------------------------------------------------------------------------
template <class T>
IPXactElementComparator<T>::~IPXactElementComparator()
{

}

//-----------------------------------------------------------------------------
// Function: IPXactElementComparator<T>::compare()
//-----------------------------------------------------------------------------
template <class T>
bool IPXactElementComparator<T>::compare(QSharedPointer<const T> first, QSharedPointer<const T> second) const
{
    if (first == second)
    {
        return true;
    }

    if (first.isNull() || second.isNull())
    {
        return false;
    }

    return compareFields(first, second);
}

//-----------------------------------------------------------------------------
// Function: IPXactElementComparator<T>::diff()
//-----------------------------------------------------------------------------
template <class T>
QList<QSharedPointer<IPXactDiff> > IPXactElementComparator<T>::diff(QSharedPointer<const T> reference, 
    QSharedPointer<const T> subject) const
{
    if (compare(reference, subject))
    {
        return noChangeDiff();
    } 

    if (subject.isNull())
    {
        return removeDiff();
    }    

    if (reference.isNull())
    { 
        return addDiff();
    }

    return diffFields(reference, subject);
}

//-----------------------------------------------------------------------------
// Function: IPXactElementComparator::addDiff()
//-----------------------------------------------------------------------------
template <class T>
QList<QSharedPointer<IPXactDiff> > IPXactElementComparator<T>::addDiff() const
{
    QList<QSharedPointer<IPXactDiff> > addList;

    QSharedPointer<IPXactDiff> add(new IPXactDiff(elementType()));
    add->setChangeType(IPXactDiff::ADD);    
    addList.append(add);

    return addList;
}

//-----------------------------------------------------------------------------
// Function: IPXactElementComparator::removeDiff()
//-----------------------------------------------------------------------------
template <class T>
QList<QSharedPointer<IPXactDiff> > IPXactElementComparator<T>::removeDiff() const
{
    QList<QSharedPointer<IPXactDiff> > removeList;

    QSharedPointer<IPXactDiff> removeDiff(new IPXactDiff(elementType()));
    removeDiff->setChangeType(IPXactDiff::REMOVE);    
    removeList.append(removeDiff);

    return removeList;
}

//-----------------------------------------------------------------------------
// Function: IPXactElementComparator::noChangeDiff()
//-----------------------------------------------------------------------------
template <class T>
QList<QSharedPointer<IPXactDiff> > IPXactElementComparator<T>::noChangeDiff() const
{
    QList<QSharedPointer<IPXactDiff> > noChangeList;

    QSharedPointer<IPXactDiff> unchangedDiff(new IPXactDiff(elementType()));
    unchangedDiff->setChangeType(IPXactDiff::NO_CHANGE);    
    noChangeList.append(unchangedDiff);

    return noChangeList;
}

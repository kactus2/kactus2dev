//-----------------------------------------------------------------------------
// File: ListComparator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.10.2014
//
// Description:
// Base class for all IP-XACT comparators comparing lists of elements.
//-----------------------------------------------------------------------------

#ifndef LISTCOMPARATOR_H
#define LISTCOMPARATOR_H

#include "IPXactElementComparator.h"
#include "IPXactDiff.h"

#include <QSharedPointer>
#include <QList>

//-----------------------------------------------------------------------------
//! Base class for all IP-XACT comparators comparing lists of elements.
//-----------------------------------------------------------------------------
template <class T>
class ListComparator : public IPXactElementComparator<T>
{
public:
    
    //! The destructor.
    virtual ~ListComparator();

    /*!
     *  Compares the two lists of elements.
     *
     *      @param [in] first    The first lists.
     *      @param [in] second   The second lists.
     *
     *      @return True, if the lists are similar, otherwise false.
     */
    virtual bool compare(QList<QSharedPointer<T> > const first, QList<QSharedPointer<T> > const second) const;    
    
    /*!
     *  Finds the differences between the two given lists.
     *
     *      @param [in] references   The reference lists to compare to.
     *      @param [in] subjects     The lists to compare against the reference.
     *
     *      @return Set of differences between the references and subjects.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diff(QList<QSharedPointer<T> > const references, 
        QList<QSharedPointer<T> > const subjects) const;

    /*!
     *  Returns the type for the element.
     *
     *      @return The element type.
     */
    virtual QString elementType() const = 0;

protected:

    /*!
     *  Creates a map of elements where the name of the element is the key.
     *
     *      @param [in] list   The elements to include in the map.
     *
     *      @return Map of elements.
     */
    virtual QMap<QString, QSharedPointer<const T> > mapByName(QList<QSharedPointer<T> > const list) const;

};

#include "ListComparator.inl"

#endif // LISTCOMPARATOR_H

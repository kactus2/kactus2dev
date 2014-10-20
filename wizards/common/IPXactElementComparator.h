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

#ifndef IPXACTELEMENTCOMPARATOR_H
#define IPXACTELEMENTCOMPARATOR_H

#include <QSharedPointer>

class IPXactDiff;

//-----------------------------------------------------------------------------
//! Base class for all IP-XACT element comparators.
//-----------------------------------------------------------------------------
template <class T>
class IPXactElementComparator 
{
public:

	//! The destructor.
    virtual ~IPXactElementComparator() {};

    /*!
     *  Compares the two elements.
     *
     *      @param [in] first    The first element.
     *      @param [in] second   The second element.
     *
     *      @return True, if the elements are similar, otherwise false.
     */
    virtual bool compare(QSharedPointer<const T> first, QSharedPointer<const T> second) const = 0;

    /*!
     *  Finds the differences between the two given elements.
     *
     *      @param [in] reference   The reference element to compare to.
     *      @param [in] subject     The element to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diff(QSharedPointer<const T> reference, 
        QSharedPointer<const T> subject) const = 0;
};

#endif // IPXACTELEMENTCOMPARATOR_H

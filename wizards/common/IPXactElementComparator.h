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
    virtual bool compare(QSharedPointer<const T> first, QSharedPointer<const T> second) const;

    /*!
     *  Compares the sub-elements of two elements.
     *
     *      @param [in] first    The first elements.
     *      @param [in] second   The second elements.
     *
     *      @return True, if the sub-elements are similar, otherwise false.
     */
    virtual bool compareFields(QSharedPointer<const T> first, QSharedPointer<const T> second) const = 0;

    /*!
     *  Finds the differences between the two given elements.
     *
     *      @param [in] reference   The reference element to compare to.
     *      @param [in] subject     The element to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diff(QSharedPointer<const T> reference, 
        QSharedPointer<const T> subject) const;

    /*!
     *  Finds the differences between the sub-elements of two given elements.
     *
     *      @param [in] reference   The reference element to compare to.
     *      @param [in] subject     The element to compare against the reference.
     *
     *      @return Set of differences between the sub-elements of reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diffFields(QSharedPointer<const T> first, 
        QSharedPointer<const T> second) const = 0;

    /*!
     *  Returns the type for the element.
     *
     *      @return The element type.
     */
    virtual QString elementType() const = 0;

protected:

    /*!
     *  Creates a diff result for an added element.     
     *
     *      @return Diff result for added element.
     */
    virtual QList<QSharedPointer<IPXactDiff> > addDiff() const;
    
    /*!
     *  Creates a diff result for a removed element.     
     *
     *      @return Diff result for removed element.
     */
    virtual QList<QSharedPointer<IPXactDiff> > removeDiff() const;
    
    /*!
     *  Creates a diff result for an unchanged element.     
     *
     *      @return Diff result for unchanged element.
     */
    virtual QList<QSharedPointer<IPXactDiff> > noChangeDiff() const;
};

#include "IPXactElementComparator.inl"

#endif // IPXACTELEMENTCOMPARATOR_H

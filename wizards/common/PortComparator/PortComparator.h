//-----------------------------------------------------------------------------
// File: PortComparator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 24.10.2014
//
// Description:
// Comparator for finding differences in ports.
//-----------------------------------------------------------------------------

#ifndef PORTCOMPARATOR_H
#define PORTCOMPARATOR_H

#include <wizards/common/ListComparator.h>
#include <wizards/common/IPXactDiff.h>

#include <QSharedPointer>

class Port;
//-----------------------------------------------------------------------------
//! Comparator for finding differences in ports.
//-----------------------------------------------------------------------------
class PortComparator : public ListComparator<Port>
{
public:

    //! The constructor.    
    PortComparator();

    //! The destructor.
    virtual ~PortComparator();
    
    /*!
     *  Compares the two ports.
     *
     *      @param [in] first    The first port.
     *      @param [in] second   The second port.
     *
     *      @return True, if the ports are similar, otherwise false.
     */
    virtual bool compare(QSharedPointer<const Port> first, QSharedPointer<const Port> second) const;

    /*!
     *  Compares the sub-elements of two ports.
     *
     *      @param [in] first    The first port.
     *      @param [in] second   The second port.
     *
     *      @return True, if the sub-elements are similar, otherwise false.
     */
    bool compareFields(QSharedPointer<const Port> first, QSharedPointer<const Port> second) const;

    /*!
     *  Compares the two lists of ports.
     *
     *      @param [in] first    The first list.
     *      @param [in] second   The second list.
     *
     *      @return True, if the lists are similar, otherwise false.
     */
    virtual bool compare(QList<QSharedPointer<Port> > const first, QList<QSharedPointer<Port> > const second) const;

    /*!
     *  Finds the differences between the two ports.
     *
     *      @param [in] reference   The port to compare to.
     *      @param [in] subject     The port to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diff(QSharedPointer<const Port> reference, 
        QSharedPointer<const Port> subject) const;

    /*!
     *  Finds the differences between the sub-elements of two given ports.
     *
     *      @param [in] reference   The reference port to compare to.
     *      @param [in] subject     The port to compare against the reference.
     *
     *      @return Set of differences between the sub-elements of reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diffFields(QSharedPointer<const Port> reference, 
        QSharedPointer<const Port> subject) const;

    /*!
     *  Finds the differences between the two given lists.
     *
     *      @param [in] reference   The reference lists to compare to.
     *      @param [in] subject     The lists to compare against the reference.
     *
     *      @return Set of differences between the reference and subject.
     */
    virtual QList<QSharedPointer<IPXactDiff> > diff(QList<QSharedPointer<Port> > const reference, 
        QList<QSharedPointer<Port> > const subject) const;
    
    /*!
     *  Returns the type for the element.
     *
     *      @return The element type.
     */
    virtual QString elementType() const;

};

#endif // PORTCOMPARATOR_H

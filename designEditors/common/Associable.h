//-----------------------------------------------------------------------------
// File: Associable.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.6.2014
//
// Description:
// Interface for items that can be associated with others.
//-----------------------------------------------------------------------------

#ifndef ASSOCIABLE_H
#define ASSOCIABLE_H

#include <QList>
#include <QPointF>

 class Association;
//-----------------------------------------------------------------------------
//! Associable class.
//-----------------------------------------------------------------------------
class Associable
{
public:

    //! The constructor.
    Associable();

    //! The destructor.
    virtual ~Associable();

    /*!
     *  Adds a new association to the item.
     *
     *      @param [in] association   The association to add.     
     */
    void addAssociation(Association* association);
   
    /*!
     *  Removes an association from the item.
     *
     *      @param [in] association   The association to remove.     
     */
    void removeAssociation(Association* association);

    /*!
     *  Defines the connection point for associations in scene coordinates.
     *     
     *      @param [in] otherEnd   The position of the other end connection point.
     *
     *      @return The connection point of the item.
     */
    virtual QPointF connectionPoint(QPointF const& otherEnd = QPointF()) const = 0;

private:

    //! The associations of the item.
   QList<Association*> associations_;

};

#endif // ASSOCIABLE_H
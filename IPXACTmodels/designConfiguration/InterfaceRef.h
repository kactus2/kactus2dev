//-----------------------------------------------------------------------------
// File: InterfaceRef.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.08.2015
//
// Description:
// Matches the ipxact:interfaceRef element.
//-----------------------------------------------------------------------------

#ifndef INTERFACEREF_H
#define INTERFACEREF_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QMap>

//-----------------------------------------------------------------------------
//! Matches the ipxact:interfaceRef element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InterfaceRef
{
public:

    /*!
     *  The constructor.
     */
    InterfaceRef();

    /*!
     *  Copy constructor.
     */
    InterfaceRef(const InterfaceRef& other);

    /*!
     *  The destructor.
     */
    ~InterfaceRef();

    /*!
     *  Get the isPresent value.
     *
     *      @return The value of isPresent.
     */
    QString getIsPresent() const;

    /*!
     *  Set the value for isPresent.
     *
     *      @param [in] newIsPresent    The new value for isPresent.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Get the name of the referenced component.
     *
     *      @return The name of the referenced component.
     */
    QString getComponentRef() const;

    /*!
     *  Set the referenced component.
     *
     *      @param [in] newComponentRef     The name of the referenced component.
     */
    void setComponentRef(QString const& newComponentRef);

    /*!
     *  Get the name of the referenced bus interface.
     *
     *      @return The name of the referenced bus interface.
     */
    QString getBusRef() const;

    /*!
     *  Set the referenced bus interface.
     *
     *      @param [in] newBusRef   The name of the referenced bus interface.
     */
    void setBusRef(QString const& newBusRef);

private:

    //! The name of the referenced component.
    QString componentRef_;
    
    //! The name of the referenced bus interface.
    QString busRef_;

    //! Defines the presenced of the element
    QString isPresent_;
};

#endif // INTERFACEREF_H

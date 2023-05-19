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

#include <IPXACTmodels/common/Extendable.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QMap>

//-----------------------------------------------------------------------------
//! Matches the ipxact:interfaceRef element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT InterfaceRef : public Extendable
{
public:

    /*!
     *  The constructor.
     */
    InterfaceRef();

    /*!
     *  Copy constructor.
     */
    InterfaceRef(const InterfaceRef& other) = default;

    /*!
     *  The destructor.
     */
    ~InterfaceRef() final = default;

    /*!
     *  Get the isPresent value.
     *
     *      @return The value of isPresent.
     */
    std::string getIsPresent() const;

    /*!
     *  Set the value for isPresent.
     *
     *      @param [in] newIsPresent    The new value for isPresent.
     */
    void setIsPresent(std::string const& newIsPresent);

    /*!
     *  Get the name of the referenced component.
     *
     *      @return The name of the referenced component.
     */
    std::string getComponentRef() const;

    /*!
     *  Set the referenced component.
     *
     *      @param [in] newComponentRef     The name of the referenced component.
     */
    void setComponentRef(std::string const& newComponentRef);

    /*!
     *  Get the name of the referenced bus interface.
     *
     *      @return The name of the referenced bus interface.
     */
    std::string getBusRef() const;

    /*!
     *  Set the referenced bus interface.
     *
     *      @param [in] newBusRef   The name of the referenced bus interface.
     */
    void setBusRef(std::string const& newBusRef);

private:

    //! The name of the referenced component.
    std::string componentRef_;
    
    //! The name of the referenced bus interface.
    std::string busRef_;

    //! Defines the presenced of the element
    std::string isPresent_;
};

#endif // INTERFACEREF_H

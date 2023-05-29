//-----------------------------------------------------------------------------
// File: HierInterface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.08.2015
//
// Description:
// Describes the ipxact:hierInterface element in an IP-XACT document.
//-----------------------------------------------------------------------------

#ifndef HIERINTERFACE_H
#define HIERINTERFACE_H

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>

//-----------------------------------------------------------------------------
// ipxact:hierInterface element. Works as a base class for other design interfaces.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT HierInterface : public Extendable
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] busRef  The name of the referenced bus interface.
     */
    explicit HierInterface(std::string busRef = std::string());

    /*!
     *  Copy constructor.
     */
    HierInterface(const HierInterface& other) = default;

    /*!
     *  The destructor.
     */
    virtual ~HierInterface();

    /*!
     *  Assignment operator.
     */
    HierInterface& operator=(const HierInterface& other);

    /*!
     *  Comparison operator.
     *
     *      @return True, if the bus interfaces reference the same interface.
     */
    bool operator==(const HierInterface& other) const;

    /*!
     *  The != operator.
     */
    bool operator!=(const HierInterface& other) const;

    /*!
     *  The < operator.
     */
    bool operator<(const HierInterface& other) const;

    /*!
     *  Get the name of the referenced bus interface.
     *
     *      @return The name of the referenced bus interface.
     */
    std::string getBusReference() const;

    /*!
     *  Set the bus interface reference.
     *
     *      @param [in] newBusReference     The name of the new bus interface reference.
     */
    void setBusReference(std::string const& newBusReference);

    /*!
     *  Get the presence.
     *
     *      @return The presence.
     */
    std::string getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new presence.
     */
    void setIsPresent(std::string const& newIsPresent);

    /*!
     *  Get the description.
     *
     *      @return The description.
     */
    std::string getDescription() const;

    /*!
     *  Set the description.
     *
     *      @param [in] newDescription  The new description.
     */
    void setDescription(std::string const& newDescription);

    /*!
     *  Set the route to this interface.
     *
     *      @param [in] newRoute    The new route.
     */
    void setRoute(QList<QPointF> newRoute);

    /*!
     *  Get the route to this interface.
     *
     *      @return The route to this interface.
     */
    QList<QPointF> getRoute() const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the referenced bus interface.
    std::string busRef_;

    //! The presence.
    std::string isPresent_;

    //! The description.
    std::string description_;
};
#endif // HIERINTERFACE_H

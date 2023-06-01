//-----------------------------------------------------------------------------
// File: AdHocConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.08.2015
//
// Description:
// Describes the ipxact:adHocConnection element in an IP-XACT document
//-----------------------------------------------------------------------------

#ifndef ADHOCCONNECTION_H
#define ADHOCCONNECTION_H

#include "PortReference.h"

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QPointF>
#include <QList>

//-----------------------------------------------------------------------------
// Describes the ipxact:adHocConnection element in an IP-XACT document
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AdHocConnection : public NameGroup, public Extendable
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] name            The name of the adHocConnection.
     *      @param [in] tiedValue       Fixed logic value for the connection.
     *      @param [in] route           The route of the connection.
     *      @param [in] offPage         Specifies whether the connection is off page or not.
     */
    AdHocConnection(std::string name,
        std::string tiedValue = std::string(),
        QList<QPointF> const& route = QList<QPointF>(),
        bool offPage = false);

    /*!
     *  The destructor.
     */
    ~AdHocConnection() override = default;

    /*!
     *  Copy constructor.
     */
    AdHocConnection(const AdHocConnection& other);

    /*!
     *  Assignment operator.
     */
    AdHocConnection& operator=(const AdHocConnection& other);

    /*!
     *  Gets the internal port references of the connection.
     *
     *      @return A pointer to a list of internal port references.
     */
    QSharedPointer<QList<QSharedPointer<PortReference> > > getInternalPortReferences() const;

    /*!
     *  Gets the external port references of the connection.     
     *
     *      @return A pointer to a list of external port references.
     */
    QSharedPointer<QList<QSharedPointer<PortReference> > > getExternalPortReferences() const;

    /*!
     *  Sets the route of the connection.
     *
     *      @param [in] route   The route to set.
     */
    void setRoute(QList<QPointF> const& route);

    /*!
     *  Gets the route of the connection.     
     *
     *      @return The route of the connection.
     */
    QList<QPointF> getRoute() const;

    /*!
     *  Checks if the connection is treated as an off-page connection.
     *
     *      @return True if off-page connection, otherwise false.
     */
    bool isOffPage() const;

    /*!
     *  Set/unset the connection to off-page connection.
     *
     *      @param [in] offPage   True if set to off-page, otherwise false.
     */
    void setOffPage(bool offPage);

    /*!
     *  Set a new list of internal port references.
     *
     *      @param [in] newInternals    The new list of internal port references.
     */
    void setInternalPortReferences(QSharedPointer<QList<QSharedPointer<PortReference> > > newInternals);

    /*!
     *  Set a new list of external port references.
     *
     *      @param [in] newExternals    The new list of external port references.
     */
    void setExternalPortReferences(QSharedPointer<QList<QSharedPointer<PortReference> > > newExternals);

    /*!
     *  Get the presence of the connection.
     *
     *      @return The presence value of the connection.
     */
    std::string getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new value for presence.
     */
    void setIsPresent(std::string const& newIsPresent);

    /*!
     *  Get the tied value of the connection.
     *
     *      @return The tied value.
     */
    std::string getTiedValue() const;

    /*!
     *  Set the tied value.
     *
     *      @param [in] newTiedValue    The new tied value.
     */
    void setTiedValue(std::string const& newTiedValue);

    /*!
     *  Change the component reference of contained interfaces.
     *
     *      @param [in] oldName     The old component reference.
     *      @param [in] newName     The new component reference.
     */
    void changeInterfaceComponentReferences(std::string const& oldName, std::string const& newName);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Defines the presence of the connection
    std::string isPresent_;

    //! Specifies a fixed logic value for this connection.
    std::string tiedValue_;

    //! A list of port references of the component instance.
    QSharedPointer<QList<QSharedPointer<PortReference> > > internalPortReferences_ =
        QSharedPointer<QList<QSharedPointer<PortReference> > >(new QList<QSharedPointer<PortReference> >());
        
    //! A list of port references of the encompassing component.
    QSharedPointer<QList<QSharedPointer<PortReference> > > externalPortReferences_ =
        QSharedPointer<QList<QSharedPointer<PortReference> > >(new QList<QSharedPointer<PortReference> >());
};
#endif // ADHOCCONNECTION_H

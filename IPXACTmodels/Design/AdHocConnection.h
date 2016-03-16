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
     *      @param [in] displayName     The display name.
     *      @param [in] description     The description.
     *      @param [in] tiedValue       Fixed logic value for the connection.
     *      @param [in] route           The route of the connection.
     *      @param [in] offPage         Specifies whether the connection is off page or not.
     */
    AdHocConnection(QString name,
        QString displayName = QString(),
        QString description = QString(),
        QString tiedValue = QString(),
        QList<QPointF> const& route = QList<QPointF>(),
        bool offPage = false);

    /*!
     *  The destructor.
     */
    virtual ~AdHocConnection();

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
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *      @param [in] newIsPresent    The new value for presence.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Get the tied value of the connection.
     *
     *      @return The tied value.
     */
    QString getTiedValue() const;

    /*!
     *  Set the tied value.
     *
     *      @param [in] newTiedValue    The new tied value.
     */
    void setTiedValue(QString const& newTiedValue);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Defines the presence of the connection
    QString isPresent_;

    //! Specifies a fixed logic value for this connection.
    QString tiedValue_;

    //! A list of port references of the component instance.
    QSharedPointer<QList<QSharedPointer<PortReference> > > internalPortReferences_;

    //! A list of port references of the encompassing component.
    QSharedPointer<QList<QSharedPointer<PortReference> > > externalPortReferences_;
};
#endif // ADHOCCONNECTION_H

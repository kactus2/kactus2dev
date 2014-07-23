//-----------------------------------------------------------------------------
// File: AdHocConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.6.2014
//
// Description:
// Describes the spirit:adHocConnection element in an IP-XACT document
//-----------------------------------------------------------------------------

#ifndef ADHOCCONNECTION_H
#define ADHOCCONNECTION_H

#include "PortRef.h"
#include "NameGroup.h"

#include "ipxactmodels_global.h"

#include <QPointF>
#include <QList>

//-----------------------------------------------------------------------------
// class AdHocConnection.
//
// Specifies a connection that is made on a port-by-port basis and not done 
// through the higher-level bus interface. These connections are between 
// component instance ports or between component instance ports and ports 
// of the encompassing component (in the case of a hierarchical component).
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AdHocConnection
{
public:
    /*! The constructor
     *
     * @param adHocConnectionNode a reference to a QDomNode where
     * the information should be parsed from.
     */
    AdHocConnection(QDomNode &adHocConnectionNode);

    //! Convenience constructor for initializing fields
    AdHocConnection(
        QString name,
        QString displayName = QString(),
        QString description = QString(),
        QString tiedValue = QString(),
        QList<PortRef> internalPortReference = QList<PortRef>(),
        QList<PortRef> externalPortReferences = QList<PortRef>(),
        QList<QPointF> const& route = QList<QPointF>(),
        bool offPage = false);

    //! The destructor.
    virtual ~AdHocConnection();

    //! Copy constructor
    AdHocConnection(const AdHocConnection& other);

    //! Assignment operator
    AdHocConnection& operator=(const AdHocConnection& other);

    /*!
     *  Gets the name of the connection.     
     *
     *      @return The name of the connection.
     */
    QString name() const;

    /*!
     *  Gets the display name of the connection.     
     *
     *      @return The display name of the connection.
     */
    QString displayName() const;

    /*!
     *  Gets the description of the connection.     
     *
     *      @return The description of the connection.
     */
    QString description() const;

    /*!
     *  Gets the internal port references of the connection.     
     *
     *      @return The internal port references of the connection.
     */
    QList<PortRef> internalPortReferences() const;

    /*!
     *  Gets the external port references of the connection.     
     *
     *      @return The external port references of the connection.
     */
    QList<PortRef> externalPortReferences() const;

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

    /*! Check if the ad hoc connection is in a valid state.
     * 
     * @param instanceNames List containing the component instance names from
     * the containing design.
     * @param errorList The list to add the possible error messages to.
     * @param parentIdentifier String from parent to help to identify the location of the error.
     *
     * \return bool True if the state is valid and writing is possible.
     */
    bool isValid(const QStringList& instanceNames,
        QStringList& errorList, 
        const QString& parentIdentifier) const;

    /*! Check if the ad hoc connection is in a valid state.
     * 
     * @param instanceNames List containing the component instance names from
     * the containing design.
     * 
     * \return bool True if the state is valid and writing is possible.
     */
    bool isValid(const QStringList& instanceNames) const;

    /*!
     *  Writes the element to XML.
     *
     *      @param [in] writer   The used XML writer.
     */
    void write(QXmlStreamWriter& writer);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    /*! MANDATORY spirit:nameGroupPort
     *
     * Container for the following values:
     *  MANDATORY spirit:name
     *  OPTIONAL spirit:displayName
     *  OPTIONAL spirit:description
     */
    NameGroup nameGroup_;

    /*! OPTIONAL spirit:tiedValue
     *
     * The logic value of this ad-hoc connection
     */
    QString tiedValue_;

    /*! MANDATORY spirit:internalPortReference
     *
     * List of PortRefs. Each references the port of a component instance.
     */
    QList<PortRef> internalPortReferences_;

    /*! OPTIONAL spirit:externalPortReference
     *
     * List of PortRefs. Each References a port of the encompassing component where this design is referred 
     * (for hierarchical ad-hoc connections).
     */
    QList<PortRef> externalPortReferences_;

    /*! VENDOREXT kactus2:route
     *
     *  The diagram route for the interconnection.
     */
    QList<QPointF> route_;

    /*! VENDOREXT kactus2:offPage
     *
     *  If true, the route is treated as an off-page connection.
     */
    bool offPage_;

};
#endif // ADHOCCONNECTION_H

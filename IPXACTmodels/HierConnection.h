//-----------------------------------------------------------------------------
// File: HierConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 6.6.2014
//
// Description:
// Describes the spirit:hierConnection element in an IP-XACT document.
//-----------------------------------------------------------------------------

#ifndef HIERCONNECTION_H
#define HIERCONNECTION_H

#include "Interface.h"
#include "VendorExtension.h"

#include <common/Global.h>

#include <QList>
#include <QPointF>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVector2D>

//-----------------------------------------------------------------------------
// class HierConnection.
//
// HierConnection represents a hierarchical interface connection
// between a bus interface on the encompassing component and a bus
// interface on a component instance of the design.
//-----------------------------------------------------------------------------
class KACTUS2_API HierConnection
{
public:

    /*! The constructor
    *
    *       @param [in] hierConnectionNode A DOM node where the information should be parsed from.
    */
    HierConnection(QDomNode &hierConnectionNode);

    //! Convenience constructor for initializing fields    
    HierConnection(QString interfaceRef_ = QString(""),
        Interface interface_ = Interface(),
        QPointF const& position = QPointF(),
        QVector2D const& direction = QVector2D(),
        QList<QPointF> const& route = QList<QPointF>(),
        bool offPage = false);

    //! Copy constructor
    HierConnection(const HierConnection& other);

    //! The destructor.
    ~HierConnection();

    //! Assignment operator
    HierConnection& operator=(const HierConnection& other);

    /*!
     *  Gets the name of the bus interface on the upper level (encompassing) component.
     *
     *      @return The bus interface name.
     */
    QString getInterfaceRef() const;

    /*!
     *  Gets the interface representing the connected bus interface on a component instance.
     *
     *      @return The connected interface.
     */
    Interface getInterface() const;

    /*!
     *  Gets the position of the connection in the diagram.
     *
     *      @return The position of the connection.
     */
    QPointF getPosition() const;

    /*!
     *  Checks if the connection is treated as an off-page connection.
     *
     *      @return True if off-page connection, otherwise false.
     */
    bool isOffPage() const;

    /*!
     *  Gets the route of the connection.     
     *
     *      @return The route of the connection.
     */
    QList<QPointF> getRoute() const;

    /*!
     *  Gets the orientation of the connection.
     *
     *      @return The orientation of the connection.
     */
    QVector2D getDirection() const;

    /*!
     *  Gets the vendor extensions of the connection.
     *     
     *      @return The vendor extensions.
     */
    QList<QSharedPointer<VendorExtension> > getVendorExtensions() const;

    /*!
     *  Sets the vendor extensions of the connection.
     *     
     *      @param [in] The vendor extensions to set.
     */
    void setVendorExtensions(QList<QSharedPointer<VendorExtension> > extensions);

    /*! Check if the hierarchical connection is in a valid state.
    * 
    *       @param [in] instanceNames     List of component instance names contained in the design.
    *       @param [in] errorList         The list to add the possible error messages to.
    *       @param [in] parentIdentifier  String from parent to help to identify the location of the error.
    *
    *       @return True if the state is valid and writing is possible.
    */
    bool isValid(QStringList const& instanceNames,  QStringList& errorList, 
        QString const& parentIdentifier) const;

    /*! Check if the hierarchical connection is in a valid state.
    * 
    *       @param instanceNames List of component instance names contained in the design.
    * 
    *       @return True if the state is valid and writing is possible.
    */
    bool isValid(QStringList const& instanceNames) const;

    /*!
    *  Writes the element to XML.
    *
    *      @param [in] writer   The used XML writer.
    */
    void write(QXmlStreamWriter& writer);

private:

    /*!
     *  Parses the vendor extensions to member variables.
     *
     *      @param [in] extensionsNode   The DOM node containing the vendor extensions.
     */
    void parseVendorExtensions(QDomNode & extensionsNode);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    /*! MANDATORY spirit:interfaceRef
    *
    * Attribute that provides one end of the interconnection; it
    * is the name of the bus interface on the encompassing
    * component
    */
    QString interfaceRef_;

    /*!Specifies the component instance bus interface for
    * connection to the encompassing component.
    */
    Interface interface_;

    /*! VENDOREXT kactus2:position
    *
    *  Specifies the position of the interface in the block diagram.
    */
    QPointF position_;

    /*! VENDOREXT kactus2:direction
    *
    *  Specifies the orientation of the interface in the block diagram.
    */
    QVector2D direction_;

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

    /*!
    * OPTIONAL (spirit: vendorExtensions)
    * HierConnection vendor extensions.
    */
    QList<QSharedPointer<VendorExtension> > vendorExtensions_;

};
#endif // HIERCONNECTION_H

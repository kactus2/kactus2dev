//-----------------------------------------------------------------------------
// File: Interconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.6.2014
//
// Description:
// Describes the spirit:interconnection element in an IP-XACT document.
//-----------------------------------------------------------------------------

#ifndef INTERCONNECTION_H
#define INTERCONNECTION_H

#include "Interface.h"

#include "generaldeclarations.h"

#include <common/Global.h>

#include <QDomNode>
#include <QString>
#include <QStringList>
#include <QPair>

//-----------------------------------------------------------------------------
// class Interconnection.
//
// Specifies a connection between one bus interface of a component
// and another bus interface of a component.
//-----------------------------------------------------------------------------
class KACTUS2_API Interconnection
{
public:
    /*! The constructor
    *
    *       @param [in] interconnectionNode     A DOM node where the information should be parsed from.
    */
    Interconnection(QDomNode& interconnectionNode);

    //! Convenience constructor for initializing fields.    
    Interconnection(QString name,
        Interface interface1,
        Interface interface2,
        QList<QPointF> const& route = QList<QPointF>(),
        bool offPage = false,
        QString displayName = QString(),
        QString description = QString());

    //! Copy constructor
    Interconnection(const Interconnection& other);

    //! The destructor.   
    virtual ~Interconnection();

    //! Assignment operator
    Interconnection& operator=(const Interconnection& other);

    /*!
     *  Returns the name of the interconnection.
     *
     *      @return The name of the interconnection.
     */
    QString name() const;

    /*!
     *  Returns the display name of the interconnection.
     *
     *      @return The display name of the interconnection.
     */
    QString displayName() const;

    /*!
     *  Returns the description of the interconnection.
     *
     *      @return The description of the interconnection.
     */
    QString description() const;

    /*!
     *  Checks if the interconnection is treated as an off-page connection.
     *
     *      @return True if off-page connection, otherwise false.
     */
    bool isOffPage() const;

    /*!
     *  Set/unset the connection to off-page connection.
     *
     *      @param [in] offPage   True if set to off-page, otherwise false.
     */
    void setOffPage(bool offpage);

    /*!
     *  Gets the route of the connection.     
     *
     *      @return The route of the connection.
     */
    QList<QPointF> getRoute() const;

    /*!
     *  Sets the route of the connection.
     *
     *      @param [in] route   The route to set.
     */
    void setRoute(QList<QPointF> route);

    /*!
     *  Gets the interfaces at the ends of the interconnection.
     *
     *      @return The active interfaces of the interconnection.
     */
    QPair<Interface, Interface> getInterfaces() const;

    /*! Check if the interconnection is in a valid state.
    * 
    *       @param [in] instanceNames       List of component instance names contained in the design.
    *       @param [in] errorList           The list to add the possible error messages to.
    *       @param [in] parentIdentifier    String from parent to help to identify the location of the error.
    *
    *       @return True if the state is valid and writing is possible.
    */
    bool isValid(QStringList const& instanceNames, QStringList& errorList, QString const& parentIdentifier) const;

    /*! Check if the interconnection is in a valid state.
    * 
    *       @param [in] instanceNames   List of component instance names contained in the design.
    * 
    *       @return True if the state is valid and writing is possible.
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

    /*! MANDATORY spirit:nameGroup
     *
     * Container for the following values:
     *  MANDATORY spirit:name
     *  OPTIONAL spirit:displayName
     *  OPTIONAL spirit:description
     */
    General::NameGroup nameGroup_;

    //! First of the two interfaces that form this interconnection.
    Interface interface1;

    //! Second of the two interfaces that form this interconnection.
    
    Interface interface2;

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
#endif // INTERCONNECTION_H

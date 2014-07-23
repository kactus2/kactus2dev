//-----------------------------------------------------------------------------
// File: PortRef.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.6.2014
//
// Description:
// Describes a port reference in an ad-hoc connection.
//-----------------------------------------------------------------------------

#ifndef PORTREF_H
#define PORTREF_H

#include "ipxactmodels_global.h"

#include <QString>
#include <QStringList>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// class PortRef.
//
// PortRef a reference to a internal or external port. 
// Internal port (spirit:internalPortReference) is a port on a component
// contained within the design and has a non-null componentRef. 
//
// External port (spirit:externalPortReference) is a port on the component containing 
// this design and has no componentRef.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PortRef
{

public:
    /*! \brief The constructor
    *
    * \param portReferenceNode A reference to a QDomNode where
    * the information should be parsed from.
    */
    PortRef(QDomNode& portReferenceNode);

    /*! \brief Convenience constructor for initializing fields
    */
    PortRef(QString portRef, QString componentRef = QString(), int left = -1, int right = -1);

    //! \brief Copy constructor
    PortRef(const PortRef& other);

    /*!
    *  The destructor.
    */
    virtual ~PortRef();

    //! \brief Assignment operator
    PortRef& operator=(const PortRef& other);

    /*!
     *  Gets the name of the referenced port.     
     *
     *      @return The referenced port.
     */
    QString getPortRef() const;

    /*!
     *  Gets the name of the referenced component instance.     
     *
     *      @return The name of the referenced component instance.
     *
     *      @remark Only for internal port references.
     */
    QString getComponentRef() const;

    /*!
     *  Gets the left index of the vector.     
     *
     *      @return The left index.
     */
    int getLeft() const;

    /*!
     *  Gets the right index of the vector.     
     *
     *      @return The right index.
     */
    int getRight() const;

    /*! \brief Check if the port ref is in a valid state.
    * 
    * \param externalRef Must be true for external refs so component reference 
    * is not checked.
    * \param instanceNames List containing the component instance names from
    * the containing design.
    * \param errorList The list to add the possible error messages to.
    * \param parentIdentifier String from parent to help to identify the location of the error.
    *
    * \return bool True if the state is valid and writing is possible.
    */
    bool isValid(bool externalRef, const QStringList& instanceNames,
        QStringList& errorList, const QString& parentIdentifier) const;

    /*! \brief Check if the port ref is in a valid state.
    * 
    * \param externalRef Must be true for external refs so component reference 
    * is not checked.
    * \param instanceNames List containing the component instance names from
    * the containing design.
    * 
    * \return bool True if the state is valid and writing is possible.
    */
    bool isValid(bool externalRef, const QStringList& instanceNames) const;

    /*!
     *  Writes the port reference to XML.     
     *
     *      @param [in]     The XML writer to use for writing.
     */
    void write(QXmlStreamWriter& writer) const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    /*! \brief MANDATORY spirit:portRef
    *
    * Name of the port referenced.
    */
    QString portRef_;

    /*! \brief MANDATORY spirit:componentRef
    * 
    * A reference to the instanceName of a component instance in this design if this 
    * is a reference to an internal port. Otherwise this field is set to a null string.
    */
    QString componentRef_;

    /*! \brief OPTIONAL spirit:left
    *
    * Left index of a vector
    */
    int left_;

    /*! \brief OPTIONAL spirit:right
    *
    * Right index of a vector
    */
    int right_;
};
#endif // PORTREF_H

//-----------------------------------------------------------------------------
// File: Interface.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.6.2014
//
// Description:
// Describes the spirit:interface type in an IP-XACT document
//-----------------------------------------------------------------------------

#ifndef INTERFACE_H
#define INTERFACE_H

#include <common/Global.h>

#include <QDomNode>
#include <QString>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// class Interface.
//
// Specifies the bus interface of a design component instance that
// is part of an interconnection.
//-----------------------------------------------------------------------------
class KACTUS2_API Interface
{

public:
    /*! The constructor
     *
     *       @param [in]   interfaceNode   A QDomNode where the information should be parsed from.
     */
    Interface(QDomNode &interfaceNode);

    /*!
     *  Convenience constructor for initializing fields.
     *
     *      @param [in] componentRef    The name of the referenced instance.
     *      @param [in] busRef          The name of the referenced bus interface.
     */
    Interface(QString componentRef = QString(""), QString busRef = QString(""));

    //! Copy constructor
    Interface(const Interface& other);

    //!  The destructor.
    virtual ~Interface();

    //! Assignment operator
    Interface& operator=(const Interface& other);

    /*!
     *  Comparison operator.
     *
     *      @param [in] other   The interface being compared to.
     *
     *      @return True, if the interfaces reference the same component and interface, otherwise false.
     */
    bool operator==(const Interface& other);


    /*!
     *  The operator !=.
     *
     *      @param [in] other   The interface being compared to.
     *
     *      @return True, if the interfaces refer to different instance or at least different bus interface,
     *              otherwise false.
     */
    bool operator!=(const Interface& other);

    /*!
     *  The operator <.
     *
     *      @param [in] other   The interface being compared to.
     *
     *      @return The operator uses the < operator of QString to compare the instance name and the bus
     *              interface name.
     */
    bool operator<(const Interface& other);

    /*!
     *  Gets the name of the component instance referenced by the interface.
     *
     *      @return The name of the component instance.
     */
    QString getComponentRef() const;

    /*!
     *  Gets the name of the bus interface referenced by the interface.
     *
     *      @return The name of the bus interface.
     */
    QString getBusRef() const;

    /*!
     *  Checks if the interface referenced a given instance and a bus interface.
     *
     *      @param [in] instanceName        The name of the component instance to compare to.
     *      @param [in] busInterfaceName    The name of the bus interface to compare to.
     *
     *      @return True, if the given instance and bus interface are reference by the interface.
     */
    bool references(QString const& instanceName, QString const& busInterfaceName);

    /*! Check if the interface is in a valid state.
     * 
     *       @param [in] instanceNames       Contains the names of component instances in containing design.
     *       @param [in] errorList           The list to add the possible error messages to.
     *       @param [in] parentIdentifier    String from parent to help to identify the location of the error.
     *
     *       @return bool True if the state is valid and writing is possible.
     */
    bool isValid(QStringList const& instanceNames, QStringList& errorList, QString const& parentIdentifier) const;

    /*! Check if the interface is in a valid state.
     * 
     *       @param [in] instanceNames     Contains the names of component instances in containing design.
     * 
     *       @return bool True if the state is valid and writing is possible.
     */
    bool isValid(const QStringList& instanceNames) const;    

    /*!
     *  Writes the interface to XML.
     *
     *      @param [in] writer   The XML writer to use.
     *
     *      @remark Writes the element using the name spirit:interface.
     */
    void write(QXmlStreamWriter& writer) const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    /*! MANDATORY spirit:componentRef
     * 
     * References the instance name of a component.
     */
    QString componentRef_;

    /*! MANDATORY spirit:busRef
     *
     * References one of the component bus interfaces.
     */
    QString busRef_;

};
#endif // INTERFACE_H

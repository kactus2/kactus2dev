//-----------------------------------------------------------------------------
// File: ComConnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.4.2012
//
// Description:
// Class encapsulating communication connection data.
//-----------------------------------------------------------------------------

#ifndef COMCONNECTION_H
#define COMCONNECTION_H

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! ComInterfaceRef structure.
//-----------------------------------------------------------------------------
struct ComInterfaceRef
{
    QString componentRef;   //!< Name reference to an SW component instance.
    QString comRef;         //!< Name reference to a COM interface that is contained by the SW component instance.
};

//-----------------------------------------------------------------------------
//! Class encapsulating API connection connection data.
//-----------------------------------------------------------------------------
class ComConnection
{
public:
    /*!
     *  Default constructor.
     */
    ComConnection();

    /*!
     *  Copy constructor.
     */
    ComConnection(ComConnection const& rhs);

    /*!
     *  Constructor which reads the API connection from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    ComConnection(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ComConnection();

    /*!
     *  Writes the contents to an XML stream.
     */
    void write(QXmlStreamWriter& writer) const;

    /*!
     *  Returns true if the connection is valid.
     *
     *      @param [out] errorList     The list for possible error messages.
     *      @param [in]  instanceNames The names of the HW/SW instances in the parent design.
     *      @param [in]  parentId      The identifier for the design containing the connections.
     */
    bool isValid(QStringList& errorList, QStringList const& instanceNames,
                 QString const& parentId) const;

    /*!
     *  Sets the name of the connection.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Sets the display name of the connection.
     *
     *      @param [in] displayName The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Sets the description of the connection.
     *
     *      @param [in] description The description to set.
     */
    void setDescription(QString const& description);

    /*!
     *  Sets the first interface.
     *
     *      @param [in] ref A reference to a COM interface.
     */
    void setInterface1(ComInterfaceRef const& ref);

    /*!
     *  Sets the first interface.
     *
     *      @param [in] ref A reference to a COM interface.
     */
    void setInterface2(ComInterfaceRef const& ref);

    /*!
     *  Returns the name of the connection.
     */
    QString const& getName() const;

    /*!
     *  Returns the display name of the SW instance.
     */
    QString const& getDisplayName() const;

    /*!
     *  Returns the description of the SW instance.
     */
    QString const& getDescription() const;

    /*!
     *  Returns the first interface.
     */
    ComInterfaceRef const& getInterface1() const;

    /*!
     *  Returns the second interface.
     */
    ComInterfaceRef const& getInterface2() const;

    /*!
     *  Assignment operator.
     */
    ComConnection& operator=(ComConnection const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the connection.
    QString name_;

    //! The display name of the connection.
    QString displayName_;

    //! The description of the connection.
    QString desc_;

    //! Provider reference.
    ComInterfaceRef interface1_;

    //! Requester reference.
    ComInterfaceRef interface2_;
};

//-----------------------------------------------------------------------------

#endif // COMCONNECTION_H

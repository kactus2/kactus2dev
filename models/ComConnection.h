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
//! Class encapsulating API dependency connection data.
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
     *  Constructor which reads the API dependency from an XML node.
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
     *  Sets the name of the dependency.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Sets the display name of the dependency.
     *
     *      @param [in] displayName The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Sets the description of the dependency.
     *
     *      @param [in] description The description to set.
     */
    void setDescription(QString const& description);

    /*!
     *  Returns the name of the dependency.
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
     *  Assignment operator.
     */
    ComConnection& operator=(ComConnection const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the dependency.
    QString name_;

    //! The display name of the dependency.
    QString displayName_;

    //! The description of the dependency.
    QString desc_;

    //! Provider reference.
    ComInterfaceRef interface1_;

    //! Requester reference.
    ComInterfaceRef interface2_;
};

//-----------------------------------------------------------------------------

#endif // COMCONNECTION_H

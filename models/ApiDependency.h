//-----------------------------------------------------------------------------
// File: ApiDependency.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 11.4.2012
//
// Description:
// Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------

#ifndef APICONNECTION_H
#define APICONNECTION_H

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! ApiInterfaceRef structure.
//-----------------------------------------------------------------------------
struct ApiInterfaceRef
{
    QString componentRef;   //!< Name reference to an SW component instance.
    QString apiRef;         //!< Name reference to an API interface that is contained by the SW component instance.
};

//-----------------------------------------------------------------------------
//! Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------
class ApiDependency
{
public:
    /*!
     *  Default constructor.
     */
    ApiDependency();

    /*!
     *  Copy constructor.
     */
    ApiDependency(ApiDependency const& rhs);

    /*!
     *  Constructor which reads the API dependency from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    ApiDependency(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ApiDependency();

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
    ApiDependency& operator=(ApiDependency const& rhs);

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
    ApiInterfaceRef providerRef_;

    //! Requester reference.
    ApiInterfaceRef requesterRef_;
};

//-----------------------------------------------------------------------------

#endif // APICONNECTION_H

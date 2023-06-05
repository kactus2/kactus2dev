//-----------------------------------------------------------------------------
// File: ApiInterconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2015
//
// Description:
// Class encapsulating API connection data.
//-----------------------------------------------------------------------------

#ifndef APIINTERCONNECTION_H
#define APIINTERCONNECTION_H

#include <IPXACTmodels/Design/Interconnection.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QDomNode>

//-----------------------------------------------------------------------------
//! Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ApiInterconnection : public Interconnection, public VendorExtension
{
public:
    /*!
     *  Default constructor.
     */
    ApiInterconnection();

    /*!
     *  Constructor which sets all the values as given.
     *
     *      @param [in] name         The name to set.
     *      @param [in] displayName  The display name to set.
     *      @param [in] description  The description to set.
     *      @param [in] ref1         The first interface reference.
     *      @param [in] ref2         The second interface reference.
     *      @param [in] imported     If true, the connection is an imported one.
     */
    ApiInterconnection(QString const& name, QString const& displayName, QString const& description,
                  QSharedPointer<HierInterface> ref1, QSharedPointer<HierInterface> ref2,
                  bool imported = false);

    /*!
     *  Copy constructor.
     */
    ApiInterconnection(ApiInterconnection const& other);

    /*!
     *  QDomNode constructor.
     *
     *      @param [in] node    The node containing the api interconnection.
     */
    ApiInterconnection(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~ApiInterconnection();

    /*!
     *  Copy constructor.
     */
    virtual ApiInterconnection* clone() const;

    /*!
     *  Gets the type of the extension.
     *
     *      @return The type of the extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents to an XML stream.
     *
     *      @param [in] writer  The selected XML stream.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Set an interface. First active interface is set as the start interface.
     *
     *      @param [in] interfaceReference  The selected interface.
     */
    void setInterface(QSharedPointer<HierInterface> interfaceReference);

    /*!
     *  Sets the flag whether the connection is an imported one and should be auto-synced.
     *
     *      @param [in] imported    If true, the connection is set as imported.
     */
    void setImported(bool imported);

    /*!
     *  Get the end interface.
     *
     *      @return The end interface.
     */
    QSharedPointer<HierInterface> getEndInterface() const;

    /*!
     *  Returns true if the connection is an imported one.
     */
    bool isImported() const;

    /*!
     *  Assignment operator.
     */
    ApiInterconnection& operator=(ApiInterconnection const& rhs);

private:

    /*!
     *  Write the end interface.
     *
     *      @param [in] writer  The writer used to write.
     */
    void writeEndInterface(QXmlStreamWriter& writer) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! If true, the connection is an imported one.
    bool imported_;
};

//-----------------------------------------------------------------------------

#endif // APIDEPENDENCY_H

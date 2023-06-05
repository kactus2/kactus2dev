//-----------------------------------------------------------------------------
// File: ComInterconnection.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 28.08.2015
//
// Description:
// Class encapsulating communication connection data.
//-----------------------------------------------------------------------------

#ifndef COMINTERCONNECTION_H
#define COMINTERCONNECTION_H

#include <IPXACTmodels/Design/Interconnection.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QDomNode>

//-----------------------------------------------------------------------------
//! Class encapsulating API connection connection data.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComInterconnection : public Interconnection, public VendorExtension
{
public:
    /*!
     *  Default constructor.
     */
    ComInterconnection();

    /*!
     *  Constructor which sets all the values as given.
     *
     *      @param [in] name         The name to set.
     *      @param [in] displayName  The display name to set.
     *      @param [in] description  The description to set.
     *      @param [in] ref1         A reference to the first COM interface.
     *      @param [in] ref2         A reference to the second COM interface.
     */
    ComInterconnection(QString const& name, QString const& displayName, QString const& description,
                       QSharedPointer<HierInterface> ref1, QSharedPointer<HierInterface> ref2);

    /*!
     *  Copy constructor.
     */
    ComInterconnection(ComInterconnection const& rhs);

    /*!
     *  QDomNode constructor.
     *
     *      @param [in] connectionNode  The node containing the com connection.
     */
    ComInterconnection(QDomNode& connectionNode);

    /*!
     *  Destructor.
     */
    ~ComInterconnection();

    /*!
     *  Clone constructor.
     */
    virtual ComInterconnection* clone() const;

    /*!
     *  Gets the type of the extension.
     *
     *      @return The type of the extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents to an XML stream.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Set an interface. First active interface is set as the start interface.
     *
     *      @param [in] interfaceReference  The selected interface.
     */
    void setInterface(QSharedPointer<HierInterface> interfaceReference);

    /*!
     *  Returns the end interface.
     *
     *      @return Pointer to the end interface.
     */
    QSharedPointer<HierInterface> getEndInterface() const;

private:

    /*!
     *  Write the end interface.
     *
     *      @param [in] writer  The writer used to write the end interface.
     */
    void writeEndInterface(QXmlStreamWriter& writer) const;
};

//-----------------------------------------------------------------------------

#endif // COMINTERCONNECTION_H

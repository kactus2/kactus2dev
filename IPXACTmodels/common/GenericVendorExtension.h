//-----------------------------------------------------------------------------
// File: GenericVendorExtension.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 8.5.2014
//
// Description:
// Generic implementation for any vendor extension.
//-----------------------------------------------------------------------------

#ifndef GENERICVENDOREXTENSION_H
#define GENERICVENDOREXTENSION_H

#include "VendorExtension.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QDomNode>
#include <QList>
#include <QObject>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// class GenericVendorExtension.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT GenericVendorExtension : public VendorExtension
{

public:

    GenericVendorExtension() = default;

    /*!
	 *  The constructor.
	 *
	 *      @param [in] extensionNode   The DOM node representation of the vendor extension.
	 */
	GenericVendorExtension(QDomNode const& extensionNode);

    GenericVendorExtension(GenericVendorExtension const& other) = default;

    /*!
     *  Clones the vendor extension.
     *
     *      @return The clone copy of the vendor extension.
     */
    virtual GenericVendorExtension* clone() const;

	/*!
	 *  The destructor.
	 */
	virtual ~GenericVendorExtension() = default;

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *      @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the vendor extension to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    QString name() const;

    void setName(QString const& name);

    QString value() const;

    void setValue(QString const& value);

    QString attributeValue(QString const& attributeName) const;

    void setAttributeValue(QString const& attributeName, QString const& attributeValue);

private:
	// Disable copying.
    
    //! Disable assign.
    GenericVendorExtension& operator=(GenericVendorExtension const& rhs);

    /*!
     *  Writes a DOM node to XML using a given XML writer.
     *
     *      @param [in] node        The DOM node to write.
     *      @param [in] writer      The XML writer to use.
     */
    void writeNode(GenericVendorExtension const& node, QXmlStreamWriter& writer) const;

    /*!
     *  Writes all child noted of a DOM node to XML using a given XML writer.
     *
     *      @param [in] node        The DOM node to write.
     *      @param [in] writer      The XML writer to use.
     */
    void writeChildNodes(GenericVendorExtension const& node, QXmlStreamWriter& writer) const;

    /*!
     *  Writes all the attributes of a DOM node to XML using a given XML writer.
     *
     *      @param [in] node        The DOM node to write.
     *      @param [in] writer      The XML writer to use.
     */
    void writeAttributes(GenericVendorExtension const& node, QXmlStreamWriter& writer) const;
    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    QString name_;

    QString value_;

    QList<QPair<QString, QString> > attributes_;

    QList<GenericVendorExtension> children_;



};
#endif // GENERICVENDOREXTENSION_H


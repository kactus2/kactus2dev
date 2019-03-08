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

    /*!
     *  Default constructor.
     */
    GenericVendorExtension() = default;

    /*!
	 *  The constructor.
	 *
	 *      @param [in] extensionNode   The DOM node representation of the vendor extension.
	 */
	GenericVendorExtension(QDomNode const& extensionNode);

    /*!
     *  Copy constructor.
     */
    GenericVendorExtension(GenericVendorExtension const& other) = default;

    /*!
     *  Assignment operator.
     */
    GenericVendorExtension& operator=(GenericVendorExtension const& rhs);

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

    /*!
     *  Get extension description.
     *
     *      @return Extension description.
     */
    QString getDescription() const;

    /*!
     *  Set a new description for the extension.
     *
     *      @param [in] newDescription  The new description.
     */
    void setDescription(QString const& newDescription);

private:

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

    //! Name of the extension.
    QString name_;

    //! Value of the extension.
    QString value_;

    //! List of attribute values.
    QVector<QPair<QString, QString> > attributes_;

    //! List of child extensions.
    QVector<GenericVendorExtension> children_;
};
#endif // GENERICVENDOREXTENSION_H


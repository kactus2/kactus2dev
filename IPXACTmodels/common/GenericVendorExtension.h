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
	 *    @param [in] extensionNode   The DOM node representation of the vendor extension.
     *    @param [in] parent          Parent extension.
	 */
	GenericVendorExtension(QDomNode const& extensionNode, GenericVendorExtension* parent = nullptr);

    /*!
     *  Copy constructor.
     *
     *    @param [in] other   The copied generic vendor extension.
     */
    GenericVendorExtension(GenericVendorExtension const& other);

    /*!
     *  Assignment operator.
     *
     *    @param [in] rhs     The assigned generic vendor extension.
     */
    GenericVendorExtension& operator=(GenericVendorExtension const& rhs);

    /*!
     *  Check whether this is the selected generic vendor extension.
     *
     *    @param [in] otherItem   The selected generic vendor extension.
     *
     *    @return True, if the selected generic vendor extension is this, false otherwise.
     */
    bool isSame(const GenericVendorExtension& otherItem) const;

    /*!
     *  Clones the vendor extension.
     *
     *    @return The clone copy of the vendor extension.
     */
    virtual GenericVendorExtension* clone() const;

	/*!
	 *  The destructor.
	 */
	virtual ~GenericVendorExtension();

    /*!
     *  Returns a type identifier for the vendor extension.
     *
     *    @return A type identifier of the vendor extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the vendor extension to XML.
     *
     *    @param [in] writer   The writer used for writing the XML.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Get the name space.
     *
     *    @return The name space.
     */
    QString nameSpace() const;

    /*!
     *  Set a new name space.
     *
     *    @param [in] newNameSpace    The new name space.
     */
    void setNameSpace(QString const& newNameSpace);

    /*!
     *  Get the name.
     *
     *    @return The name.
     */
    QString name() const;

    /*!
     *  Set a new name.
     *
     *    @param [in] name    The new name.
     */
    void setName(QString const& name);

    /*!
     *  Get the value.
     *
     *    @return The value.
     */
    QString value() const;

    /*!
     *  Set a new value.
     *
     *    @param [in] value   The new value.
     */
    void setValue(QString const& value);

    /*!
     *  Get the value of the selected attribute.
     *
     *    @param [in] attributeName   Name of the selected attribute.
     *
     *    @return Value of the selected attribute.
     */
    QString attributeValue(QString const& attributeName) const;

    /*!
     *  Set a new value for the selected attribute.
     *
     *    @param [in] attributeName   Name of the selected attribute.
     *    @param [in] attributeValue  New value for the selected attribute.
     */
    void setAttributeValue(QString const& attributeName, QString const& attributeValue);

    /*!
     *  Get extension description.
     *
     *    @return Extension description.
     */
    QString getDescription() const;

    /*!
     *  Set a new description for the extension.
     *
     *    @param [in] newDescription  The new description.
     */
    void setDescription(QString const& newDescription);

    /*!
     *  Check if this extension has a description child element.
     *
     *    @return True, if a description is found, false otherwise.
     */
    bool hasDescription() const;

    /*!
     *  Get the index of the description child element..
     *
     *    @return Index of the description child element.
     */
    int getDescriptionIndex() const;

    /*!
     *  Get a list of the contained child generic extensions.
     *
     *    @return Reference to the list of child generic extensions.
     */
    QVector<GenericVendorExtension*>& getChildExtensions();

    /*!
     *  Remove the selected indexed child extensions.
     *
     *    @param [in] startIndex  The first index to be removed.
     *    @param [in] endIndex    The last index to be removed.
     */
    void removeIndexedChildExtensions(int const& startIndex, int const& endIndex);

    /*!
     *  Get the parent vendor extension.
     *
     *    @return The parent vendor extension.
     */
    GenericVendorExtension* getParent() const;

    /*!
     *  Set a new parent vendor extension.
     *
     *    @param [in] newParent   The new parent vendor extension.
     */
    void setParent(GenericVendorExtension* newParent);

    /*!
     *  Get a list of the attributes.
     *
     *    @return List of attribute type / value pairs.
     */
    QVector<QPair<QString, QString> > getAttributes() const;

    /*!
     *  Set new attributes.
     *
     *    @param [in] newAttributes   List of new attributes.
     */
    void setNewAttributes(QVector<QPair<QString, QString> > newAttributes);

private:

    /*!
     *  Writes a DOM node to XML using a given XML writer.
     *
     *    @param [in] node        The DOM node to write.
     *    @param [in] writer      The XML writer to use.
     */
    void writeNode(GenericVendorExtension const& node, QXmlStreamWriter& writer) const;

    /*!
     *  Writes all child noted of a DOM node to XML using a given XML writer.
     *
     *    @param [in] node        The DOM node to write.
     *    @param [in] writer      The XML writer to use.
     */
    void writeChildNodes(GenericVendorExtension const& node, QXmlStreamWriter& writer) const;

    /*!
     *  Writes all the attributes of a DOM node to XML using a given XML writer.
     *
     *    @param [in] node        The DOM node to write.
     *    @param [in] writer      The XML writer to use.
     */
    void writeAttributes(GenericVendorExtension const& node, QXmlStreamWriter& writer) const;
    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! Namespace for the element.
    QString nameSpace_;

    //! Name of the extension.
    QString name_;

    //! Value of the extension.
    QString value_;

    //! List of attribute values.
    QVector<QPair<QString, QString> > attributes_;

    //! List of child extensions.
    QVector<GenericVendorExtension*> children_;

    //! The parent generic vendor extension.
    GenericVendorExtension* parent_ = nullptr;
};

Q_DECLARE_METATYPE(GenericVendorExtension*)

#endif // GENERICVENDOREXTENSION_H


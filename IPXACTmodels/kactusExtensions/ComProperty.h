//-----------------------------------------------------------------------------
// File: ComProperty.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.4.2012
//
// Description:
// Property class for communication definitions.
//-----------------------------------------------------------------------------

#ifndef COMPROPERTY_H
#define COMPROPERTY_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/VendorExtension.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Property class for communication definitions.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ComProperty : public VendorExtension
{
public:
    /*!
     *  Default constructor.
     */
    ComProperty();

    /*!
     *  Copy constructor.
     */
    ComProperty(ComProperty const& rhs);

    /*!
     *  Constructor which reads the property from an XML node.
     */
    ComProperty(QDomNode const& node);

    /*!
     *  Destructor.
     */
    ~ComProperty();

    /*!
     *  Copy the com property.
     *
     *      @return The copied com property.
     */
    virtual ComProperty* clone() const;

    /*!
     *  Get the type of the com property extension.
     *
     *      @return The type of this extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the contents of the property to an XML stream.
     *
     *      @param [in] write The XML stream writer.
     */
    virtual void write(QXmlStreamWriter& writer) const;

    /*!
     *  Returns true if the property contents are valid.
     *
     *      @param [out] errorList  The list of errors found in the contents.
     *      @param [in]  parentId   The identifier of the containing COM definition.
     */
    void findErrors(QVector<QString>& errorList, QString const& parentId) const;

    /*!
     *  Returns true if the property contents are valid.
     */
    bool validate() const;

    /*!
     *  Sets the name of the property.
     */
    void setName(QString const& name);

    /*!
     *  Sets the property required/optional.
     *
     *      @param [in] required If true, the property is a required one; otherwise it is optional.
     */
    void setRequired(bool required);

    /*!
     *  Sets the property type.
     *
     *      @param [in] type The property type as a string.
     */
    void setType(QString const& type);

    /*!
     *  Sets the default value for the property.
     *
     *      @param [in] defaultValue The default value to set.
     */
    void setDefaultValue(QString const& defaultValue);

    /*!
     *  Sets the description of the property.
     *
     *      @param [in] description The description to set.
     */
    void setDescription(QString const& description);

    /*!
     *  Returns the name of the property.
     */
    QString name() const;

    /*!
     *  Returns true if the property is required.
     */
    bool isRequired() const;

    /*!
     *  Returns the property type.
     */
    QString getType() const;

    /*!
     *  Returns the default value for the property.
     */
    QString getDefaultValue() const;

    /*!
     *  Returns the description of the property.
     */
    QString getDescription() const;

    /*!
     *  Assignment operator.
     */
    ComProperty& operator=(ComProperty const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the property.
    QString name_;

    //! If true, the property is a required one.
    bool required_;

    //! The property type.
    QString type_;

    //! The default value.
    QString defaultValue_;

    //! The property description.
    QString desc_;
};

//-----------------------------------------------------------------------------

#endif // COMPROPERTY_H

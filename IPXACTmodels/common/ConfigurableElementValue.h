//-----------------------------------------------------------------------------
// File: ConfigurableElementValue.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.08.2015
//
// Description:
// Matches the ipxact:configurableElementValue.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEELEMENTVALUE_H
#define CONFIGURABLEELEMENTVALUE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QMap>
#include <QString>

//-----------------------------------------------------------------------------
//! Matches the ipxact:configurableElementValue.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ConfigurableElementValue
{
public:

    /*!
     *  The basic constructor.
     */
    ConfigurableElementValue();

    /*!
     *  The constructor.
     *
     *      @param [in] value           The value of the configurable element.
     *      @param [in] referenceId     The reference to the id of an element.
     */
    ConfigurableElementValue(QString const& value, QString const& referenceId);

    /*!
     *  The copy constructor.
     */
    ConfigurableElementValue(const ConfigurableElementValue& other);

    /*!
     *  The destructor.
     */
    ~ConfigurableElementValue();

    /*!
     *  Get the configurable value.
     *
     *      @return The configurable value.
     */
    QString getConfigurableValue() const;

    /*!
     *  Set the configurable value.
     *
     *      @param [in] newConfigurableValue    The new value for the configurable element.
     */
    void setConfigurableValue(QString const& newConfigurableValue);

    /*!
     *  Get the id of the referenced element.
     *
     *      @return The id of referenced element.
     */
    QString getReferenceId() const;

    /*!
     *  Set the id of the referenced element.
     *
     *      @param [in] newReferenceId  The id of the new reference.
     */
    void setReferenceId(QString const& newReferenceId);

    /*!
     *  Get the attributes associated with this configurable element value.
     *
     *      @return A map containing pairs of attribute names and values.
     */
    QMap<QString, QString> getAttributes() const;

    /*!
     *  Insert an attribute for the configurable element value.
     *
     *      @param [in] attributeKey    The key for the attribute.
     *      @param [in] attributeValue  The value for the attribute.
     */
    void insertAttribute(QString attributeKey, QString attributeValue);

private:

    //! The value of the element.
    QString value_;

    //! The attributes of the element.
    QMap<QString, QString> attributes_;
};

#endif // CONFIGURABLEELEMENTVALUE_H

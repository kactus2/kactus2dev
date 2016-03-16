//-----------------------------------------------------------------------------
// File: ConfigurableElementValue.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.08.2015
//
// Description:
// Matches the ipxact:configurableElementValue.
//-----------------------------------------------------------------------------

#include "ConfigurableElementValue.h"

//-----------------------------------------------------------------------------
// Function: ConfigurableElementValue::ConfigurableElementValue()
//-----------------------------------------------------------------------------
ConfigurableElementValue::ConfigurableElementValue():
value_(),
attributes_()
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementValue::ConfigurableElementValue()
//-----------------------------------------------------------------------------
ConfigurableElementValue::ConfigurableElementValue(QString const& value, QString const& referenceId):
value_(value),
attributes_()
{
    setReferenceId(referenceId);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementValue::ConfigurableElementValue()
//-----------------------------------------------------------------------------
ConfigurableElementValue::ConfigurableElementValue(const ConfigurableElementValue& other):
value_(other.value_),
attributes_(other.attributes_)
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementValue::~ConfigurableElementValue()
//-----------------------------------------------------------------------------
ConfigurableElementValue::~ConfigurableElementValue()
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementValue::getConfigurableValue()
//-----------------------------------------------------------------------------
QString ConfigurableElementValue::getConfigurableValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementValue::setConfigurableValue()
//-----------------------------------------------------------------------------
void ConfigurableElementValue::setConfigurableValue(QString const& newConfigurableValue)
{
    value_ = newConfigurableValue;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementValue::getReferenceId()
//-----------------------------------------------------------------------------
QString ConfigurableElementValue::getReferenceId() const
{
    return attributes_.value("referenceId");
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementValue::setReferenceId()
//-----------------------------------------------------------------------------
void ConfigurableElementValue::setReferenceId(QString const& newReferenceId)
{
    attributes_.insert("referenceId", newReferenceId);
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementValue::getAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> ConfigurableElementValue::getAttributes() const
{
    return attributes_;
}

//-----------------------------------------------------------------------------
// Function: ConfigurableElementValue::insertAttribute()
//-----------------------------------------------------------------------------
void ConfigurableElementValue::insertAttribute(QString attributeKey, QString attributeValue)
{
    attributes_.insert(attributeKey, attributeValue);
}
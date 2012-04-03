//-----------------------------------------------------------------------------
// File: ComProperty.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.4.2012
//
// Description:
// Property class for communication definitions.
//-----------------------------------------------------------------------------

#include "ComProperty.h"

#include "../exceptions/write_error.h"

#include "generaldeclarations.h"

//-----------------------------------------------------------------------------
// Function: ComProperty::ComProperty()
//-----------------------------------------------------------------------------
ComProperty::ComProperty() : name_(), required_(true), type_("string"), defaultValue_("")
{
}

//-----------------------------------------------------------------------------
// Function: ComProperty::ComProperty()
//-----------------------------------------------------------------------------
ComProperty::ComProperty(ComProperty const& rhs) : name_(rhs.name_),
                                                   required_(rhs.required_),
                                                   type_(rhs.type_),
                                                   defaultValue_(rhs.defaultValue_)
{
}

//-----------------------------------------------------------------------------
// Function: ComProperty::ComProperty()
//-----------------------------------------------------------------------------
ComProperty::ComProperty(QDomNode& node) : name_(), required_(true), type_("string"), defaultValue_("")
{
    name_ = node.attributes().namedItem("kactus2:name").nodeValue();
    required_ = General::str2Bool(node.attributes().namedItem("kactus2:required").nodeValue(), false);
    type_ = node.attributes().namedItem("kactus2:propertyType").nodeValue();
    defaultValue_ = node.attributes().namedItem("kactus2:defaultValue").nodeValue();
}

//-----------------------------------------------------------------------------
// Function: ComProperty::~ComProperty()
//-----------------------------------------------------------------------------
ComProperty::~ComProperty()
{
}

//-----------------------------------------------------------------------------
// Function: ComProperty::write()
//-----------------------------------------------------------------------------
void ComProperty::write(QXmlStreamWriter& writer)
{
    // Check that the name and type are valid.
    if (name_.isEmpty())
    {
        throw Write_error(QObject::tr("Mandatory name missing in kactus2:property"));
    }

    if (type_.isEmpty())
    {
        throw Write_error(QObject::tr("Mandatory type missing in kactus2:property"));
    }

    writer.writeEmptyElement("kactus2:property");
    writer.writeAttribute("kactus2:name", name_);
    writer.writeAttribute("kactus2:required", General::bool2Str(required_));
    writer.writeAttribute("kactus2:propertyType", type_);
    writer.writeAttribute("kactus2:defaultValue", defaultValue_);
}

//-----------------------------------------------------------------------------
// Function: ComProperty::isValid()
//-----------------------------------------------------------------------------
bool ComProperty::isValid() const
{
    return (!name_.isEmpty() && !type_.isEmpty());
}

//-----------------------------------------------------------------------------
// Function: ComProperty::setName()
//-----------------------------------------------------------------------------
void ComProperty::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::setRequired()
//-----------------------------------------------------------------------------
void ComProperty::setRequired(bool required)
{
    required_ = required;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::setDefaultValue()
//-----------------------------------------------------------------------------
void ComProperty::setDefaultValue(QString const& defaultValue)
{
    defaultValue_ = defaultValue;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::getName()
//-----------------------------------------------------------------------------
QString const& ComProperty::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::isRequired()
//-----------------------------------------------------------------------------
bool ComProperty::isRequired() const
{
    return required_;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::getType()
//-----------------------------------------------------------------------------
QString const& ComProperty::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::getDefaultValue()
//-----------------------------------------------------------------------------
QString const& ComProperty::getDefaultValue() const
{
    return defaultValue_;
}

//-----------------------------------------------------------------------------
// Function: ComProperty::operator=()
//-----------------------------------------------------------------------------
ComProperty& ComProperty::operator=(ComProperty const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        required_ = rhs.required_;
        type_ = rhs.type_;
        defaultValue_ = rhs.defaultValue_;
    }

    return *this;
}

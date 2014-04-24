//-----------------------------------------------------------------------------
// File: ApiFunctionParameter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.4.2012
//
// Description:
// API function parameter class for API definitions.
//-----------------------------------------------------------------------------

#include "ApiFunctionParameter.h"

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::ApiFunctionParameter()
//-----------------------------------------------------------------------------
ApiFunctionParameter::ApiFunctionParameter()
    : name_(),
      type_(),
      comTransferType_(),
      comDirection_(General::DIRECTION_INVALID),
      contentSource_(),
      dependentParamIndex_(-1),
      desc_()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::ApiFunctionParameter()
//-----------------------------------------------------------------------------
ApiFunctionParameter::ApiFunctionParameter(ApiFunctionParameter const& rhs)
    : name_(rhs.name_),
      type_(rhs.type_),
      comTransferType_(rhs.comTransferType_),
      comDirection_(rhs.comDirection_),
      contentSource_(rhs.contentSource_),
      dependentParamIndex_(rhs.dependentParamIndex_),
      desc_(rhs.desc_)
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::ApiFunctionParameter()
//-----------------------------------------------------------------------------
ApiFunctionParameter::ApiFunctionParameter(QDomNode& node)
    : name_(),
      type_(),
      comTransferType_(),
      comDirection_(General::DIRECTION_INVALID),
      contentSource_(),
      dependentParamIndex_(-1),
      desc_()
{
    Q_ASSERT(node.nodeName() == "kactus2:functionParameter");

    name_ = node.attributes().namedItem("kactus2:name").nodeValue();
    type_ = node.attributes().namedItem("kactus2:type").nodeValue();
    comTransferType_ = node.attributes().namedItem("kactus2:transferType").nodeValue();
    comDirection_ = General::str2Direction(node.attributes().namedItem("kactus2:comDirection").nodeValue(), General::DIRECTION_INVALID);
    contentSource_ = node.attributes().namedItem("kactus2:contentSource").nodeValue();
    dependentParamIndex_ = node.attributes().namedItem("kactus2:dependentParamIndex").nodeValue().toInt();
    desc_ = node.attributes().namedItem("kactus2:description").nodeValue();
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::~ApiFunctionParameter()
//-----------------------------------------------------------------------------
ApiFunctionParameter::~ApiFunctionParameter()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::write()
//-----------------------------------------------------------------------------
void ApiFunctionParameter::write(QXmlStreamWriter& writer)
{
    writer.writeEmptyElement("kactus2:functionParameter");
    writer.writeAttribute("kactus2:name", name_);
    writer.writeAttribute("kactus2:type", type_);
    writer.writeAttribute("kactus2:transferType", comTransferType_);

    if (comDirection_ != General::DIRECTION_INVALID)
    {
        writer.writeAttribute("kactus2:comDirection", General::direction2Str(comDirection_));
    }

    writer.writeAttribute("kactus2:contentSource", contentSource_);
    writer.writeAttribute("kactus2:dependentParamIndex", QString::number(dependentParamIndex_));
    writer.writeAttribute("kactus2:description", desc_);
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::isValid()
//-----------------------------------------------------------------------------
bool ApiFunctionParameter::isValid(QStringList& errorList, QString const& parentId) const
{
    bool valid = true;
    QString const thisId = QObject::tr("function parameter '%1'").arg(name_);

    if (name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for a function parameter in %1").arg(parentId));
        valid = false;
    }

    if (type_.isEmpty())
    {
        errorList.append(QObject::tr("No type specified for %1").arg(thisId));
        valid = false;
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::isValid()
//-----------------------------------------------------------------------------
bool ApiFunctionParameter::isValid() const
{
    return (!name_.isEmpty() && !type_.isEmpty());
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::setName()
//-----------------------------------------------------------------------------
void ApiFunctionParameter::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::setType()
//-----------------------------------------------------------------------------
void ApiFunctionParameter::setType(QString const& type)
{
    type_ = type;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::setComTransferType()
//-----------------------------------------------------------------------------
void ApiFunctionParameter::setComTransferType(QString const& comTransferType)
{
    comTransferType_ = comTransferType;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::setComDirection()
//-----------------------------------------------------------------------------
void ApiFunctionParameter::setComDirection(General::Direction comDirection)
{
    comDirection_ = comDirection;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::setContentSource()
//-----------------------------------------------------------------------------
void ApiFunctionParameter::setContentSource(QString const& contentSource)
{
    contentSource_ = contentSource;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::setDependentParameterIndex()
//-----------------------------------------------------------------------------
void ApiFunctionParameter::setDependentParameterIndex(int index)
{
    dependentParamIndex_ = index;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::setDescription()
//-----------------------------------------------------------------------------
void ApiFunctionParameter::setDescription(QString const& desc)
{
    desc_ = desc;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getName()
//-----------------------------------------------------------------------------
QString const& ApiFunctionParameter::getName() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getType()
//-----------------------------------------------------------------------------
QString const& ApiFunctionParameter::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getComTransferType()
//-----------------------------------------------------------------------------
QString const& ApiFunctionParameter::getComTransferType() const
{
    return comTransferType_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getComDirection()
//-----------------------------------------------------------------------------
General::Direction ApiFunctionParameter::getComDirection() const
{
    return comDirection_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getContentSource()
//-----------------------------------------------------------------------------
QString const& ApiFunctionParameter::getContentSource() const
{
    return contentSource_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getDependentParameterIndex()
//-----------------------------------------------------------------------------
int ApiFunctionParameter::getDependentParameterIndex() const
{
    return dependentParamIndex_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getDescription()
//-----------------------------------------------------------------------------
QString const& ApiFunctionParameter::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::operator=()
//-----------------------------------------------------------------------------
ApiFunctionParameter& ApiFunctionParameter::operator=(ApiFunctionParameter const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        type_ = rhs.type_;
        comTransferType_ = rhs.comTransferType_;
        comDirection_ = rhs.comDirection_;
        contentSource_ = rhs.contentSource_;
        dependentParamIndex_ = rhs.dependentParamIndex_;
        desc_ = rhs.desc_;
    }

    return *this;
}


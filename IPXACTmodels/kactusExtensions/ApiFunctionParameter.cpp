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
      comDirection_(DirectionTypes::DIRECTION_INVALID),
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
      comDirection_(DirectionTypes::DIRECTION_INVALID),
      contentSource_(),
      dependentParamIndex_(-1),
      desc_()
{
    Q_ASSERT(node.nodeName() == "kactus2:functionParameter");

    name_ = node.attributes().namedItem("name").nodeValue();
    type_ = node.attributes().namedItem("type").nodeValue();
    comTransferType_ = node.attributes().namedItem("transferType").nodeValue();
    comDirection_ = DirectionTypes::str2Direction(node.attributes().namedItem("comDirection").nodeValue(), DirectionTypes::DIRECTION_INVALID);
    contentSource_ = node.attributes().namedItem("contentSource").nodeValue();
    dependentParamIndex_ = node.attributes().namedItem("dependentParamIndex").nodeValue().toInt();
    desc_ = node.attributes().namedItem("description").nodeValue();
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
    writer.writeAttribute("name", name_);
    writer.writeAttribute("type", type_);
    writer.writeAttribute("transferType", comTransferType_);

    if (comDirection_ != DirectionTypes::DIRECTION_INVALID)
    {
        writer.writeAttribute("comDirection", DirectionTypes::direction2Str(comDirection_));
    }

    writer.writeAttribute("contentSource", contentSource_);
    writer.writeAttribute("dependentParamIndex", QString::number(dependentParamIndex_));
    writer.writeAttribute("description", desc_);
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::findErrors()
//-----------------------------------------------------------------------------
void ApiFunctionParameter::findErrors(QVector<QString>& errorList, QString const& parentId) const
{
    QString const thisId = QObject::tr("function parameter '%1'").arg(name_);

    if (name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for a function parameter in %1").arg(parentId));
    }

    if (type_.isEmpty())
    {
        errorList.append(QObject::tr("No type specified for %1").arg(thisId));
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::isValid()
//-----------------------------------------------------------------------------
bool ApiFunctionParameter::validate() const
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
void ApiFunctionParameter::setComDirection(DirectionTypes::Direction comDirection)
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
// Function: ApiFunctionParameter::name()
//-----------------------------------------------------------------------------
QString ApiFunctionParameter::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getType()
//-----------------------------------------------------------------------------
QString ApiFunctionParameter::getType() const
{
    return type_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getComTransferType()
//-----------------------------------------------------------------------------
QString ApiFunctionParameter::getComTransferType() const
{
    return comTransferType_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getComDirection()
//-----------------------------------------------------------------------------
DirectionTypes::Direction ApiFunctionParameter::getComDirection() const
{
    return comDirection_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunctionParameter::getContentSource()
//-----------------------------------------------------------------------------
QString ApiFunctionParameter::getContentSource() const
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


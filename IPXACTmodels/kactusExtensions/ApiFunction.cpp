//-----------------------------------------------------------------------------
// File: ApiFunction.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 2.4.2012
//
// Description:
// Class for managing one API function and its information.
//-----------------------------------------------------------------------------

#include "ApiFunction.h"

#include "ApiFunctionParameter.h"

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ApiFunction::ApiFunction()
//-----------------------------------------------------------------------------
ApiFunction::ApiFunction()
    : name_(),
      returnType_(QStringLiteral("void")),
      returnValueDesc_(),
      desc_(),
      params_()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::ApiFunction()
//-----------------------------------------------------------------------------
ApiFunction::ApiFunction(ApiFunction const& rhs) : name_(rhs.name_),
                                                   returnType_(rhs.returnType_),
                                                   returnValueDesc_(rhs.returnValueDesc_),
                                                   desc_(rhs.desc_),
                                                   params_()
{
    foreach (QSharedPointer<ApiFunctionParameter> param, rhs.params_)
    {
        params_.append(QSharedPointer<ApiFunctionParameter>(new ApiFunctionParameter(*param.data())));
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::ApiFunction()
//-----------------------------------------------------------------------------
ApiFunction::ApiFunction(QDomNode& node)
    : name_(),
      returnType_(QStringLiteral("void")),
      returnValueDesc_(),
      desc_(),
      params_()
{
    Q_ASSERT(node.nodeName() == QLatin1String("kactus2:function"));

    // Read basic information from attributes.
    name_ = node.attributes().namedItem(QStringLiteral("name")).nodeValue();
    returnType_ = node.attributes().namedItem(QStringLiteral("returnType")).nodeValue();
    desc_ = node.attributes().namedItem(QStringLiteral("description")).nodeValue();

    // Parse return value and parameters from the child nodes.
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode paramNode = node.childNodes().at(i);

        if (paramNode.nodeName() == QLatin1String("kactus2:returnValue"))
        {
            returnType_ = paramNode.attributes().namedItem(QStringLiteral("type")).nodeValue();
            returnValueDesc_ = paramNode.attributes().namedItem(QStringLiteral("description")).nodeValue();
        }
        if (paramNode.nodeName() == QLatin1String("kactus2:functionParameter"))
        {
            QSharedPointer<ApiFunctionParameter> param(new ApiFunctionParameter(paramNode));
            params_.append(param);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::~ApiFunction()
//-----------------------------------------------------------------------------
ApiFunction::~ApiFunction()
{
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::write()
//-----------------------------------------------------------------------------
void ApiFunction::write(QXmlStreamWriter& writer)
{
    writer.writeStartElement(QStringLiteral("kactus2:function"));
    writer.writeAttribute(QStringLiteral("name"), name_);
    writer.writeAttribute(QStringLiteral("description"), desc_);

    writer.writeEmptyElement(QStringLiteral("kactus2:returnValue"));
    writer.writeAttribute(QStringLiteral("type"), returnType_);
    writer.writeAttribute(QStringLiteral("description"), returnValueDesc_);

    foreach (QSharedPointer<ApiFunctionParameter> param, params_)
    {
        param->write(writer);
    }

    writer.writeEndElement(); // kactus2:function
}


//-----------------------------------------------------------------------------
// Function: ApiFunction::findErrors()
//-----------------------------------------------------------------------------
void ApiFunction::findErrors(QVector<QString>& errorList, QString const& parentId) const
{

    QString const thisId = QObject::tr("API function '%1'").arg(name_);

    if (name_.isEmpty())
    {
        errorList.append(QObject::tr("No name specified for an API function in %1").arg(parentId));
    } 

    if (returnType_.isEmpty())
    {
        errorList.append(QObject::tr("No return value type specified for %1").arg(thisId));
    }

    // Validate the function parameters.
    QStringList paramNames;

    foreach (QSharedPointer<ApiFunctionParameter> param, params_)
    {
        if (paramNames.contains(param->name()))
        {
            errorList.append(QObject::tr("Multiple definitions of function parameter '%1' in %2").arg(
                param->name(), thisId));
        }
        else
        {
            paramNames.append(param->name());
        }

        param->findErrors(errorList, thisId);     
    }
}


//-----------------------------------------------------------------------------
// Function: ApiFunction::validate()
//-----------------------------------------------------------------------------
bool ApiFunction::validate() const
{
    if (name_.isEmpty() || returnType_.isEmpty())
    {
        return false;
    }

    // Validate the function parameters.
    QStringList paramNames;

    foreach (QSharedPointer<ApiFunctionParameter> param, params_)
    {
        if (paramNames.contains(param->name()))
        {
            return false;
        }
        else
        {
            paramNames.append(param->name());
        }

        if (!param->validate())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::setName()
//-----------------------------------------------------------------------------
void ApiFunction::setName(QString const& name)
{
    name_ = name;
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::setReturnValueType()
//-----------------------------------------------------------------------------
void ApiFunction::setReturnValueType(QString const& type)
{
    returnType_ = type;
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::setReturnValueDescription()
//-----------------------------------------------------------------------------
void ApiFunction::setReturnValueDescription(QString const& desc)
{
    returnValueDesc_ = desc;
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::setDescription()
//-----------------------------------------------------------------------------
void ApiFunction::setDescription(QString const& desc)
{
    desc_ = desc;
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::addParam()
//-----------------------------------------------------------------------------
void ApiFunction::addParam(QSharedPointer<ApiFunctionParameter> param, int index)
{
    Q_ASSERT(!params_.contains(param));

    if (index < 0)
    {
        params_.append(param);
    }
    else
    {
        params_.insert(index, param);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::removeParam()
//-----------------------------------------------------------------------------
void ApiFunction::removeParam(int index)
{
    Q_ASSERT(index >= 0 && index < params_.count());
    params_.removeAt(index);
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::name()
//-----------------------------------------------------------------------------
QString const& ApiFunction::name() const
{
    return name_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::getReturnValueType()
//-----------------------------------------------------------------------------
QString const& ApiFunction::getReturnValueType() const
{
    return returnType_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::getReturnValueDescription()
//-----------------------------------------------------------------------------
QString const& ApiFunction::getReturnValueDescription() const
{
    return returnValueDesc_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::getDescription()
//-----------------------------------------------------------------------------
QString const& ApiFunction::getDescription() const
{
    return desc_;
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::getParam()
//-----------------------------------------------------------------------------
QSharedPointer<ApiFunctionParameter> ApiFunction::getParam(int index)
{
    Q_ASSERT(index >= 0 && index < params_.count());
    return params_.at(index);
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::getParam()
//-----------------------------------------------------------------------------
QSharedPointer<ApiFunctionParameter const> ApiFunction::getParam(int index) const
{
    Q_ASSERT(index >= 0 && index < params_.count());
    return params_.at(index);
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::getParamCount()
//-----------------------------------------------------------------------------
int ApiFunction::getParamCount() const
{
    return params_.count();
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::generateToolTipText()
//-----------------------------------------------------------------------------
void ApiFunction::generateToolTipText(int paramIndex, QString& text) const
{
    text.clear();

    text = QStringLiteral("<p style='white-space:pre'>");

    // First add the return value hint and the function name.
    text.append(returnType_);
    text.append(QLatin1Char(' '));
    text.append(name_);
    text.append(QLatin1Char('('));

    // Then add the function parameters.
    for (int i = 0; i < params_.count(); ++i)
    {
        // Highlight the parameter of the given index.
        if (i == paramIndex)
        {
            text.append(QStringLiteral("<b>"));
            text.append(params_.at(i)->getType());
            text.append(QLatin1Char(' '));
            text.append(params_.at(i)->name());
            text.append(QStringLiteral("</b>"));
        }
        else
        {
            text.append(params_.at(i)->getType());
            text.append(QLatin1Char(' '));
            text.append(params_.at(i)->name());
        }

        // Add comma only if this is not the last parameter.
        if (i < params_.count() - 1)
        {
            text.append(QStringLiteral(", "));
        }
    }

    text.append(QStringLiteral(")<br>"));
    text.append(desc_);
    
    if (paramIndex < params_.size())
    {
        text.append(QStringLiteral("<br><b>Parameter:</b> "));
        text.append(params_.at(paramIndex)->getDescription());
    }
}

//-----------------------------------------------------------------------------
// Function: ApiFunction::operator=()
//-----------------------------------------------------------------------------
ApiFunction& ApiFunction::operator=(ApiFunction const& rhs)
{
    if (&rhs != this)
    {
        name_ = rhs.name_;
        returnType_ = rhs.returnType_;
        desc_ = rhs.desc_;

        params_.clear();

        foreach (QSharedPointer<ApiFunctionParameter> param, rhs.params_)
        {
            params_.append(QSharedPointer<ApiFunctionParameter>(new ApiFunctionParameter(*param.data())));
        }
    }

    return *this;
}

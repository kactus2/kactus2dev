//-----------------------------------------------------------------------------
// File: ApiDefinition.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 3.4.2012
//
// Description:
// Class which encapsulates the handling of custom API definition object.
//-----------------------------------------------------------------------------

#include "ApiDefinition.h"

#include "ApiFunction.h"

//-----------------------------------------------------------------------------
// Function: ApiDefinition::ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::ApiDefinition(VLNV const& vlnv) : LibraryComponent(vlnv),
                                                 language_(),
                                                 dataTypes_(),
                                                 functions_()
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::ApiDefinition(ApiDefinition const& rhs) : LibraryComponent(rhs),
                                                         language_(rhs.language_),
                                                         dataTypes_(rhs.dataTypes_),
                                                         functions_()
{
    foreach (QSharedPointer<ApiFunction> func, rhs.functions_)
    {
        functions_.append(QSharedPointer<ApiFunction>(new ApiFunction(*func.data())));
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::ApiDefinition(QDomDocument& doc)
{
    LibraryComponent::vlnv_->setType(VLNV::COMDEFINITION);

    // Find the IP-Xact root element.
    QDomNodeList nodeList = doc.childNodes();

    int i = 0;

    while (!nodeList.at(i).hasChildNodes())
    {
        ++i;
    }

    QDomNode apiNode = doc.childNodes().item(i);

    // Parse child nodes.
    for (int i = 0; i < apiNode.childNodes().count(); ++i)
    {
        QDomNode childNode = apiNode.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

        if (childNode.nodeName() == "kactus2:language")
        {
            language_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:dataTypes")
        {
            parseDataTypes(childNode);
        }
        else if (childNode.nodeName() == "kactus2:functions")
        {
            parseFunctions(childNode);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::~ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::~ApiDefinition()
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::write()
//-----------------------------------------------------------------------------
void ApiDefinition::write(QFile& file)
{
    QXmlStreamWriter writer(&file);

    // Set the writer to use auto formatting for a clearer output.
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(-1);

    // Write basic information.
    LibraryComponent::write(writer);
    LibraryComponent::writeVLNV(writer);

    if (!LibraryComponent::description_.isEmpty())
    {
        writer.writeTextElement("spirit:description", description_);
    }

    // Write data types.
    writer.writeStartElement("kactus2:dataTypes");

    foreach (QString const& type, dataTypes_)
    {
        writer.writeEmptyElement("kactus2:dataType");
        writer.writeAttribute("kactus2:name", type);
    }

    writer.writeEndElement(); // kactus2:dataTypes

    // Write properties.
    writer.writeStartElement("kactus2:functions");

    foreach (QSharedPointer<ApiFunction> func, functions_)
    {
        func->write(writer);
    }

    writer.writeEndElement(); // kactus2:functions
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::isValid()
//-----------------------------------------------------------------------------
bool ApiDefinition::isValid(QStringList& errorList) const
{
    QString thisIdentifier(QObject::tr("the containing API definition"));
    bool valid = true;

    if (!vlnv_)
    {
        errorList.append(QObject::tr("No vlnv specified for the API definition."));
        valid = false;
    }
    else if (!vlnv_->isValid(errorList, thisIdentifier))
    {
        valid = false;
    }
    else
    {
        thisIdentifier = QObject::tr("API definition '%1'").arg(vlnv_->toString());
    }

    // Check for multiple definitions of same data type.
    QStringList dataTypeNames;

    foreach (QString const& dataType, dataTypes_)
    {
        if (dataTypeNames.contains(dataType))
        {
            errorList.append(QObject::tr("Data type '%1' defined multiple times"
                                         "in '%2'").arg(dataType, thisIdentifier));
            valid = false;
        }
        else
        {
            dataTypeNames.push_back(dataType);
        }
    }

    // Validate the functions.
    QStringList funcNames;

    foreach (QSharedPointer<ApiFunction> func, functions_)
    {
        if (funcNames.contains(func->getName()))
        {
            errorList.append(QObject::tr("Function with name '%1' defined multiple times"
                                         "in %2").arg(func->getName(), thisIdentifier));
            valid = false;
        }
        else
        {
            funcNames.append(func->getName());
        }

        if (!func->isValid(errorList, thisIdentifier))
        {
            valid = false;
        }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::isValid()
//-----------------------------------------------------------------------------
bool ApiDefinition::isValid() const
{
    if (!vlnv_ || !vlnv_->isValid())
    {
        return false;
    }

    // Check for multiple definitions of same data type.
    QStringList dataTypeNames;

    foreach (QString const& dataType, dataTypes_)
    {
        if (dataTypeNames.contains(dataType))
        {
            return false;
        }
        else
        {
            dataTypeNames.push_back(dataType);
        }
    }

    // Validate the functions.
    QStringList funcNames;

    foreach (QSharedPointer<ApiFunction> func, functions_)
    {
        if (funcNames.contains(func->getName()))
        {
            return false;
        }
        else
        {
            funcNames.append(func->getName());
        }

        if (!func->isValid())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::setLanguage()
//-----------------------------------------------------------------------------
void ApiDefinition::setLanguage(QString const& language)
{
    language_ = language;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::addDataType()
//-----------------------------------------------------------------------------
void ApiDefinition::addDataType(QString const& type)
{
    if (!dataTypes_.contains(type))
    {
        dataTypes_.append(type);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::removeDataType()
//-----------------------------------------------------------------------------
void ApiDefinition::removeDataType(QString const& type)
{
    dataTypes_.removeOne(type);
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::setDataTypes()
//-----------------------------------------------------------------------------
void ApiDefinition::setDataTypes(QStringList const& types)
{
    dataTypes_ = types;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::addFunction()
//-----------------------------------------------------------------------------
void ApiDefinition::addFunction(QSharedPointer<ApiFunction> func)
{
    functions_.append(func);
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::removeFunction()
//-----------------------------------------------------------------------------
void ApiDefinition::removeFunction(int index)
{
    Q_ASSERT(index >= 0 && index < getFunctionCount());
    functions_.removeAt(index);
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getLanguage()
//-----------------------------------------------------------------------------
QString const& ApiDefinition::getLanguage() const
{
    return language_;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getDataTypes()
//-----------------------------------------------------------------------------
QStringList const& ApiDefinition::getDataTypes() const
{
    return dataTypes_;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::parseDataTypes()
//-----------------------------------------------------------------------------
void ApiDefinition::parseDataTypes(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode typeNode = node.childNodes().at(i);

        if (typeNode.nodeName() == "kactus2:dataType")
        {
            QString name = typeNode.attributes().namedItem("kactus2:name").nodeValue();
            dataTypes_.append(name);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::parseFunctions()
//-----------------------------------------------------------------------------
void ApiDefinition::parseFunctions(QDomNode& node)
{
    for (int i = 0; i < node.childNodes().count(); ++i)
    {
        QDomNode propNode = node.childNodes().at(i);

        if (propNode.nodeName() == "kactus2:function")
        {
            QSharedPointer<ApiFunction> func(new ApiFunction(propNode));
            functions_.append(func);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getFunction()
//-----------------------------------------------------------------------------
QSharedPointer<ApiFunction> ApiDefinition::getFunction(int index)
{
    Q_ASSERT(index >= 0 && index < getFunctionCount());
    return functions_.at(index);
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getFunctionCount()
//-----------------------------------------------------------------------------
int ApiDefinition::getFunctionCount() const
{
    return functions_.count();
}

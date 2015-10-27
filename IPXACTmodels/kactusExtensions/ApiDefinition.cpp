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
#include "GenericVendorExtension.h"

//-----------------------------------------------------------------------------
// Function: ApiDefinition::ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::ApiDefinition(VLNV const& vlnv) : Document(vlnv),
                                                 language_(),
                                                 comDefRef_(),
                                                 dataTypes_(),
                                                 functions_()
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::ApiDefinition(ApiDefinition const& rhs) : Document(rhs),
                                                         language_(rhs.language_),
                                                         comDefRef_(rhs.comDefRef_),
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
    : Document(),
      language_(),
      comDefRef_(),
      dataTypes_(),
      functions_()
{
	QString vendor = doc.firstChildElement("ipxact:vendor").firstChild().nodeValue();
	QString library = doc.firstChildElement("ipxact:library").firstChild().nodeValue();
	QString name = doc.firstChildElement("ipxact:name").firstChild().nodeValue();
	QString version = doc.firstChildElement("ipxact:version").firstChild().nodeValue();

	VLNV itemVLNV;
	itemVLNV.setType(VLNV::APIDEFINITION);
	itemVLNV.setVendor(vendor);
	itemVLNV.setLibrary(library);
	itemVLNV.setName(name);
	itemVLNV.setVersion(version);

	QDomNodeList extensionNodes = doc.firstChildElement("ipxact:vendorExtensions").childNodes();

	int extensionCount = extensionNodes.count();
	for (int i = 0; i < extensionCount; i++)
	{
		QDomNode extensionNode = extensionNodes.at(i);

		if (!extensionNode.nodeName().startsWith("kactus2:"))
		{
			QSharedPointer<VendorExtension> extension(new GenericVendorExtension(extensionNode));
			getVendorExtensions()->append(extension);
		}
	}

    QDomNode apiNode = doc.childNodes().item(0);

    // Parse child nodes.
    for (int i = 0; i < apiNode.childNodes().count(); ++i)
    {
        QDomNode childNode = apiNode.childNodes().at(i);

        if (childNode.isComment())
        {
            continue;
        }

		if (childNode.nodeName() ==QString("ipxact:description"))
		{
			setDescription( childNode.childNodes().at(0).nodeValue() );
		}
        else if (childNode.nodeName() == "kactus2:language")
        {
            language_ = childNode.childNodes().at(0).nodeValue();
        }
        else if (childNode.nodeName() == "kactus2:comDefinitionRef")
        {
            comDefRef_ = VLNV::createVLNV(childNode, VLNV::COMDEFINITION);
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
// Function: ApiDefinition::clone()
//-----------------------------------------------------------------------------
QSharedPointer<Document> ApiDefinition::clone() const
{
    return QSharedPointer<Document>(new ApiDefinition(*this));
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::write()
//-----------------------------------------------------------------------------
void ApiDefinition::write(QXmlStreamWriter& writer)
{
    // Set the writer to use auto formatting for a clearer output.
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(-1);

	// write the element that specifies the type of the document.
	writer.writeStartElement(getVlnv().getTypestr());

	// Write basic information.
	writer.writeTextElement("ipxact:vendor", getVlnv().getVendor());
	writer.writeTextElement("ipxact:library", getVlnv().getLibrary());
	writer.writeTextElement("ipxact:name", getVlnv().getName());
	writer.writeTextElement("ipxact:version", getVlnv().getVersion());

    if (!Document::getDescription().isEmpty())
    {
        writer.writeTextElement("ipxact:description", Document::getDescription());
    }

    // Write COM definition reference.
    writer.writeEmptyElement("kactus2:comDefinitionRef");
    comDefRef_.writeAsAttributes(writer);

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

	if (getVendorExtensions()->isEmpty())
	{
		writer.writeStartElement("ipxact:vendorExtensions");
		foreach (QSharedPointer<VendorExtension> extension, *getVendorExtensions())
		{
			extension->write(writer);
		}
		writer.writeEndElement(); // ipxact:vendorExtensions
	}

    writer.writeEndElement(); // kactus2:apiDefinition
    writer.writeEndDocument();
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::isValid()
//-----------------------------------------------------------------------------
bool ApiDefinition::isValid(QStringList& errorList) const
{
    QString thisIdentifier(QObject::tr("the containing API definition"));
    bool valid = true;

    if (!getVlnv().isValid(errorList, thisIdentifier))
    {
        valid = false;
    }
    else
    {
        thisIdentifier = QObject::tr("API definition '%1'").arg(getVlnv().toString());
    }

    // Check that the COM definition reference is valid.
    if (!comDefRef_.isEmpty() && !comDefRef_.isValid(errorList, thisIdentifier))
    {
        valid = false;
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
        if (funcNames.contains(func->name()))
        {
            errorList.append(QObject::tr("Function with name '%1' defined multiple times"
                                         "in %2").arg(func->name(), thisIdentifier));
            valid = false;
        }
        else
        {
            funcNames.append(func->name());
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
    if ( !getVlnv().isValid() )
    {
        return false;
    }

    // Check that the COM definition reference is valid.
    if (!comDefRef_.isEmpty() && !comDefRef_.isValid())
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
        if (funcNames.contains(func->name()))
        {
            return false;
        }
        else
        {
            funcNames.append(func->name());
        }

        if (!func->isValid())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getDependentFiles()
//-----------------------------------------------------------------------------
QStringList ApiDefinition::getDependentFiles() const
{
    return QStringList();
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getDependentVLNVs()
//-----------------------------------------------------------------------------
QList<VLNV> ApiDefinition::getDependentVLNVs() const
{
    QList<VLNV> vlnvs;

    if (comDefRef_.isValid())
    {
        vlnvs.append(comDefRef_);
    }

    return vlnvs;
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
// Function: ApiDefinition::removeFunctions()
//-----------------------------------------------------------------------------
void ApiDefinition::removeFunctions()
{
    functions_.clear();
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
// Function: ApiDefinition::getFunction()
//-----------------------------------------------------------------------------
QSharedPointer<ApiFunction const> ApiDefinition::getFunction(int index) const
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

//-----------------------------------------------------------------------------
// Function: ApiDefinition::setComDefinitionRef()
//-----------------------------------------------------------------------------
void ApiDefinition::setComDefinitionRef(VLNV const& vlnv)
{
    comDefRef_ = vlnv;
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::getComDefinitionRef()
//-----------------------------------------------------------------------------
VLNV const& ApiDefinition::getComDefinitionRef() const
{
    return comDefRef_;
}

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

#include <IPXACTmodels/common/GenericVendorExtension.h>

//-----------------------------------------------------------------------------
// Function: ApiDefinition::ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::ApiDefinition(VLNV const& vlnv) : Document(vlnv),
                                                 language_(),
                                                 comDefRef_(),
                                                 dataTypes_(),
												 functions_(new QList<QSharedPointer<ApiFunction> >)
{

}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::ApiDefinition(ApiDefinition const& rhs) : Document(rhs),
                                                         language_(rhs.language_),
                                                         comDefRef_(rhs.comDefRef_),
                                                         dataTypes_(rhs.dataTypes_),
                                                         functions_(new QList<QSharedPointer<ApiFunction> >)
{
    foreach (QSharedPointer<ApiFunction> func, *rhs.functions_)
    {
        functions_->append(QSharedPointer<ApiFunction>(new ApiFunction(*func.data())));
    }
}
#include <QDebug>
//-----------------------------------------------------------------------------
// Function: ApiDefinition::ApiDefinition()
//-----------------------------------------------------------------------------
ApiDefinition::ApiDefinition(QDomDocument& doc)
    : Document(),
      language_(),
      comDefRef_(),
      dataTypes_(),
      functions_(new QList<QSharedPointer<ApiFunction> >)
{
	QDomElement documentElement = doc.documentElement();

	QString vendor = documentElement.firstChildElement("ipxact:vendor").firstChild().nodeValue();
	QString library = documentElement.firstChildElement("ipxact:library").firstChild().nodeValue();
	QString name = documentElement.firstChildElement("ipxact:name").firstChild().nodeValue();
	QString version = documentElement.firstChildElement("ipxact:version").firstChild().nodeValue();

	VLNV itemVLNV;
	itemVLNV.setType(VLNV::APIDEFINITION);
	itemVLNV.setVendor(vendor);
	itemVLNV.setLibrary(library);
	itemVLNV.setName(name);
	itemVLNV.setVersion(version);

	setVlnv(itemVLNV);

	QDomNodeList extensionNodes = documentElement.firstChildElement("ipxact:vendorExtensions").childNodes();

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
	
    // Parse child nodes.
    for (int i = 0; i < documentElement.childNodes().count(); ++i)
    {
        QDomNode childNode = documentElement.childNodes().at(i);

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
// Function: ApiDefinition::findErrors()
//-----------------------------------------------------------------------------
void ApiDefinition::findErrors(QVector<QString>& errorList) const
{
    QString thisIdentifier(QObject::tr("the containing API definition"));

    if (getVlnv().isValid(errorList, thisIdentifier))
    {
        thisIdentifier = QObject::tr("API definition '%1'").arg(getVlnv().toString());
    }

    // Check that the COM definition reference is valid.
    if (!comDefRef_.isEmpty())
    {
        comDefRef_.isValid(errorList, thisIdentifier);
    }

    // Check for multiple definitions of same data type.
    QStringList dataTypeNames;

    foreach (QString const& dataType, dataTypes_)
    {
        if (dataTypeNames.contains(dataType))
        {
            errorList.append(QObject::tr("Data type '%1' defined multiple times in '%2'").arg(dataType, 
                thisIdentifier));
        }
        else
        {
            dataTypeNames.push_back(dataType);
        }
    }

    // Validate the functions.
    QStringList funcNames;

    foreach (QSharedPointer<ApiFunction> func, *functions_)
    {
        if (funcNames.contains(func->name()))
        {
            errorList.append(QObject::tr("Function with name '%1' defined multiple times in %2").arg(func->name(),
                thisIdentifier));
        }
        else
        {
            funcNames.append(func->name());
        }

        func->findErrors(errorList, thisIdentifier);
    }
}

//-----------------------------------------------------------------------------
// Function: ApiDefinition::validate()
//-----------------------------------------------------------------------------
bool ApiDefinition::validate() const
{
    if (!getVlnv().isValid() )
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

    foreach (QSharedPointer<ApiFunction> func, *functions_)
    {
        if (funcNames.contains(func->name()))
        {
            return false;
        }
        else
        {
            funcNames.append(func->name());
        }

        if (!func->validate())
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
// Function: ApiDefinition::removeFunctions()
//-----------------------------------------------------------------------------
void ApiDefinition::removeFunctions()
{
    functions_->clear();
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
            functions_->append(func);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: Document::getFunctions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ApiFunction> > > ApiDefinition::getFunctions() const
{
	return functions_;
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

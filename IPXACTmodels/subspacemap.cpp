/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#include "memorymapitem.h"
#include "subspacemap.h"


#include "GenericVendorExtension.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ParameterReader.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/validators/ParameterValidator.h>

#include <QList>
#include <QSharedPointer>
#include <QDomNode>
#include <QObject>
#include <QXmlStreamWriter>
#include "XmlUtils.h"


//-----------------------------------------------------------------------------
// Function: SubspaceMap::SubspaceMap()
//-----------------------------------------------------------------------------
SubspaceMap::SubspaceMap(QDomNode &memoryMapNode): MemoryMapItem(memoryMapNode),
parameters_(),
vendorExtensions_()
{
	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i)
    {
		if (memoryMapNode.childNodes().at(i).nodeName() == QString("spirit:parameters"))
        {
            ParameterReader reader;
			// go through all parameters
			for (int j = 0; j < memoryMapNode.childNodes().at(i).childNodes().count(); ++j)
            {
				QDomNode parameterNode = memoryMapNode.childNodes().at(i).childNodes().at(j);
				parameters_.append(QSharedPointer<Parameter>(reader.createParameterFrom(parameterNode)));
			}
		}

        else if (memoryMapNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = memoryMapNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j)
            {
                QDomNode extension = memoryMapNode.childNodes().at(j);
                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extension)));
            }
        }
	}
}

//-----------------------------------------------------------------------------
// Function: SubspaceMap::~SubspaceMap()
//-----------------------------------------------------------------------------
SubspaceMap::SubspaceMap():
MemoryMapItem(),
parameters_(),
vendorExtensions_()
{

}

//-----------------------------------------------------------------------------
// Function: SubspaceMap::SubspaceMap()
//-----------------------------------------------------------------------------
SubspaceMap::SubspaceMap(SubspaceMap const& other):
MemoryMapItem(other),
parameters_(),
vendorExtensions_(other.vendorExtensions_)
{
    foreach (QSharedPointer<Parameter> param, other.parameters_)
    {
        QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*param.data()));
        parameters_.append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: SubspaceMap::operator=()
//-----------------------------------------------------------------------------
SubspaceMap& SubspaceMap::operator=(SubspaceMap const& other)
{
	if (this != &other)
    {
        MemoryMapItem::operator=(other);
        vendorExtensions_ = other.vendorExtensions_;

        parameters_.clear();
        foreach (QSharedPointer<Parameter> param, other.parameters_)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*param.data()));
            parameters_.append(copy);
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMap::~SubspaceMap()
//-----------------------------------------------------------------------------
SubspaceMap::~SubspaceMap()
{
	parameters_.clear();
}

//-----------------------------------------------------------------------------
// Function: SubspaceMap::clone()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMapItem> SubspaceMap::clone() const
{
	return QSharedPointer<MemoryMapItem>(new SubspaceMap(*this));
}

//-----------------------------------------------------------------------------
// Function: SubspaceMap::write()
//-----------------------------------------------------------------------------
void SubspaceMap::write(QXmlStreamWriter& writer)
{
	writer.writeStartElement("spirit:subspaceMap");

	MemoryMapItem::write(writer);

    if (parameters_.size() != 0)
    {
        writer.writeStartElement("ipxact:parameters");

        ParameterWriter parameterWriter;
        // write each parameter
        for (int i = 0; i < parameters_.size(); ++i)
        {
            parameterWriter.writeParameter(writer, parameters_.at(i));
        }

        writer.writeEndElement(); // ipxact:parameters
    }

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:subspaceMap
}

//-----------------------------------------------------------------------------
// Function: SubspaceMap::isValid()
//-----------------------------------------------------------------------------
bool SubspaceMap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList& errorList,
    const QString& parentIdentifier ) const
{
	bool valid = true;
    const QString thisIdentifier(QObject::tr("subspace map %1").arg(name()));

    if (name().isEmpty())
    {
		errorList.append(QObject::tr("No name specified for subspace map within %1").arg(parentIdentifier));
		valid = false;
	}

	if (!attributes_.contains(QString("spirit:masterRef")))
    {
		errorList.append(QObject::tr("No master reference set for %1 within %2").arg(thisIdentifier, 
            parentIdentifier));
		valid = false;
	}

	if (baseAddress_.isEmpty())
    {
		errorList.append(QObject::tr("No base address set for %1 within %2").arg(thisIdentifier, parentIdentifier));
		valid = false;
	}

    ParameterValidator validator;
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        errorList.append(validator.findErrorsIn(param.data(), parentIdentifier, componentChoices));
        if (!validator.validate(param.data(), componentChoices)) 
        {
            valid = false;
        }
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMap::isValid()
//-----------------------------------------------------------------------------
bool SubspaceMap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
    if (name().isEmpty())
    {
		return false;
	}

	if (!attributes_.contains(QString("spirit:masterRef")))
    {
		return false;
	}

	if (baseAddress_.isEmpty())
    {
		return false;
	}

    ParameterValidator validator;
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        if (!validator.validate(param.data(), componentChoices)) 
        {
            return false;
        }
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMap::getParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > const& SubspaceMap::getParameters()
{
	return parameters_;
}

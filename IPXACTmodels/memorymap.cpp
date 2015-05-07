/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#include "memorymap.h"

#include "GenericVendorExtension.h"

#include <QDomNode>
#include <QXmlStreamWriter>
#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: memorymap::MemoryMap()
//-----------------------------------------------------------------------------
MemoryMap::MemoryMap(QDomNode &memoryMapNode):
AbstractMemoryMap(memoryMapNode),
addressUnitBits_(8),
vendorExtensions_(),
memoryRemaps_(new QList<QSharedPointer<MemoryRemap> >())
{
	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i)
    {
		QDomNode tempNode = memoryMapNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:addressUnitBits"))
        {
			addressUnitBits_ = tempNode.childNodes().at(0).nodeValue().toInt();
		}

        else if (tempNode.nodeName() == QString("spirit:memoryRemaps"))
        {
            int remapCount = tempNode.childNodes().count();
            for (int j = 0; j < remapCount; ++j)
            {
                QDomNode remapNode = tempNode.childNodes().at(j);
                QSharedPointer<MemoryRemap> memoryRemap (new MemoryRemap(remapNode));
                memoryRemaps_->append(memoryRemap);
            }
        }

        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j)
            {
                QDomNode extensionNode = tempNode.childNodes().at(j);
                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
            }
        }
	}
}

//-----------------------------------------------------------------------------
// Function: memorymap::MemoryMap()
//-----------------------------------------------------------------------------
MemoryMap::MemoryMap():
AbstractMemoryMap(),
addressUnitBits_(8),
vendorExtensions_(),
memoryRemaps_(new QList<QSharedPointer<MemoryRemap> >())
{

}

//-----------------------------------------------------------------------------
// Function: memorymap::MemoryMap()
//-----------------------------------------------------------------------------
MemoryMap::MemoryMap( const MemoryMap &other ):
AbstractMemoryMap(other),
addressUnitBits_(other.addressUnitBits_),
vendorExtensions_(other.vendorExtensions_),
memoryRemaps_(new QList<QSharedPointer<MemoryRemap> >())
{
    foreach (QSharedPointer<MemoryRemap> memoryRemap, *other.memoryRemaps_)
    {
        if (memoryRemap)
        {
            QSharedPointer<MemoryRemap> copy = QSharedPointer<MemoryRemap>(new MemoryRemap(*memoryRemap.data()));
            memoryRemaps_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: memorymap::operator=()
//-----------------------------------------------------------------------------
MemoryMap & MemoryMap::operator=( const MemoryMap &other )
{
	if (this != &other)
    {
        AbstractMemoryMap::operator=(other);
		addressUnitBits_ = other.addressUnitBits_;
		vendorExtensions_ = other.vendorExtensions_;

        foreach (QSharedPointer<MemoryRemap> memoryRemap, *other.memoryRemaps_)
        {
            if (memoryRemap)
            {
                QSharedPointer<MemoryRemap> copy = QSharedPointer<MemoryRemap>(new MemoryRemap(*memoryRemap.data()));
                memoryRemaps_->append(copy);
            }
        }
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: memorymap::~MemoryMap()
//-----------------------------------------------------------------------------
MemoryMap::~MemoryMap()
{

}

//-----------------------------------------------------------------------------
// Function: memorymap::write()
//-----------------------------------------------------------------------------
void MemoryMap::write(QXmlStreamWriter& writer)
{
    AbstractMemoryMap::write(writer);

	if (addressUnitBits_ != 0)
    {
		writer.writeTextElement("spirit:addressUnitBits", QString::number(addressUnitBits_));
	}

    if (!memoryRemaps_->isEmpty())
    {
        writer.writeStartElement("spirit:memoryRemaps");

        foreach(QSharedPointer<MemoryRemap> memoryRemap, *memoryRemaps_)
        {
            memoryRemap->write(writer);
        }

        writer.writeEndElement(); // spirit:memoryRemaps
    }

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }
}

//-----------------------------------------------------------------------------
// Function: memorymap::isValid()
//-----------------------------------------------------------------------------
bool MemoryMap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList remapStateNames, QStringList& errorList,
    const QString& parentIdentifier) const
{
    bool valid = AbstractMemoryMap::isValid(componentChoices, errorList, parentIdentifier);

    const QString thisIdentifier(QObject::tr("memory map %1").arg(getName()));

    QStringList memoryRemapNames;
    foreach (QSharedPointer<MemoryRemap> singleMemoryRemap, *memoryRemaps_)
    {
        if (memoryRemapNames.contains(singleMemoryRemap->getName()))
        {
            errorList.append(QObject::tr("%1 contains several memory remaps with the name %2").arg(
                thisIdentifier).arg(singleMemoryRemap->getName()));
            valid = false;
        }
        else
        {
            memoryRemapNames.append(singleMemoryRemap->getName());
        }
        if (!singleMemoryRemap->isValid(componentChoices, remapStateNames, errorList, thisIdentifier))
        {
            valid = false;
        }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: memorymap::isValid()
//-----------------------------------------------------------------------------
bool MemoryMap::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList remapStateNames) const
{
    bool valid = AbstractMemoryMap::isValid(componentChoices);

    QStringList memoryRemapNames;
    foreach (QSharedPointer<MemoryRemap> singleMemoryRemap, *memoryRemaps_)
    {
        if (memoryRemapNames.contains(singleMemoryRemap->getName()))
        {
            return false;
        }
        else
        {
            memoryRemapNames.append(singleMemoryRemap->getName());
        }

        if (!singleMemoryRemap->isValid(componentChoices, remapStateNames))
        {
            return false;
        }
    }

    return valid;
}

//-----------------------------------------------------------------------------
// Function: memorymap::setAddressUnitBits()
//-----------------------------------------------------------------------------
void MemoryMap::setAddressUnitBits(unsigned int addressUnitBits)
{
	addressUnitBits_ = addressUnitBits;
}

//-----------------------------------------------------------------------------
// Function: memorymap::getAddressUnitBits()
//-----------------------------------------------------------------------------
unsigned int MemoryMap::getAddressUnitBits() const
{
	return addressUnitBits_;
}

//-----------------------------------------------------------------------------
// Function: memorymap::getMemoryRemaps()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<MemoryRemap> > > MemoryMap::getMemoryRemaps()
{
    return memoryRemaps_;
}
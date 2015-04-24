/* 
 *
 *  Created on: 29.7.2010
 *      Author: Antti Kamppi
 */

#include "bank.h"
#include "memorymapitem.h"
#include "subspacemap.h"
#include "addressblock.h"
#include "generaldeclarations.h"
#include "parameter.h"
#include "memoryblockdata.h"
#include "GenericVendorExtension.h"

#include <QString>
#include <QDomNode>
#include <QList>
#include <QObject>
#include <QXmlStreamWriter>
#include <QSharedPointer>
#include "XmlUtils.h"

Bank::Bank(QDomNode &memoryMapNode): 
MemoryMapItem(memoryMapNode),
items_(),
memoryBlockData_(0),
vendorExtensions_()
{

	// the temporary variables to store the parsed values for memoryBlockData_
	General::Usage usage = General::USAGE_COUNT;
	General::BooleanValue volatileValue = General::BOOL_UNSPECIFIED;
	General::Access access = General::ACCESS_COUNT;
	QList<QSharedPointer<Parameter> > tempParameters;

	for (int i = 0; i < memoryMapNode.childNodes().count(); ++i) {

		QDomNode tempNode = memoryMapNode.childNodes().at(i);

		// get SubSpaceMap
		if (tempNode.nodeName() == QString("spirit:subspaceMap")) {
			SubspaceMap *temp = new SubspaceMap(tempNode);
			items_.append(QSharedPointer<MemoryMapItem>(temp));
		}

		// get Bank
		else if (tempNode.nodeName() == QString("spirit:bank")) {
			Bank *temp = new Bank(tempNode);
			items_.append(QSharedPointer<MemoryMapItem>(temp));
		}

		// get addressBlock
		else if (tempNode.nodeName() == QString("spirit:addressBlock")) {
			AddressBlock *temp = new AddressBlock(tempNode);
			items_.append(QSharedPointer<MemoryMapItem>(temp));
		}

		// get usage
		else if (tempNode.nodeName() == QString("spirit:usage")) {
			usage = General::str2Usage(tempNode.childNodes().at(0).nodeValue(),
					General::USAGE_COUNT);
		}

		// get volatile
		else if (tempNode.nodeName() == QString("spirit:volatile")) {
			QString volatileStr = tempNode.childNodes().at(0).nodeValue();
			volatileValue = General::str2BooleanValue(volatileStr);
		}

		// get access
		else if (tempNode.nodeName() == QString("spirit:access")) {
			access = General::str2Access(tempNode.childNodes().at(0).
					nodeValue(), General::ACCESS_COUNT);
		}

		// get parameters
		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// go through all parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				QDomNode parameterNode = tempNode.childNodes().at(j);
				Parameter *temp = new Parameter(parameterNode);
				tempParameters.append(QSharedPointer<Parameter>(temp));
			}
		}

        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j) {
                QDomNode extensionNode = tempNode.childNodes().at(j);
                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
            }
        }
	}

	// parsing information is over so save the values to memoryBlockData
	memoryBlockData_ = QSharedPointer<MemoryBlockData>(
			new MemoryBlockData(usage, volatileValue, access, tempParameters));
	tempParameters.clear();
}

Bank::Bank( const Bank &other ):
MemoryMapItem(other),
items_(),
memoryBlockData_(),
vendorExtensions_(other.vendorExtensions_)
{

	foreach (QSharedPointer<MemoryMapItem> memItem, other.items_) {
		if (memItem) {

			AddressBlock* addrBlock = dynamic_cast<AddressBlock*>(memItem.data());
			Bank* bank = dynamic_cast<Bank*>(memItem.data());
			SubspaceMap* subMap = dynamic_cast<SubspaceMap*>(memItem.data());
			QSharedPointer<MemoryMapItem> copy;

			if (addrBlock) {
				copy = QSharedPointer<MemoryMapItem>(
					new AddressBlock(*addrBlock));
			}
			else if (bank) {
				copy = QSharedPointer<MemoryMapItem>(
					new Bank(*bank));
			}
			else if (subMap) {
				copy = QSharedPointer<MemoryMapItem>(
					new SubspaceMap(*subMap));
			}
			if (copy)
				items_.append(copy);
		}
	}

	if (other.memoryBlockData_) {
		memoryBlockData_ = QSharedPointer<MemoryBlockData>(
			new MemoryBlockData(*other.memoryBlockData_.data()));
	}
}

Bank & Bank::operator=( const Bank &other ) {
	if (this != &other) {
		MemoryMapItem::operator=(other);
        vendorExtensions_ = other.vendorExtensions_;

		items_.clear();
		foreach (QSharedPointer<MemoryMapItem> memItem, other.items_) {
			if (memItem) {

				AddressBlock* addrBlock = dynamic_cast<AddressBlock*>(memItem.data());
				Bank* bank = dynamic_cast<Bank*>(memItem.data());
				SubspaceMap* subMap = dynamic_cast<SubspaceMap*>(memItem.data());
				QSharedPointer<MemoryMapItem> copy;

				if (addrBlock) {
					copy = QSharedPointer<MemoryMapItem>(
						new AddressBlock(*addrBlock));
				}
				else if (bank) {
					copy = QSharedPointer<MemoryMapItem>(
						new Bank(*bank));
				}
				else if (subMap) {
					copy = QSharedPointer<MemoryMapItem>(
						new SubspaceMap(*subMap));
				}
				if (copy)
					items_.append(copy);
			}
		}

		if (other.memoryBlockData_) {
			memoryBlockData_ = QSharedPointer<MemoryBlockData>(
				new MemoryBlockData(*other.memoryBlockData_.data()));
		}
		else
			memoryBlockData_ = QSharedPointer<MemoryBlockData>();
	}
	return *this;
}

Bank::~Bank() {
	items_.clear();

	memoryBlockData_.clear();
}

QSharedPointer<MemoryMapItem> Bank::clone() const {
	return QSharedPointer<MemoryMapItem>(new Bank(*this));
}

void Bank::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:bank");

	// write the base class information
	MemoryMapItem::write(writer);

	// write the collection of address blocks, banks or subspaceMaps
	for (int i = 0; i < items_.size(); ++i) {
		items_.at(i)->write(writer);
	}

	if (memoryBlockData_) {
		memoryBlockData_->write(writer);
	}

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:bank
}

//-----------------------------------------------------------------------------
// Function: bank::isValid()
//-----------------------------------------------------------------------------
bool Bank::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, QStringList& errorList,
    const QString& parentIdentifier ) const
{
	bool valid = true;
    const QString thisIdentifier(QObject::tr("bank %1").arg(getName()));

    if (getName().isEmpty())
    {
		errorList.append(QObject::tr("No name specified for bank within %1").arg(parentIdentifier));
		valid = false;
	}

	if (!attributes_.contains(QString("spirit:bankAlignment"))) {
		errorList.append(QObject::tr("No bank alignment set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (baseAddress_.isEmpty()) {
		errorList.append(QObject::tr("No base address set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (memoryBlockData_ && !memoryBlockData_->isValid(componentChoices, errorList, thisIdentifier)) {
		valid = false;
	}

	if (items_.isEmpty()) {
		errorList.append(QObject::tr("At least one sub item must be listed in %1"
			" within %2").arg(thisIdentifier).arg(parentIdentifier));
		valid = false;
	}
	else {
		foreach (QSharedPointer<MemoryMapItem> memItem, items_) {
			if (!memItem->isValid(componentChoices, errorList, thisIdentifier)) {
				valid = false;
			}
		}
	}

	return valid;
}

//-----------------------------------------------------------------------------
// Function: bank::isValid()
//-----------------------------------------------------------------------------
bool Bank::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const
{
    if (getName().isEmpty())
    {
		return false;
	}

	if (!attributes_.contains(QString("spirit:bankAlignment"))) {
		return false;
	}

	if (baseAddress_.isEmpty()) {
		return false;
	}

	if (memoryBlockData_ && !memoryBlockData_->isValid(componentChoices))
    {
		return false;
	}

	if (items_.isEmpty()) {
		return false;
	}
	else {
		foreach (QSharedPointer<MemoryMapItem> memItem, items_) {
			if (!memItem->isValid(componentChoices)) {
				return false;
			}
		}
	}

	return true;
}

void Bank::setItems(QList<QSharedPointer<MemoryMapItem> > &items) {
	items_ = items;
}

const QList<QSharedPointer<MemoryMapItem> >& Bank::getItems() {
	return items_;
}

General::Access Bank::getAccess() const {
	return memoryBlockData_->getAccess();
}

General::Usage Bank::getUsage() const {
	return memoryBlockData_->getUsage();
}

const QList<QSharedPointer<Parameter> >& Bank::getParameters() {
	return memoryBlockData_->getParameters();
}

void Bank::setParameters(QList<QSharedPointer<Parameter> > &parameters) {
	// save the new parameters
	memoryBlockData_->setParameters(parameters);
}

void Bank::setAccess(General::Access access) {
	memoryBlockData_->setAccess(access);
}

General::BooleanValue Bank::getVolatile() const {
	return memoryBlockData_->getVolatile();
}

void Bank::setUsage(General::Usage usage) {
	memoryBlockData_->setUsage(usage);
}

void Bank::setVolatile(const General::BooleanValue volatileSetting) {
	memoryBlockData_->setVolatile(volatileSetting);
}

/* 
 *
 *  Created on: 18.6.2010
 */

#include "ipxactmodel.h"
#include "ipxactitem.h"
#include "vlnv.h"

#include <QtXml>
#include <QString>
#include <QDomNodeList>
#include <QTextStream>
#include <QSharedPointer>
#include <QObject>

/*!
 * The VLNV tags that name the elements containing information
 */
const QString VENDOR_("spirit:vendor");
const QString LIBRARY_("spirit:library");
const QString NAME_("spirit:name");
const QString VERSION_("spirit:version");

// Constructor
IPXactModel::IPXactModel(QDomDocument document, QObject *parent):
									QAbstractItemModel(parent), IPXactDoc_(document) {

	// create the root item
	rootItem_ = QSharedPointer<IPXactItem>(new IPXactItem(IPXactDoc_, 0));
}

// Destructor
IPXactModel::~IPXactModel() {
}

// get the parents model index
QModelIndex IPXactModel::parent(const QModelIndex &child) const {
	// if the parameter is invalid, an invalid model index is returned
	if (!child.isValid()) {
		return QModelIndex();
	}

	// pointer to the child item
	IPXactItem *childItem = static_cast<IPXactItem*>(child.internalPointer());
	IPXactItem *parent = childItem->parent();

	// if the parent is invalid or is root then invalid model index is returned
	if (!parent || parent == rootItem_) {
		return QModelIndex();
	}

	// create new index for the parent and return it
	return createIndex(parent->row(), 0, parent);
}

// get the data from the identified item
QVariant IPXactModel::data(const QModelIndex &index, int role) const {
	// if given index is invalid then invalid data is returned
	if (!index.isValid()) {
		return QVariant();
	}

	// valid data is returned only for supported roles
	if ((role == Qt::DisplayRole) || (role == Qt::EditRole)) {

		// get the pointer to the item
		IPXactItem *item = static_cast<IPXactItem*>(index.internalPointer());

		// get the node owned by the item
		QDomNode *node = item->node();

		// get list of the attributes for the node
		QStringList attributes;
		QDomNamedNodeMap attributeMap = node->attributes();

		// the data to be showed is determined by the column of the item
		switch (index.column()) {
		// first column shows the name of the element
		case 0:
			return node->nodeName();

			// second column shows the attributes of the element
		case 1:
			for (int i = 0; i < attributeMap.count(); ++i) {
				QDomNode attribute = attributeMap.item(i);
				attributes << attribute.nodeName() + "=\""
						+ attribute.nodeValue() + "\"";
			}
			return attributes.join(" ");

			// third column shows the values of the elements
		case 2:
			return node->nodeValue().split("\n").join(" ");

			// no more columns are shown
		default:
			return QVariant();
		}
	}
	else if (role == Qt::ToolTipRole) {
		switch (index.column()) {
		case 0:
			return QObject::tr("Name of the element");
		case 1:
			return QObject::tr("Attributes of the element");
		case 2:
			return QObject::tr("Value of the element");
		default:
			return QVariant();
		}
	}
	else {
		return QVariant();
	}
}

bool IPXactModel::setData(const QModelIndex &index, const QVariant &value,
		int role) {
	// if the index is invalid, nothing is done
	if (!index.isValid()) {
		return false;
	}

	// the data is edited only for supported roles
	if (role != Qt::EditRole) {
		return false;
	}

	// get the pointer to the item
	IPXactItem *item = static_cast<IPXactItem*>(index.internalPointer());

	// get the node from the wrapper
	QDomNode *node = item->node();

	// select which column to edit
	switch (index.column()) {
	// the element name cannot be edited
	case 0:
		return false;
		// the attributes cannot be edited
	case 1:
		return false;
		// the value of an element can be modified
	case 2:
		// set the new value
		node->setNodeValue(value.toString());
		// emit the signal to inform the change
		emit(dataChanged(index, index));
		return true;
	default:
		return false;
	}
}

// Return the amount of columns the item has
int IPXactModel::columnCount(const QModelIndex&) const {
	return 3;
}

// Get the headerdata of the given section
QVariant IPXactModel::headerData(int section, Qt::Orientation orientation,
		int role) const {
	// valid data is returned for only approved roles
	if (((role == Qt::DisplayRole) ||
			(role == Qt::EditRole)) &&
			(orientation == Qt::Horizontal)) {
		switch (section) {
		case 0:
			return tr("Name");
		case 1:
			return tr("Attributes");
		case 2:
			return tr("Value");
		default:
			return QVariant();
		}
	}

	// The role was not approved
	return QVariant();
}

// get the falgs associated with the item
Qt::ItemFlags IPXactModel::flags(const QModelIndex &index) const {
	// is given index is invalid
	if (!index.isValid()) {
		return Qt::ItemIsEnabled;
	}

	if (index.column() != 2) {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	else {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	}
}

// return numebr of rows located under the identified item
int IPXactModel::rowCount(const QModelIndex &parent) const {
	if (parent.column() > 0) {
		return 0;
	}

	// get the pointer to the parent
	IPXactItem *parentItem = 0;

	// if the given item is invalid it is interpreted as root item
	if (!parent.isValid()) {
		parentItem = rootItem_.data();
	}
	else {
		parentItem = static_cast<IPXactItem*>(parent.internalPointer());
	}

	// return the number of the child items (rows) the parent has
	return parentItem->node()->childNodes().count();
}

// create an index for the item with the given row, column and parent
QModelIndex IPXactModel::index(int row, int column,
		const QModelIndex & parent) const {

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	IPXactItem *parentItem;

	// if the index of the parent is invalid then it is the root item
	if (!parent.isValid()) {
		parentItem = rootItem_.data();
	}
	else {
		parentItem = static_cast<IPXactItem*>(parent.internalPointer());
	}

	IPXactItem *child = parentItem->child(row);
	// if the child exists
	if (child) {
		return createIndex(row, column, child);
	}
	else {
		return QModelIndex();
	}
}

// get the VLNV tag of the IP-XACT document
VLNV IPXactModel::getVLNV() {
	// first the correct element is searched for by its name
	// and then the child of the element (which is the text element that holds
	// the value of the element) is added to the struct

	QString temp;

	// get the type of the document
	QDomNodeList nodeList = IPXactDoc_.childNodes();

	// search for the first element with children
	int i = 0;
	while (!nodeList.at(i).hasChildNodes()) {
		++i;

		if (i >= nodeList.size()) {
			return VLNV();
		}
	}
	temp = nodeList.at(i).nodeName();
	QTextStream stream(&temp);
	stream.skipWhiteSpace();
	QString type;
	stream >> type;

	// the vendor information
	QDomNodeList vendorList = IPXactDoc_.elementsByTagName(VENDOR_);
	temp = vendorList.item(0).childNodes().item(0).nodeValue();
	QTextStream stream2(&temp);
	stream2.skipWhiteSpace();
	QString vendor;
	stream2 >> vendor;

	// the library information
	QDomNodeList libraryList = IPXactDoc_.elementsByTagName(LIBRARY_);
	temp = libraryList.item(0).childNodes().item(0).nodeValue();
	QTextStream stream3(&temp);
	stream3.skipWhiteSpace();
	QString library;
	stream3 >> library;

	// The name information
	QDomNodeList nameList = IPXactDoc_.elementsByTagName(NAME_);
	temp = nameList.item(0).childNodes().item(0).nodeValue();
	QTextStream stream4(&temp);
	stream4.skipWhiteSpace();
	QString name;
	stream4 >> name;

	// The version information
	QDomNodeList versionList = IPXactDoc_.elementsByTagName(VERSION_);
	temp = versionList.item(0).childNodes().item(0).nodeValue();
	QTextStream stream5(&temp);
	stream5.skipWhiteSpace();
	QString version;
	stream5 >> version;

	return VLNV(type, vendor, library, name, version);
}

VLNV::IPXactType IPXactModel::getDocType() const {
	QDomNodeList nodeList = IPXactDoc_.childNodes();
	int i = 0;

	if (nodeList.isEmpty())
		return VLNV::INVALID;

	// search for the firstelement with children (=document type)
	while (!nodeList.at(i).hasChildNodes()) {
		++i;

		// if theres no nodes left then this was not ip-xact file
		if (i >= nodeList.count())
			return VLNV::INVALID;
	}

	// skip possible whitespaces in the file
	QString temp = nodeList.at(i).nodeName();
	QTextStream stream(&temp);
	stream.skipWhiteSpace();
	QString type;
	stream >> type;

	// return the type without whitespaces
	return VLNV::string2Type(type);
}

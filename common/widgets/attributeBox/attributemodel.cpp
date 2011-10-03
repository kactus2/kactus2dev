/* 
 *
 *  Created on: 15.1.2011
 *      Author: Antti Kamppi
 */

#include "attributemodel.h"

// struct constructor
AttributeModel::AttributePair::AttributePair(const QString name,
		const QString value): name_(name), value_(value) {
}

AttributeModel::AttributeModel(QObject* parent): QAbstractTableModel(parent),
		attributes_() {
}

AttributeModel::~AttributeModel() {
}

int AttributeModel::rowCount(const QModelIndex&) const {
	return attributes_.size();
}

int AttributeModel::columnCount(const QModelIndex&) const {
	return 2;
}

QVariant AttributeModel::data(const QModelIndex& index, int role) const {

	// if not valid index then nothing is returned
	if (!index.isValid()) {
		return QVariant();
	}

	// only displayRole and editRole are supported
	if (role != Qt::DisplayRole && role != Qt::EditRole) {
		return QVariant();
	}

	// which column is asked
	switch (index.column()) {
	case 0: {
		return attributes_.at(index.row())->name_;
	}
	case 1: {
		return attributes_.at(index.row())->value_;
	}
	// invalid column
	default: {
		return QVariant();
	}
	}
}

QVariant AttributeModel::headerData(int section, Qt::Orientation orientation,
		int role) const {

	// only horizontal headers
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}

	// Headers are only displayed, they can not be edited
	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	switch (section) {
	case 0: {
		return tr("Name");
	}
	case 1: {
		return tr("Value");
	}
	// invalid section
	default: {
		return QVariant();
	}
	}
}

bool AttributeModel::setData(const QModelIndex& index, const QVariant& value,
		int role) {

	// only valid indexes
	if (!index.isValid()) {
		return false;
	}
	// only edit role can edit
	else if (role != Qt::EditRole) {
		return false;
	}

	// is name or value edited
	switch (index.column()) {
	case 0: {
		attributes_.at(index.row())->name_ = value.toString();
		emit attributeChanged(index.column());
		return true;
	}
	case 1: {
		attributes_.at(index.row())->value_ = value.toString();
		emit attributeChanged(index.column());
		return true;
	}
	// invalid column
	default: {
		return false;
	}
	}
}

Qt::ItemFlags AttributeModel::flags(const QModelIndex& index) const {

	// if invalid index
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QMap<QString, QString> AttributeModel::getAttributes() const {
	QMap<QString, QString> attributes;

	// go through all attributes stored in the model
	for (int i = 0; i < attributes_.size(); ++i) {
		attributes.insert(attributes_.at(i)->name_,
				attributes_.at(i)->value_);
	}

	return attributes;
}

void AttributeModel::removeAttribute(QModelIndex index) {

	if (!index.isValid()) {
		return;
	}

	beginRemoveRows(QModelIndex(), index.row(), index.row());
	attributes_.removeAt(index.row());
	endRemoveRows();

	// resize the columns to hold the attributes in case the columns can be 
	// resized
	emit attributeChanged(0);
	emit attributeChanged(1);
}

void AttributeModel::addAttribute(const QString& name, const QString& value) {
	beginInsertRows(QModelIndex(), attributes_.size(), attributes_.size());
	attributes_.append(QSharedPointer<AttributeModel::AttributePair>(
			new AttributeModel::AttributePair(name, value)));
	endInsertRows();

	// resize the columns to hold the new attributes
	emit attributeChanged(0);
	emit attributeChanged(1);
}

bool AttributeModel::validAttributes() const {

	for (int i = 0; i < attributes_.size(); ++i) {

		// if name or value is empty
		if (attributes_.at(i)->name_.isEmpty()) {
			return false;
		}
		else if (attributes_.at(i)->value_.isEmpty()) {
			return false;
		}
	}
	// no empty strings were found
	return true;
}

void AttributeModel::setAttributes(const QMap<QString, QString>& attributes) {

	beginResetModel();

	// remove the old attributes first
	attributes_.clear();

	// add all attributes from the map to this model
	for (QMap<QString, QString>::const_iterator i = attributes.begin();
			i != attributes.end(); ++i) {

		// add the pair to model data structure
		AttributeModel::AttributePair* pair = new AttributeModel::AttributePair(
				i.key(), i.value());
		attributes_.append(QSharedPointer<AttributeModel::AttributePair>(pair));
	}

	endResetModel();
}

void AttributeModel::clear() {

	beginResetModel();
	attributes_.clear();
	endResetModel();
}

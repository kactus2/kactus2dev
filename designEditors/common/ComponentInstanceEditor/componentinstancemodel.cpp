/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: componentinstancemodel.cpp
 *		Project: Kactus 2
 */

#include "componentinstancemodel.h"

#include <designEditors/common/DesignDiagram.h>

#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/HWDesign/HWChangeCommands.h>

ComponentInstanceModel::ComponentInstanceModel(QObject *parent):
QAbstractTableModel(parent),
component_(0),
values_(),
table_(),
editProvider_(0) {
}

ComponentInstanceModel::~ComponentInstanceModel() {
}

void ComponentInstanceModel::setComponent( ComponentItem* component ) {

	Q_ASSERT(component);

	// if there is previously selected component
	if (component_) {
		component_->disconnect(this);
	}

	component_ = component;

    // get the edit provider that manages the undo/redo stack
    DesignDiagram* diagram = static_cast<DesignDiagram*>(component->scene());
    editProvider_ = &diagram->getEditProvider();
    	
	values_ = component->getConfigurableElements();
	readValues();

	connect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
		this, SLOT(changeElements(const QMap<QString, QString>&)), Qt::UniqueConnection);

	// if the connected component is destroyed then clear this editor
	connect(component_, SIGNAL(destroyed(ComponentItem*)),
		    this, SLOT(clear()), Qt::UniqueConnection);
}

void ComponentInstanceModel::clear() {

	// if previous component has been specified
	if (component_) {
		// disconnect the component's signals
		component_->disconnect(this);
	}

	component_ = 0;
	values_.clear();

	beginResetModel();
	table_.clear();
	endResetModel();
}

int ComponentInstanceModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid())
		return 0;
	return table_.size();
}

int ComponentInstanceModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid())
		return 0;
	return 2;
}

QVariant ComponentInstanceModel::data( const QModelIndex& index, 
									  int role /*= Qt::DisplayRole*/ ) const {

	if (!index.isValid())
		return QVariant();
	else if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {
		switch (index.column()) {
			case 0:
				return table_.at(index.row()).name_;
			case 1:
				return table_.at(index.row()).value_;
			default:
				return QVariant();
		}
	}
	else 
		return QVariant();
}

QVariant ComponentInstanceModel::headerData( int section,
											Qt::Orientation orientation, 
											int role /*= Qt::DisplayRole */ ) const {
								
	if (orientation != Qt::Horizontal)
		return QVariant();

	if (role == Qt::DisplayRole) {
		switch (section) {
			case 0:
				return tr("Name");
			case 1:
				return tr("Value");
			default:
				return QVariant();
		}
	}
	else
		return QVariant();
}

bool ComponentInstanceModel::setData( const QModelIndex& index,
									 const QVariant& value, 
									 int role /*= Qt::EditRole */ ) {

	if (!index.isValid())
		return false;
	else if (index.row() < 0 || index.row() >= table_.size())
		return false;

	if (role == Qt::EditRole) {
		switch (index.column()) {
			case 0: {
				table_[index.row()].name_ = value.toString();
				break;
					}
			case 1: {
				table_[index.row()].value_ = value.toString();
				break;
					}
			default:
				return false;
		}
		save();
		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else
		return false;
}

Qt::ItemFlags ComponentInstanceModel::flags( const QModelIndex& index ) const {

	if (!index.isValid())
		return Qt::NoItemFlags;
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

void ComponentInstanceModel::save() {

	// create the map that contains the new values.
	QMap<QString, QString> newValues;
	for (int i = 0; i < table_.size(); ++i) {
		newValues.insert(table_.at(i).name_, table_.at(i).value_);
	}

	QSharedPointer<ComponentConfElementChangeCommand> cmd(new ComponentConfElementChangeCommand(component_,
		newValues));

	// disconnect the signal to avoid infinite loops of changes.
	disconnect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
		this, SLOT(changeElements(const QMap<QString, QString>&)));
	editProvider_->addCommand(cmd);
	connect(component_, SIGNAL(confElementsChanged(const QMap<QString, QString>&)),
		this, SLOT(changeElements(const QMap<QString, QString>&)), Qt::UniqueConnection);

	// update the current data structure.
	values_ = newValues;
}

void ComponentInstanceModel::onAdd() {
	ComponentInstanceModel::ConfigurableElement element(QString(""), QString(""));
	if (!table_.contains(element)) {
		beginInsertRows(QModelIndex(), table_.size(), table_.size());
		table_.append(element);
		endInsertRows();
		emit contentChanged();
	}
}

void ComponentInstanceModel::onAddItem( const QModelIndex& index ) {
	int row = table_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	ComponentInstanceModel::ConfigurableElement element(QString(""), QString(""));

	if (!table_.contains(element)) {
		beginInsertRows(QModelIndex(), row, row);
		table_.insert(row, element);
		endInsertRows();

		// tell also parent widget that contents have been changed
		emit contentChanged();
	}
}

void ComponentInstanceModel::onRemove( const QModelIndex& index ) {

	if (!index.isValid())
		return;

	// remove the indexed configurable element
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	table_.removeAt(index.row());
	endRemoveRows();
	
	// save the changes to the diagram component
	save();

	emit contentChanged();
}

void ComponentInstanceModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return;
	}

	// remove the indexed configurable element
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	table_.removeAt(index.row());
	endRemoveRows();

	// save the changes to the diagram component
	save();

	emit contentChanged();
}



void ComponentInstanceModel::changeElements( const QMap<QString, QString>& confElements ) {
	values_ = confElements;
	readValues();
}

void ComponentInstanceModel::readValues() {
	beginResetModel();

	// remove the items after the previous component instance
	table_.clear();

	// insert items from this component instance
	for (QMap<QString, QString>::iterator i = values_.begin();
		i != values_.end(); ++i) {
			table_.append(ConfigurableElement(i.key(), i.value()));
	}
	endResetModel();
}

ComponentInstanceModel::ConfigurableElement::ConfigurableElement( 
	const QString& name, const QString& value ):
name_(name), 
value_(value) {
}

ComponentInstanceModel::ConfigurableElement::ConfigurableElement(const QString& name):
name_(name),
value_() {
}

bool ComponentInstanceModel::ConfigurableElement::operator==( 
	const ConfigurableElement& other ) const {
	return name_ == other.name_;
}

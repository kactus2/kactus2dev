/* 
 *  	Created on: 24.8.2011
 *      Author: Antti Kamppi
 * 		filename: activeviewmodel.cpp
 *		Project: Kactus 2
 */

#include "activeviewmodel.h"

#include <designEditors/HWDesign/HWComponentItem.h>
#include <designEditors/SystemDesign/HWMappingItem.h>
#include <designEditors/SystemDesign/SWComponentItem.h>
#include <designEditors/HWDesign/HWDesignWidget.h>
#include <designEditors/HWDesign/HWChangeCommands.h>

#include <IPXACTmodels/component.h>

ActiveViewModel::ActiveViewModel(QObject *parent):
QAbstractTableModel(parent),
designWidget_(NULL),
table_(),
desConf_(),
instances_() {
}

ActiveViewModel::~ActiveViewModel() {
}

void ActiveViewModel::setDesign( DesignWidget* designWidget, 
								QSharedPointer<DesignConfiguration> desConf) {
	Q_ASSERT(designWidget);

	// remove the previous instances and disconnect their signals
	clear();

	designWidget_ = designWidget;

	editProvider_ = designWidget->getGenericEditProvider();

	desConf_ = desConf;
	// if design configuration is not used.
	if (!desConf_) {

		return;
	}

	beginResetModel();

	// Retrieve only those instances that should be shown, depending on the implementation attribute.
    instances_ = designWidget->getInstances();

	for (int i = 0; i < instances_.size(); ++i) {
		
		const QString instanceName = instances_.at(i)->name();

		// if theres a defined active view for the instance.
		if (desConf_->hasActiveView(instanceName)) {
			table_.append(InstanceViewPair(instanceName, desConf_->getActiveView(instanceName)));
		}
		// if no active view has been defined.
		else {
			table_.append(InstanceViewPair(instanceName));
		}

		connect(instances_.at(i), SIGNAL(nameChanged(const QString&, const QString&)),
			this, SLOT(renameInstance(const QString&, const QString&)), Qt::UniqueConnection);
		connect(instances_.at(i), SIGNAL(destroyed(ComponentItem*)),
			this, SLOT(removeInstance(ComponentItem*)), Qt::UniqueConnection);
	}

	endResetModel();

	connect(designWidget, SIGNAL(componentInstantiated(ComponentItem*)),
		this, SLOT(addInstance(ComponentItem*)), Qt::UniqueConnection);
	connect(designWidget, SIGNAL(componentInstanceRemoved(ComponentItem*)),
		this, SLOT(removeInstance(ComponentItem*)), Qt::UniqueConnection);
}

void ActiveViewModel::clear() {

	// if previous design widget has been set then disconnect it
	if (designWidget_) {
		designWidget_->disconnect(this);
		designWidget_ = 0;
	}

	// disconnect all component instances and clear the list
	foreach(ComponentItem* diaComp, instances_) {
		diaComp->disconnect(this);
	}
	instances_.clear();

	// clear the pointers
	desConf_.clear();
	editProvider_.clear();

	beginResetModel();
	table_.clear();
	endResetModel();
}

int ActiveViewModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid())
		return 0;
	return table_.size();
}

int ActiveViewModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid())
		return 0;
	return 2;
}

QVariant ActiveViewModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid())
		return QVariant();
	else if (index.row() < 0 || index.row() >= table_.size())
		return QVariant();

	if (role == Qt::DisplayRole) {

		switch (index.column()) {
			case 0: {
				return table_.at(index.row()).instanceName_;
					}
			case 1: {
				return table_.at(index.row()).viewName_;
					}
			default:
				return QVariant();
		}
	}
	else if (role == Qt::UserRole) {
		// if the possible view names are requested.
		if (index.column() == 1) {
			
			// search the diagram component that matches the asked instance name.
			ComponentItem* searched = 0;
			foreach (ComponentItem* diaComp, instances_) {
				// if instance name matches
				if (diaComp->name() == table_.at(index.row()).instanceName_) {
					searched = diaComp;
					break;
				}
			}
			Q_ASSERT(searched);

			// return the views the component model contains.
            if (searched->type() == HWComponentItem::Type)
            {
			    return searched->componentModel()->getViewNames();
            }
            else
            {
                return searched->componentModel()->getSWViewNames();
            }
		}
		else
			return QVariant();
	}
	// is unsupported role
	else
		return QVariant();
}

bool ActiveViewModel::setData(const QModelIndex& index, const QVariant& value,
							  int role /*= Qt::EditRole*/ ) {
								 
	if (!index.isValid())
		return false;
	else if (index.row() < 0 || index.row() >= table_.size())
		return false;

	if (role == Qt::EditRole) {
	
		switch (index.column()) {
			// only the view name can be changed
			case 1: {	

				// create a command to the undo stack
				QSharedPointer<ComponentActiveViewChangeCommand> command = QSharedPointer<ComponentActiveViewChangeCommand>(
					new ComponentActiveViewChangeCommand(
					table_[index.row()].instanceName_, table_[index.row()].viewName_, value.toString(), this));
				editProvider_->addCommand(command, false);

				// make the change
				table_[index.row()].viewName_ = value.toString();

				// save the new active view to the design configuration
				desConf_->addViewConfiguration(table_.at(index.row()).instanceName_,
					table_.at(index.row()).viewName_);
				emit dataChanged(index, index);
				return true;
					}
			default:
				return false;
		}
	}
	else
		return false;
}

QVariant ActiveViewModel::headerData(int section, Qt::Orientation orientation,
									 int role /*= Qt::DisplayRole*/ ) const {

	if (role != Qt::DisplayRole)
		return QVariant();
	else if (section < 0)
		return QVariant();
	else if (orientation != Qt::Horizontal)
		return QVariant();

	if (role == Qt::DisplayRole) {
		switch (section) {
		   case 0: {
			   return tr("Instance name");
				   }
		   case 1: {
			   return tr("Active view");
				   }
		   default: {
			   return QVariant();
					}
		}
	}
	// if unsupported role
	else
		return QVariant();
}

Qt::ItemFlags ActiveViewModel::flags(const QModelIndex& index ) const {
	if (!index.isValid())
		return Qt::NoItemFlags;

	// the instance name can not be edited.
	else if (index.column() == 0)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	// active view can be edited.
	else
		return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void ActiveViewModel::renameInstance(const QString& newName,  const QString& oldName) {

	InstanceViewPair newItem(oldName);
	
	// if instance with same name is not found
	if (!table_.contains(newItem))
		return;
	else {
		// find the row of the named instance
		int row = table_.indexOf(newItem);
		
		// remove the configuration for the old name
		desConf_->removeViewConfiguration(table_[row].instanceName_);

		// rename the instance
		table_[row].instanceName_ = newName;

		// save the new active view to the design configuration
		desConf_->addViewConfiguration(table_.at(row).instanceName_,
			table_.at(row).viewName_);

		// inform views that data has changed
		QModelIndex modelIndex = QAbstractTableModel::index(row, 0, QModelIndex());
		emit dataChanged(modelIndex, modelIndex);
	}
}

void ActiveViewModel::removeInstance( ComponentItem* diaComp ) {

	if (diaComp) {
		InstanceViewPair itemToRemove(diaComp->name());
		
		// if item is found on the table
		if (table_.contains(itemToRemove)) {
			int row = table_.indexOf(itemToRemove);
			
			// remove the item from the table
			beginRemoveRows(QModelIndex(), row, row);
			table_.removeAt(row);
			endRemoveRows();

			// remove the configuration from the design conf
			desConf_->removeViewConfiguration(diaComp->name());
		}

		if (instances_.contains(diaComp)) {
			instances_.removeAll(diaComp);
		}
		
		// remove the connections between the diagram component and this model.
		diaComp->disconnect(this);
	}
}

void ActiveViewModel::addInstance( ComponentItem* diaComp ) {

	// if instance is already in the model
	if (instances_.contains(diaComp))
		return;

	// add the pointer to the component instance
	instances_.append(diaComp);

	// add the item to the model to be displayed
	InstanceViewPair newItem(diaComp->name());

	// if the component only has one view it can be selected as default
	QStringList viewNames = diaComp->componentModel()->getViewNames();
	if (viewNames.size() == 1) {
		
		// set the only view as default
		QString viewName = viewNames.first();
		newItem.viewName_ = viewName;
		desConf_->addViewConfiguration(diaComp->name(), viewName);
	}

	beginInsertRows(QModelIndex(), table_.size(), table_.size());
	table_.append(newItem);
	endInsertRows();

	connect(diaComp, SIGNAL(destroyed(ComponentItem*)),
		this, SLOT(removeInstance(ComponentItem*)), Qt::UniqueConnection);
	connect(diaComp, SIGNAL(nameChanged(const QString&, const QString&)),
		this, SLOT(renameInstance(const QString&, const QString&)), Qt::UniqueConnection);
}

void ActiveViewModel::setActiveView( const QString& instanceName, const QString& viewName ) {
	// search all component instances
	for (int i = 0; i < table_.size(); ++i) {
		
		// if instance is found
		if (table_.at(i).instanceName_ == instanceName) {

			// update the view name on the screen
			beginResetModel();
			table_[i].viewName_ = viewName;
			endResetModel();
	
			// update the design configuration
			if (desConf_) {
				desConf_->addViewConfiguration(instanceName, viewName);
			}

			return;
		}
	}
}

bool ActiveViewModel::hasConfiguration() const {
	// if design configuration exists 
	if (desConf_)
		return true;
	return false;
}


ActiveViewModel::InstanceViewPair::InstanceViewPair(const QString& instanceName, 
													const QString& viewName ):
instanceName_(instanceName),
viewName_(viewName) {
}

ActiveViewModel::InstanceViewPair::InstanceViewPair(const QString& instanceName ):
instanceName_(instanceName),
viewName_() {
}

bool ActiveViewModel::InstanceViewPair::operator==(const InstanceViewPair& other ) {
	return instanceName_ == other.instanceName_;
}

bool ActiveViewModel::InstanceViewPair::operator!=(const InstanceViewPair& other ) {
	return instanceName_ != other.instanceName_;
}

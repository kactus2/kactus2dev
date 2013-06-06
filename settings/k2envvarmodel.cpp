/* 
 *	Created on:	5.6.2013
 *	Author:		Antti Kamppi
 *	File name:	k2envvarmodel.cpp
 *	Project:		Kactus 2
*/

#include "k2envvarmodel.h"
#include <PluginSystem/IPlugin.h>

#include <QColor>
#include <QStringList>

K2EnvVarModel::K2EnvVarModel(QSettings& settings, PluginManager& pluginMgr, QObject *parent):
QAbstractTableModel(parent),
table_() {

	beginResetModel();

	// find the variables required by the active plugins
	 QList<IPlugin*> plugins = pluginMgr.getActivePlugins();
	 QList<IPlugin::ExternalProgramRequirements> pluginRequirements;
	 foreach (IPlugin* plugin, plugins) {
		 pluginRequirements.append(plugin->getProgramRequirements());
	 }

	 // add the plugin variables to the list
	 QStringList usedNames;
	 foreach (IPlugin::ExternalProgramRequirements const& req, pluginRequirements) {

		 // do not add variables with no name
		 if (req.name_.isEmpty()) {
			 continue;
		 }
		 // if same variable is used by several plugins then add it only once
		 else if (usedNames.contains(req.name_)) {
			 continue;
		 }

		 // for the next loops
		 usedNames.append(req.name_);

		 // add the variable
		 K2EnvVarModel::EnvSetting envVar(req.name_, QString(), req.filters_, req.description_, true);
		 table_.append(envVar);
	 }

	// list of the variable names
	QStringList variableNames = settings.value("K2Variables/variableNames").toStringList();

	// find the settings for each variable
	foreach(const QString& name, variableNames) {

		// create the variable from the settings
		QString value = settings.value("K2Variables/" + name + "/value").toString();
		QString suffix = settings.value("K2Variables/" + name + "/suffix").toString();
		QString desc = settings.value("K2Variables/" + name + "/description").toString();
		K2EnvVarModel::EnvSetting envVar(name, value, suffix, desc, false);

		// if the variable was defined by the plugins
		if (table_.contains(envVar)) {

			// other settings are defined by the plugins but value is set by settings
			int index = table_.indexOf(envVar);
			table_[index].value_ = envVar.value_;
		}
		// if the variable is new and not defined by the plugins
		else {
			table_.append(envVar);
		}
	}

	// Sort the variables before showing them
	qSort(table_);

	endResetModel();
}

K2EnvVarModel::~K2EnvVarModel() {
}

void K2EnvVarModel::apply( QSettings& settings ) {
	// first clear all previously defined variables to remove duplicates
	settings.remove("K2Variables");

	QStringList variableNames;

	foreach (K2EnvVarModel::EnvSetting const& envVar, table_) {

		// unnamed variables are not stored
		if (envVar.name_.isEmpty()) {
			continue;
		}

		variableNames.append(envVar.name_);

		// save the settings for the variable name
		settings.setValue("K2Variables/" + envVar.name_ + "/value", envVar.value_);
		settings.setValue("K2Variables/" + envVar.name_ + "/suffix", envVar.fileSuffix_);
		settings.setValue("K2Variables/" + envVar.name_ + "/description", envVar.description_);
	}

	settings.setValue("K2Variables/variableNames", variableNames);
}

int K2EnvVarModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return table_.size();
}

int K2EnvVarModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return K2EnvVarModel::COLUMN_COUNT;
}

Qt::ItemFlags K2EnvVarModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return Qt::NoItemFlags;
	}

	Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	switch (index.column()) {

		// name and description can't be edited for plugin variables
	case K2EnvVarModel::NAME_COLUMN:
	case K2EnvVarModel::DESCRIPTION_COLUMN: {
		if (!table_.at(index.row()).fromPlugin_) {
			flags |= Qt::ItemIsEditable;
		}
		break;
												}
	case K2EnvVarModel::VALUE_COLUMN: {
		flags |= Qt::ItemIsEditable;
		break;
												 }
	default: {
		break;
				}
	}
	return flags;
}

QVariant K2EnvVarModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
		case K2EnvVarModel::NAME_COLUMN: {
			return tr("Name");
												}
		case K2EnvVarModel::VALUE_COLUMN: {
			return tr("Value");
												}
		case K2EnvVarModel::FILE_SUFFIX_COLUMN: {
			return tr("File Suffix");
														 }
		case K2EnvVarModel::DESCRIPTION_COLUMN: {
			return tr("Description");
															 }
		default: {
			return QVariant();
					}
		}
	}
	else {
		return QVariant();
	}
}

QVariant K2EnvVarModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
		case K2EnvVarModel::NAME_COLUMN: {
			return table_.at(index.row()).name_;
													}
		case K2EnvVarModel::VALUE_COLUMN: {
			return table_.at(index.row()).value_;
													 }
		case K2EnvVarModel::FILE_SUFFIX_COLUMN: {
			return table_.at(index.row()).fileSuffix_;
															 }
		case K2EnvVarModel::DESCRIPTION_COLUMN: {
			return table_.at(index.row()).description_;
															 }
		default: {
			return QVariant();
					}
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
		case K2EnvVarModel::NAME_COLUMN: {
			return QColor("LemonChiffon");
												}
		default:
			return QColor("white");
		}
	}
	else {
		return QVariant();
	}
}

bool K2EnvVarModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
		case K2EnvVarModel::NAME_COLUMN: {

			// plugin variable's name can't be modified
			if (table_[index.row()].fromPlugin_) {
				return false;
			}

			QString newName = value.toString();

			// if a variable with the name already exists
			K2EnvVarModel::EnvSetting testVar(newName);
			if (table_.contains(testVar)) {
				return false;
			}

			table_[index.row()].name_ = newName;
			break;
													}
		case K2EnvVarModel::VALUE_COLUMN: {
			table_[index.row()].value_ = value.toString();
			break;
													 }
		case K2EnvVarModel::FILE_SUFFIX_COLUMN: {
			// plugin variable's file suffix can't be modified
			if (table_[index.row()].fromPlugin_) {
				return false;
			}

			table_[index.row()].fileSuffix_ = value.toString();
			break;
															 }
		case K2EnvVarModel::DESCRIPTION_COLUMN: {
			// plugin variable's description can't be modified
			if (table_[index.row()].fromPlugin_) {
				return false;
			}

			table_[index.row()].description_ = value.toString();
			break;
															 }
		default: {
			return false;
					}
		}

		emit dataChanged(index, index);
		return true;
	}
	else {
		return false;
	}
}

void K2EnvVarModel::onAddItem( const QModelIndex& index ) {
	int row = table_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	table_.insert(row, K2EnvVarModel::EnvSetting());
	endInsertRows();
}

void K2EnvVarModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= table_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	table_.removeAt(index.row());
	endRemoveRows();
}

bool K2EnvVarModel::containsEmptyVariables() const {
	foreach (K2EnvVarModel::EnvSetting const& envVar, table_) {
		if (envVar.name_.isEmpty()) {
			return true;
		}
	}

	// no empty variables defined
	return false;
}

K2EnvVarModel::EnvSetting::EnvSetting( const QString& name,
	const QString& value,
	const QString& fileSuffix, 
	const QString& description,
	bool fromPlugin ):
name_(name),
	value_(value),
	fileSuffix_(fileSuffix),
	description_(description),
	fromPlugin_(fromPlugin) {

}

K2EnvVarModel::EnvSetting::EnvSetting():
name_(),
value_(),
fileSuffix_(),
description_(),
fromPlugin_(false) {
}

K2EnvVarModel::EnvSetting::EnvSetting( const EnvSetting& other ):
name_(other.name_),
	value_(other.value_),
	fileSuffix_(other.fileSuffix_),
	description_(other.description_),
	fromPlugin_(other.fromPlugin_) {

}

K2EnvVarModel::EnvSetting::EnvSetting( const QString& name ):
name_(name),
	value_(),
	fileSuffix_(),
	description_(),
	fromPlugin_(false) {
}

K2EnvVarModel::EnvSetting& K2EnvVarModel::EnvSetting::operator=( const EnvSetting& other ) {
	if (this != &other) {
		name_ = other.name_;
		value_ = other.value_;
		fileSuffix_ = other.fileSuffix_;
		description_ = other.description_;
		fromPlugin_ = other.fromPlugin_;
	}
	return *this;
}

bool K2EnvVarModel::EnvSetting::operator<( const EnvSetting& other ) const {
	return name_.compare(other.name_, Qt::CaseInsensitive) < 0;
}

bool K2EnvVarModel::EnvSetting::operator==( const EnvSetting& other ) const {
	return name_.compare(other.name_, Qt::CaseInsensitive) == 0;
}

bool K2EnvVarModel::EnvSetting::operator!=( const EnvSetting& other ) const {
	return name_.compare(other.name_, Qt::CaseInsensitive) != 0;
}

/* 
 *  	Created on: 14.6.2012
 *      Author: Antti Kamppi
 * 		filename: cpusmodel.cpp
 *		Project: Kactus 2
 */

#include "cpusmodel.h"
#include "cpusdelegate.h"

#include <QStringList>
#include <QColor>

CpusModel::CpusModel( QSharedPointer<Component> component,
					 QObject *parent ):
QAbstractTableModel(parent),
component_(component),
cpus_(component->getCpus()) {
	Q_ASSERT(component_);
}

CpusModel::~CpusModel() {
}

int CpusModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return cpus_.size();
}

int CpusModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return CpusDelegate::COLUMN_COUNT;
}

Qt::ItemFlags CpusModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant CpusModel::headerData( int section, 
							   Qt::Orientation orientation, 
							   int role /*= Qt::DisplayRole*/ ) const {

	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case CpusDelegate::NAME_COLUMN: {
				return tr("Name");
												}
			case CpusDelegate::DISPLAY_NAME_COLUMN: {
				return tr("Display name");
														}
			case CpusDelegate::ADDRSPACE_COLUMN: {
				return tr("Address space\nreferences");
													 }
			case CpusDelegate::DESCRIPTION_COLUMN: {
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

QVariant CpusModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= cpus_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case CpusDelegate::NAME_COLUMN: {
				return cpus_.at(index.row())->getName();
												}
			case CpusDelegate::DISPLAY_NAME_COLUMN: {
				return cpus_.at(index.row())->getDisplayName();
														}
			case CpusDelegate::ADDRSPACE_COLUMN: {
				// each group name if in it's own index
				QStringList addrSpaceNames = cpus_.at(index.row())->getAddressSpaceRefs();

				// concatenate the names to a single string with spaces in between
				QString str;
				foreach (QString addrSpaceName, addrSpaceNames) {
					str += addrSpaceName;
					str += " ";
				}

				// return the string with all group names
				return str;
													 }
			case CpusDelegate::DESCRIPTION_COLUMN: {
				return cpus_.at(index.row())->getDescription();
													   }
			default: {
				return QVariant();
					 }
		}
	}
	// user display role for interface column returns a QStringList
	else if (CpusDelegate::USER_DISPLAY_ROLE == role && 
		index.column() == CpusDelegate::ADDRSPACE_COLUMN) {
			return cpus_.at(index.row())->getAddressSpaceRefs();
	}
	else if (Qt::ForegroundRole == role) {

		// address space names are needed to check that references to address spaces are valid
		QStringList addrSpaceNames = component_->getAddressSpaceNames();

		if (cpus_.at(index.row())->isValid(addrSpaceNames)) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else {
		return QVariant();
	}
}

bool CpusModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= cpus_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case CpusDelegate::NAME_COLUMN: {
				cpus_[index.row()]->setName(value.toString());
				break;
												}
			case CpusDelegate::DISPLAY_NAME_COLUMN: {
				cpus_[index.row()]->setDisplayName(value.toString());
				break;
														}
			case CpusDelegate::ADDRSPACE_COLUMN: {
				QString str = value.toString();
				QStringList addrSpaceNames = str.split(' ', QString::SkipEmptyParts);
				cpus_[index.row()]->setAddressSpaceRefs(addrSpaceNames);
				break;
													 }
			case CpusDelegate::DESCRIPTION_COLUMN: {
				cpus_[index.row()]->setDescription(value.toString());
				break;
													   }
			default: {
				return false;
					 }
		}

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	// user edit role for interface column operates on QStringList
	else if (CpusDelegate::USER_EDIT_ROLE == role &&
		CpusDelegate::ADDRSPACE_COLUMN == index.column()) {
			cpus_[index.row()]->setAddressSpaceRefs(value.toStringList());
			emit dataChanged(index, index);
			emit contentChanged();
			return true;
	}
	else {
		return false;
	}
}

bool CpusModel::isValid() const {
	// interface names are needed to check that references to bus interfaces are valid
	QStringList addrSpaceNames = component_->getAddressSpaceNames();

	// if at least one address space is invalid
	foreach (QSharedPointer<Cpu> cpu, cpus_) {
		if (!cpu->isValid(addrSpaceNames)) {
			return false;
		}
	}
	// all address spaces were valid
	return true;
}

void CpusModel::onAddItem( const QModelIndex& index ) {
	int row = cpus_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	cpus_.insert(row, QSharedPointer<Cpu>(new Cpu()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit cpuAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void CpusModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= cpus_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	cpus_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit cpuRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

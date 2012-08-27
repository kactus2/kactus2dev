/* 
 *  	Created on: 25.8.2012
 *      Author: Antti Kamppi
 * 		filename: registertablemodel.cpp
 *		Project: Kactus 2
 */

#include "registertablemodel.h"
#include "registerdelegate.h"

#include <QColor>

RegisterTableModel::RegisterTableModel(QSharedPointer<Register> reg, 
									   QObject *parent):
QAbstractTableModel(parent),
reg_(reg),
fields_(reg->getFields()) {

	Q_ASSERT(reg);
}

RegisterTableModel::~RegisterTableModel() {
}

int RegisterTableModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return fields_.size();
}

int RegisterTableModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return RegisterDelegate::COLUMN_COUNT;
}

Qt::ItemFlags RegisterTableModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}
	// if the field is not testable then the test constraint can not be set
	if (index.column() == RegisterDelegate::TEST_CONSTR_COLUMN &&
		!fields_.at(index.row())->getTestable()) {
			return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

QVariant RegisterTableModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case RegisterDelegate::NAME_COLUMN: {
				return tr("Field\nname");
													}
			case RegisterDelegate::DESC_COLUMN: {
				return tr("Description");
													  }
			case RegisterDelegate::OFFSET_COLUMN: {
				return tr("Offset");
												 }
			case RegisterDelegate::WIDTH_COLUMN: {
				return tr("Width");
												   }
			case RegisterDelegate::VOLATILE_COLUMN: {
				return tr("Volatile");
													}
			case RegisterDelegate::ACCESS_COLUMN: {
				return tr("Access");
												  }
			case RegisterDelegate::MOD_WRITE_COLUMN: {
				return tr("Modified\nwrite value");
												   }
			case RegisterDelegate::READ_ACTION_COLUMN: {
				return tr("Read\naction");
														   }
			case RegisterDelegate::TESTABLE_COLUMN: {
				return tr("Testable");
														  }
			case RegisterDelegate::TEST_CONSTR_COLUMN: {
				return tr("Test\nconstraint");
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

QVariant RegisterTableModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= fields_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case RegisterDelegate::NAME_COLUMN: {
				return fields_.at(index.row())->getName();
												}
			case RegisterDelegate::DESC_COLUMN: {
				return fields_.at(index.row())->getDescription();
												}
			case RegisterDelegate::OFFSET_COLUMN: {
				return fields_.at(index.row())->getBitOffset();
												  }
			case RegisterDelegate::WIDTH_COLUMN: {
				return fields_.at(index.row())->getBitWidth();
												 }
			case RegisterDelegate::VOLATILE_COLUMN: {
				return fields_.at(index.row())->getVolatile();
													}
			case RegisterDelegate::ACCESS_COLUMN: {
				return General::access2Str(fields_.at(index.row())->getAccess());
												  }
			case RegisterDelegate::MOD_WRITE_COLUMN: {
				return General::modifiedWrite2Str(fields_.at(index.row())->getModifiedWrite());
													 }
			case RegisterDelegate::READ_ACTION_COLUMN: {
				return General::readAction2Str(fields_.at(index.row())->getReadAction());
													   }
			case RegisterDelegate::TESTABLE_COLUMN: {
				return fields_.at(index.row())->getTestable();
													}
			case RegisterDelegate::TEST_CONSTR_COLUMN: {
				return General::testConstraint2Str(fields_.at(index.row())->getTestConstraint());
													   }
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		if (fields_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case RegisterDelegate::NAME_COLUMN:
			case RegisterDelegate::OFFSET_COLUMN:
			case RegisterDelegate::WIDTH_COLUMN: {
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

bool RegisterTableModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= fields_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case RegisterDelegate::NAME_COLUMN: {
				fields_.at(index.row())->setName(value.toString());
				break;
												}
			case RegisterDelegate::DESC_COLUMN: {
				fields_.at(index.row())->setDescription(value.toString());
				break;
												}
			case RegisterDelegate::OFFSET_COLUMN: {
				fields_.at(index.row())->setBitOffset(value.toInt());
				break;
												  }
			case RegisterDelegate::WIDTH_COLUMN: {
				fields_.at(index.row())->setBitWidth(value.toUInt());
				break;
												 }
			case RegisterDelegate::VOLATILE_COLUMN: {
				fields_.at(index.row())->setVolatile(value.toBool());
				break;
													}
			case RegisterDelegate::ACCESS_COLUMN: {
				fields_.at(index.row())->setAccess(General::str2Access(value.toString(), General::ACCESS_COUNT));
				break;
												  }
			case RegisterDelegate::MOD_WRITE_COLUMN: {
				fields_.at(index.row())->setModifiedWrite(General::str2ModifiedWrite(value.toString()));
				break;
													 }
			case RegisterDelegate::READ_ACTION_COLUMN: {
				fields_.at(index.row())->setReadAction(General::str2ReadAction(value.toString()));
				break;
													   }
			case RegisterDelegate::TESTABLE_COLUMN: {
				bool testable = value.toBool();
				fields_.at(index.row())->setTestable(testable);
				// if testable is disabled then clear the test constraint
				if (!testable) {
					fields_.at(index.row())->setTestConstraint(General::TESTCONSTRAINT_COUNT);
					QModelIndex constrIndex = createIndex(index.row(), index.column() + 1, index.internalPointer());
					emit dataChanged(constrIndex, constrIndex);
				}
				break;
													}
			case RegisterDelegate::TEST_CONSTR_COLUMN: {
				fields_.at(index.row())->setTestConstraint(General::str2TestConstraint(value.toString()));
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
	else {
		return false;
	}
}

bool RegisterTableModel::isValid() const {
	foreach (QSharedPointer<Field> field, fields_) {
		if (!field->isValid()) {
			return false;
		}
	}
	return true;
}

void RegisterTableModel::onAddItem( const QModelIndex& index ) {
	int row = fields_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	fields_.insert(row, QSharedPointer<Field>(new Field()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit fieldAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void RegisterTableModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= fields_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	fields_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit fieldRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

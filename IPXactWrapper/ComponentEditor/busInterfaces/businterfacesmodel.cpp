/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: businterfacesmodel.cpp
 *		Project: Kactus 2
 */

#include "businterfacesmodel.h"
#include <LibraryManager/libraryinterface.h>
#include "businterfacesdelegate.h"
#include <models/generaldeclarations.h>
#include <LibraryManager/vlnv.h>

#include <QMap>
#include <QString>
#include <QColor>
#include <QMimeData>
#include <QVariant>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel()
//-----------------------------------------------------------------------------
BusInterfacesModel::BusInterfacesModel( LibraryInterface* libHandler, 
									   QSharedPointer<Component> component,
									   QObject *parent ):
QAbstractTableModel(parent),
libHandler_(libHandler),
component_(component),
busifs_(component->getBusInterfaces())
{

	Q_ASSERT(libHandler_);
	Q_ASSERT(component_);
}

//-----------------------------------------------------------------------------
// Function: ~BusInterfacesModel()
//-----------------------------------------------------------------------------
BusInterfacesModel::~BusInterfacesModel() {
}

//-----------------------------------------------------------------------------
// Function: rowCount()
//-----------------------------------------------------------------------------
int BusInterfacesModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return busifs_.size();
}

//-----------------------------------------------------------------------------
// Function: columnCount()
//-----------------------------------------------------------------------------
int BusInterfacesModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return BusInterfacesDelegate::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags BusInterfacesModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	// bus def and abs def can not be edited but can have data dropped on them
	else if (BusInterfacesDelegate::BUSDEF_COLUMN == index.column() ||
		BusInterfacesDelegate::ABSDEF_COLUMN == index.column()) {
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: headerData()
//-----------------------------------------------------------------------------
QVariant BusInterfacesModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case BusInterfacesDelegate::NAME_COLUMN: {
				return tr("Name");
												}
			case BusInterfacesDelegate::BUSDEF_COLUMN: {
				return tr("Bus\nDefinition");
														}
			case BusInterfacesDelegate::ABSDEF_COLUMN: {
				return tr("Abstraction\ndefinition");
													 }
			case BusInterfacesDelegate::IF_MODE_COLUMN: {
				return tr("Interface\nmode");
													   }
			case BusInterfacesDelegate::DESCRIPTION_COLUMN: {
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

//-----------------------------------------------------------------------------
// Function: data()
//-----------------------------------------------------------------------------
QVariant BusInterfacesModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= busifs_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case BusInterfacesDelegate::NAME_COLUMN: {
				return busifs_.at(index.row())->getName();
												}
			case BusInterfacesDelegate::BUSDEF_COLUMN: {
				return busifs_.at(index.row())->getBusType().toString(":");
														}
			case BusInterfacesDelegate::ABSDEF_COLUMN: {
				return busifs_.at(index.row())->getAbstractionType().toString(":");
													 }
			case BusInterfacesDelegate::IF_MODE_COLUMN: {
				General::InterfaceMode mode = busifs_.at(index.row())->getInterfaceMode();
				return General::interfaceMode2Str(mode);
													   }
			case BusInterfacesDelegate::DESCRIPTION_COLUMN: {
				return busifs_.at(index.row())->getDescription();
															}
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		// list of component's physical ports and their bounds is needed for
		// bus interface to check the port maps
		QList<General::PortBounds> physPorts = component_->getPortBounds();

		if (busifs_.at(index.row())->isValid(physPorts)) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case BusInterfacesDelegate::NAME_COLUMN:
			case BusInterfacesDelegate::BUSDEF_COLUMN:
			case BusInterfacesDelegate::IF_MODE_COLUMN: {
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

//-----------------------------------------------------------------------------
// Function: setData()
//-----------------------------------------------------------------------------
bool BusInterfacesModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= busifs_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case BusInterfacesDelegate::NAME_COLUMN: {
				busifs_[index.row()]->setName(value.toString());
				break;
												}
			case BusInterfacesDelegate::BUSDEF_COLUMN: {
				VLNV busType = VLNV(VLNV::BUSDEFINITION, value.toString(), ":");
				busifs_[index.row()]->setBusType(busType);
				break;
														}
			case BusInterfacesDelegate::ABSDEF_COLUMN: {
				VLNV absType = VLNV(VLNV::ABSTRACTIONDEFINITION, value.toString(), ":");
				busifs_[index.row()]->setAbstractionType(absType);
				break;
													 }
			case BusInterfacesDelegate::IF_MODE_COLUMN: {
				QString modeStr = value.toString();
				General::InterfaceMode mode = General::str2Interfacemode(modeStr, General::MASTER);
				busifs_[index.row()]->setInterfaceMode(mode);
				break;
														}
			case BusInterfacesDelegate::DESCRIPTION_COLUMN: {
				busifs_[index.row()]->setDescription(value.toString());
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

//-----------------------------------------------------------------------------
// Function: supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions BusInterfacesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: mimeTypes()
//-----------------------------------------------------------------------------
QStringList BusInterfacesModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: dropMimeData()
//-----------------------------------------------------------------------------
bool BusInterfacesModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, 
    const QModelIndex &parent)
{
    if ( action == Qt::IgnoreAction)
    {
        return true;
    }

    // Dropped data must be directly on parent.
    if ( row != -1 || column != -1 || !parent.isValid() )
    {
        return false;
    }

    if ( row > rowCount() )
    {
        return false;
    }

    QVariant variant = data->imageData();

    if ( !variant.canConvert<VLNV>())
    {
        return false;
    }

    VLNV vlnv = variant.value<VLNV>();

    if ( parent.column() == BusInterfacesDelegate::BUSDEF_COLUMN)
    {
        if ( vlnv.getType() != VLNV::BUSDEFINITION )
        {
            return false;
        }

        setData(index(parent.row(),parent.column()),vlnv.toString(":"));

        // If only one possible absDef, set it automatically.
        QList<VLNV> absDefVLNVs;
        if (libHandler_->getChildren(absDefVLNVs, vlnv) == 1) 
        {
            setData(index(parent.row(),BusInterfacesDelegate::ABSDEF_COLUMN), absDefVLNVs.first().toString(":"));
        }

        emit contentChanged();

    }

    else if ( parent.column() == BusInterfacesDelegate::ABSDEF_COLUMN )
    {
        if ( vlnv.getType() != VLNV::ABSTRACTIONDEFINITION ) {
            return false;
        }

        setData(index(parent.row(),parent.column()),vlnv.toString(":"));
        emit contentChanged();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: onAddItem()
//-----------------------------------------------------------------------------
void BusInterfacesModel::onAddItem( const QModelIndex& index ) {
	int row = busifs_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	busifs_.insert(row, QSharedPointer<BusInterface>(new BusInterface()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit busifAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: onRemoveItem()
//-----------------------------------------------------------------------------
void BusInterfacesModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= busifs_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	busifs_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit busifRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::onMoveItem()
//-----------------------------------------------------------------------------
void BusInterfacesModel::onMoveItem( const QModelIndex& originalPos, const QModelIndex& newPos ) {

    // if there was no item in the starting point
    if (!originalPos.isValid()) {
        return;
    }
    // if the indexes are the same
    else if (originalPos == newPos) {
        return;
    }
    else if (originalPos.row() < 0 || originalPos.row() >= busifs_.size()) {
        return;
    }

    int source = originalPos.row();
    int target = 0;

    // if the new position is invalid index then put the item last in the table
    if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= busifs_.size()) {

        beginResetModel();
        QSharedPointer<BusInterface> busIf = busifs_.takeAt(originalPos.row());
        busifs_.append(busIf);
        target = busifs_.size() - 1;
        endResetModel();
    }
    // if both indexes were valid
    else {
        beginResetModel();
        busifs_.swap(originalPos.row(), newPos.row());
        target = newPos.row();
        endResetModel();
    }

    emit busIfMoved(source, target);

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function:  BusInterfacesModel::isValid()
//-----------------------------------------------------------------------------
bool BusInterfacesModel::isValid() const {
	// list of component's physical ports and their bounds is needed for
	// bus interface to check the port maps
	QList<General::PortBounds> physPorts = component_->getPortBounds();

	// if at least one address space is invalid
	foreach (QSharedPointer<BusInterface> busif, busifs_) {
		if (!busif->isValid(physPorts)) {
			return false;
		}
	}
	// all address spaces were valid
	return true;
}
/* 
 *  	Created on: 28.6.2012
 *      Author: Antti Kamppi
 * 		filename: cominterfacesmodel.cpp
 *		Project: Kactus 2
 */

#include "cominterfacesmodel.h"
#include "cominterfacesdelegate.h"
#include <IPXACTmodels/generaldeclarations.h>
#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/ComDefinition.h>

#include <QColor>
#include <QMimeData>

ComInterfacesModel::ComInterfacesModel(LibraryInterface* libHandler,
									   QSharedPointer<Component> component,
									   QObject *parent):
QAbstractTableModel(parent),
libHandler_(libHandler),
comIfs_(component->getComInterfaces()) {

	Q_ASSERT(component);
	Q_ASSERT(libHandler_);
}

ComInterfacesModel::~ComInterfacesModel() {
}

int ComInterfacesModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return comIfs_.size();
}

int ComInterfacesModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return ComInterfacesDelegate::COLUMN_COUNT;
}

Qt::ItemFlags ComInterfacesModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	// Com definition column can only be dropped..
	if (ComInterfacesDelegate::COM_DEF_COLUMN == index.column()) {
		return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant ComInterfacesModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case ComInterfacesDelegate::NAME_COLUMN: {
				return tr("Name");
												}
			case ComInterfacesDelegate::COM_DEF_COLUMN: {
				return tr("COM definition");
														}
			case ComInterfacesDelegate::TRANSFER_TYPE_COLUMN: {
				return tr("Transfer type");
													 }
			case ComInterfacesDelegate::DIRECTION_COLUMN: {
				return tr("Direction");
													   }
			case ComInterfacesDelegate::DESCRIPTION_COLUMN: {
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

QVariant ComInterfacesModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= comIfs_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case ComInterfacesDelegate::NAME_COLUMN: {
				return comIfs_.at(index.row())->getName();
												}
			case ComInterfacesDelegate::COM_DEF_COLUMN: {
				return comIfs_.at(index.row())->getComType().toString(":");
														}
			case ComInterfacesDelegate::TRANSFER_TYPE_COLUMN: {
				return comIfs_.at(index.row())->getTransferType();
													 }
			case ComInterfacesDelegate::DIRECTION_COLUMN: {
				return General::direction2Str(comIfs_.at(index.row())->getDirection());
													   }
			case ComInterfacesDelegate::DESCRIPTION_COLUMN: {
				return comIfs_.at(index.row())->getDescription();
															}
			default: {
				return QVariant();
					 }
		}
	}
	else if (ComInterfacesDelegate::TRANSFER_TYPE_OPTIONS == role) {
		// find the vlnv of the com interface
		VLNV comDefVLNV = comIfs_.at(index.row())->getComType();
		
		// if the com def is not defined
		if (!comDefVLNV.isValid()) {
			return QStringList();
		}
		// if the com def does not exist in the library.
		else if (!libHandler_->contains(comDefVLNV)) {
			return QStringList();
		}
		// if the object is not a com definition
		else if (libHandler_->getDocumentType(comDefVLNV) != VLNV::COMDEFINITION) {
			return QStringList();
		}

		// parse the com definition
		QSharedPointer<LibraryComponent const> libComp = libHandler_->getModelReadOnly(comDefVLNV);
		const QSharedPointer<ComDefinition const> comDef = libComp.staticCast<ComDefinition const>();
		
		// and return the transfer types specified in the com definition
		return comDef->getTransferTypes();
	}
	else if (Qt::ForegroundRole == role) {

		if (comIfs_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case ComInterfacesDelegate::NAME_COLUMN:
			case ComInterfacesDelegate::DIRECTION_COLUMN: {
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

bool ComInterfacesModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= comIfs_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case ComInterfacesDelegate::NAME_COLUMN: {
				comIfs_[index.row()]->setName(value.toString());
				break;
												}
			case ComInterfacesDelegate::COM_DEF_COLUMN: {
				VLNV comDefVLNV = VLNV(VLNV::COMDEFINITION, value.toString(), ":");
				// parse the com definition
				QSharedPointer<LibraryComponent const> libComp = libHandler_->getModelReadOnly(comDefVLNV);
				const QSharedPointer<ComDefinition const> comDef = libComp.staticCast<ComDefinition const>();
				comIfs_[index.row()]->setComType(comDefVLNV,&comDef->getProperties());
				break;
														}
			case ComInterfacesDelegate::TRANSFER_TYPE_COLUMN: {
				comIfs_[index.row()]->setTransferType(value.toString());
				break;
													 }
			case ComInterfacesDelegate::DIRECTION_COLUMN: {
				comIfs_[index.row()]->setDirection(General::str2Direction(value.toString(), General::IN));
				break;
													   }
			case ComInterfacesDelegate::DESCRIPTION_COLUMN: {
				comIfs_[index.row()]->setDescription(value.toString());
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

bool ComInterfacesModel::isValid() const {
	foreach (QSharedPointer<ComInterface> comIf, comIfs_) {
		if (!comIf->isValid()) {
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions ComInterfacesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: mimeTypes()
//-----------------------------------------------------------------------------
QStringList ComInterfacesModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: dropMimeData()
//-----------------------------------------------------------------------------
bool ComInterfacesModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, 
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

    if ( parent.column() == ComInterfacesDelegate::COM_DEF_COLUMN)
    {
        if ( vlnv.getType() != VLNV::COMDEFINITION )
        {
            return false;
        }

        setData(index(parent.row(),parent.column()),vlnv.toString(":"));
        emit contentChanged();
    }

    return true;
}

void ComInterfacesModel::onAddItem( const QModelIndex& index ) {
	int row = comIfs_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	comIfs_.insert(row, QSharedPointer<ComInterface>(new ComInterface()));
	endInsertRows();

	// inform navigation tree that COM interface is added
	emit comIfAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ComInterfacesModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= comIfs_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	comIfs_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that COM interface has been removed
	emit comIfRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

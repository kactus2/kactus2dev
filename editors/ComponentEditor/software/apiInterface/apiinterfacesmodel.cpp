/* 
 *  	Created on: 27.6.2012
 *      Author: Antti Kamppi
 * 		filename: apiinterfacesmodel.cpp
 *		Project: Kactus 2
 */

#include "apiinterfacesmodel.h"
#include "apiinterfacesdelegate.h"
#include <IPXACTmodels/ApiInterface.h>
#include <IPXACTmodels/vlnv.h>

#include <QColor>
#include <QMimeData>

ApiInterfacesModel::ApiInterfacesModel(QSharedPointer<Component> component,
									   QObject *parent):
QAbstractTableModel(parent),
apis_(component->getApiInterfaces()) {

	Q_ASSERT(component);
}

ApiInterfacesModel::~ApiInterfacesModel() {
}

int ApiInterfacesModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return apis_.size();
}

int ApiInterfacesModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return ApiInterfacesDelegate::COLUMN_COUNT;
}

Qt::ItemFlags ApiInterfacesModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	// API definition can only be dropped.
	else if (ApiInterfacesDelegate::API_DEF_COLUMN == index.column()) {
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant ApiInterfacesModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case ApiInterfacesDelegate::NAME_COLUMN: {
				return tr("Name");
													 }
			case ApiInterfacesDelegate::API_DEF_COLUMN: {
				return tr("API definition");
													   }
			case ApiInterfacesDelegate::DEPENDENCY_COLUMN: {
				return tr("Dependency");
													   }
			case ApiInterfacesDelegate::DISPLAY_NAME_COLUMN: {
				return tr("Display name");
														}
			case ApiInterfacesDelegate::DESCRIPTION_COLUMN: {
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

QVariant ApiInterfacesModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= apis_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case ApiInterfacesDelegate::NAME_COLUMN: {
				return apis_.at(index.row())->getName();
													 }
			case ApiInterfacesDelegate::API_DEF_COLUMN: {
				return apis_.at(index.row())->getApiType().toString(":");
													   }
			case ApiInterfacesDelegate::DEPENDENCY_COLUMN: {
				return dependencyDirection2Str(apis_.at(index.row())->getDependencyDirection());
													   }
			case ApiInterfacesDelegate::DISPLAY_NAME_COLUMN: {
				return apis_.at(index.row())->getDisplayName();
														}
			case ApiInterfacesDelegate::DESCRIPTION_COLUMN: {
				return apis_.at(index.row())->getDescription();
															}
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		if (apis_.at(index.row())->isValid()) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case ApiInterfacesDelegate::NAME_COLUMN: {
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

bool ApiInterfacesModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= apis_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case ApiInterfacesDelegate::NAME_COLUMN: {
				apis_[index.row()]->setName(value.toString());
				break;
													 }
			case ApiInterfacesDelegate::API_DEF_COLUMN: {
				VLNV apiDef = VLNV(VLNV::APIDEFINITION, value.toString(), ":");
				apis_[index.row()]->setApiType(apiDef);
				break;
													   }
			case ApiInterfacesDelegate::DEPENDENCY_COLUMN: {
				QString text = value.toString();
				apis_[index.row()]->setDependencyDirection(str2DependencyDirection(text, DEPENDENCY_REQUESTER));
				break;
													   }
			case ApiInterfacesDelegate::DISPLAY_NAME_COLUMN: {
				apis_[index.row()]->setDisplayName(value.toString());
				break;
														}
			case ApiInterfacesDelegate::DESCRIPTION_COLUMN: {
				apis_[index.row()]->setDescription(value.toString());
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

bool ApiInterfacesModel::isValid() const {
	
	foreach (QSharedPointer<ApiInterface> apiIf, apis_) {
		if (!apiIf->isValid()) {
			return false;
		}
	}

	// all api interfaces were valid
	return true;
}

//-----------------------------------------------------------------------------
// Function: supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions ApiInterfacesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: mimeTypes()
//-----------------------------------------------------------------------------
QStringList ApiInterfacesModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: dropMimeData()
//-----------------------------------------------------------------------------
bool ApiInterfacesModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, 
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

    if ( parent.column() == ApiInterfacesDelegate::API_DEF_COLUMN )
    {
        if ( vlnv.getType() != VLNV::APIDEFINITION )
        {
            return false;
        }

        setData(index(parent.row(),parent.column()),vlnv.toString(":"));
        emit contentChanged();
    }

    return true;
}

void ApiInterfacesModel::onAddItem( const QModelIndex& index ) {
	int row = apis_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	apis_.insert(row, QSharedPointer<ApiInterface>(new ApiInterface()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit apiAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void ApiInterfacesModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= apis_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	apis_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit apiRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

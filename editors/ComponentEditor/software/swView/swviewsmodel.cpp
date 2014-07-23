/* 
 *  	Created on: 27.6.2012
 *      Author: Antti Kamppi
 * 		filename: swviewsmodel.cpp
 *		Project: Kactus 2
 */

#include "swviewsmodel.h"
#include "swviewsdelegate.h"
#include <IPXACTmodels/vlnv.h>
#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>

#include <QColor>
#include <QMimeData>

SWViewsModel::SWViewsModel(LibraryInterface* libHandler,
                           QSharedPointer<Component> component,
						   QObject *parent):
QAbstractTableModel(parent),
views_(component->getSWViews()),
libHandler_(libHandler),
component_(component) {

    Q_ASSERT(libHandler_);
	Q_ASSERT(component_);
}

SWViewsModel::~SWViewsModel() {

}

int SWViewsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return views_.size();
}

int SWViewsModel::columnCount( const QModelIndex& parent /*= QModelIndex()*/ ) const {
	if (parent.isValid()) {
		return 0;
	}
	return SWViewsDelegate::COLUMN_COUNT;
}

Qt::ItemFlags SWViewsModel::flags( const QModelIndex& index ) const {
	if (!index.isValid()) {
		return Qt::NoItemFlags;
	}

	// hierarchy reference can only be dropped
	else if (SWViewsDelegate::HIER_REF_COLUMN == index.column()) {
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	}
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant SWViewsModel::headerData( int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/ ) const {
	if (orientation != Qt::Horizontal) {
		return QVariant();
	}
	if (Qt::DisplayRole == role) {

		switch (section) {
			case SWViewsDelegate::NAME_COLUMN: {
				return tr("Name");
													 }
			case SWViewsDelegate::HIER_REF_COLUMN: {
				return tr("Hierarchy reference");
													   }
			case SWViewsDelegate::DISPLAY_NAME_COLUMN: {
				return tr("Display name");
													   }
			case SWViewsDelegate::DESCRIPTION_COLUMN: {
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

QVariant SWViewsModel::data( const QModelIndex& index, int role /*= Qt::DisplayRole*/ ) const {
	if (!index.isValid()) {
		return QVariant();
	}
	else if (index.row() < 0 || index.row() >= views_.size()) {
		return QVariant();
	}

	if (Qt::DisplayRole == role) {

		switch (index.column()) {
			case SWViewsDelegate::NAME_COLUMN: {
				return views_.at(index.row())->getName();
													 }
			case SWViewsDelegate::HIER_REF_COLUMN: {
				return views_.at(index.row())->getHierarchyRef().toString(":");
													   }
			case SWViewsDelegate::DISPLAY_NAME_COLUMN: {
				return views_.at(index.row())->getDisplayName();
													   }
			case SWViewsDelegate::DESCRIPTION_COLUMN: {
				return views_.at(index.row())->getDescription();
														}
			default: {
				return QVariant();
					 }
		}
	}
	else if (Qt::ForegroundRole == role) {

		QStringList fileSetNames = component_->getFileSetNames();
		QStringList cpuNames = component_->getCpuNames();

		if (views_.at(index.row())->isValid(fileSetNames, cpuNames)) {
			return QColor("black");
		}
		else {
			return QColor("red");
		}
	}
	else if (Qt::BackgroundRole == role) {
		switch (index.column()) {
			case SWViewsDelegate::NAME_COLUMN:
			case SWViewsDelegate::HIER_REF_COLUMN: {
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

bool SWViewsModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ ) {
	if (!index.isValid()) {
		return false;
	}
	else if (index.row() < 0 || index.row() >= views_.size()) {
		return false;
	}

	if (Qt::EditRole == role) {

		switch (index.column()) {
			case SWViewsDelegate::NAME_COLUMN: {
				views_[index.row()]->setName(value.toString());
				break;
													 }
			case SWViewsDelegate::HIER_REF_COLUMN: {
				VLNV hierRef = VLNV(VLNV::DESIGNCONFIGURATION, value.toString(), ":");
				views_[index.row()]->setHierarchyRef(hierRef);
				break;
													   }
			case SWViewsDelegate::DISPLAY_NAME_COLUMN: {
				views_[index.row()]->setDisplayName(value.toString());
				break;
													   }
			case SWViewsDelegate::DESCRIPTION_COLUMN: {
				views_[index.row()]->setDescription(value.toString());
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

bool SWViewsModel::isValid() const {
	QStringList fileSetNames = component_->getFileSetNames();
	QStringList cpuNames = component_->getCpuNames();

	// check that each software view is valid
	foreach (QSharedPointer<SWView> swView, views_) {
		if (!swView->isValid(fileSetNames, cpuNames)) {
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions SWViewsModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: mimeTypes()
//-----------------------------------------------------------------------------
QStringList SWViewsModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: dropMimeData()
//-----------------------------------------------------------------------------
bool SWViewsModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, 
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

    if ( parent.column() == SWViewsDelegate::HIER_REF_COLUMN )
    {
        if (vlnv.getType() == VLNV::DESIGN )
        {
            // Determine the component type which must be SW.
            QSharedPointer<LibraryComponent const> libComp = libHandler_->getModelReadOnly(vlnv);
            QSharedPointer<Design const> design = libComp.staticCast<Design const>();

            if (design && design->getDesignImplementation() == KactusAttribute::KTS_SW)
            {
                setData(index(parent.row(),parent.column()),vlnv.toString(":"));
                emit contentChanged();
            }
        }
        else if (vlnv.getType() == VLNV::DESIGNCONFIGURATION)
        {
            // Determine the component type which must be SW.
            QSharedPointer<LibraryComponent const> libComp = libHandler_->getModelReadOnly(vlnv);
            QSharedPointer<DesignConfiguration const> designConf = libComp.staticCast<DesignConfiguration const>();

            if (designConf && designConf->getDesignConfigImplementation() == KactusAttribute::KTS_SW)
            {
                setData(index(parent.row(),parent.column()),vlnv.toString(":"));
                emit contentChanged();
            }
        }       
    }
   
    return true;
}

void SWViewsModel::onAddItem( const QModelIndex& index ) {
	int row = views_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid()) {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	views_.insert(row, QSharedPointer<SWView>(new SWView()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit viewAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

void SWViewsModel::onRemoveItem( const QModelIndex& index ) {
	// don't remove anything if index is invalid
	if (!index.isValid()) {
		return;
	}
	// make sure the row number if valid
	else if (index.row() < 0 || index.row() >= views_.size()) {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	views_.removeAt(index.row());
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit viewRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

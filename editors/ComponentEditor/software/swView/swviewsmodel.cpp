//-----------------------------------------------------------------------------
// File: swviewsmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.06.2012
//
// Description:
// The model to manage the software views for a table view.
//-----------------------------------------------------------------------------

#include "swviewsmodel.h"

#include "swviewsdelegate.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/kactusExtensions/SWView.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QColor>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: SWViewsModel::SWViewsModel()
//-----------------------------------------------------------------------------
SWViewsModel::SWViewsModel(LibraryInterface* libHandler, QSharedPointer<Component> component, QObject* parent):
QAbstractTableModel(parent),
    swViews_(component->getSWViews()),
    libHandler_(libHandler),
    component_(component)
{
    Q_ASSERT(libHandler_);
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::~SWViewsModel()
//-----------------------------------------------------------------------------
SWViewsModel::~SWViewsModel()
{

}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::rowCount()
//-----------------------------------------------------------------------------
int SWViewsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return swViews_.size();
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::columnCount()
//-----------------------------------------------------------------------------
int SWViewsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return SWViewsDelegate::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags SWViewsModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }
    // hierarchy reference can only be dropped
    else if (SWViewsDelegate::HIER_REF_COLUMN == index.column())
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
    }

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::headerData()
//-----------------------------------------------------------------------------
QVariant SWViewsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if (section == SWViewsDelegate::NAME_COLUMN)
    {
        return tr("Name");
    }
    else if (section == SWViewsDelegate::HIER_REF_COLUMN)
    {
        return tr("Hierarchy reference");
    }
    else if (section == SWViewsDelegate::DISPLAY_NAME_COLUMN)
    {
        return tr("Display name");
    }
    else if (section == SWViewsDelegate::DESCRIPTION_COLUMN)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::data()
//-----------------------------------------------------------------------------
QVariant SWViewsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= swViews_.size())
    {
		return QVariant();
	}
	
	if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == SWViewsDelegate::NAME_COLUMN)
        {
            return swViews_.at(index.row())->name();
        }
        else if (index.column() == SWViewsDelegate::HIER_REF_COLUMN)
        {
            return swViews_.at(index.row())->getHierarchyRef().toString(":");
        }
        else if (index.column() == SWViewsDelegate::DISPLAY_NAME_COLUMN)
        {
            return swViews_.at(index.row())->displayName();
        }
        else if (index.column() == SWViewsDelegate::DESCRIPTION_COLUMN)
        {
            return swViews_.at(index.row())->description();
        }
        else
        {
            return QVariant();
        }
	}

	else if (role == Qt::ForegroundRole)
    {
		QStringList fileSetNames = component_->getFileSetNames();
		QStringList cpuNames = component_->getCpuNames();

		if (swViews_.at(index.row())->isValid(fileSetNames, cpuNames))
        {
			return QColor("black");
		}
		else
        {
			return QColor("red");
		}
	}

	else if (role == Qt::BackgroundRole)
    {
		if (index.column() == SWViewsDelegate::NAME_COLUMN || index.column() == SWViewsDelegate::HIER_REF_COLUMN)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
    }
    else
    {
        return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::setData()
//-----------------------------------------------------------------------------
bool SWViewsModel::setData( QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= swViews_.size())
    {
		return false;
	}
	
	if (role == Qt::EditRole)
    {
        if (index.column() == SWViewsDelegate::NAME_COLUMN)
        {
            swViews_.at(index.row())->setName(value.toString());				
        }
        else if (index.column() == SWViewsDelegate::HIER_REF_COLUMN)
        {
            VLNV hierRef = VLNV(VLNV::DESIGNCONFIGURATION, value.toString(), ":");
            swViews_.at(index.row())->setHierarchyRef(hierRef);
        }
        else if (index.column() == SWViewsDelegate::DISPLAY_NAME_COLUMN)
        {
            swViews_.at(index.row())->setDisplayName(value.toString());
        }
        else if (index.column() == SWViewsDelegate::DESCRIPTION_COLUMN)
        {
            swViews_.at(index.row())->setDescription(value.toString());
        }
        else
        {
            return false;
        }

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}
	else
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::isValid()
//-----------------------------------------------------------------------------
bool SWViewsModel::isValid() const
{
	QStringList fileSetNames = component_->getFileSetNames();
	QStringList cpuNames = component_->getCpuNames();

	// check that each software view is valid
	foreach (QSharedPointer<SWView> swView, swViews_)
    {
		if (!swView->isValid(fileSetNames, cpuNames))
        {
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions SWViewsModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList SWViewsModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::dropMimeData()
//-----------------------------------------------------------------------------
bool SWViewsModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, 
    QModelIndex const& parent)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    // Dropped data must be directly on parent.
    if (row != -1 || column != -1 || !parent.isValid())
    {
        return false;
    }

    if ( row > rowCount() )
    {
        return false;
    }

    QVariant variant = data->imageData();

    if (!variant.canConvert<VLNV>())
    {
        return false;
    }

    VLNV vlnv = variant.value<VLNV>();

    if (parent.column() == SWViewsDelegate::HIER_REF_COLUMN)
    {
        if (vlnv.getType() == VLNV::DESIGN )
        {
            // Determine the component type which must be SW.
            QSharedPointer<Document const> libComp = libHandler_->getModelReadOnly(vlnv);
            QSharedPointer<Design const> design = libComp.staticCast<Design const>();

            if (design && design->getImplementation() == KactusAttribute::SW)
            {
                setData(index(parent.row(), parent.column()), vlnv.toString(":"));
                emit contentChanged();
            }
        }
        else if (vlnv.getType() == VLNV::DESIGNCONFIGURATION)
        {
            // Determine the component type which must be SW.
            QSharedPointer<Document const> libComp = libHandler_->getModelReadOnly(vlnv);
            QSharedPointer<DesignConfiguration const> designConf = libComp.staticCast<DesignConfiguration const>();

            if (designConf && designConf->getImplementation() == KactusAttribute::SW)
            {
                setData(index(parent.row(), parent.column()), vlnv.toString(":"));
                emit contentChanged();
            }
        }       
    }
   
    return true;
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::onAddItem()
//-----------------------------------------------------------------------------
void SWViewsModel::onAddItem(QModelIndex const& index)
{
	int row = swViews_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	swViews_.insert(row, QSharedPointer<SWView>(new SWView()));
    component_->setSWViews(swViews_);
	endInsertRows();

	// inform navigation tree that file set is added
	emit viewAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SWViewsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void SWViewsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= swViews_.size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	swViews_.removeAt(index.row());
    component_->setSWViews(swViews_);
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit viewRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

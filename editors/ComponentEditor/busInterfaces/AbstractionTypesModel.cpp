//-----------------------------------------------------------------------------
// File: AbstractionTypesModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.01.2018
//
// Description:
// Table model for bus interface abstraction types.
//-----------------------------------------------------------------------------

#include "AbstractionTypesModel.h"

#include <common/KactusColors.h>

#include <editors/ComponentEditor/busInterfaces/AbstractionTypesConstants.h>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/AbstractionType.h>

#include <QFont>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::AbstractionTypesModel()
//-----------------------------------------------------------------------------
AbstractionTypesModel::AbstractionTypesModel(QObject* parent):
QAbstractTableModel(parent),
abstractions_(new QList<QSharedPointer<AbstractionType> > ())
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::~AbstractionTypesModel()
//-----------------------------------------------------------------------------
AbstractionTypesModel::~AbstractionTypesModel()
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::rowCount()
//-----------------------------------------------------------------------------
int AbstractionTypesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return abstractions_->count();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::columnCount()
//-----------------------------------------------------------------------------
int AbstractionTypesModel::columnCount(const QModelIndex& /*parent*/) const
{
    return AbstractionTypesConstants::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::headerData()
//-----------------------------------------------------------------------------
QVariant AbstractionTypesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (section == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
        {
            return tr("Abstraction definition");
        }
        else if (section == AbstractionTypesConstants::VIEWREFERENCES)
        {
            return tr("View references");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::data()
//-----------------------------------------------------------------------------
QVariant AbstractionTypesModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= abstractions_->size())
    {
		return QVariant();
	}

    QSharedPointer<AbstractionType> indexedAbstraction = abstractions_->at(index.row());

    if (indexedAbstraction)
    {
        if (role == AbstractionTypesConstants::ABSTRACTIONVLNVROLE && indexedAbstraction->getAbstractionRef())
        {
            VLNV abstractionVLNV = *indexedAbstraction->getAbstractionRef().data();
            return QVariant::fromValue(abstractionVLNV);
        }
        else if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
        {
            if (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
            {
                QString abstractionString("");
                if (indexedAbstraction->getAbstractionRef())
                {
                    abstractionString = indexedAbstraction->getAbstractionRef()->toString();
                }

                return abstractionString;
            }
            else if (index.column() == AbstractionTypesConstants::VIEWREFERENCES)
            {
                return indexedAbstraction->getViewReferences()->join(AbstractionTypesConstants::VIEW_SEPARATOR);
            }
        }
        else if (Qt::ForegroundRole == role)
        {
//             return blackForValidRedForInvalid(index, typeDefinition);
        }
        else if (role == Qt::BackgroundRole)
        {
            if (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
            {
                return KactusColors::MANDATORY_FIELD;
            }
            else
            {
                return KactusColors::REGULAR_FIELD;
            }
        }
        else if (!index.parent().isValid() && role == Qt::FontRole)
        {
            QFont font;
            font.setBold(true);
            return font;
        }
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::setData()
//-----------------------------------------------------------------------------
bool AbstractionTypesModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0)
    {
		return false;
    }
    else if (data(index, Qt::DisplayRole) == value)
    {
        return true;
    }

    QSharedPointer<AbstractionType> indexedAbstraction = abstractions_->at(index.row());

	if (role == Qt::EditRole)
    {
        if (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION && value.canConvert<VLNV> ())
        {
            VLNV newAbstractionReference = value.value<VLNV>();

            if (!indexedAbstraction->getAbstractionRef())
            {
                QSharedPointer<ConfigurableVLNVReference> abstractionReference(new ConfigurableVLNVReference());
                indexedAbstraction->setAbstractionRef(abstractionReference);
            }

            indexedAbstraction->getAbstractionRef()->setVLNV(newAbstractionReference);
        }

        else if (index.column() == AbstractionTypesConstants::VIEWREFERENCES)
        {
            QSharedPointer<QStringList> newViews(new QStringList(value.toStringList()));
            indexedAbstraction->setViewReferences(newViews);
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::onAddItem()
//-----------------------------------------------------------------------------
void AbstractionTypesModel::onAddItem(QModelIndex const& index)
{
    int row = abstractions_->size();

    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);
    abstractions_->insert(row, QSharedPointer<AbstractionType>(new AbstractionType()));
    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AbstractionTypesModel::onRemoveItem(QModelIndex const& index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= abstractions_->size())
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    abstractions_->removeAt(index.row());
    endRemoveRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::onChangeSelectedBusInterface()
//-----------------------------------------------------------------------------
void AbstractionTypesModel::onChangeSelectedBusInterface(QSharedPointer<BusInterface> newBus)
{
    beginResetModel();

    abstractions_ = newBus->getAbstractionTypes();

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags AbstractionTypesModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return Qt::ItemIsDropEnabled;
    }
    else if (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
    }

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::getFirstAbstraction()
//-----------------------------------------------------------------------------
QSharedPointer<ConfigurableVLNVReference> AbstractionTypesModel::getFirstAbstraction() const
{
    QSharedPointer<ConfigurableVLNVReference> currentVLNV;
    if (!abstractions_->isEmpty() && abstractions_->first()->getAbstractionRef())
    {
        currentVLNV = abstractions_->first()->getAbstractionRef();
    }

    return currentVLNV;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions AbstractionTypesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList AbstractionTypesModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::dropMimeData()
//-----------------------------------------------------------------------------
bool AbstractionTypesModel::dropMimeData(QMimeData const* data, Qt::DropAction action, int row, int column, 
    QModelIndex const& parent)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }
    if (row != -1 || column != -1)
    {
        return false;
    }

    QVariant variant = data->imageData();
    if (!variant.canConvert<VLNV>())
    {
        return false;
    }

    VLNV vlnv = variant.value<VLNV>();
    if (vlnv.getType() != VLNV::ABSTRACTIONDEFINITION)
    {
        return false;
    }

    QModelIndex abstractionIndex = parent;
    if (!parent.isValid())
    {
        onAddItem(QModelIndex());

        abstractionIndex = index(abstractions_->count() - 1, AbstractionTypesConstants::ABSTRACTIONDEFINITION);
    }

    if (abstractionIndex.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
    {
        setData(abstractionIndex, variant);
        emit contentChanged();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::addNewAbstractionTypeWithVLNV()
//-----------------------------------------------------------------------------
void AbstractionTypesModel::addNewAbstractionTypeWithVLNV(VLNV const& newAbstractionVLNV)
{
    onAddItem(QModelIndex());
    QSharedPointer<AbstractionType> newAbstraction = abstractions_->last();

    if (!newAbstraction->getAbstractionRef())
    {
        QSharedPointer<ConfigurableVLNVReference> newReference(new ConfigurableVLNVReference());
        newAbstraction->setAbstractionRef(newReference);
    }

    newAbstraction->getAbstractionRef()->setVLNV(newAbstractionVLNV);
}

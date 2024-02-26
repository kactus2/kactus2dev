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
#include <KactusAPI/include/AbstractionTypeInterface.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/common/DocumentUtils.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/AbstractionType.h>
#include <IPXACTmodels/Component/validators/AbstractionTypeValidator.h>

#include <QFont>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::AbstractionTypesModel()
//-----------------------------------------------------------------------------
AbstractionTypesModel::AbstractionTypesModel(AbstractionTypeInterface* abstractionInterface, 
    QSharedPointer<Component> component, LibraryInterface* library, QObject* parent) :
QAbstractTableModel(parent),
abstractionInterface_(abstractionInterface),
containingComponent_(component),
library_(library)
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

    return abstractionInterface_->itemCount();
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
    if (!index.isValid() || index.row() < 0 || index.row() >= abstractionInterface_->itemCount())
    {
		return QVariant();
	}

    bool rowHasAbstraction = abstractionInterface_->hasAbstractionReference(index.row());

    if (role == AbstractionTypesConstants::ABSTRACTIONVLNVROLE && rowHasAbstraction)
    {
        VLNV abstractionVLNV = *abstractionInterface_->getAbstractionReference(index.row()).data();
        return QVariant::fromValue(abstractionVLNV);
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
    {
        if (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
        {
            QString abstractionString("");
            if (rowHasAbstraction)
            {
                abstractionString =
                    QString::fromStdString(abstractionInterface_->getIndexedAbstraction(index.row()));
            }

            return abstractionString;
        }
        else if (index.column() == AbstractionTypesConstants::VIEWREFERENCES)
        {
            return QString::fromStdString(abstractionInterface_->getCombinedViews(index.row()));
        }
    }
    else if (Qt::ForegroundRole == role)
    {
        return blackForValidRedForInvalid(index);
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

	if (role == Qt::EditRole)
    {
        if (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION && value.canConvert<VLNV> ())
        {
            VLNV newAbstractionReference = value.value<VLNV>();

            std::string referenceVendor = newAbstractionReference.getVendor().toStdString();
            std::string referenceLibrary = newAbstractionReference.getLibrary().toStdString();
            std::string referenceName = newAbstractionReference.getName().toStdString();
            std::string referenceVersion = newAbstractionReference.getVersion().toStdString();

            abstractionInterface_->setAbstraction(
                index.row(), referenceVendor, referenceLibrary, referenceName, referenceVersion);
        }

        else if (index.column() == AbstractionTypesConstants::VIEWREFERENCES)
        {
            std::vector<std::string> newViews;
            for (auto view : value.toStringList())
            {
                newViews.push_back(view.toStdString());
            }

            abstractionInterface_->setViewReferences(index.row(), newViews);
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::setVLNVForAbstraction()
//-----------------------------------------------------------------------------
void AbstractionTypesModel::setVLNVForAbstraction(int const& abstractionIndex, VLNV const& newVLNV)
{
    std::string referenceVendor = newVLNV.getVendor().toStdString();
    std::string referenceLibrary = newVLNV.getLibrary().toStdString();
    std::string referenceName = newVLNV.getName().toStdString();
    std::string referenceVersion = newVLNV.getVersion().toStdString();

    abstractionInterface_->setAbstraction(
        abstractionIndex, referenceVendor, referenceLibrary, referenceName, referenceVersion);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::onAddItem()
//-----------------------------------------------------------------------------
void AbstractionTypesModel::onAddItem(QModelIndex const& index)
{
    int row = abstractionInterface_->itemCount();

    if (index.isValid())
    {
        row = index.row();
    }

    beginInsertRows(QModelIndex(), row, row);

    abstractionInterface_->addAbstraction(row);

    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void AbstractionTypesModel::onRemoveItem(QModelIndex const& index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= abstractionInterface_->itemCount())
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index.row(), index.row());

    abstractionInterface_->removeAbstraction(index.row());

    endRemoveRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::onChangeSelectedBusInterface()
//-----------------------------------------------------------------------------
void AbstractionTypesModel::onChangeSelectedBusInterface()
{
    beginResetModel();
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

    // Check compatibility of drag & dropped abstraction definition.
    if (!DocumentUtils::documentsHaveMatchingStdRevisions(vlnv, containingComponent_->getVlnv(), library_))
    {
        emit stdRevisionMismatch();
        return false;
    }

    QModelIndex abstractionIndex = parent;
    if (!parent.isValid())
    {
        onAddItem(QModelIndex());

        abstractionIndex =
            index(abstractionInterface_->itemCount() - 1, AbstractionTypesConstants::ABSTRACTIONDEFINITION);
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

    int newAbstractionIndex = abstractionInterface_->itemCount() - 1;

    std::string referenceVendor = newAbstractionVLNV.getVendor().toStdString();
    std::string referenceLibrary = newAbstractionVLNV.getLibrary().toStdString();
    std::string referenceName = newAbstractionVLNV.getName().toStdString();
    std::string referenceVersion = newAbstractionVLNV.getVersion().toStdString();

    abstractionInterface_->setAbstraction(
        newAbstractionIndex, referenceVendor, referenceLibrary, referenceName, referenceVersion);
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::blackForValidRedForInvalid()
//-----------------------------------------------------------------------------
QVariant AbstractionTypesModel::blackForValidRedForInvalid(QModelIndex const& index) const
{
    if (validateIndex(index))
    {
        return KactusColors::REGULAR_TEXT;
    }
    else
    {
        return KactusColors::ERROR;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::validateIndex()
//-----------------------------------------------------------------------------
bool AbstractionTypesModel::validateIndex(QModelIndex const& index) const
{
    if (index.column() == AbstractionTypesConstants::ABSTRACTIONDEFINITION)
    {
        return abstractionInterface_->hasValidAbstractionReference(index.row());
    }
    else if (index.column() == AbstractionTypesConstants::VIEWREFERENCES)
    {
        return abstractionInterface_->hasValidViewReferences(index.row());
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionTypesModel::resetModel()
//-----------------------------------------------------------------------------
void AbstractionTypesModel::resetModel()
{
    beginResetModel();
    endResetModel();
}

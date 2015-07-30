//-----------------------------------------------------------------------------
// File: businterfacesmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// The model that manages the objects for BusInterfacesEditor.
//-----------------------------------------------------------------------------

#include "businterfacesmodel.h"

#include "BusInterfaceColumns.h"

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/masterinterface.h>
#include <IPXACTmodels/mirroredslaveinterface.h>

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <QColor>
#include <QCoreApplication>
#include <QMap>
#include <QMessageBox>
#include <QMimeData>
#include <QRegularExpression>
#include <QString>
#include <QVariant>

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::BusInterfacesModel()
//-----------------------------------------------------------------------------
BusInterfacesModel::BusInterfacesModel(LibraryInterface* libHandler, QSharedPointer<Component> component,
                                       QSharedPointer<ParameterFinder> parameterFinder, QObject *parent):
QAbstractTableModel(parent),
libHandler_(libHandler),
component_(component),
busifs_(component->getBusInterfaces()),
parameterFinder_(parameterFinder)
{
	Q_ASSERT(libHandler_);
	Q_ASSERT(component_);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::~BusInterfacesModel()
//-----------------------------------------------------------------------------
BusInterfacesModel::~BusInterfacesModel()
{
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::rowCount()
//-----------------------------------------------------------------------------
int BusInterfacesModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return busifs_.size();
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::columnCount()
//-----------------------------------------------------------------------------
int BusInterfacesModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return BusInterfaceColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags BusInterfacesModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}
	// bus def and abs def can not be edited but can have data dropped on them
	else if (index.column() == BusInterfaceColumns::BUSDEF ||
		index.column() == BusInterfaceColumns::ABSDEF)
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::headerData()
//-----------------------------------------------------------------------------
QVariant BusInterfacesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
		return QVariant();
    }

    if (section == BusInterfaceColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == BusInterfaceColumns::BUSDEF)
    {
        return tr("Bus\ndefinition");
    }
    else if (section == BusInterfaceColumns::ABSDEF)
    {
        return tr("Abstraction\ndefinition");
    }
    else if (section == BusInterfaceColumns::INTERFACE_MODE)
    {
        return tr("Interface\nmode");
    }
    else if (section == BusInterfaceColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::data()
//-----------------------------------------------------------------------------
QVariant BusInterfacesModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= busifs_.size())
    {
		return QVariant();
	}

    QSharedPointer<BusInterface> busInterface = busifs_.at(index.row());
	if (role == Qt::DisplayRole)
    {
        if (index.column() == BusInterfaceColumns::NAME)
        {
            return busInterface->getName();
        }
        else if ( index.column() == BusInterfaceColumns::BUSDEF)
        {
            return busInterface->getBusType().toString(":");
        }
        else if (index.column() == BusInterfaceColumns::ABSDEF)
        {
            return busInterface->getAbstractionType().toString(":");
        }
        else if (index.column() == BusInterfaceColumns::INTERFACE_MODE)
        {
            General::InterfaceMode mode = busInterface->getInterfaceMode();
            return General::interfaceMode2Str(mode);
        }
        else if (index.column() == BusInterfaceColumns::DESCRIPTION)
        {
            return busInterface->getDescription().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return QVariant();
        }
	}
    else if ((role == Qt::EditRole || role == Qt::ToolTipRole) && 
        index.column() == BusInterfaceColumns::DESCRIPTION)
    {
        return busInterface->getDescription();
    }
	else if (role == Qt::ForegroundRole)
    {
		if (busInterface->isValid(component_->getPortBounds(), component_->getMemoryMapNames(),
            component_->getAddressSpaceNames(), component_->getChoices()))
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
        if (index.column() == BusInterfaceColumns::NAME ||
            index.column() == BusInterfaceColumns::BUSDEF ||
            index.column() == BusInterfaceColumns::INTERFACE_MODE)
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
// Function: BusInterfacesModel::setData()
//-----------------------------------------------------------------------------
bool BusInterfacesModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= busifs_.size())
    {
		return false;
	}

    QSharedPointer<BusInterface> busInterface = busifs_.at(index.row());
	if (role == Qt::EditRole)
    {
        if (index.column() == BusInterfaceColumns::NAME)
        {
            busInterface->setName(value.toString());
        }
        else if (index.column() == BusInterfaceColumns::BUSDEF)
        {
            VLNV busType = VLNV(VLNV::BUSDEFINITION, value.toString(), ":");
            busInterface->setBusType(busType);
        }
        else if (index.column() ==  BusInterfaceColumns::ABSDEF)
        {
            VLNV absType = VLNV(VLNV::ABSTRACTIONDEFINITION, value.toString(), ":");
            busInterface->setAbstractionType(absType);
        }
        else if (index.column() ==  BusInterfaceColumns::INTERFACE_MODE)
        {
            QString modeStr = value.toString();
            General::InterfaceMode mode = General::str2Interfacemode(modeStr, General::MASTER);
            busInterface->setInterfaceMode(mode);
        }
        else if (index.column() ==  BusInterfaceColumns::DESCRIPTION) {
            busInterface->setDescription(value.toString());
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
// Function: BusInterfacesModel::supportedDropActions()
//-----------------------------------------------------------------------------
Qt::DropActions BusInterfacesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList BusInterfacesModel::mimeTypes() const
{
    QStringList types(QAbstractTableModel::mimeTypes());
    types << "application/x-qt-image";
    return types;
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::dropMimeData()
//-----------------------------------------------------------------------------
bool BusInterfacesModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, 
    const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
    {
        return true;
    }

    // Dropped data must be directly on parent.
    if (row != -1 || column != -1 || !parent.isValid() || row > rowCount())
    {
        return false;
    }

    QVariant variant = data->imageData();
    if (!variant.canConvert<VLNV>())
    {
        return false;
    }

    VLNV vlnv = variant.value<VLNV>();

    if (parent.column() == BusInterfaceColumns::BUSDEF)
    {
        if (vlnv.getType() != VLNV::BUSDEFINITION)
        {
            return false;
        }

        setData(index(parent.row(), parent.column()), vlnv.toString(":"));

        // If only one possible absDef, set it automatically.
        QList<VLNV> absDefVLNVs;
        if (libHandler_->getChildren(absDefVLNVs, vlnv) == 1) 
        {
            setData(index(parent.row(),BusInterfaceColumns::ABSDEF), absDefVLNVs.first().toString(":"));
        }

        emit contentChanged();
    }
    else if (parent.column() == BusInterfaceColumns::ABSDEF)
    {
        if ( vlnv.getType() != VLNV::ABSTRACTIONDEFINITION )
        {
            return false;
        }

        setData(index(parent.row(),parent.column()),vlnv.toString(":"));
        emit contentChanged();
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::onAddItem()
//-----------------------------------------------------------------------------
void BusInterfacesModel::onAddItem(QModelIndex const& index)
{
	int row = busifs_.size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
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
// Function: BusInterfacesModel::onRemoveItem()
//-----------------------------------------------------------------------------
void BusInterfacesModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= busifs_.size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    removeReferencesFromExpressions(index.row());
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
void BusInterfacesModel::onMoveItem(QModelIndex const& originalPos, QModelIndex const& newPos)
{
    // if there was no item in the starting point
    if (!originalPos.isValid() || originalPos == newPos || originalPos.row() < 0 || originalPos.row() >= busifs_.size())
    {
        return;
    }

    int source = originalPos.row();
    int target = 0;

    // if the new position is invalid index then put the item last in the table
    if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= busifs_.size())
    {
        beginResetModel();
        QSharedPointer<BusInterface> busIf = busifs_.takeAt(originalPos.row());
        busifs_.append(busIf);
        target = busifs_.size() - 1;
        endResetModel();
    }
    // if both indexes were valid
    else
    {
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
bool BusInterfacesModel::isValid() const
{
	QList<General::PortBounds> physPorts = component_->getPortBounds();
    QStringList memoryMaps = component_->getMemoryMapNames();
    QStringList addressSpaces = component_->getAddressSpaceNames();

	foreach (QSharedPointer<BusInterface> busif, busifs_)
    {
		if (!busif->isValid(physPorts, memoryMaps, addressSpaces, component_->getChoices()))
        {
			return false;
		}
	}
	// all bus interfaces were valid
	return true;
}

//-----------------------------------------------------------------------------
// Function: businterfacesmodel::removeReferencesFromExpressions()
//-----------------------------------------------------------------------------
void BusInterfacesModel::removeReferencesFromExpressions(int busInterfaceIndex)
{
    QSharedPointer<BusInterface> removedInterface = busifs_.at(busInterfaceIndex);

    if (removedInterface->getInterfaceMode() == General::MASTER ||
        removedInterface->getInterfaceMode() == General::MIRROREDMASTER ||
        removedInterface->getInterfaceMode() == General::MIRROREDSLAVE)
    {
        QStringList expressions;
        if (removedInterface->getMaster())
        {
            QSharedPointer<MasterInterface> removedMaster = removedInterface->getMaster();
            expressions.append(removedMaster->getBaseAddress());
        }
        else if (removedInterface->getMirroredSlave())
        {
            QSharedPointer<MirroredSlaveInterface> removedMirrorSlave = removedInterface->getMirroredSlave();
            expressions.append(removedMirrorSlave->getRange());
            expressions.append(removedMirrorSlave->getRemapAddress());
        }

        ReferenceCalculator referenceCalculator(parameterFinder_);
        QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(expressions);

        foreach (QString referencedId, referencedParameters.keys())
        {
            for (int i = 0; i < referencedParameters.value(referencedId); ++i)
            {
                emit decreaseReferences(referencedId);
            }
        }
    }
}
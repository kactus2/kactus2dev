//-----------------------------------------------------------------------------
// File: businterfacesmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 15.06.2012
//
// Description:
// The model that manages the objects for BusInterfacesEditor.
//-----------------------------------------------------------------------------

#include "businterfacesmodel.h"
#include "BusInterfaceColumns.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/InitiatorInterface.h>
#include <IPXACTmodels/Component/MirroredTargetInterface.h>
#include <IPXACTmodels/Component/validators/BusInterfaceValidator.h>

#include <KactusAPI/include/BusInterfaceInterface.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <common/KactusColors.h>

#include <QCoreApplication>
#include <QMap>
#include <QMessageBox>
#include <QMimeData>
#include <QRegularExpression>
#include <QString>
#include <QVariant>
#include <QClipboard>
#include <QApplication>

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::BusInterfacesModel()
//-----------------------------------------------------------------------------
BusInterfacesModel::BusInterfacesModel(LibraryInterface* libHandler,
    QSharedPointer<ParameterFinder> parameterFinder, BusInterfaceInterface* busInterface, QObject *parent):
QAbstractTableModel(parent),
libHandler_(libHandler),
busInterface_(busInterface),
parameterFinder_(parameterFinder)
{
    Q_ASSERT(libHandler_);
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

    return busInterface_->itemCount();
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
	else if (index.column() == BusInterfaceColumns::BUSDEF || index.column() == BusInterfaceColumns::ABSDEF)
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
        return tr("Bus definition");
    }
    else if (section == BusInterfaceColumns::ABSDEF)
    {
        return tr("Abstraction definition");
    }
    else if (section == BusInterfaceColumns::INTERFACE_MODE)
    {
        return tr("Interface mode");
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
    if (!index.isValid() || index.row() < 0 || index.row() >= busInterface_->itemCount())
    {
		return QVariant();
	}

    std::string busName = busInterface_->getIndexedItemName(index.row());
	if (role == Qt::DisplayRole || (
        (role == Qt::EditRole || role == Qt::ToolTipRole) && index.column() != BusInterfaceColumns::DESCRIPTION))
    {
        if (index.column() == BusInterfaceColumns::NAME)
        {
            return QString::fromStdString(busName);
        }
        else if (index.column() == BusInterfaceColumns::BUSDEF)
        {
            return busInterface_->getBusType(busName).toString(":");
        }
        else if (index.column() == BusInterfaceColumns::ABSDEF)
        {
            if (role == Qt::ToolTipRole)
            {
                QStringList references;
                for (auto abstractionReference : busInterface_->getAbstractionReferences(busName))
                {
                    references.append(QString::fromStdString(abstractionReference));
                }

                return references.join("\n");
            }
            else
            {
                return QString::fromStdString(busInterface_->getAbstractionReferenceString(busName));
            }
        }
        else if (index.column() == BusInterfaceColumns::INTERFACE_MODE)
        {
            return QString::fromStdString(busInterface_->getModeString(busName));
        }
        else if (index.column() == BusInterfaceColumns::DESCRIPTION)
        {
            return QString::fromStdString(busInterface_->getDescription(busName)).replace(
                QRegularExpression("\n.*$", QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return QVariant();
        }
	}
    else if ((role == Qt::EditRole || role == Qt::ToolTipRole) && 
        index.column() == BusInterfaceColumns::DESCRIPTION)
    {
        return QString::fromStdString(busInterface_->getDescription(busName));
    }
	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == BusInterfaceColumns::NAME && busInterface_->itemHasValidName(busName))
        {
            return KactusColors::REGULAR_TEXT;     
        }
        else if (index.column() == BusInterfaceColumns::BUSDEF && busInterface_->hasValidBusType(busName))
        {
            return KactusColors::REGULAR_TEXT;
        }
        else if (index.column() == BusInterfaceColumns::ABSDEF && busInterface_->hasValidAbstractionTypes(busName))
        {
            return KactusColors::REGULAR_TEXT;
        }
        else if (index.column() == BusInterfaceColumns::INTERFACE_MODE &&
            busInterface_->getMode(busName) != General::INTERFACE_MODE_COUNT)
        {
            return KactusColors::REGULAR_TEXT;  
        }
        else if (index.column() == BusInterfaceColumns::DESCRIPTION)
        {
            return KactusColors::REGULAR_TEXT;
        }
        else
        {
            return KactusColors::ERROR;
        }
	}
	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == BusInterfaceColumns::NAME ||
            index.column() == BusInterfaceColumns::BUSDEF ||
            index.column() == BusInterfaceColumns::INTERFACE_MODE)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
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
    if (!index.isValid() || index.row() < 0 || index.row() >= busInterface_->itemCount())
    {
		return false;
	}

    std::string busName = busInterface_->getIndexedItemName(index.row());
    std::string valueString = value.toString().toStdString();
	if (role == Qt::EditRole)
    {
        if (index.column() == BusInterfaceColumns::NAME)
        {
            busInterface_->setName(busName, valueString);
        }
        else if (index.column() == BusInterfaceColumns::BUSDEF)
        {
            VLNV busType = VLNV(VLNV::BUSDEFINITION, value.toString(), ":");
            if (!busType.isValid())
            {
                return false;
            }

            busInterface_->setBustype(busName, busType.getVendor().toStdString(),
                busType.getLibrary().toStdString(), busType.getName().toStdString(),
                busType.getVersion().toStdString());
        }
        else if (index.column() == BusInterfaceColumns::ABSDEF)
        {
            if (value.toString().isEmpty())
            {
                busInterface_->removeAbstractionTypes(busName);
            }
            else
            {
                QSharedPointer<ConfigurableVLNVReference> absType(new ConfigurableVLNVReference(VLNV(
                    VLNV::ABSTRACTIONDEFINITION, value.toString(), ":")));
                if (!absType->isValid())
                {
                    return false;
                }

                busInterface_->addAbstractionType(busName, absType->getVendor().toStdString(),
                    absType->getLibrary().toStdString(), absType->getName().toStdString(),
                    absType->getVersion().toStdString());
            }
        }
        else if (index.column() == BusInterfaceColumns::INTERFACE_MODE)
        {
            busInterface_->setMode(busName, valueString);
        }
        else if (index.column() == BusInterfaceColumns::DESCRIPTION)
        {
            busInterface_->setDescription(busName, valueString);
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
bool BusInterfacesModel::dropMimeData(QMimeData const* data, Qt::DropAction action, int row, int column, 
    QModelIndex const& parent)
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
            setData(index(parent.row(), BusInterfaceColumns::ABSDEF), absDefVLNVs.first().toString(":"));
        }

        emit contentChanged();
    }
    else if (parent.column() == BusInterfaceColumns::ABSDEF)
    {
        if (vlnv.getType() != VLNV::ABSTRACTIONDEFINITION)
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
    int row = busInterface_->itemCount();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
    busInterface_->addBusInterface(row);
	endInsertRows();

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
    if (!index.isValid() || index.row() < 0 || index.row() >= busInterface_->itemCount())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    removeReferencesFromExpressions(index.row());
    busInterface_->removeBusInterface(busInterface_->getIndexedItemName(index.row()));
	endRemoveRows();

	// inform navigation tree that file set has been removed
	emit busifRemoved(index.row());

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: businterfacesmodel::removeReferencesFromExpressions()
//-----------------------------------------------------------------------------
void BusInterfacesModel::removeReferencesFromExpressions(int busInterfaceIndex)
{
    std::string busName = busInterface_->getIndexedItemName(busInterfaceIndex);

    QStringList expressions;
    for (auto singleExpression : busInterface_->getAllExpressions(busName))
    {
        expressions.append(QString::fromStdString(singleExpression));
    }

    ReferenceCalculator referenceCalculator(parameterFinder_);
    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(expressions);

    foreach(QString referencedId, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit decreaseReferences(referencedId);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::onMoveItem()
//-----------------------------------------------------------------------------
void BusInterfacesModel::onMoveItem(QModelIndex const& originalPos, QModelIndex const& newPos)
{
    // if there was no item in the starting point
    if (!originalPos.isValid() || originalPos == newPos || originalPos.row() < 0 ||
        originalPos.row() >= busInterface_->itemCount())
    {
        return;
    }

    int source = originalPos.row();
    int target = 0;

    beginResetModel();
    busInterface_->swapBusInterfaces(originalPos.row(), newPos.row());
    endResetModel();

    if (!newPos.isValid() || newPos.row() < 0 || newPos.row() >= busInterface_->itemCount())
    {
        target = busInterface_->itemCount() - 1;
    }
    else
    {
        target = newPos.row();
    }

    emit busIfMoved(source, target);
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::onCopyRows()
//-----------------------------------------------------------------------------
void BusInterfacesModel::onCopyRows(QModelIndexList indexList)
{
    std::vector<int> busIndexes;
    for (auto index : indexList)
    {
        busIndexes.push_back(index.row());
    }
    
    busInterface_->copyRows(busIndexes);
}

//-----------------------------------------------------------------------------
// Function: BusInterfacesModel::onPasteRows()
//-----------------------------------------------------------------------------
void BusInterfacesModel::onPasteRows()
{
    int pastedBusCount = busInterface_->getPasteRowCount();
    if (pastedBusCount == 0)
    {
        return;
    }

    int rowBegin = busInterface_->itemCount();
    int rowEnd = rowBegin + pastedBusCount - 1;

    beginInsertRows(QModelIndex(), rowBegin, rowEnd);

    busInterface_->pasteRows();

    for (int i = rowBegin; i <= rowEnd; ++i)
    {
        emit busifAdded(i);
        increaseReferencesInPastedBus(QString::fromStdString(busInterface_->getIndexedItemName(i)));
    }

    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: businterfacesmodel::increaseReferencesInPastedBus()
//-----------------------------------------------------------------------------
void BusInterfacesModel::increaseReferencesInPastedBus(QString const& busName)
{
    QMap<QString, int> referencedParameters = getReferencedParameters(busName);

    QMapIterator<QString, int> refParameterIterator(referencedParameters);
    while (refParameterIterator.hasNext())
    {
        refParameterIterator.next();
        for (int i = 0; i < refParameterIterator.value(); ++i)
        {
            emit increaseReferences(refParameterIterator.key());
        }
    }
}

//-----------------------------------------------------------------------------
// Function: businterfacesmodel::getReferencedParameters()
//-----------------------------------------------------------------------------
QMap<QString, int> BusInterfacesModel::getReferencedParameters(QString const& busName) const
{
    auto expressionList = busInterface_->getAllExpressions(busName.toStdString());
    QStringList expressionListQT;
    for (auto expression : expressionList)
    {
        expressionListQT.append(QString::fromStdString(expression));
    }

    ReferenceCalculator refCalculator(parameterFinder_);

    return refCalculator.getReferencedParameters(expressionListQT);
}

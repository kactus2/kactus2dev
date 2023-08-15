//-----------------------------------------------------------------------------
// File: RenodePeripheralsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.02.2023
//
// Description:
// The model to manage the Renode peripheral details.
//-----------------------------------------------------------------------------

#include "RenodePeripheralsModel.h"

#include <common/KactusColors.h>

#include <QColor>
#include <QDir>

#include <Plugins/RenodeGenerator/CPUDialog/RenodeColumns.h>
#include <Plugins/RenodeGenerator/CPUDialog/RenodeUtilities.h>

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::RenodePeripheralsModel()
//-----------------------------------------------------------------------------
RenodePeripheralsModel::RenodePeripheralsModel(QObject *parent):
QAbstractTableModel(parent)
{
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::setupPeripherals()
//-----------------------------------------------------------------------------
void RenodePeripheralsModel::setupPeripherals(QVector<QSharedPointer<RenodeStructs::cpuPeripheral>> newPeripherals)
{
    beginResetModel();

    peripherals_.clear();
    peripherals_ = newPeripherals;

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::setupTemplates()
//-----------------------------------------------------------------------------
void RenodePeripheralsModel::setupTemplates(QVector<QSharedPointer<RenodeStructs::peripheralTemplate> > newTemplates)
{
    pythonTemplates_ = newTemplates;
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::rowCount()
//-----------------------------------------------------------------------------
int RenodePeripheralsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return peripherals_.count();
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::columnCount()
//-----------------------------------------------------------------------------
int RenodePeripheralsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return PeripheralColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags RenodePeripheralsModel::flags(QModelIndex const& index) const
{
	if (!index.isValid() || index.column() == PeripheralColumns::NAME || index.column() == PeripheralColumns::BASEADDRESS || index.column() == PeripheralColumns::SIZE)
    {
		return Qt::NoItemFlags;
	}

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (index.column() == PeripheralColumns::INITABLE)
    {
        flags |= Qt::ItemIsUserCheckable;
    }
    else
    {
        flags |= Qt::ItemIsEditable;
    }

    return flags;
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::headerData()
//-----------------------------------------------------------------------------
QVariant RenodePeripheralsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (section == PeripheralColumns::NAME)
        {
            return tr("Peripheral");
        }
        else if (section == PeripheralColumns::CLASS)
        {
            return tr("Class");
        }
        else if (section == PeripheralColumns::BASEADDRESS)
        {
            return tr("Base address");
        }
        else if (section == PeripheralColumns::SIZE)
        {
            return tr("Size");
        }
        else if (section == PeripheralColumns::INITABLE)
        {
            return tr("Initable");
        }
        else if (section == PeripheralColumns::TEMPLATE)
        {
            return tr("Template");
        }
        else if (section == PeripheralColumns::FILEPATH)
        {
            return tr("File path");
        }
	}
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::data()
//-----------------------------------------------------------------------------
QVariant RenodePeripheralsModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= peripherals_.size())
    {
		return QVariant();
	}

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return valueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return tooltipForIndex(index);
    }
	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == PeripheralColumns::NAME || index.column() == PeripheralColumns::BASEADDRESS ||
            index.column() == PeripheralColumns::SIZE ||
            ((index.column() == PeripheralColumns::INITABLE || index.column() == PeripheralColumns::FILEPATH || index.column() == PeripheralColumns::TEMPLATE) &&
                classIsPythonPeripherals(index) == false))
        {
            return KactusColors::DISABLED_TEXT;
        }
        else
        {
            return KactusColors::REGULAR_TEXT;
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        return backgroundColourForIndex(index);
    }
    else if (Qt::CheckStateRole == role)
    {
        if (index.column() == PeripheralColumns::INITABLE)
        {
            QSharedPointer<RenodeStructs::cpuPeripheral> indexedPeripheral = peripherals_.at(index.row());
            if (index.column() == PeripheralColumns::INITABLE && indexedPeripheral->initable_)
            {
                return Qt::Checked;
            }
            else
            {
                return Qt::Unchecked;
            }
        }
        else
        {
            return QVariant();
        }
    }
    else if (role == Qt::DecorationRole && index.column() == PeripheralColumns::FILEPATH)
    {
        return QIcon(QString(":/icons/common/graphics/opened-folder.png"));
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::setData()
//-----------------------------------------------------------------------------
bool RenodePeripheralsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= peripherals_.size())
    {
		return false;
	}

    QSharedPointer<RenodeStructs::cpuPeripheral> indexedPeripheral = peripherals_.at(index.row());
    if (Qt::EditRole == role)
    {
        if (index.column() == PeripheralColumns::CLASS)
        {
            indexedPeripheral->className_ = value.toString();
        }
        else if (index.column() == PeripheralColumns::FILEPATH)
        {
            indexedPeripheral->filePath_ = value.toString();
        }
        else if (index.column() == PeripheralColumns::TEMPLATE)
        {
            auto templateName = value.toString();
            QSharedPointer<RenodeStructs::peripheralTemplate> selectedTemplate = RenodeUtilities::getTemplateFromList(templateName, pythonTemplates_);

            indexedPeripheral->template_ = selectedTemplate;
            if (selectedTemplate)
            {
                indexedPeripheral->filePath_ = selectedTemplate->path_;

				QModelIndex pathIndex = index.sibling(index.row(), PeripheralColumns::FILEPATH);
				emit dataChanged(pathIndex, pathIndex);
            }
        }

        emit dataChanged(index, index);
        return true;
    }
    else if (role == Qt::CheckStateRole)
    {
        if (index.column() == PeripheralColumns::INITABLE)
        {
            indexedPeripheral->initable_ = value.toBool();
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant RenodePeripheralsModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<RenodeStructs::cpuPeripheral> indexedPeripheral = peripherals_.at(index.row());

    if (index.column() == PeripheralColumns::NAME)
    {
        return indexedPeripheral->peripheralName_;
    }
    else if (index.column() == PeripheralColumns::BASEADDRESS)
    {
        return indexedPeripheral->baseAddress_;
    }
    else if (index.column() == PeripheralColumns::SIZE)
    {
        return indexedPeripheral->size_;
    }
    else if (index.column() == PeripheralColumns::CLASS)
    {
        return indexedPeripheral->className_;
    }
    else if (index.column() == PeripheralColumns::FILEPATH)
    {
        return indexedPeripheral->filePath_;
    }
    else if (index.column() == PeripheralColumns::TEMPLATE && indexedPeripheral->template_)
    {
        return indexedPeripheral->template_->identifier_;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::tooltipForIndex()
//-----------------------------------------------------------------------------
QVariant RenodePeripheralsModel::tooltipForIndex(QModelIndex const& index) const
{
    if (index.column() == PeripheralColumns::NAME)
    {
        return QString("Name of the peripheral");
    }
    else if (index.column() == PeripheralColumns::BASEADDRESS)
    {
        return QString("The base address");
    }
    else if (index.column() == PeripheralColumns::SIZE)
    {
        return QString("Size of the peripheral as a hexadecimal value");
    }
    else if (index.column() == PeripheralColumns::CLASS)
    {
        return QString("The type name. The default namespace Antmicro.Renode.Peripherals is omitted");
    }
    else if (index.column() == PeripheralColumns::INITABLE)
    {
        return QString("If true, the peripheral can be initialized and executes code from the isInit section\nOnly applicable for class " + RenodeConstants::PYTHONPERIPHERAL);
    }
    else if (index.column() == PeripheralColumns::TEMPLATE)
    {
        return QString("Template for python peripherals.");
    }
    else if (index.column() == PeripheralColumns::FILEPATH)
    {
        return QString("Relative path to the python file for this peripheral\nOnly applicable for class " + RenodeConstants::PYTHONPERIPHERAL);
    }

    return QVariant();
}


//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::backgroundColourForIndex()
//-----------------------------------------------------------------------------
QVariant RenodePeripheralsModel::backgroundColourForIndex(QModelIndex const& index) const
{
    if ((index.column() == PeripheralColumns::INITABLE || index.column() == PeripheralColumns::FILEPATH || index.column() == PeripheralColumns::TEMPLATE) && classIsPythonPeripherals(index) == false)
    {
        return KactusColors::DISABLED_FIELD;
    }
    
    return KactusColors::REGULAR_FIELD;
}

//-----------------------------------------------------------------------------
// Function: RenodePeripheralsModel::classIsPythonPeripherals()
//-----------------------------------------------------------------------------
bool RenodePeripheralsModel::classIsPythonPeripherals(QModelIndex const& index) const
{
    QModelIndex classIndex = index.sibling(index.row(), PeripheralColumns::CLASS);
    QString peripheralClass = classIndex.data(Qt::DisplayRole).toString();
    
    return peripheralClass == RenodeConstants::PYTHONPERIPHERAL;
}

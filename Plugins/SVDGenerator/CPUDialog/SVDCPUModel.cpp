//-----------------------------------------------------------------------------
// File: SVDCPUModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 18.05.2021
//
// Description:
// The model to manage the SVD CPU details.
//-----------------------------------------------------------------------------

#include "SVDCPUModel.h"

#include "SVDCPUColumns.h"

#include <common/KactusColors.h>

#include <Plugins/SVDGenerator/ConnectivityGraphUtilities.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: SVDCPUModel::SVDCPUModel()
//-----------------------------------------------------------------------------
SVDCPUModel::SVDCPUModel(QObject *parent):
QAbstractTableModel(parent),
cpus_()
{
}

//-----------------------------------------------------------------------------
// Function: SVDCPUModel::setupCPUDetails()
//-----------------------------------------------------------------------------
void SVDCPUModel::setupCPUDetails(QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes>> cpuDetails)
{
    beginResetModel();

    cpus_.clear();
    cpus_ = cpuDetails;

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: SVDCPUModel::rowCount()
//-----------------------------------------------------------------------------
int SVDCPUModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

	return cpus_.count();
}

//-----------------------------------------------------------------------------
// Function: SVDCPUModel::columnCount()
//-----------------------------------------------------------------------------
int SVDCPUModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return SVDCPUColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags SVDCPUModel::flags(QModelIndex const& index) const
{
	if (!index.isValid() || index.column() == SVDCPUColumns::NAME)
    {
		return Qt::NoItemFlags;
	}

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.column() == SVDCPUColumns::CREATESVD || index.column() == SVDCPUColumns::MPUPRESENT ||
        index.column() == SVDCPUColumns::FPUPRESENT || index.column() == SVDCPUColumns::VENDORSYSTICKCONFIG)
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
// Function: SVDCPUModel::headerData()
//-----------------------------------------------------------------------------
QVariant SVDCPUModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}

	if (Qt::DisplayRole == role)
    {
        if (section == SVDCPUColumns::NAME)
        {
            return "CPU";
        }
        if (section == SVDCPUColumns::CREATESVD)
        {
            return "Create\nSVD";
        }
        else if (section == SVDCPUColumns::REVISION)
        {
            return "Revision";
        }
        else if (section == SVDCPUColumns::NVICPRIOBITS)
        {
            return "NVIC\nbits";
        }
        else if (section == SVDCPUColumns::ENDIAN)
        {
            return "Endian";
        }
        else if (section == SVDCPUColumns::MPUPRESENT)
        {
            return "MPU\npresence";
        }
        else if (section == SVDCPUColumns::FPUPRESENT)
        {
            return "FPU\npresence";
        }
        else if (section == SVDCPUColumns::VENDORSYSTICKCONFIG)
        {
            return "Vendor specific\nsystem tick timer";
        }
	}
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: SVDCPUModel::data()
//-----------------------------------------------------------------------------
QVariant SVDCPUModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= cpus_.size())
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
        if (index.column() == SVDCPUColumns::NAME)
        {
            return KactusColors::DISABLED_TEXT;
        }
        else if (index.column() == SVDCPUColumns::REVISION &&
                valueForIndex(index).toString() == ConnectivityGraphUtilities::REVISION_FORMAT)
        {
            return QColor(Qt::lightGray);
        }
        else
        {
            return KactusColors::REGULAR_TEXT;
        }
    }
    else if (Qt::CheckStateRole == role)
    {
        if (index.column() == SVDCPUColumns::CREATESVD || index.column() == SVDCPUColumns::MPUPRESENT ||
            index.column() == SVDCPUColumns::FPUPRESENT || index.column() == SVDCPUColumns::VENDORSYSTICKCONFIG)
        {
            QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> indexedCPU = cpus_.at(index.row());
            if ((index.column() == SVDCPUColumns::CREATESVD && indexedCPU->createFile_) ||
                (index.column() == SVDCPUColumns::MPUPRESENT && indexedCPU->mpuPresent_) ||
                (index.column() == SVDCPUColumns::FPUPRESENT && indexedCPU->fpuPresent_) ||
                (index.column() == SVDCPUColumns::VENDORSYSTICKCONFIG && indexedCPU->vendorSystickConfig_))
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

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: MemoryMapModel::setData()
//-----------------------------------------------------------------------------
bool SVDCPUModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= cpus_.size())
    {
		return false;
	}

    QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> indexedCPU = cpus_.at(index.row());
    if (Qt::EditRole == role)
    {
        if (index.column() == SVDCPUColumns::REVISION)
        {
            indexedCPU->revision_ = value.toString();
        }
        else if (index.column() == SVDCPUColumns::ENDIAN)
        {
            indexedCPU->endian_ = value.toString();
        }
        else if (index.column() == SVDCPUColumns::NVICPRIOBITS)
        {
            indexedCPU->nvicPrioBits_ = value.toString();
        }

        emit dataChanged(index, index);
        return true;
    }
    else if (role == Qt::CheckStateRole)
    {
        if (index.column() == SVDCPUColumns::CREATESVD)
        {
            indexedCPU->createFile_ = value.toBool();
        }
        if (index.column() == SVDCPUColumns::MPUPRESENT)
        {
            indexedCPU->mpuPresent_ = value.toBool();
        }
        else if (index.column() == SVDCPUColumns::FPUPRESENT)
        {
            indexedCPU->fpuPresent_ = value.toBool();
        }
        else if (index.column() == SVDCPUColumns::VENDORSYSTICKCONFIG)
        {
            indexedCPU->vendorSystickConfig_ = value.toBool();
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SVDCPUModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant SVDCPUModel::valueForIndex(QModelIndex const& index) const
{
    QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> indexedCPU = cpus_.at(index.row());

    if (index.column() == SVDCPUColumns::NAME)
    {
        return indexedCPU->cpuName_;
    }
    else if (index.column() == SVDCPUColumns::REVISION)
    {
        QString revisionText = indexedCPU->revision_;
        if (revisionText.isEmpty())
        {
            revisionText = ConnectivityGraphUtilities::REVISION_FORMAT;
        }

        return revisionText;
    }
    else if (index.column() == SVDCPUColumns::ENDIAN)
    {
        return indexedCPU->endian_;
    }
    else if (index.column() == SVDCPUColumns::NVICPRIOBITS)
    {
        return indexedCPU->nvicPrioBits_;
    }
   
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: SVDCPUModel::tooltipForIndex()
//-----------------------------------------------------------------------------
QVariant SVDCPUModel::tooltipForIndex(QModelIndex const& index) const
{
    if (index.column() == SVDCPUColumns::REVISION)
    {
        return QString("Define the HW revision of the processor.\nThe version format is rNpM (N,M = [0 - 99])");
    }
    else if (index.column() == SVDCPUColumns::ENDIAN)
    {
        return QString("Define the endianness of the processor being one of :\n"
            "little: little endian memory(least significant byte gets allocated at the lowest address).\n"
            "big : byte invariant big endian data organization(most significant byte gets allocated at the "
                "lowest address).\n"
            "selectable : little and big endian are configurable for the device and become active after the "
                "next reset.\n"
            "other : the endianness is neither little nor big endian.");
    }
    else if (index.column() == SVDCPUColumns::NVICPRIOBITS)
    {
        return QString(
            "Define the number of bits available in the Nested Vectored Interrupt Controller(NVIC) for "
            "configuring priority.");
    }
    else if (index.column() == SVDCPUColumns::MPUPRESENT)
    {
        return QString("Indicate whether the processor is equipped with a memory protection unit(MPU).");
    }
    else if (index.column() == SVDCPUColumns::FPUPRESENT)
    {
        return QString("Indicate whether the processor is equipped with a hardware floating point unit(FPU).\n"
            "Cortex - M4, Cortex - M7, Cortex - M33 and Cortex - M35P are the only available Cortex - M "
            "processor with an optional FPU.");
    }
    else if (index.column() == SVDCPUColumns::VENDORSYSTICKCONFIG)
    {
        return QString("Indicate whether the processor implements a vendor - specific System Tick Timer.\n"
            "If false, then the Arm - defined System Tick Timer is available.\n"
            "If true, then a vendor - specific System Tick Timer must be implemented.");
    }

    return QVariant();
}

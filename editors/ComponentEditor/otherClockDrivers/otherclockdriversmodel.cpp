//-----------------------------------------------------------------------------
// File: otherclockdriversmodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 04.04.2011
//
// Description:
// Table model that contains the items to edit otherClockDrivers of a component.
//-----------------------------------------------------------------------------

#include "otherclockdriversmodel.h"

#include "OtherClockDriverColumns.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/OtherClockDriver.h>

#include <IPXACTmodels/Component/validators/OtherClockDriverValidator.h>

#include <QColor>

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::OtherClockDriversModel()
//-----------------------------------------------------------------------------
OtherClockDriversModel::OtherClockDriversModel(QSharedPointer<Component> component,
    QSharedPointer<OtherClockDriverValidator> clockValidator, QObject *parent):
QAbstractTableModel(parent),
component_(component), 
table_(component->getOtherClockDrivers()),
clockValidator_(clockValidator)
{

}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::~OtherClockDriversModel()
//-----------------------------------------------------------------------------
OtherClockDriversModel::~OtherClockDriversModel()
{
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::rowCount()
//-----------------------------------------------------------------------------
int OtherClockDriversModel::rowCount(QModelIndex const&  parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return table_->size();
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::columnCount()
//-----------------------------------------------------------------------------
int OtherClockDriversModel::columnCount(QModelIndex const&  parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return OtherClockDriverColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::data()
//-----------------------------------------------------------------------------
QVariant OtherClockDriversModel::data(QModelIndex const&  index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= table_->size())
    {
		return QVariant();
    }

	if (role == Qt::DisplayRole)
    {
        if (index.column() == OtherClockDriverColumns::NAME)
        {
            return table_->at(index.row())->getClockName();
        }
        else if (index.column() == OtherClockDriverColumns::CLOCK_SOURCE)
        {
            return table_->at(index.row())->getClockSource();
        }
        else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD)
        {
            return table_->at(index.row())->getClockPeriod()->getValue();
        }
        else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD_UNIT)
        {
            return table_->at(index.row())->getClockPeriod()->timeUnitToString();
        }
        else if (index.column() == OtherClockDriverColumns::PULSE_OFFSET)
        {
            return table_->at(index.row())->getClockPulseOffset()->getValue();
        }
        else if (index.column() == OtherClockDriverColumns::PULSE_OFFSET_UNIT)
        {
            return table_->at(index.row())->getClockPulseOffset()->timeUnitToString();
        }
        else if (index.column() == OtherClockDriverColumns::PULSE_VALUE)
        {
            return table_->at(index.row())->getClockPulseValue();
        }
        else if (index.column() == OtherClockDriverColumns::PULSE_DURATION)
        {
            return table_->at(index.row())->getClockPulseDuration()->getValue();
        }
        else if (index.column() == OtherClockDriverColumns::PULSE_DURATION_UNIT)
        {
            return table_->at(index.row())->getClockPulseDuration()->timeUnitToString();
        }
        else
        {
            return QVariant();
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == OtherClockDriverColumns::NAME ||
            index.column() == OtherClockDriverColumns::CLOCK_PERIOD ||
            index.column() == OtherClockDriverColumns::PULSE_OFFSET ||
            index.column() == OtherClockDriverColumns::PULSE_VALUE ||
            index.column() == OtherClockDriverColumns::PULSE_DURATION)
        {
            return QColor("LemonChiffon");
        }
        else
            return QColor("white");
    }

    else if (role == Qt::ForegroundRole)
    {
        if (validateIndex(index))
        {
            return QColor("black");
		}
		else
        {
			return QColor("red");
		}
	}

	else
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::headerData()
//-----------------------------------------------------------------------------
QVariant OtherClockDriversModel::headerData(int section, Qt::Orientation orientation, int role) const
{	
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
		return QVariant();
    }

    if (section == OtherClockDriverColumns::NAME)
    {
        return tr("Clock\nname");
    }
    else if (section == OtherClockDriverColumns::CLOCK_SOURCE)
    {
        return tr("Clock\nsource");
    }
    else if (section == OtherClockDriverColumns::CLOCK_PERIOD)
    {
        return tr("Clock\nperiod");
    }
    else if (section == OtherClockDriverColumns::CLOCK_PERIOD_UNIT)
    {
        return tr("Period\nunit");
    }
    else if (section == OtherClockDriverColumns::PULSE_OFFSET)
    {
        return tr("Pulse\noffset");
    }
    else if (section == OtherClockDriverColumns::PULSE_OFFSET_UNIT)
    {
        return tr("Offset\nunit");
    }
    else if (section == OtherClockDriverColumns::PULSE_VALUE)
    {
        return tr("Pulse\nvalue");
    }
    else if (section == OtherClockDriverColumns::PULSE_DURATION)
    {
        return tr("Pulse\nduration");
    }
    else if (section == OtherClockDriverColumns::PULSE_DURATION_UNIT)
    {
        return tr("Duration\nunit");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::setData()
//-----------------------------------------------------------------------------
bool OtherClockDriversModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= table_->size() || role != Qt::EditRole)
    {
        return false;
    }

    if (index.column() == OtherClockDriverColumns::NAME)
    {
        table_->value(index.row())->setClockName(value.toString());
    }
    else if (index.column() == OtherClockDriverColumns::CLOCK_SOURCE)
    {
        table_->value(index.row())->setClockSource(value.toString());
    }
    else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD)
    {
        table_->value(index.row())->getClockPeriod()->setValue(value.toString());
    }
    else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD_UNIT)
    {
        table_->value(index.row())->getClockPeriod()->setTimeUnit(value.toString());
    }
    else if (index.column() == OtherClockDriverColumns::PULSE_OFFSET)
    {
        table_->value(index.row())->getClockPulseOffset()->setValue(value.toString());        
    }
    else if (index.column() == OtherClockDriverColumns::PULSE_OFFSET_UNIT)
    {
        table_->value(index.row())->getClockPulseOffset()->setTimeUnit(value.toString());
    }
    else if (index.column() == OtherClockDriverColumns::PULSE_VALUE)
    {
        table_->value(index.row())->setClockPulseValue(value.toString());
    }
    else if (index.column() == OtherClockDriverColumns::PULSE_DURATION)
    {
        table_->value(index.row())->getClockPulseDuration()->setValue(value.toString());
    }
    else if (index.column() == OtherClockDriverColumns::PULSE_DURATION_UNIT)
    {
        table_->value(index.row())->getClockPulseDuration()->setTimeUnit(value.toString());
    }
    else
    {
        return false;
    }

    emit dataChanged(index, index);
    emit contentChanged();
    return true;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags OtherClockDriversModel::flags(QModelIndex const&  index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::onRemoveRow()
//-----------------------------------------------------------------------------
void OtherClockDriversModel::onRemoveRow(int row)
{
	// if row is invalid
	if (row < 0 || row >= table_->size())
    {
		return;
    }

	beginRemoveRows(QModelIndex(), row, row);

	// remove the object from the map
	table_->removeAt(row);

	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::onRemoveItem()
//-----------------------------------------------------------------------------
void OtherClockDriversModel::onRemoveItem(QModelIndex const&  index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= table_->size())
    {
		return;
    }

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	table_->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::onAddRow()
//-----------------------------------------------------------------------------
void OtherClockDriversModel::onAddRow()
{
	beginInsertRows(QModelIndex(), table_->size(), table_->size());
	table_->append(QSharedPointer<OtherClockDriver>(new OtherClockDriver()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: OtherClockDriversModel::onAddItem()
//-----------------------------------------------------------------------------
void OtherClockDriversModel::onAddItem(QModelIndex const& index)
{
	int row = table_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	table_->insert(row, QSharedPointer<OtherClockDriver>(new OtherClockDriver()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: otherclockdriversmodel::validateIndex()
//-----------------------------------------------------------------------------
bool OtherClockDriversModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<OtherClockDriver> currentClockDriver = table_->at(index.row());

    if (index.column() == OtherClockDriverColumns::NAME)
    {
        return clockValidator_->hasValidName(currentClockDriver->getClockName());
    }
    else if (index.column() == OtherClockDriverColumns::CLOCK_PERIOD)
    {
        return clockValidator_->hasValidClockValue(currentClockDriver->getClockPeriod());
    }
    else if (index.column() == OtherClockDriverColumns::PULSE_OFFSET)
    {
        return clockValidator_->hasValidClockValue(currentClockDriver->getClockPulseOffset());
    }
    else if (index.column() == OtherClockDriverColumns::PULSE_VALUE)
    {
        return clockValidator_->hasValidClockPulseValue(currentClockDriver);
    }
    else if (index.column() == OtherClockDriverColumns::PULSE_DURATION)
    {
        return clockValidator_->hasValidClockValue(currentClockDriver->getClockPulseDuration());
    }

    return true;
}

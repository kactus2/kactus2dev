//-----------------------------------------------------------------------------
// File: PowerDomainsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.10.2023
//
// Description:
// Table model that contains the items to edit PowerDomains of a component.
//-----------------------------------------------------------------------------

#include "PowerDomainsModel.h"

#include "PowerDomainColumns.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/PowerDomain.h>

#include <IPXACTmodels/Component/validators/PowerDomainValidator.h>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::PowerDomainsModel()
//-----------------------------------------------------------------------------
PowerDomainsModel::PowerDomainsModel(QSharedPointer<Component> component,
    QSharedPointer<PowerDomainValidator > validator,
    ExpressionSet expressions,
    QObject* parent) : 
ReferencingTableModel(expressions.finder, parent),
    ParameterizableTable(expressions.finder),
component_(component), 
table_(component->getPowerDomains()),
expressionFormatter_(expressions.formatter),
validator_(validator)
{
    setExpressionParser(expressions.parser);
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::~PowerDomainsModel()
//-----------------------------------------------------------------------------
PowerDomainsModel::~PowerDomainsModel()
{
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::rowCount()
//-----------------------------------------------------------------------------
int PowerDomainsModel::rowCount(QModelIndex const&  parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return table_->size();
}


//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::columnCount()
//-----------------------------------------------------------------------------
int PowerDomainsModel::columnCount(QModelIndex const&  parent) const
{
	if (parent.isValid())
    {
		return 0;
    }

	return PowerDomainColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::data()
//-----------------------------------------------------------------------------
QVariant PowerDomainsModel::data(QModelIndex const&  index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= table_->size())
    {
		return QVariant();
    }

	if (role == Qt::DisplayRole )
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(expressionOrValueForIndex(index).toString());
        }

        return expressionOrValueForIndex(index);

    }
    else if (role == Qt::EditRole)
    {
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedValueFor(expressionOrValueForIndex(index).toString());
        }

        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.column() == PowerDomainColumns::NAME)
        {
            return KactusColors::MANDATORY_FIELD;
        }

        return KactusColors::REGULAR_FIELD;
    }

    else if (role == Qt::ForegroundRole)
    {
        return blackForValidOrRedForInvalidIndex(index);
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::headerData()
//-----------------------------------------------------------------------------
QVariant PowerDomainsModel::headerData(int section, Qt::Orientation orientation, int role) const
{	
	if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    {
		return QVariant();
    }

    if (section == PowerDomainColumns::NAME)
    {
        return tr("Name");
    }
    else if (section == PowerDomainColumns::DISPLAY_NAME)
    {
        return tr("Display name");
    }
    else if (section == PowerDomainColumns::ALWAYS_ON)
    {
        return tr("Always on") + getExpressionSymbol();
    }
    else if (section == PowerDomainColumns::SUBDOMAIN)
    {
        return tr("Subdomain of");
    }
    else if (section == PowerDomainColumns::SHORT_DESCRIPTION)
    {
        return tr("Short description");
    }
    else if (section == PowerDomainColumns::DESCRIPTION)
    {
        return tr("Description");
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::setData()
//-----------------------------------------------------------------------------
bool PowerDomainsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= table_->size() || role != Qt::EditRole)
    {
        return false;
    }

    if (index.column() == PowerDomainColumns::NAME)
    {
        table_->value(index.row())->setName(value.toString());
    }
    else if (index.column() == PowerDomainColumns::DISPLAY_NAME)
    {
        table_->value(index.row())->setDisplayName(value.toString());
    }
    else if (index.column() == PowerDomainColumns::ALWAYS_ON)
    {
         table_->value(index.row())->setAlwaysOn(value.toString());
    }
    else if (index.column() == PowerDomainColumns::SUBDOMAIN)
    {
        table_->value(index.row())->setSubDomainOf(value.toString());
    }
    else if (index.column() == PowerDomainColumns::SHORT_DESCRIPTION)
    {
        table_->value(index.row())->setShortDescription(value.toString());
    }
    else if (index.column() == PowerDomainColumns::DESCRIPTION)
    {
        table_->value(index.row())->setDescription(value.toString());
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
// Function: PowerDomainsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PowerDomainsModel::flags(QModelIndex const&  index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

	return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::onRemoveRow()
//-----------------------------------------------------------------------------
void PowerDomainsModel::onRemoveRow(int row)
{
	// if row is invalid
	if (row < 0 || row >= table_->size())
    {
		return;
    }

	beginRemoveRows(QModelIndex(), row, row);

    removeReferencesInItemOnRow(row);
	table_->removeAt(row);

	endRemoveRows();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void PowerDomainsModel::onRemoveItem(QModelIndex const&  index)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= table_->size())
    {
		return;
    }

	beginRemoveRows(QModelIndex(), index.row(), index.row());
    removeReferencesInItemOnRow(index.row());
	table_->removeAt(index.row());
	endRemoveRows();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::onAddRow()
//-----------------------------------------------------------------------------
void PowerDomainsModel::onAddRow()
{
	beginInsertRows(QModelIndex(), table_->size(), table_->size());
	table_->append(QSharedPointer<PowerDomain>(new PowerDomain()));
	endInsertRows();

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::onAddItem()
//-----------------------------------------------------------------------------
void PowerDomainsModel::onAddItem(QModelIndex const& index)
{
	int row = table_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	table_->insert(row, QSharedPointer<PowerDomain>(new PowerDomain()));
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsmodel::validateIndex()
//-----------------------------------------------------------------------------
bool PowerDomainsModel::validateIndex(QModelIndex const& index) const
{
    QSharedPointer<PowerDomain> domain = table_->at(index.row());

    if (index.column() == PowerDomainColumns::NAME)
    {
        return validator_->hasValidName(domain->name());
    }
    else if (index.column() == PowerDomainColumns::ALWAYS_ON)
    {
        return validator_->hasValidAlwaysOn(domain);
    }
    else if (index.column() == PowerDomainColumns::SUBDOMAIN)
    {
        return validator_->hasValidSubDomainOf(domain);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int PowerDomainsModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    return table_->at(row)->getAlwaysOn().count(valueID);
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool PowerDomainsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == PowerDomainColumns::ALWAYS_ON;
}

//-----------------------------------------------------------------------------
// Function: PowerDomainsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant PowerDomainsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == PowerDomainColumns::NAME)
    {
        return table_->at(index.row())->name();
    }
    else if (index.column() == PowerDomainColumns::DISPLAY_NAME)
    {
        return table_->at(index.row())->displayName();
    }
    else if (index.column() == PowerDomainColumns::ALWAYS_ON)
    {
        return table_->at(index.row())->getAlwaysOn();
    }
    else if (index.column() == PowerDomainColumns::SUBDOMAIN)
    {
        return table_->at(index.row())->getSubDomainOf();
    }
    else if (index.column() == PowerDomainColumns::SHORT_DESCRIPTION)
    {
        return table_->at(index.row())->shortDescription();
    }
    else if (index.column() == PowerDomainColumns::DESCRIPTION)
    {
        return table_->at(index.row())->description();
    }
    else
    {
        return QVariant();
    }
}

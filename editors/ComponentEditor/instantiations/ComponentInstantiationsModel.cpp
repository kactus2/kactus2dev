//-----------------------------------------------------------------------------
// File: ComponentInstantiationsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Model for component instantiations summary.
//-----------------------------------------------------------------------------

#include "ComponentInstantiationsModel.h"
#include "ComponentInstantiationsColumns.h"

#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

#include <common/KactusColors.h>

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>
#include <editors/ComponentEditor/instantiations/interfaces/ComponentInstantiationInterface.h>

#include <QStringList>
#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::ComponentInstantiationsModel()
//-----------------------------------------------------------------------------
ComponentInstantiationsModel::ComponentInstantiationsModel(QSharedPointer<ParameterFinder> finder,
    ComponentInstantiationInterface* instantiationInterface, QObject* parent):
QAbstractTableModel(parent),
parameterFinder_(finder),
instantiationInterface_(instantiationInterface)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::~ComponentInstantiationsModel()
//-----------------------------------------------------------------------------
ComponentInstantiationsModel::~ComponentInstantiationsModel()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::rowCount()
//-----------------------------------------------------------------------------
int ComponentInstantiationsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
    return instantiationInterface_->itemCount();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::columnCount()
//-----------------------------------------------------------------------------
int ComponentInstantiationsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}

    return ComponentInstantiationsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags ComponentInstantiationsModel::flags(QModelIndex const& index) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
	}

	// Other columns can be edited.
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::headerData()
//-----------------------------------------------------------------------------
QVariant ComponentInstantiationsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
    }

    if (Qt::DisplayRole == role)
    {
        if (section == ComponentInstantiationsColumns::NAME)
        {
            return tr("Name");
        }
        else if (section == ComponentInstantiationsColumns::DISPLAY_NAME)
        {
            return tr("Display name");
        }
        else if (section == ComponentInstantiationsColumns::LANGUAGE)
        {
            return tr("Language");
        }
        else if (section == ComponentInstantiationsColumns::DESCRIPTION)
        {
            return tr("Description");
        }
        else
        {
            return QVariant();
        }
    }
    else
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::data()
//-----------------------------------------------------------------------------
QVariant ComponentInstantiationsModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= instantiationInterface_->itemCount())
    {
        return QVariant();
    }

    std::string instantiationName = instantiationInterface_->getIndexedItemName(index.row());

    if (role == Qt::EditRole && index.column() == ComponentInstantiationsColumns::DESCRIPTION)
    {
        return QString::fromStdString(instantiationInterface_->getDescription(instantiationName));
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == ComponentInstantiationsColumns::NAME)
        {
            if (!instantiationName.empty())
            {
                return QString::fromStdString(instantiationName);
            }
            else
            {
                return "unnamed";
            }
        }
        else if (index.column() == ComponentInstantiationsColumns::DISPLAY_NAME)
        {
            return QString::fromStdString(instantiationInterface_->getDisplayName(instantiationName));
        }
        else if (index.column() == ComponentInstantiationsColumns::LANGUAGE)
        {
            return QString::fromStdString(instantiationInterface_->getLanguage(instantiationName));
        }
        else if (index.column() == ComponentInstantiationsColumns::DESCRIPTION)
        {
            QString instantiationDescription =
                QString::fromStdString(instantiationInterface_->getDescription(instantiationName));
            return instantiationDescription.replace(
                QRegularExpression("\n.*$", QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return QVariant();
        }
	}
	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == ComponentInstantiationsColumns::NAME &&
            !instantiationInterface_->itemHasValidName(instantiationName))
        {
            return KactusColors::ERROR;
        }
        else
        {
            return KactusColors::REGULAR_TEXT;
        }
	}
	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == ComponentInstantiationsColumns::NAME)
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
// Function: ComponentInstantiationsModel::setData()
//-----------------------------------------------------------------------------
bool ComponentInstantiationsModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= instantiationInterface_->itemCount())
    {
		return false;
	}

    std::string instantiationName = instantiationInterface_->getIndexedItemName(index.row());

	if (role == Qt::EditRole)
    {
        std::string newValue = value.toString().toStdString();

        if (index.column() == ComponentInstantiationsColumns::NAME)
        {
            instantiationInterface_->setName(instantiationName, newValue);
        }
        else if (index.column() == ComponentInstantiationsColumns::DISPLAY_NAME)
        {
            instantiationInterface_->setDisplayName(instantiationName, newValue);
        }
        else if (index.column() == ComponentInstantiationsColumns::LANGUAGE)
        {
            instantiationInterface_->setLanguage(instantiationName, newValue);
        }
        else if (index.column() == ComponentInstantiationsColumns::DESCRIPTION)
        {
            instantiationInterface_->setDescription(instantiationName, newValue);
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
// Function: ComponentInstantiationsModel::onAddItem()
//-----------------------------------------------------------------------------
void ComponentInstantiationsModel::onAddItem(QModelIndex const& index)
{
    int row = instantiationInterface_->itemCount();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);

    instantiationInterface_->addComponentInstantiation(row);

	endInsertRows();
	
	emit componentInstantiationAdded(row);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void ComponentInstantiationsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
    if (!index.isValid() || index.row() < 0 || index.row() >= instantiationInterface_->itemCount())
    {
		return;
	}

    std::string instantiationName = instantiationInterface_->getIndexedItemName(index.row());

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    decreaseReferencesInRemovedComponentInstantiation(instantiationName);
    instantiationInterface_->removeComponentInstantiation(instantiationName);

    endRemoveRows();

	emit componentInstantiationRemoved(index.row());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::decreaseReferencesInRemovedComponentInstantiation()
//-----------------------------------------------------------------------------
void ComponentInstantiationsModel::decreaseReferencesInRemovedComponentInstantiation(
    std::string const& instantiationName)
{
    QStringList expressionList;

    std::vector<std::string> names;
    names.push_back(instantiationName);

    for (auto expression : instantiationInterface_->getExpressionsInSelectedItems(names))
    {
        expressionList.append(QString::fromStdString(expression));
    }

    ReferenceCalculator referenceCalculator(parameterFinder_);
    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(expressionList);
    foreach (QString referencedID, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedID); ++i)
        {
            emit decreaseReferences(referencedID);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::getAllReferencableValuesFromParameter()
//-----------------------------------------------------------------------------
QStringList ComponentInstantiationsModel::getAllReferencableValuesFromParameter(
    QSharedPointer<Parameter> selectedParameter) const
{
    QStringList referenceList;

    referenceList.append(selectedParameter->getValue());
    referenceList.append(selectedParameter->getVectorLeft());
    referenceList.append(selectedParameter->getVectorRight());
    referenceList.append(selectedParameter->getAttribute("kactus2:arrayLeft"));
    referenceList.append(selectedParameter->getAttribute("kactus2:arrayRight"));

    return referenceList;
}

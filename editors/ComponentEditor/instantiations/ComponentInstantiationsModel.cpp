//-----------------------------------------------------------------------------
// File: ComponentInstantiationsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.01.2016
//
// Description:
// Model for component instantiations summary.
//-----------------------------------------------------------------------------

#include "ComponentInstantiationsModel.h"
#include "ComponentInstantiationsColumns.h"

#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <QStringList>
#include <QColor>
#include <QRegularExpression>

#include <IPXACTmodels/Component/validators/InstantiationsValidator.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::ComponentInstantiationsModel()
//-----------------------------------------------------------------------------
ComponentInstantiationsModel::ComponentInstantiationsModel(QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> finder, QSharedPointer<InstantiationsValidator> validator, QObject* parent):
QAbstractTableModel(parent),
component_(component),
instantiations_(component->getComponentInstantiations()),
parameterFinder_(finder),
validator_(validator)
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
    return instantiations_->size();
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
	if (!index.isValid() || index.row() < 0 || index.row() >= instantiations_->size())
    {
        return QVariant();
    }

    QSharedPointer<ComponentInstantiation> instantiation = instantiations_->at(index.row());

    if (role == Qt::EditRole && index.column() == ComponentInstantiationsColumns::DESCRIPTION)
    {
        return instantiation->description();
    }
    else if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == ComponentInstantiationsColumns::NAME)
        {
            if (!instantiation->name().isEmpty())
            {
                return instantiation->name();
            }
            else
            {
                return "unnamed";
            }
        }
        else if (index.column() == ComponentInstantiationsColumns::DISPLAY_NAME)
        {
            return instantiation->displayName();
        }
        else if (index.column() == ComponentInstantiationsColumns::LANGUAGE)
        {
            return instantiation->getLanguage();
        }
        else if (index.column() == ComponentInstantiationsColumns::DESCRIPTION)
        {
            return instantiation->description().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return QVariant();
        }
	}
	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == ComponentInstantiationsColumns::NAME && 
            !validator_->hasValidName(instantiation->name()))
        {
            return QColor("red");
        }
        else
        {
            return QColor("black");
        }
	}
	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == ComponentInstantiationsColumns::NAME)
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
// Function: ComponentInstantiationsModel::setData()
//-----------------------------------------------------------------------------
bool ComponentInstantiationsModel::setData(QModelIndex const& index, const QVariant& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= instantiations_->size())
    {
		return false;
	}

    QSharedPointer<ComponentInstantiation> instantiation = instantiations_->at(index.row());

	if (role == Qt::EditRole)
    {
        if (index.column() == ComponentInstantiationsColumns::NAME)
        {
            instantiation->setName(value.toString());
        }
        else if (index.column() == ComponentInstantiationsColumns::DISPLAY_NAME)
        {
            instantiation->setDisplayName(value.toString());
        }
        else if (index.column() == ComponentInstantiationsColumns::LANGUAGE)
        {
            instantiation->setLanguage(value.toString());
        }
        else if (index.column() == ComponentInstantiationsColumns::DESCRIPTION)
        {
            instantiation->setDescription(value.toString());
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
	int row = instantiations_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	instantiations_->insert(row, QSharedPointer<ComponentInstantiation>(new ComponentInstantiation()));
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
	if (!index.isValid() || index.row() < 0 || index.row() >= instantiations_->size())
    {
		return;
	}

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());

    decreaseReferencesInRemovedComponentInstantiation(instantiations_->at(index.row()));

	instantiations_->removeAt(index.row());
	endRemoveRows();

	emit componentInstantiationRemoved(index.row());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationsModel::decreaseReferencesInRemovedComponentInstantiation()
//-----------------------------------------------------------------------------
void ComponentInstantiationsModel::decreaseReferencesInRemovedComponentInstantiation(
    QSharedPointer<ComponentInstantiation> instantiation)
{
    QStringList expressionList;
    foreach (QSharedPointer<FileBuilder> builder, *instantiation->getDefaultFileBuilders())
    {
        expressionList.append(builder->getReplaceDefaultFlags());
    }

    foreach (QSharedPointer<ModuleParameter> moduleParameter, *instantiation->getModuleParameters())
    {
        expressionList.append(getAllReferencableValuesFromParameter(moduleParameter));
    }

    foreach (QSharedPointer<Parameter> singleParameter, *instantiation->getParameters())
    {
        expressionList.append(getAllReferencableValuesFromParameter(singleParameter));
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

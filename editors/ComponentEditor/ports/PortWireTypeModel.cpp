//-----------------------------------------------------------------------------
// File: PortWireTypeModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.11.2017
//
// Description:
// Table model that can be used to display port wire type definitions.
//-----------------------------------------------------------------------------

#include "PortWireTypeModel.h"

#include <editors/ComponentEditor/ports/PortWireTypeColumns.h>

#include <common/KactusColors.h>

#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/Wire.h>
#include <IPXACTmodels/Component/WireTypeDef.h>

#include <IPXACTmodels/Component/validators/PortTypeValidator.h>

#include <QFont>

namespace PortWireConstants
{
    QString const DEFINITIONSEPARATOR = "; ";
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::PortWireTypeModel()
//-----------------------------------------------------------------------------
PortWireTypeModel::PortWireTypeModel(QSharedPointer<PortTypeValidator> typeValidator, QObject *parent):
QAbstractItemModel(parent),
wireTypeDefinitions_(),
typeValidator_(typeValidator)
{

}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::~PortWireTypeModel()
//-----------------------------------------------------------------------------
PortWireTypeModel::~PortWireTypeModel()
{

}


//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::index()
//-----------------------------------------------------------------------------
QModelIndex PortWireTypeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (wireTypeDefinitions_)
    {
        if (!parent.isValid() && 0 <= row && row < wireTypeDefinitions_->size())
        {
            return createIndex(row, column, wireTypeDefinitions_->at(row).data());
        }
        else
        {
            QSharedPointer<WireTypeDef> wireTypeDefinition = getIndexedWireTypeDefinition(parent);
            if (0 <= row && row < wireTypeDefinition->getViewRefs()->size())
            {
                return createIndex(row, column, &(*wireTypeDefinition->getViewRefs())[row]);
            }
        }
    }

    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::parent()
//-----------------------------------------------------------------------------
QModelIndex PortWireTypeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    if (wireTypeDefinitions_)
    {
        foreach (QSharedPointer<WireTypeDef> wireType, *wireTypeDefinitions_)
        {
            if (child.internalPointer() == wireType.data())
            {
                return QModelIndex();
            }
        }
    }

    QString* childItem = static_cast<QString*>(child.internalPointer());
    if (childItem)
    {
        for (int i = 0; i < wireTypeDefinitions_->size(); ++i)
        {
            QSharedPointer<WireTypeDef> wireDefinition = wireTypeDefinitions_->at(i);
            for (int j = 0; j < wireDefinition->getViewRefs()->count(); ++j)
            {
                QString* viewReference = &(*wireDefinition->getViewRefs())[j];
                if (childItem == viewReference)
                {
                    return createIndex(i, 0, wireDefinition.data());
                }
            }
        }
    }

    //! This should not be reached.
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::rowCount()
//-----------------------------------------------------------------------------
int PortWireTypeModel::rowCount(const QModelIndex& parent) const
{
    if (wireTypeDefinitions_)
    {
        if (!parent.isValid())
        {
            return wireTypeDefinitions_->count();
        }
        else if (!parent.parent().isValid())
        {
            QSharedPointer<QStringList> viewsForType = getViewsForType(parent);
            if (viewsForType)
            {
                return viewsForType->count();
            }
        }
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::getViewsForType()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> PortWireTypeModel::getViewsForType(QModelIndex const& typeIndex) const
{
    QString typeName = wireTypeDefinitions_->at(typeIndex.row())->getTypeName();
    QSharedPointer<QStringList> definitionList = getTypeDefinitionsForType(typeIndex);

    foreach (QSharedPointer<WireTypeDef> wireType, *wireTypeDefinitions_)
    {
        if (wireType->getTypeName() == typeName && wireType->getTypeDefinitions() == definitionList)
        {
            return wireType->getViewRefs();
        }
    }

    return QSharedPointer<QStringList>();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::getTypeDefinitionsForType()
//-----------------------------------------------------------------------------
QSharedPointer<QStringList> PortWireTypeModel::getTypeDefinitionsForType(QModelIndex const& typeIndex) const
{
    return wireTypeDefinitions_->at(typeIndex.row())->getTypeDefinitions();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::columnCount()
//-----------------------------------------------------------------------------
int PortWireTypeModel::columnCount(const QModelIndex& /*parent*/) const
{
    return PortWireTypeColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::headerData()
//-----------------------------------------------------------------------------
QVariant PortWireTypeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        if (section == PortWireTypeColumns::TYPE)
        {
            return tr("Type name / View");
        }
        else if (section == PortWireTypeColumns::TYPE_DEFINITIONS)
        {
            return tr("Type definitions");
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::data()
//-----------------------------------------------------------------------------
QVariant PortWireTypeModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0)
    {
		return QVariant();
	}

    QSharedPointer<WireTypeDef> typeDefinition = getIndexedWireTypeDefinition(index);

    if (typeDefinition)
    {
        if (role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
        {
            if (index.column() == PortWireTypeColumns::TYPE)
            {
                if (!index.parent().isValid())
                {
                    return typeDefinition->getTypeName();
                }
                else
                {
                    return typeDefinition->getViewRefs()->at(index.row());
                }
            }
            else if (!index.parent().isValid() && index.column() == PortWireTypeColumns::TYPE_DEFINITIONS)
            {
                return typeDefinition->getTypeDefinitions()->join(PortWireConstants::DEFINITIONSEPARATOR);
            }
        }
        else if (Qt::ForegroundRole == role)
        {
            return blackForValidRedForInvalid(index, typeDefinition);
        }
        else if (!index.parent().isValid() && index.column() == PortWireTypeColumns::TYPE && role == Qt::FontRole)
        {
            QFont font;
            font.setBold(true);
            return font;
        }
    }
    
    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::setData()
//-----------------------------------------------------------------------------
bool PortWireTypeModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0)
    {
		return false;
    }
    else if (data(index, Qt::DisplayRole) == value)
    {
        return true;
    }

    QSharedPointer<WireTypeDef> singleWireTypeDefinition = getIndexedWireTypeDefinition(index);

	if (role == Qt::EditRole)
    {
        if (index.column() == PortWireTypeColumns::TYPE)
        {
            if (!index.parent().isValid())
            {
                singleWireTypeDefinition->setTypeName(value.toString());
            }
            else
            {
                QSharedPointer<QStringList> viewReferences = singleWireTypeDefinition->getViewRefs();
                viewReferences->replace(index.row(), value.toString());
            }
        }
        else if (index.column() == PortWireTypeColumns::TYPE_DEFINITIONS)
        {
            QString typeDefinitionString = value.toString();
            QSharedPointer<QStringList> newTypeDefinitions(new QStringList(
                typeDefinitionString.split(PortWireConstants::DEFINITIONSEPARATOR)));
            singleWireTypeDefinition->setTypeDefinitions(newTypeDefinitions);
        }

        emit dataChanged(index, index);
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::getIndexedWireTypeDefinition()
//-----------------------------------------------------------------------------
QSharedPointer<WireTypeDef> PortWireTypeModel::getIndexedWireTypeDefinition(QModelIndex index) const
{
    if (index.isValid())
    {
        if (!index.parent().isValid())
        {
            return wireTypeDefinitions_->at(index.row());
        }
        else
        {
            QModelIndex typeNameIndex = index.parent().sibling(index.parent().row(), PortWireTypeColumns::TYPE);
            QString typeName = wireTypeDefinitions_->at(typeNameIndex.row())->getTypeName();
            QSharedPointer<QStringList> definitionList =
                wireTypeDefinitions_->at(typeNameIndex.row())->getTypeDefinitions();

            foreach (QSharedPointer<WireTypeDef> typeDefinition, *wireTypeDefinitions_)
            {
                if (typeDefinition->getTypeName() == typeName &&
                    typeDefinition->getTypeDefinitions() == definitionList)
                {
                    return typeDefinition;
                }
            }
        }
    }

    return QSharedPointer<WireTypeDef>();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::onAddItems()
//-----------------------------------------------------------------------------
void PortWireTypeModel::onAddItems(QModelIndexList const& indexList)
{
    int typeAdditions = 1;

    if (!indexList.isEmpty())
    {
        int previousRow = indexList.first().row();
        foreach (QModelIndex singleIndex, indexList)
        {
            if (singleIndex.isValid() && singleIndex.row() != previousRow)
            {
                ++typeAdditions;
            }

            previousRow = singleIndex.row();
        }
    }

    int typeCount = wireTypeDefinitions_->count();

    beginInsertRows(QModelIndex(), typeCount, typeCount + typeAdditions);

    for (int i = 0; i < typeAdditions; ++i)
    {
        wireTypeDefinitions_->insert(typeCount, QSharedPointer<WireTypeDef>(new WireTypeDef()));
    }

    endInsertRows();

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::onAddViewReferences()
//-----------------------------------------------------------------------------
void PortWireTypeModel::onAddViewReferences(QModelIndexList const& indexList)
{
    bool viewReferenceCreated = false;

    foreach (QModelIndex singleIndex, indexList)
    {
        if (singleIndex.isValid() && !singleIndex.parent().isValid())
        {
            QSharedPointer<WireTypeDef> typeDefinition = getIndexedWireTypeDefinition(singleIndex);
            int newViewReferenceRow = typeDefinition->getViewRefs()->count();

            beginInsertRows(singleIndex, newViewReferenceRow, newViewReferenceRow);
            typeDefinition->getViewRefs()->append(QString(""));
            endInsertRows();

            viewReferenceCreated = true;
        }
    }

    if (viewReferenceCreated)
    {
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::onRemoveSelectedItems()
//-----------------------------------------------------------------------------
void PortWireTypeModel::onRemoveSelectedItems(QModelIndexList const& indexList)
{
    QModelIndexList childIndexList;
    QModelIndexList parentIndexList;

    foreach (QModelIndex singleIndex, indexList)
    {
        if (singleIndex.isValid())
        {
            if (singleIndex.parent().isValid())
            {
                childIndexList.append(singleIndex);
            }
            else
            {
                parentIndexList.append(singleIndex);
            }
        }
    }

    if (childIndexList.isEmpty() && parentIndexList.isEmpty())
    {
        return;
    }

    for (int i = childIndexList.size() - 1; i >= 0; i--)
    {
        QModelIndex childIndex = childIndexList.at(i);
        QSharedPointer<WireTypeDef> parentTypeDefinition = wireTypeDefinitions_->at(childIndex.parent().row());
        beginRemoveRows(childIndex.parent(), childIndex.row(), childIndex.row());
        parentTypeDefinition->getViewRefs()->removeAt(childIndex.row());
        endRemoveRows();
    }

    for (int i = parentIndexList.size() - 1; i >= 0; i--)
    {
        QModelIndex parentIndex = parentIndexList.at(i);
        beginRemoveRows(QModelIndex(), parentIndex.row(), parentIndex.row());
        wireTypeDefinitions_->removeAt(parentIndex.row());
        endRemoveRows();
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::onRemoveAllViewsFromSelectedTypes()
//-----------------------------------------------------------------------------
void PortWireTypeModel::onRemoveAllViewsFromSelectedTypes(QModelIndexList const& indexList)
{
    bool changeInContent = false;

    beginResetModel();

    foreach (QModelIndex parentIndex, indexList)
    {
        if (parentIndex.isValid() && !parentIndex.parent().isValid())
        {
            QSharedPointer<WireTypeDef> typeDefinition = getIndexedWireTypeDefinition(parentIndex);
            typeDefinition->getViewRefs()->clear();

            changeInContent = true;
        }
    }

    endResetModel();

    if (changeInContent)
    {
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::onChangePortSelection()
//-----------------------------------------------------------------------------
void PortWireTypeModel::onChangePortSelection(QSharedPointer<Port> newPort)
{
    beginResetModel();

    if (wireTypeDefinitions_)
    {
        wireTypeDefinitions_ = QSharedPointer<QList<QSharedPointer<WireTypeDef> > >();
    }

    if (newPort && newPort->getWire())
    {
        wireTypeDefinitions_ = newPort->getWire()->getWireTypeDefs();
    }

    endResetModel();
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags PortWireTypeModel::flags(const QModelIndex& index) const
{
    if (!index.isValid() ||
        (index.isValid() && index.parent().isValid() && index.column() == PortWireTypeColumns::TYPE_DEFINITIONS))
    {
        return Qt::NoItemFlags;
    }

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::blackForValidRedForInvalid()
//-----------------------------------------------------------------------------
QVariant PortWireTypeModel::blackForValidRedForInvalid(QModelIndex const& index,
    QSharedPointer<WireTypeDef> typeDefinition) const
{
    if (validateIndex(index, typeDefinition))
    {
        return KactusColors::REGULAR_TEXT;
    }
    else
    {
        return KactusColors::ERROR;
    }
}

//-----------------------------------------------------------------------------
// Function: PortWireTypeModel::validateIndex()
//-----------------------------------------------------------------------------
bool PortWireTypeModel::validateIndex(QModelIndex const& index, QSharedPointer<WireTypeDef> typeDefinition) const
{
    if (index.column() == PortWireTypeColumns::TYPE)
    {
        if (!index.parent().isValid())
        {
            return typeValidator_->validate(typeDefinition, wireTypeDefinitions_);
        }
        else
        {            
            return typeValidator_->hasValidView(typeDefinition->getViewRefs()->at(index.row()), wireTypeDefinitions_);
        }
    }

    return true;
}

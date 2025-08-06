//-----------------------------------------------------------------------------
// File: SubspaceMapModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.02.2022
//
// Description:
// The model to manage the subspace maps of a single memory map.
//-----------------------------------------------------------------------------

#include "SubspaceMapModel.h"

#include <editors/ComponentEditor/memoryMaps/SubspaceMapColumns.h>
#include <KactusAPI/include/SubspaceMapInterface.h>

#include <common/KactusColors.h>

#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::SubspaceMapModel()
//-----------------------------------------------------------------------------
SubspaceMapModel::SubspaceMapModel(SubspaceMapInterface* subspaceInterface,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ParameterFinder> parameterFinder,
    Document::Revision docRevision, QObject *parent):
MemoryBlockModel(subspaceInterface, expressionParser, parameterFinder, docRevision, parent),
subspaceInterface_(subspaceInterface)
{

}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::columnCount()
//-----------------------------------------------------------------------------
int SubspaceMapModel::columnCount(QModelIndex const& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return SubspaceMapColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags SubspaceMapModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    if (index.column() == SubspaceMapColumns::SEGMENTREFERENCE)
    {
        std::string subspaceName = subspaceInterface_->getIndexedItemName(index.row());
        if (!subspaceName.empty())
        {
            std::string masterReference = subspaceInterface_->getInitiatorReference(subspaceName);
            std::string segmentReference = subspaceInterface_->getSegmentReference(subspaceName);
            if (masterReference.empty() && segmentReference.empty())
            {
                return Qt::ItemIsEnabled;
            }
        }
    }

    return MemoryBlockModel::flags(index);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::headerData()
//-----------------------------------------------------------------------------
QVariant SubspaceMapModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (Qt::DisplayRole == role)
    {
        if (section == SubspaceMapColumns::INITIATORREFERENCE)
        {
            QString initiatorHeader = docRevision_ == Document::Revision::Std22 
                ? tr("Initiator\ninterface") : tr("Master\ninterface");

            return initiatorHeader;
        }
        else if (section == SubspaceMapColumns::SEGMENTREFERENCE)
        {
            QString segmentHeader = tr("Segment");
            return segmentHeader;
        }
    }

    return MemoryBlockModel::headerData(section, orientation, role);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::data()
//-----------------------------------------------------------------------------
QVariant SubspaceMapModel::data(QModelIndex const& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= subspaceInterface_->itemCount())
    {
        return QVariant();
    }

    std::string blockName = subspaceInterface_->getIndexedItemName(index.row());

    if (role == Qt::BackgroundRole && index.column() == SubspaceMapColumns::INITIATORREFERENCE)
    {
        return KactusColors::MANDATORY_FIELD;
    }

    return MemoryBlockModel::data(index, role);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::setData()
//-----------------------------------------------------------------------------
bool SubspaceMapModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= subspaceInterface_->itemCount())
    {
        return false;
    }

    if (Qt::EditRole == role)
    {
        std::string blockName = subspaceInterface_->getIndexedItemName(index.row());

        if (index.column() == SubspaceMapColumns::INITIATORREFERENCE)
        {
            if (!subspaceInterface_->setMasterReference(blockName, value.toString().toStdString()))
            {
                return false;
            }
        }
        else if (index.column() == SubspaceMapColumns::SEGMENTREFERENCE)
        {
            if (!subspaceInterface_->setSegmentReference(blockName, value.toString().toStdString()))
            {
                return false;
            }
        }
        else
        {
            return MemoryBlockModel::setData(index, value, role);
        }

        if (index.column() == SubspaceMapColumns::INITIATORREFERENCE ||
            index.column() == SubspaceMapColumns::SEGMENTREFERENCE)
        {
            emit childAddressingChanged(index.row(), false);
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
// Function: SubspaceMapModel::validateIndex()
//-----------------------------------------------------------------------------
bool SubspaceMapModel::validateIndex(QModelIndex const& index) const
{
    std::string blockName = subspaceInterface_->getIndexedItemName(index.row());
    if (index.column() == SubspaceMapColumns::INITIATORREFERENCE)
    {
        return subspaceInterface_->hasValidMasterReference(blockName);
    }
    else if (index.column() == SubspaceMapColumns::SEGMENTREFERENCE)
    {
        return subspaceInterface_->hasValidSegmentReference(blockName);
    }

    return MemoryBlockModel::validateIndex(index);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant SubspaceMapModel::valueForIndex(QModelIndex const& index) const
{
    std::string blockName = subspaceInterface_->getIndexedItemName(index.row());

    if (index.column() == SubspaceMapColumns::INITIATORREFERENCE)
    {
        return QString::fromStdString(subspaceInterface_->getInitiatorReference(blockName));
    }
    else if (index.column() == SubspaceMapColumns::SEGMENTREFERENCE)
    {
        return QString::fromStdString(subspaceInterface_->getSegmentReference(blockName));
    }

    return MemoryBlockModel::valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool SubspaceMapModel::isValidExpressionColumn(QModelIndex const& index) const
{
    int column = index.column();
    return column == SubspaceMapColumns::BASE || column == SubspaceMapColumns::IS_PRESENT;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList SubspaceMapModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:subspaceMap";

    return types;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::nameColumn()
//-----------------------------------------------------------------------------
int SubspaceMapModel::nameColumn() const
{
    return SubspaceMapColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::baseAddressColumn()
//-----------------------------------------------------------------------------
int SubspaceMapModel::baseAddressColumn() const
{
    return SubspaceMapColumns::BASE;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::isPresentColumn()
//-----------------------------------------------------------------------------
int SubspaceMapModel::isPresentColumn() const
{
    return SubspaceMapColumns::IS_PRESENT;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapModel::descriptionColumn()
//-----------------------------------------------------------------------------
int SubspaceMapModel::descriptionColumn() const
{
    return SubspaceMapColumns::DESCRIPTION_COLUMN;
}

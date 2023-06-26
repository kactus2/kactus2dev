//-----------------------------------------------------------------------------
// File: RegionsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.02.2012
//
// Description:
// The model that can be used to display the segments to be edited.
//-----------------------------------------------------------------------------

#include "RegionsModel.h"
#include "RegionColumns.h"

#include <KactusAPI/include/ExpressionFormatter.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/Component/Region.h>

#include <IPXACTmodels/Component/validators/RegionValidator.h>


#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include <common/KactusColors.h>

//-----------------------------------------------------------------------------
// Function: RegionsModel::RegionsModel()
//-----------------------------------------------------------------------------
RegionsModel::RegionsModel(QSharedPointer<QList<QSharedPointer<Region> > > regions,
    QSharedPointer<RegionValidator> validator,
    ExpressionSet expressions, QObject *parent):
ReferencingTableModel(expressions.finder, parent),
ParameterizableTable(expressions.finder),
regions_(regions),
validator_(validator),
expressionFormatter_(expressions.formatter)
{
    setExpressionParser(expressions.parser);
	Q_ASSERT(regions_);
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::rowCount()
//-----------------------------------------------------------------------------
int RegionsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return regions_->count();
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::columnCount()
//-----------------------------------------------------------------------------
int RegionsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return RegionColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::setData()
//-----------------------------------------------------------------------------
QVariant RegionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (section == RegionColumns::NAME)
        {
            return tr("Name");
        }
        else if (section == RegionColumns::DISPLAY_NAME)
        {
            return tr("Display\nname");
        }
        else if (section == RegionColumns::SHORT_DESCRIPTION)
        {
            return tr("Short\ndescription");
        }
        else if (section == RegionColumns::OFFSET)
        {
            return tr("Offset\n[AUB]") + getExpressionSymbol();
        }
        else if (section == RegionColumns::RANGE)
        {
            return tr("Range\n[AUB]") + getExpressionSymbol();
        }
        else if (section == RegionColumns::DESCRIPTION)
        {
            return tr("Description");
        }
        else
        {
            return QVariant();
        }
    }
    else // if unsupported role
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::data()
//-----------------------------------------------------------------------------
QVariant RegionsModel::data( QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= regions_->size())
    {
		return QVariant();
    }

    if (role == Qt::DisplayRole)
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
	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == RegionColumns::NAME || index.column() ==  RegionColumns::OFFSET ||
            index.column() == RegionColumns::RANGE)
        {            
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
    }
	else if (role == Qt::ForegroundRole)
    {
        return blackForValidOrRedForInvalidIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedValueFor(expressionOrValueForIndex(index).toString());
        }

        return expressionOrValueForIndex(index);
    }
	else // if unsupported role
    {
		return QVariant();
	}
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::setData()
//-----------------------------------------------------------------------------
bool RegionsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= regions_->size())
    {
		return false;
    }

    if (role == Qt::EditRole) 
    {
        QSharedPointer<Region> targetRegion = regions_->at(index.row());
        if (index.column() ==  RegionColumns::NAME)
        {
            QString oldName = targetRegion->name();
            targetRegion->setName(value.toString());

            emit regionRenamed(oldName, value.toString());
        }
        else if (index.column() == RegionColumns::DISPLAY_NAME)
        {
            targetRegion->setDisplayName(value.toString());

            emit regionChanged(targetRegion);
        }
        else if (index.column() == RegionColumns::SHORT_DESCRIPTION)
        {
            targetRegion->setShortDescription(value.toString());

            emit regionChanged(targetRegion);
        }
        else if (index.column() == RegionColumns::OFFSET)
        {
            targetRegion->setOffset(value.toString());
            emit regionChanged(targetRegion);
        }
        else if (index.column() == RegionColumns::RANGE)
        {
            targetRegion->setRange(value.toString());
            emit regionChanged(targetRegion);
        }
        else if (index.column() == RegionColumns::DESCRIPTION)
        {
            targetRegion->setDescription(value.toString());
        }
        else
        {
            return false;
        }

        emit dataChanged(index, index);
        return true;
    }
	else // is unsupported role
    {
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags RegionsModel::flags( QModelIndex const& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::onAddItem()
//-----------------------------------------------------------------------------
void RegionsModel::onAddItem(QModelIndex const& index)
{
	int row = regions_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	// the last segmented address
	quint64 previousEnd = 0;
    
	// if this is the first item to add then do not increase address
	if (!regions_->isEmpty())
    {
		previousEnd = getLastRegionedAddress() + 1;
	}

	// convert the address to hexadecimal form
	QString newBase = QString::number(previousEnd, 16);
	newBase = newBase.toUpper();
	newBase.prepend("'h");

	QSharedPointer<Region> segment(new Region());
	segment->setOffset(newBase);

	beginInsertRows(QModelIndex(), row, row);
	regions_->insert(row, segment);
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();

	emit regionAdded(segment);
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void RegionsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= regions_->size())
    {
		return;
	}

    removeReferencesInItemOnRow(index.row());

	emit regionRemoved(regions_->at(index.row())->name());

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
    removeReferencesInItemOnRow(index.row());
	regions_->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool RegionsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == RegionColumns::OFFSET || index.column() == RegionColumns::RANGE;
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant RegionsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == RegionColumns::NAME)
    {
        return regions_->at(index.row())->name();
    }
    else if (index.column() == RegionColumns::DISPLAY_NAME)
    {
        return regions_->at(index.row())->displayName();
    }
    else if (index.column() == RegionColumns::SHORT_DESCRIPTION)
    {
        return regions_->at(index.row())->shortDescription();
    }
    else if (index.column() == RegionColumns::OFFSET)
    {
        return regions_->at(index.row())->getAddressOffset();
    }
    else if (index.column() ==  RegionColumns::RANGE)
    {
        return regions_->at(index.row())->getRange();
    }
    else if (index.column() ==  RegionColumns::DESCRIPTION)
    {
        return regions_->at(index.row())->description();
    }
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::validateIndex()
//-----------------------------------------------------------------------------
bool RegionsModel::validateIndex(QModelIndex const& index) const
{
    if (index.column() == RegionColumns::NAME)
    {
        QStringList segmentNames;
        for (QSharedPointer<Region> segment : *regions_)
        {
            segmentNames.append(segment->name());
        }

        return segmentNames.count(regions_->at(index.row())->name()) == 1;
    }
    else if (index.column() == RegionColumns::OFFSET)
    {
        return validator_->hasValidAddressOffset(regions_->at(index.row()));
    }
    else if (index.column() ==  RegionColumns::RANGE)
    {
        return validator_->hasValidRange(regions_->at(index.row()));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int RegionsModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    int referencesInOffset = regions_->at(row)->getAddressOffset().count(valueID);
    int referencesInRange = regions_->at(row)->getRange().count(valueID);

    return referencesInOffset + referencesInRange;
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::getLastRegionedAddress()
//-----------------------------------------------------------------------------
quint64 RegionsModel::getLastRegionedAddress() const
{
    quint64 lastAddress = 0;
    for (QSharedPointer<Region> segment : *regions_)
    {
        quint64 segmentOffset = parseExpressionToDecimal(segment->getAddressOffset()).toUInt();
        quint64 segmentSize = qMax(parseExpressionToDecimal(segment->getRange()).toUInt(), uint(1));
        
        lastAddress = qMax(lastAddress, segmentOffset + segmentSize - 1);
    }

    return lastAddress;
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::onCopyRows()
//-----------------------------------------------------------------------------
void RegionsModel::onCopyRows(QModelIndexList indexList)
{
    QList<QSharedPointer<Region> > copiedRegions;
    for (QModelIndex const& index : indexList)
    {
        QSharedPointer<Region> segment = regions_->at(index.row());
        copiedRegions.append(segment);
    }

    QVariant addressSpaceVariant;
    addressSpaceVariant.setValue(copiedRegions);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:region", QByteArray());
    newMimeData->setImageData(addressSpaceVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::onPasteRows()
//-----------------------------------------------------------------------------
void RegionsModel::onPasteRows()
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<Region> > >())
        {
            ReferenceCalculator referenceCalculator(getParameterFinder());

            QList<QSharedPointer<Region> > newRegions = pasteVariant.value<QList<QSharedPointer<Region> > >();

            int rowBegin = regions_->size();
            int rowEnd = rowBegin + newRegions.size() - 1;

            beginInsertRows(QModelIndex(), rowBegin, rowEnd);

            for (QSharedPointer<Region> copyRegion : newRegions)
            {
                QSharedPointer<Region> newRegion (new Region(*copyRegion));
                newRegion->setName(getUniqueName(newRegion->name(), getCurrentItemNames()));

                regions_->append(newRegion);

                increaseReferencesInPastedRegion(newRegion, referenceCalculator);

                emit regionAdded(newRegion);
            }

            endInsertRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::getCurrentItemNames()
//-----------------------------------------------------------------------------
QStringList RegionsModel::getCurrentItemNames()
{
    QStringList names;
    for (QSharedPointer<Region> segment : *regions_)
    {
        names.append(segment->name());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: RegionsModel::increaseReferencesInPastedRegion()
//-----------------------------------------------------------------------------
void RegionsModel::increaseReferencesInPastedRegion(QSharedPointer<Region> pastedRegion,
    ReferenceCalculator& referenceCalculator)
{
    QStringList segmentExpressions;
    segmentExpressions.append(pastedRegion->getAddressOffset());
    segmentExpressions.append(pastedRegion->getRange());

    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(segmentExpressions);

    QMapIterator<QString, int> refParameterIterator (referencedParameters);
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
// Function: RegionsModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList RegionsModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:region";

    return types;
}

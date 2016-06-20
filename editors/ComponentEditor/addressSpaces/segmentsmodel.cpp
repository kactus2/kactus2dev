//-----------------------------------------------------------------------------
// File: segmentsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 22.02.2012
//
// Description:
// The model that can be used to display the segments to be edited.
//-----------------------------------------------------------------------------

#include "segmentsmodel.h"
#include "SegmentColumns.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/Component/AddressSpace.h>
#include <IPXACTmodels/Component/Segment.h>

#include <QColor>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: SegmentsModel::SegmentsModel()
//-----------------------------------------------------------------------------
SegmentsModel::SegmentsModel(QSharedPointer<AddressSpace> addrSpace,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QObject *parent):
ReferencingTableModel(parameterFinder, parent),
ParameterizableTable(parameterFinder),
addrSpace_(addrSpace),
segments_(addrSpace->getSegments()),
expressionFormatter_(expressionFormatter)
{
	Q_ASSERT(addrSpace);
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::~SegmentsModel()
//-----------------------------------------------------------------------------
SegmentsModel::~SegmentsModel()
{

}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::rowCount()
//-----------------------------------------------------------------------------
int SegmentsModel::rowCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return segments_->count();
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::columnCount()
//-----------------------------------------------------------------------------
int SegmentsModel::columnCount(QModelIndex const& parent) const
{
	if (parent.isValid())
    {
		return 0;
	}
	return SegmentColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::setData()
//-----------------------------------------------------------------------------
QVariant SegmentsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal)
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        if (section == SegmentColumns::NAME)
        {
            return tr("Name");
        }
        else if (section == SegmentColumns::OFFSET)
        {
            return tr("Offset [AUB]") + getExpressionSymbol();
        }
        else if (section == SegmentColumns::RANGE)
        {
            return tr("Range [AUB]") + getExpressionSymbol();
        }
        else if (section == SegmentColumns::DESCRIPTION)
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
// Function: SegmentsModel::data()
//-----------------------------------------------------------------------------
QVariant SegmentsModel::data( QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= segments_->size())
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
        if (index.column() == SegmentColumns::NAME || index.column() ==  SegmentColumns::OFFSET ||
            index.column() == SegmentColumns::RANGE)
        {            
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
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
// Function: SegmentsModel::setData()
//-----------------------------------------------------------------------------
bool SegmentsModel::setData(QModelIndex const& index, QVariant const& value, int role)
{
	if (!index.isValid() || index.row() < 0 || index.row() >= segments_->size())
    {
		return false;
    }

    if (role == Qt::EditRole) 
    {
        QSharedPointer<Segment> targetSegment = segments_->at(index.row());
        if (index.column() ==  SegmentColumns::NAME)
        {
            QString oldName = targetSegment->name();
            targetSegment->setName(value.toString());

            emit segmentRenamed(oldName, value.toString());
        }
        else if (index.column() == SegmentColumns::OFFSET)
        {
            targetSegment->setOffset(value.toString());
            emit segmentChanged(targetSegment);
        }
        else if (index.column() == SegmentColumns::RANGE)
        {
            targetSegment->setRange(value.toString());
            emit segmentChanged(targetSegment);
        }
        else if (index.column() == SegmentColumns::DESCRIPTION)
        {
            targetSegment->setDescription(value.toString());
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
// Function: SegmentsModel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags SegmentsModel::flags( QModelIndex const& index ) const
{
	if (!index.isValid())
    {
		return Qt::NoItemFlags;
    }

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::isValid()
//-----------------------------------------------------------------------------
bool SegmentsModel::isValid() const
{	
	// check all segments
	foreach (QSharedPointer<Segment> segment, *segments_)
    {
		/*if (!segment->isValid())
        {
			return false;
		}*/
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::onAddItem()
//-----------------------------------------------------------------------------
void SegmentsModel::onAddItem(QModelIndex const& index)
{
	int row = segments_->size();

	// if the index is valid then add the item to the correct position
	if (index.isValid())
    {
		row = index.row();
	}

	// the last segmented address
	quint64 previousEnd = 0;
    
	// if this is the first item to add then do not increase address
	if (!segments_->isEmpty())
    {
		previousEnd = getLastSegmentedAddress() + 1;
	}

	// convert the address to hexadecimal form
	QString newBase = QString::number(previousEnd, 16);
	newBase = newBase.toUpper();
	newBase.prepend("'h");

	QSharedPointer<Segment> segment(new Segment());
	segment->setOffset(newBase);

	beginInsertRows(QModelIndex(), row, row);
	segments_->insert(row, segment);
	endInsertRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();

	emit segmentAdded(segment);
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::onRemoveItem()
//-----------------------------------------------------------------------------
void SegmentsModel::onRemoveItem(QModelIndex const& index)
{
	// don't remove anything if index is invalid
	if (!index.isValid() || index.row() < 0 || index.row() >= segments_->size())
    {
		return;
	}

    removeReferencesInItemOnRow(index.row());

	emit segmentRemoved(segments_->at(index.row())->name());

	// remove the specified item
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	segments_->removeAt(index.row());
	endRemoveRows();

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool SegmentsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == SegmentColumns::OFFSET || index.column() == SegmentColumns::RANGE;
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant SegmentsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (index.column() == SegmentColumns::NAME)
    {
        return segments_->at(index.row())->name();
    }
    else if (index.column() == SegmentColumns::OFFSET)
    {
        return segments_->at(index.row())->getAddressOffset();
    }
    else if (index.column() ==  SegmentColumns::RANGE)
    {
        return segments_->at(index.row())->getRange();
    }
    else if (index.column() ==  SegmentColumns::DESCRIPTION)
    {
        return segments_->at(index.row())->description();
    }
    else 
    {
        return QVariant();
    }
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::validateIndex()
//-----------------------------------------------------------------------------
bool SegmentsModel::validateIndex(QModelIndex const& index) const
{
    if (index.column() == SegmentColumns::NAME)
    {
        QStringList segmentNames;
        foreach (QSharedPointer<Segment> segment, *segments_)
        {
            segmentNames.append(segment->name());
        }

        return segmentNames.count(segments_->at(index.row())->name()) == 1;
    }
    else if (index.column() == SegmentColumns::OFFSET)
    {
        QString offset = segments_->at(index.row())->getAddressOffset();
        return isValuePlainOrExpression(offset);
    }
    else if (index.column() ==  SegmentColumns::RANGE)
    {
        QString range = segments_->at(index.row())->getRange();
        return isValuePlainOrExpression(range);
    }
    else if (index.column() ==  SegmentColumns::DESCRIPTION)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::getAllReferencesToIdInItemOnRow()
//-----------------------------------------------------------------------------
int SegmentsModel::getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const
{
    int referencesInOffset = segments_->at(row)->getAddressOffset().count(valueID);
    int referencesInRange = segments_->at(row)->getRange().count(valueID);

    return referencesInOffset + referencesInRange;
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::getLastSegmentedAddress()
//-----------------------------------------------------------------------------
quint64 SegmentsModel::getLastSegmentedAddress() const
{
    quint64 lastAddress = 0;
    foreach (QSharedPointer<Segment> segment, *segments_)
    {
        quint64 segmentOffset = parseExpressionToDecimal(segment->getAddressOffset()).toUInt();
        quint64 segmentSize = qMax(parseExpressionToDecimal(segment->getRange()).toUInt(), uint(1));
        
        lastAddress = qMax(lastAddress, segmentOffset + segmentSize - 1);
    }

    return lastAddress;
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::onCopyRows()
//-----------------------------------------------------------------------------
void SegmentsModel::onCopyRows(QModelIndexList indexList)
{
    QList<QSharedPointer<Segment> > copiedSegments;
    foreach (QModelIndex index, indexList)
    {
        QSharedPointer<Segment> segment = segments_->at(index.row());
        copiedSegments.append(segment);
    }

    QVariant addressSpaceVariant;
    addressSpaceVariant.setValue(copiedSegments);

    QMimeData* newMimeData = new QMimeData();
    newMimeData->setData("text/xml/ipxact:segment", QByteArray());
    newMimeData->setImageData(addressSpaceVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::onPasteRows()
//-----------------------------------------------------------------------------
void SegmentsModel::onPasteRows()
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<Segment> > >())
        {
            ReferenceCalculator referenceCalculator(getParameterFinder());

            QList<QSharedPointer<Segment> > newSegments = pasteVariant.value<QList<QSharedPointer<Segment> > >();

            int rowBegin = segments_->size();
            int rowEnd = rowBegin + newSegments.size() - 1;

            beginInsertRows(QModelIndex(), rowBegin, rowEnd);

            foreach(QSharedPointer<Segment> copySegment, newSegments)
            {
                QSharedPointer<Segment> newSegment (new Segment(*copySegment.data()));
                newSegment->setName(getUniqueName(newSegment->name(), getCurrentItemNames()));

                segments_->append(newSegment);

                increaseReferencesInPastedSegment(newSegment, referenceCalculator);

                emit segmentAdded(newSegment);
            }

            endInsertRows();

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::getCurrentItemNames()
//-----------------------------------------------------------------------------
QStringList SegmentsModel::getCurrentItemNames()
{
    QStringList names;
    foreach (QSharedPointer<Segment> segment, *segments_)
    {
        names.append(segment->name());
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: SegmentsModel::increaseReferencesInPastedSegment()
//-----------------------------------------------------------------------------
void SegmentsModel::increaseReferencesInPastedSegment(QSharedPointer<Segment> pastedSegment,
    ReferenceCalculator& referenceCalculator)
{
    QStringList segmentExpressions;
    segmentExpressions.append(pastedSegment->getAddressOffset());
    segmentExpressions.append(pastedSegment->getRange());

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
// Function: SegmentsModel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList SegmentsModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:segment";

    return types;
}

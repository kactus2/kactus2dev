//-----------------------------------------------------------------------------
// File: memorymapsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 21.08.2012
//
// Description:
// The model to manage the memory maps summary.
//-----------------------------------------------------------------------------

#include "memorymapsmodel.h"

#include "MemoryMapsColumns.h"

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/MemoryMapExpressionsGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/MemoryRemapExpressionGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>

#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>

#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::MemoryMapsModel()
//-----------------------------------------------------------------------------
MemoryMapsModel::MemoryMapsModel(QSharedPointer<Component> component,
    QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    QSharedPointer<MemoryMapValidator> memoryMapValidator,
    QObject *parent):
QAbstractItemModel(parent),
    ParameterizableTable(parameterFinder),
    component_(component),
    rootMemoryMaps_(component->getMemoryMaps()),
    expressionFormatter_(expressionFormatter),
    memoryMapValidator_(memoryMapValidator)
{
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::MemoryMapsModel()
//-----------------------------------------------------------------------------
MemoryMapsModel::~MemoryMapsModel()
{

}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::rowCount()
//-----------------------------------------------------------------------------
int MemoryMapsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (!parent.isValid())
    {
        return rootMemoryMaps_->size();
    }

    else if (!parent.parent().isValid())
    {
        return rootMemoryMaps_->at(parent.row())->getMemoryRemaps()->size();
    }

    else
    {
        return 0;
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::columnCount()
//-----------------------------------------------------------------------------
int MemoryMapsModel::columnCount(QModelIndex const& /*parent*/) const
{
    return MemoryMapsColumns::COLUMN_COUNT;
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::flags()
//-----------------------------------------------------------------------------
Qt::ItemFlags MemoryMapsModel::flags(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

	// interface references are made in bus interface editor
    if (index.column() == MemoryMapsColumns::INTERFACE_COLUMN)
    {
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}

    else if ((!index.parent().isValid() && index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN) ||
        (index.parent().isValid() && index.column() == MemoryMapsColumns::AUB_COLUMN))
    {
        return Qt::ItemIsSelectable;
    }

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::headerData()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::headerData(int section, Qt::Orientation orientation, int role)
    const
{
	if (orientation != Qt::Horizontal)
    {
		return QVariant();
	}
	if (role == Qt::DisplayRole)
    {
        if (section == MemoryMapsColumns::NAME_COLUMN)
        {
            return tr("Memory map\nname");
        }
        else if (section == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            return tr("Remap state");
        }
        else if (section == MemoryMapsColumns::AUB_COLUMN)
        {
            return tr("Address unit\nbits (AUB)");
        }
        else if (section == MemoryMapsColumns::INTERFACE_COLUMN)
        {
            return tr("Slave interface\nbinding");
        }
        else if (section == MemoryMapsColumns::IS_PRESENT)
        {
            return tr("Is present") + getExpressionSymbol();
        }
        else if (section == MemoryMapsColumns::DESCRIPTION_COLUMN)
        {
            return tr("Description");
        }
	}
    else if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::data()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || !isIndexValid(index))
    {
		return QVariant();
	}

    if (Qt::DisplayRole == role) 
    {
        if (isValidExpressionColumn(index))
        {
            return expressionFormatter_->formatReferringExpression(valueForIndex(index).toString());
        }
        else if (index.column() == MemoryMapsColumns::DESCRIPTION_COLUMN)
        {
            return valueForIndex(index).toString().replace(QRegularExpression("\n.*$", 
                QRegularExpression::DotMatchesEverythingOption), "...");
        }
        else
        {
            return valueForIndex(index);
        }
    }
    else if (role == Qt::EditRole)
    {
        if (index.column() == MemoryMapsColumns::DESCRIPTION_COLUMN)
        {
            return valueForIndex(index);
        }

        return valueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        if (isValidExpressionColumn(index))
        {
            return formattedValueFor(valueForIndex(index).toString());
        }
        else
        {
            return valueForIndex(index).toString();
        }
    }
	else if (role == Qt::UserRole)
    {
        QSharedPointer<MemoryMap> parentMemoryMap =
            getParentMemoryMap(getIndexedMemoryRemap(index.parent(), index.row()));

        if (!index.parent().isValid())
        {
            parentMemoryMap = rootMemoryMaps_->at(index.row());
        }

        return component_->getSlaveInterfaces(parentMemoryMap->name());
	}
	else if (role == Qt::ForegroundRole)
    {
        if(index.column() == MemoryMapsColumns::INTERFACE_COLUMN ||
            (!index.parent().isValid() && index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN) ||
            (index.parent().isValid() && index.column() == MemoryMapsColumns::AUB_COLUMN))
        {
            return QColor("gray");
        }

        return blackForValidOrRedForInvalidIndex(index);
	}
	else if (role == Qt::BackgroundRole)
    {
        if (index.column() == MemoryMapsColumns::NAME_COLUMN ||
            index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            return QColor("LemonChiffon");
        }
        else
        {
            return QColor("white");
        }
	}

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::setData()
//-----------------------------------------------------------------------------
bool MemoryMapsModel::setData( const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/ )
{
	if (!index.isValid() || !isIndexValid(index))
    {
		return false;
	}

	if (role == Qt::EditRole)
    {
        if (index.column() == MemoryMapsColumns::NAME_COLUMN)
        {
            if (!index.parent().isValid())
            {
                rootMemoryMaps_->at(index.row())->setName(value.toString());
            }
            else
            {
                QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(index.parent(), index.row());
                memoryRemap->setName(value.toString());
            }

            emit graphicsChanged();
        }
        else if (index.column() == MemoryMapsColumns::AUB_COLUMN)
        {
            if (!index.parent().isValid())
            {
                rootMemoryMaps_->at(index.row())->setAddressUnitBits(value.toString());
                emit aubChangedOnRow(index.row());
            }
        }
        else if (index.column() == MemoryMapsColumns::DESCRIPTION_COLUMN)
        {
            if (!index.parent().isValid())
            {
                rootMemoryMaps_->at(index.row())->setDescription(value.toString());
            }
            else
            {
                QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(index.parent(), index.row());
                memoryRemap->setDescription(value.toString());
            }
        }
        else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            if (index.parent().isValid())
            {
                QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(index.parent(), index.row());

                memoryRemap->setRemapState(value.toString());
            }
        }
        else if (index.column() == MemoryMapsColumns::IS_PRESENT)
        {
            if (!index.parent().isValid())
            {
                rootMemoryMaps_->at(index.row())->setIsPresent(value.toString());
            }
            else
            {
                QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(index.parent(), index.row());
                memoryRemap->setIsPresent(value.toString());
            }

            emit graphicsChanged();
        }

		emit dataChanged(index, index);
		emit contentChanged();
		return true;
	}

    return false;
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::onAddItem()
//-----------------------------------------------------------------------------
void MemoryMapsModel::onAddItem( const QModelIndex& index )
{
	int row = rootMemoryMaps_->size();

	if (index.isValid())
    {
		row = index.row();
	}

	beginInsertRows(QModelIndex(), row, row);
	rootMemoryMaps_->insert(row, QSharedPointer<MemoryMap>(new MemoryMap()));
	endInsertRows();

	// inform navigation tree that file set is added
	emit memoryMapAdded(row);

	// tell also parent widget that contents have been changed
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::onRemoveItem()
//-----------------------------------------------------------------------------
void MemoryMapsModel::onRemoveItem( const QModelIndex& index )
{
	if (!index.isValid() || !isIndexValid(index))
    {
		return;
	}

    if (!index.parent().isValid())
    {
        decreaseReferencesWithRemovedMemoryMap(rootMemoryMaps_->at(index.row()));

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        rootMemoryMaps_->removeAt(index.row());
        endRemoveRows();

        // inform navigation tree that file set has been removed
        emit memoryMapRemoved(index.row());

        // tell also parent widget that contents have been changed
        emit contentChanged();
    }
    else
    {
        QSharedPointer<MemoryRemap> removedMemoryRemap = getIndexedMemoryRemap(index.parent(), index.row());
        QSharedPointer<MemoryMap> parentMemoryMap = getParentMemoryMap(removedMemoryRemap);

        decreaseReferencesWithRemovedMemoryRemap(removedMemoryRemap);

        beginRemoveRows(index.parent(), index.row(), index.row());
        parentMemoryMap->getMemoryRemaps()->removeAt(index.row());
        endRemoveRows();

        emit memoryRemapRemoved(index.row(), parentMemoryMap);
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::decreaseReferencesWithRemovedMemoryMap()
//-----------------------------------------------------------------------------
void MemoryMapsModel::decreaseReferencesWithRemovedMemoryMap(QSharedPointer<MemoryMap> removedMemoryMap)
{
    MemoryMapExpressionGatherer memoryMapGatherer;

    QStringList expressionList = memoryMapGatherer.getExpressions(removedMemoryMap);

    ReferenceCalculator memoryMapReferenceCalculator(getParameterFinder());
    QMap<QString, int> referencedParameters = memoryMapReferenceCalculator.getReferencedParameters(expressionList);

    foreach (QString referencedId, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit decreaseReferences(referencedId);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::decreaseReferencesWithRemovedMemoryRemap()
//-----------------------------------------------------------------------------
void MemoryMapsModel::decreaseReferencesWithRemovedMemoryRemap(QSharedPointer<MemoryRemap> removedMemoryRemap)
{
    QMap<QString, int> referencedParameters = getReferencedParameters(removedMemoryRemap);

    foreach (QString referencedId, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit decreaseReferences(referencedId);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::onAddMemoryRemap()
//-----------------------------------------------------------------------------
void MemoryMapsModel::onAddMemoryRemap(const QModelIndex& index)
{
    if (!index.isValid() || !isIndexValid(index) || index.parent().isValid())
    {
        return;
    }

    QSharedPointer<MemoryMap> parentMemoryMap = rootMemoryMaps_->at(index.row());
    int rowOfNewItem = parentMemoryMap->getMemoryRemaps()->count();

    increaseReferencesWithNewRemap(parentMemoryMap);

    QSharedPointer<MemoryRemap> newMemoryRemap (new MemoryRemap);
    foreach (QSharedPointer<MemoryBlockBase> mapBlock, *parentMemoryMap->getMemoryBlocks())
    {
        QSharedPointer<MemoryBlockBase> remapBlock = mapBlock->clone();
        newMemoryRemap->getMemoryBlocks()->append(remapBlock);
    }

    beginInsertRows(index, rowOfNewItem, rowOfNewItem);
    parentMemoryMap->getMemoryRemaps()->insert(rowOfNewItem, newMemoryRemap);
    endInsertRows();

    emit memoryRemapAdded(rowOfNewItem, parentMemoryMap);
    emit contentChanged();
}


//-----------------------------------------------------------------------------
// Function: memorymapsmodel::increaseReferencesWithNewRemap()
//-----------------------------------------------------------------------------
void MemoryMapsModel::increaseReferencesWithNewRemap(QSharedPointer<MemoryMap> parentMemoryMap)
{
    QMap<QString, int> referencedParameters = getReferencedParameters(parentMemoryMap);

    foreach (QString referencedId, referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit increaseReferences(referencedId);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::getReferencedParameters()
//-----------------------------------------------------------------------------
QMap<QString, int> MemoryMapsModel::getReferencedParameters(QSharedPointer<MemoryMapBase> memoryMap) const
{
    MemoryRemapExpressionGatherer* memoryRemapGatherer = new MemoryRemapExpressionGatherer();

    QStringList expressionList = memoryRemapGatherer->getExpressions(memoryMap);

    ReferenceCalculator* memoryMapReferenceCalculator = new ReferenceCalculator(getParameterFinder());
    return memoryMapReferenceCalculator->getReferencedParameters(expressionList);
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::index()
//-----------------------------------------------------------------------------
QModelIndex MemoryMapsModel::index(int row, int column, const QModelIndex &parent /* = QModelIndex() */ ) const
{
    if (!parent.isValid() && 0 <= row && row < rootMemoryMaps_->size())
    {
        return createIndex(row, column, rootMemoryMaps_->at(row).data());
    }
    else
    {
        QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(parent, row);

        return createIndex(row, column, memoryRemap.data());
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::parent()
//-----------------------------------------------------------------------------
QModelIndex MemoryMapsModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    foreach (QSharedPointer<MemoryMap> rootItem, *rootMemoryMaps_)
    {
        if (rootItem.data() == child.internalPointer())
        {
            return QModelIndex();
        }
    }

    MemoryRemap* childItem = static_cast<MemoryRemap*>(child.internalPointer());
    return createParentIndexForMemoryRemap(childItem);
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::createParentIndexForChildItem()
//-----------------------------------------------------------------------------
QModelIndex MemoryMapsModel::createParentIndexForMemoryRemap(MemoryRemap* childItem) const
{
    for (int i = 0; i < rootMemoryMaps_->size(); ++i)
    {
        foreach (QSharedPointer<MemoryRemap> memoryRemap, *rootMemoryMaps_->at(i)->getMemoryRemaps()) 
        {
            if (memoryRemap.data() == childItem)
            {
                return createIndex(i, 0, rootMemoryMaps_->at(i).data());
            }
        }
    }

    //! This should not be reached.
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::getIndexedMemoryRemap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryRemap> MemoryMapsModel::getIndexedMemoryRemap(QModelIndex const& parentIndex, int row) const
{
    foreach (QSharedPointer<MemoryMap> rootItem, *rootMemoryMaps_)
    {
        if (rootItem.data() == parentIndex.internalPointer())
        {
            for (int childRow = 0; childRow < rootItem->getMemoryRemaps()->size(); ++childRow)
            {
                if (childRow == row)
                {
                    return rootItem->getMemoryRemaps()->at(childRow);
                }
            }
        }
    }

    return QSharedPointer<MemoryRemap> (new MemoryRemap());
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::getParentMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMap> MemoryMapsModel::getParentMemoryMap(QSharedPointer<MemoryRemap> memoryRemap) const
{
    foreach (QSharedPointer<MemoryMap> memoryMap, *rootMemoryMaps_)
    {
        if (memoryMap->getMemoryRemaps()->contains(memoryRemap))
        {
            return memoryMap;
        }
    }

    return QSharedPointer<MemoryMap> (new MemoryMap());
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::isIndexValid()
//-----------------------------------------------------------------------------
bool MemoryMapsModel::isIndexValid(QModelIndex const& index) const
{
    if (!index.parent().isValid())
    {
        if (index.row() < 0 || index.row() >= rootMemoryMaps_->size())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    else
    {
        QSharedPointer<MemoryRemap> memoryRemapAtIndex = getIndexedMemoryRemap(index.parent(), index.row());
        QSharedPointer<MemoryMap> parentMemoryMap = getParentMemoryMap(memoryRemapAtIndex);
        if (index.row() < 0 || index.row() >= parentMemoryMap->getMemoryRemaps()->size())
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::onCopyRows()
//-----------------------------------------------------------------------------
void MemoryMapsModel::onCopyRows(QModelIndexList indexList)
{
    QList<QSharedPointer<MemoryMapBase> > copiedMemoryMaps;
    bool hasMemoryMaps = false;
    bool hasMemoryRemaps = false;
    foreach (QModelIndex index, indexList)
    {
        QSharedPointer<MemoryMapBase> newCopy;

        if (index.parent().isValid())
        {
            newCopy = getIndexedMemoryRemap(index.parent(), index.row());
            hasMemoryRemaps = true;
        }
        else
        {
            newCopy = rootMemoryMaps_->at(index.row());
            hasMemoryMaps = true;
        }

        if (!copiedMemoryMaps.contains(newCopy))
        {
            copiedMemoryMaps.append(newCopy);
        }
    }

    QVariant memoryMapVariant;
    memoryMapVariant.setValue(copiedMemoryMaps);

    QMimeData* newMimeData = new QMimeData();
    if (hasMemoryMaps)
    {
        newMimeData->setData("text/xml/ipxact:memoryMap", QByteArray());
    }
    if (hasMemoryRemaps)
    {
        newMimeData->setData("text/xml/ipxact:memoryRemap", QByteArray());
    }
    newMimeData->setImageData(memoryMapVariant);

    QApplication::clipboard()->setMimeData(newMimeData);
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::onPasteRows()
//-----------------------------------------------------------------------------
void MemoryMapsModel::onPasteRows(QModelIndex index)
{
    const QMimeData* pasteData = QApplication::clipboard()->mimeData();

    if (pasteData->hasImage())
    {
        QVariant pasteVariant = pasteData->imageData();
        if (pasteVariant.canConvert<QList<QSharedPointer<MemoryMapBase> > >())
        {
            MemoryRemapExpressionGatherer remapExpressionsGatherer;
            MemoryMapExpressionGatherer memoryMapExpressionsGatherer;
            ReferenceCalculator referenceCalculator(getParameterFinder());

            QList<QSharedPointer<MemoryMapBase> > newBaseMemoryMaps =
                pasteVariant.value<QList<QSharedPointer<MemoryMapBase> > >();

            foreach (QSharedPointer<MemoryMapBase> baseMemory, newBaseMemoryMaps)
            {
                QSharedPointer<MemoryMap> memoryMap = baseMemory.dynamicCast<MemoryMap>();
                if (memoryMap)
                {
                    createPastedMemoryMap(memoryMap, memoryMapExpressionsGatherer, referenceCalculator);
                }
                else
                {
                    QSharedPointer<MemoryRemap> memoryRemap = baseMemory.dynamicCast<MemoryRemap>();
                    if (memoryRemap)
                    {
                        createPastedMemoryRemap(memoryRemap, index, remapExpressionsGatherer, referenceCalculator);
                    }
                }
            }

            emit contentChanged();
        }
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::createNewMemoryMap()
//-----------------------------------------------------------------------------
void MemoryMapsModel::createPastedMemoryMap(QSharedPointer<MemoryMap> memoryMap,
    MemoryMapExpressionGatherer& gatherer, ReferenceCalculator& referenceCalculator)
{
    int insertBegin = rootMemoryMaps_->size();
    beginInsertRows(QModelIndex(), insertBegin, insertBegin);

    QSharedPointer<MemoryMap> copyMemoryMap (new MemoryMap(*memoryMap.data()));
    copyMemoryMap->setName(createUniqueName(copyMemoryMap->name(), getRootMemoryMapNames()));

    rootMemoryMaps_->append(copyMemoryMap);

    increaseReferencesInPastedMap(gatherer.getExpressions(copyMemoryMap), referenceCalculator);

    emit memoryMapAdded(insertBegin);

    emit aubChangedOnRow(insertBegin);

    endInsertRows();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::createUniqueName()
//-----------------------------------------------------------------------------
QString MemoryMapsModel::createUniqueName(QString const& originalName, QStringList currentNames) const
{
    QString name = originalName;
    int trailingNumber = 1;

    bool match =  true;
    while (match)
    {
        match = false;        
        for(int row = 0; row < currentNames.size(); row++)
        {
            if (name.compare(currentNames.at(row)) == 0)
            {
                match = true;
                name = originalName + "_" + QString::number(trailingNumber);
                trailingNumber++;
            }
        }
    }

    return name;
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::getRootMemoryMapNames()
//-----------------------------------------------------------------------------
QStringList MemoryMapsModel::getRootMemoryMapNames() const
{
    QStringList mapNames;
    foreach (QSharedPointer<MemoryMap> memoryMap, *rootMemoryMaps_)
    {
        mapNames.append(memoryMap->name());
    }

    return mapNames;
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::increaseReferencesInPastedMemoryMap()
//-----------------------------------------------------------------------------
void MemoryMapsModel::increaseReferencesInPastedMap(QStringList mapExpressions,
    ReferenceCalculator& referenceCalculator)
{
    QMap<QString, int> referencedParameters = referenceCalculator.getReferencedParameters(mapExpressions);

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
// Function: memorymapsmodel::createPastedMemoryRemap()
//-----------------------------------------------------------------------------
void MemoryMapsModel::createPastedMemoryRemap(QSharedPointer<MemoryRemap> memoryRemap,
    QModelIndex const& parentIndex, MemoryRemapExpressionGatherer& gatherer,
    ReferenceCalculator& referenceCalculator)
{
    if (parentIndex.isValid())
    {
        QSharedPointer<MemoryMap> parentMemoryMap = rootMemoryMaps_->at(parentIndex.row());
        int rowOfNewItem = parentMemoryMap->getMemoryRemaps()->count();

        QSharedPointer<MemoryRemap> copiedMemoryRemap (new MemoryRemap(*memoryRemap.data()));
        copiedMemoryRemap->setName(
            createUniqueName(copiedMemoryRemap->name(), getMemoryRemapNames(parentMemoryMap)));

        beginInsertRows(parentIndex, rowOfNewItem, rowOfNewItem);

        parentMemoryMap->getMemoryRemaps()->append(copiedMemoryRemap);

        increaseReferencesInPastedMap(gatherer.getExpressions(memoryRemap), referenceCalculator);

        emit memoryRemapAdded(rowOfNewItem, parentMemoryMap);

        endInsertRows();
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::getMemoryRemapNames()
//-----------------------------------------------------------------------------
QStringList MemoryMapsModel::getMemoryRemapNames(QSharedPointer<MemoryMap> currentMap) const
{
    QStringList remapNames;
    foreach (QSharedPointer<MemoryRemap> remap, *currentMap->getMemoryRemaps())
    {
        remapNames.append(remap->name());
    }

    return remapNames;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool MemoryMapsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == MemoryMapsColumns::IS_PRESENT;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsModel::validateIndex()
//-----------------------------------------------------------------------------
bool MemoryMapsModel::validateIndex(QModelIndex const& index) const
{
    int columnIndex = index.column();

    if (index.parent().isValid() && columnIndex == MemoryMapsColumns::REMAPSTATE_COLUMN)
    {
        QSharedPointer<MemoryRemap> memoryRemap = getIndexedMemoryRemap(index.parent(), index.row());

        if (columnIndex == MemoryMapsColumns::NAME_COLUMN)
        {
            return memoryMapValidator_->hasValidName(memoryRemap);
        }
        else if (columnIndex == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            return !memoryMapValidator_->remapStateIsNotValid(memoryRemap);
        }
        else if (columnIndex == MemoryMapsColumns::IS_PRESENT)
        {
            return memoryMapValidator_->hasValidIsPresent(memoryRemap);
        }
    }

    else if (!index.parent().isValid())
    {
        QSharedPointer<MemoryMap> currentMap = rootMemoryMaps_->at(index.row());

        if (columnIndex == MemoryMapsColumns::NAME_COLUMN)           
        {
            return memoryMapValidator_->hasValidName(currentMap);
        }
        else if (columnIndex == MemoryMapsColumns::AUB_COLUMN)
        {
            return memoryMapValidator_->hasValidAddressUnitBits(currentMap);
        }
        else if (columnIndex == MemoryMapsColumns::IS_PRESENT)
        {
            return memoryMapValidator_->hasValidIsPresent(currentMap);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::valueForIndex(QModelIndex const& index) const
{
    if (index.column() == MemoryMapsColumns::NAME_COLUMN)
    {
        if (!index.parent().isValid())
        {
            return rootMemoryMaps_->at(index.row())->name();
        }
        else
        {
            QSharedPointer<MemoryRemap> childItem = getIndexedMemoryRemap(index.parent(), index.row());
            return childItem->name();
        }
    }
    else if (index.column() == MemoryMapsColumns::AUB_COLUMN)
    {
        QSharedPointer<MemoryMap> parentMemoryMap;

        if (!index.parent().isValid())
        {
            parentMemoryMap = rootMemoryMaps_->at(index.row());
        }
        else
        {
            parentMemoryMap = getParentMemoryMap(getIndexedMemoryRemap(index.parent(), index.row()));
        }

        return parentMemoryMap->getAddressUnitBits();

    }
    else if (index.column() == MemoryMapsColumns::INTERFACE_COLUMN)
    {
        QSharedPointer<MemoryMap> parentMemoryMap;
        if (!index.parent().isValid())
        {
            parentMemoryMap = rootMemoryMaps_->at(index.row());
        }
        else
        {
            parentMemoryMap = getParentMemoryMap(getIndexedMemoryRemap(index.parent(), index.row()));
        }

        QStringList interfaceNames = component_->getSlaveInterfaces(parentMemoryMap->name());
        if (interfaceNames.isEmpty() || !parentMemoryMap)
        {
            return tr("No binding");
        }
        else
        {
            return interfaceNames.join(", ");
        }
    }
    else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
    {
        if (!index.parent().isValid())
        {
            return QString(tr("Default"));
        }
        else
        {
            QSharedPointer<MemoryRemap> childItem = getIndexedMemoryRemap(index.parent(), index.row());
            if (!childItem->getRemapState().isEmpty())
            {
                return childItem->getRemapState();
            }
            else
            {
                return QString(tr("No remap state selected."));
            }
        }
    }
    else if (index.column() == MemoryMapsColumns::IS_PRESENT)
    {
        if (!index.parent().isValid())
        {
            return rootMemoryMaps_->at(index.row())->getIsPresent();
        }
        else
        {
            QSharedPointer<MemoryRemap> childItem = getIndexedMemoryRemap(index.parent(), index.row());
            return childItem->getIsPresent();
        }
    }
    else if (index.column() == MemoryMapsColumns::DESCRIPTION_COLUMN)
    {
        if (!index.parent().isValid())
        {
            return rootMemoryMaps_->at(index.row())->description();
        }
        else
        {
            QSharedPointer<MemoryRemap> childItem = getIndexedMemoryRemap(index.parent(), index.row());
            return childItem->description();
        }
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::mimeTypes()
//-----------------------------------------------------------------------------
QStringList MemoryMapsModel::mimeTypes() const
{
    QStringList types(QAbstractItemModel::mimeTypes());

    types << "text/xml/ipxact:memoryMap";
    types << "text/xml/ipxact:memoryRemap";

    return types;
}
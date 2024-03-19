//-----------------------------------------------------------------------------
// File: memorymapsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.08.2012
//
// Description:
// The model to manage the memory maps summary.
//-----------------------------------------------------------------------------

#include "memorymapsmodel.h"

#include "MemoryMapsColumns.h"

#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/MemoryMapInterface.h>
#include <KactusAPI/include/MemoryMapExpressionsGatherer.h>
#include <KactusAPI/include//MemoryRemapExpressionGatherer.h>
#include <editors/ComponentEditor/memoryMaps/memoryMapsExpressionCalculators/ReferenceCalculator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/MemoryMapBase.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/Component/MemoryRemap.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/validators/MemoryMapValidator.h>

#include <common/KactusColors.h>

#include <QRegularExpression>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::MemoryMapsModel()
//-----------------------------------------------------------------------------
MemoryMapsModel::MemoryMapsModel(QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionParser> expressionParser, MemoryMapInterface* mapInterface,
    Document::Revision docRevision, const int ROWHEIGHT, QObject *parent):
QAbstractItemModel(parent),
ParameterizableTable(parameterFinder),
mapInterface_(mapInterface),
docRevision_(docRevision),
ROWHEIGHT_(ROWHEIGHT)
{
    setExpressionParser(expressionParser);
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::rowCount()
//-----------------------------------------------------------------------------
int MemoryMapsModel::rowCount( const QModelIndex& parent /*= QModelIndex()*/ ) const
{
    if (!parent.isValid())
    {
        return mapInterface_->itemCount();
    }

    else if (!parent.parent().isValid())
    {
        return mapInterface_->remapCount(mapInterface_->getIndexedItemName(parent.row()));
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

    if (docRevision_ == Document::Revision::Std22)
    {
        if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }

        if (index.parent().isValid() && index.column() == MemoryMapsColumns::AUB_COLUMN)
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
    }
    else
    {
        if ((!index.parent().isValid() && index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN) ||
            (index.parent().isValid() && index.column() == MemoryMapsColumns::AUB_COLUMN))
        {
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
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
            return docRevision_ == Document::Revision::Std22 ? tr("Mode reference") : tr("Remap state");
        }
        else if (section == MemoryMapsColumns::AUB_COLUMN)
        {
            return tr("Address unit\nbits (AUB)") + getExpressionSymbol();
        }
        else if (section == MemoryMapsColumns::INTERFACE_COLUMN)
        {
            if (docRevision_ == Document::Revision::Std22)
            {
                return tr("Target interface\nbinding");
            }
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

    if (role == Qt::DisplayRole) 
    {
        if (isValidExpressionColumn(index))
        {
            return formattedExpressionForIndex(index);
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
        return expressionOrValueForIndex(index);
    }
    else if (role == Qt::ToolTipRole)
    {
        return valueForIndex(index);
    }
	else if (role == Qt::UserRole)
    {
        QPair<QString, QString> mapRemapNames = getIndexedMapRemapNames(index);
        QString mapName = mapRemapNames.first;

        QStringList slaves;
        for (auto associatedSlave : mapInterface_->getAssociatedSlaveInterfaces(mapName.toStdString()))
        {
            slaves.append(QString::fromStdString(associatedSlave));
        }

        return slaves;
    }
	else if (role == Qt::ForegroundRole)
    {
        if (index.column() == MemoryMapsColumns::INTERFACE_COLUMN ||
            (!index.parent().isValid() && index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN) ||
            (index.parent().isValid() && index.column() == MemoryMapsColumns::AUB_COLUMN))
        {
            return KactusColors::DISABLED_TEXT;
        }

        if (docRevision_ == Document::Revision::Std22 && index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            if (index.data(Qt::DisplayRole).toString() == "None")
            {
                return KactusColors::ERROR;
            }

            return KactusColors::DISABLED_TEXT;
        }

        return blackForValidOrRedForInvalidIndex(index);
	}
	else if (role == Qt::BackgroundRole)
    {
        if (docRevision_ == Document::Revision::Std22)
        {
            if (index.column() == MemoryMapsColumns::NAME_COLUMN)
            {
                return KactusColors::MANDATORY_FIELD;
            }
            else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
            {
                return KactusColors::DISABLED_FIELD;
            }
            else
            {
                return KactusColors::REGULAR_FIELD;
            }
        }

        if (index.column() == MemoryMapsColumns::NAME_COLUMN ||
            index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else
        {
            return KactusColors::REGULAR_FIELD;
        }
	}
    else if (index.column() == MemoryMapsColumns::NAME_COLUMN && role == Qt::SizeHintRole)
    {
        return QSize(40, ROWHEIGHT_);
    }
    else if (role == Qt::FontRole)
    {
        return italicForEvaluatedValue(index);
    }

    return QVariant();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::getIndexedMapRemapNames()
//-----------------------------------------------------------------------------
QPair<QString, QString> MemoryMapsModel::getIndexedMapRemapNames(QModelIndex const& index) const
{
    std::string mapName = "";
    std::string remapName = "";
    if (index.parent().isValid())
    {
        mapName = mapInterface_->getIndexedItemName(index.parent().row());
        remapName = mapInterface_->getIndexedRemapName(mapName, index.row());
    }
    else
    {
        mapName = mapInterface_->getIndexedItemName(index.row());
    }

    QPair<QString, QString> itemNames;
    itemNames.first = QString::fromStdString(mapName);
    itemNames.second = QString::fromStdString(remapName);

    return itemNames;
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

    bool itemIsMemoryMap = !index.parent().isValid();

    QPair<QString, QString> itemNames = getIndexedMapRemapNames(index);
    std::string mapName = itemNames.first.toStdString();
    std::string remapName = itemNames.second.toStdString();

	if (role == Qt::EditRole)
    {
        if (index.column() == MemoryMapsColumns::NAME_COLUMN)
        {
            QString parentMapName = QString::fromStdString(mapName);

            if (itemIsMemoryMap)
            {
                mapInterface_->setName(mapName, value.toString().toStdString());

                QString newName = QString::fromStdString(mapInterface_->getIndexedItemName(index.row()));

                emit memoryMapNameChanged(parentMapName, newName);
            }
            else
            {
                mapInterface_->setRemapName(mapName, remapName, value.toString().toStdString());

                QString oldName = QString::fromStdString(remapName);
                QString newName = QString::fromStdString(mapInterface_->getIndexedRemapName(mapName, index.row()));

                emit memoryRemapNameChanged(parentMapName, oldName, newName);
            }

            emit graphicsChanged();
        }
        else if (index.column() == MemoryMapsColumns::AUB_COLUMN && itemIsMemoryMap)
        {
            mapInterface_->setAddressUnitBits(mapName, value.toString().toStdString());
            emit aubChangedOnRow(index.row());
        }
        else if (index.column() == MemoryMapsColumns::DESCRIPTION_COLUMN)
        {
            if (itemIsMemoryMap)
            {
                mapInterface_->setDescription(mapName, value.toString().toStdString());
            }
            else
            {
                mapInterface_->setRemapDescription(mapName, remapName, value.toString().toStdString());
            }
        }
        else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            if (!itemIsMemoryMap)
            {
                mapInterface_->setRemapState(mapName, remapName, value.toString().toStdString());
            }
        }
        else if (index.column() == MemoryMapsColumns::IS_PRESENT)
        {
            if (itemIsMemoryMap)
            {
                mapInterface_->setIsPresent(mapName, value.toString().toStdString());
            }
            else
            {
                mapInterface_->setIsPresent(mapName, value.toString().toStdString(), remapName);
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
    int row = mapInterface_->itemCount();

	if (index.isValid())
    {
        if (index.parent().isValid())
        {
            row = index.parent().row();
        }
        else
        {
            row = index.row();
        }
	}

	beginInsertRows(QModelIndex(), row, row);
    mapInterface_->addMemoryMap(row);
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
    QPair<QString, QString> itemNames = getIndexedMapRemapNames(index);
    QString mapName = itemNames.first;
    QString remapName = itemNames.second;

    if (!index.parent().isValid())
    {
        decreaseReferencesWithRemovedMemoryMap(mapName, "");

        beginRemoveRows(QModelIndex(), index.row(), index.row());
        mapInterface_->removeMap(mapName.toStdString());
        endRemoveRows();

        // inform navigation tree that file set has been removed
        emit memoryMapRemoved(index.row());

        // tell also parent widget that contents have been changed
    }
    else
    {
        decreaseReferencesWithRemovedMemoryMap(mapName, remapName);

        beginRemoveRows(index.parent(), index.row(), index.row());
        mapInterface_->removeRemap(mapName.toStdString(), remapName.toStdString());
        endRemoveRows();

        emit memoryRemapRemoved(index.row(), mapName);
    }

    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::decreaseReferencesWithRemovedMemoryMap()
//-----------------------------------------------------------------------------
void MemoryMapsModel::decreaseReferencesWithRemovedMemoryMap(QString const& mapName, QString const& remapName)
{
    auto expressionList = mapInterface_->getMapExpressions(mapName.toStdString(), remapName.toStdString());
    QStringList expressionListQT;
    for (auto const& expression : expressionList)
    {
        expressionListQT.append(QString::fromStdString(expression));
    }

    ReferenceCalculator memoryMapReferenceCalculator(getParameterFinder());
    QMap<QString, int> referencedParameters =
        memoryMapReferenceCalculator.getReferencedParameters(expressionListQT);

    for (auto const& referencedId : referencedParameters.keys())
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

    std::string parentMapName = mapInterface_->getIndexedItemName(index.row());
    int rowOfNewItem = mapInterface_->remapCount(parentMapName);

    increaseReferencesWithNewRemap(QString::fromStdString(parentMapName));

    beginInsertRows(index, rowOfNewItem, rowOfNewItem);
    mapInterface_->addMemoryRemap(parentMapName);
    endInsertRows();

    emit memoryRemapAdded(rowOfNewItem, QString::fromStdString(parentMapName));
    emit contentChanged();
}


//-----------------------------------------------------------------------------
// Function: memorymapsmodel::increaseReferencesWithNewRemap()
//-----------------------------------------------------------------------------
void MemoryMapsModel::increaseReferencesWithNewRemap(QString const& memoryMapName)
{
    auto expressionList = mapInterface_->getMapExpressions(memoryMapName.toStdString(), "");
    QStringList expressionListQT;
    for (auto const& expression : expressionList)
    {
        expressionListQT.append(QString::fromStdString(expression));
    }

    ReferenceCalculator memoryMapReferenceCalculator(getParameterFinder());
    QMap<QString, int> referencedParameters =
        memoryMapReferenceCalculator.getReferencedParameters(expressionListQT);

    for (auto const& referencedId : referencedParameters.keys())
    {
        for (int i = 0; i < referencedParameters.value(referencedId); ++i)
        {
            emit increaseReferences(referencedId);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::index()
//-----------------------------------------------------------------------------
QModelIndex MemoryMapsModel::index(int row, int column, const QModelIndex &parent /* = QModelIndex() */ ) const
{
    if (!parent.isValid() && 0 <= row && row < mapInterface_->itemCount())
    {
        std::string mapName = mapInterface_->getIndexedItemName(row);
        return createIndex(row, column, mapInterface_->getMapPointer(mapName));
    }
    else
    {
        std::string mapName = mapInterface_->getIndexedItemName(parent.row());
        std::string remapName = mapInterface_->getIndexedRemapName(mapName, row);

        return createIndex(row, column, mapInterface_->getRemapPointer(mapName, remapName));
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

    for (auto mapName : mapInterface_->getItemNames())
    {
        MemoryMap* mapPointer = mapInterface_->getMapPointer(mapName);
        if (mapPointer && mapPointer == child.internalPointer())
        {
            return QModelIndex();
        }
    }

    MemoryRemap* childItem = static_cast<MemoryRemap*>(child.internalPointer());
    if (childItem)
    {
        return createParentIndexForMemoryRemap(childItem);
    }
    else
    {
        return QModelIndex();
    }
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::createParentIndexForChildItem()
//-----------------------------------------------------------------------------
QModelIndex MemoryMapsModel::createParentIndexForMemoryRemap(MemoryRemap* childItem) const
{
    std::string remapName = childItem->name().toStdString();
    for (int i = 0; i < mapInterface_->itemCount(); ++i)
    {
        std::string mapName = mapInterface_->getIndexedItemName(i);
        MemoryRemap* remapPointer = mapInterface_->getRemapPointer(mapName, remapName);
        if (remapPointer == childItem)
        {
            MemoryMap* mapPointer = mapInterface_->getMapPointer(mapName);
            return createIndex(i, 0, mapPointer);
        }
    }

    //! This should not be reached.
    return QModelIndex();
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::isIndexValid()
//-----------------------------------------------------------------------------
bool MemoryMapsModel::isIndexValid(QModelIndex const& index) const
{
    if (!index.parent().isValid())
    {
        if (index.row() < 0 || index.row() >= mapInterface_->itemCount())
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
        std::string mapName = mapInterface_->getIndexedItemName(index.parent().row());
        if (index.row() < 0 || index.row() >= mapInterface_->remapCount(mapName))
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
void MemoryMapsModel::onCopyRows(QModelIndexList const& indexList)
{
    std::vector<std::string> copiedRows;
    for (auto const& index : indexList)
    {
        QString row = QString::number(index.row());
        if (index.parent().isValid())
        {
            row = QString::number(index.parent().row()) + "_" + QString::number(index.row());
        }

        copiedRows.push_back(row.toStdString());
    }

    mapInterface_->copyRows(copiedRows);
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::onPasteRows()
//-----------------------------------------------------------------------------
void MemoryMapsModel::onPasteRows(QModelIndex const& index)
{
    std::string mapName = "";
    QModelIndex parentIndex = index;
    if (index.isValid())
    {
        if (!index.parent().isValid() && index.row() >= 0 && index.row() < mapInterface_->itemCount())
        {
            mapName = mapInterface_->getIndexedItemName(index.row());
        }
        else if (index.parent().isValid() && index.parent().row() >= 0 && index.parent().row())
        {
            mapName = mapInterface_->getIndexedItemName(index.parent().row());
            parentIndex = index.parent();
        }
    }

    int mapRowBegin = mapInterface_->itemCount();
    int remapRowBegin = mapInterface_->remapCount(mapName);

    std::vector<std::string> pastedMapNames = mapInterface_->pasteMemoryMaps();
    std::vector<std::string> pastedRemapNames = mapInterface_->pasteMemoryRemaps(mapName);

    int mapCount = static_cast<int>(pastedMapNames.size());
    int remapCount = static_cast<int>(pastedRemapNames.size());

    if (mapCount == 0 && remapCount == 0)
    {
        return;
    }

    ReferenceCalculator referenceCalculator(getParameterFinder());
    if (mapCount > 0)
    {
        int rowEnd = mapRowBegin + mapCount - 1;

        beginInsertRows(QModelIndex(), mapRowBegin, rowEnd);

        for (int i = mapRowBegin; i <= rowEnd; ++i)
        {
            std::string copyName = mapInterface_->getIndexedItemName(i);

            QStringList expressionList;
            for (auto const& expression : mapInterface_->getMapExpressions(copyName, ""))
            {
                expressionList.append(QString::fromStdString(expression));
            }

            increaseReferencesInPastedMap(expressionList, referenceCalculator);

            emit memoryMapAdded(i);
            emit aubChangedOnRow(i);
        }

        endInsertRows();
    }
    if (remapCount > 0)
    {
        int rowEnd = remapRowBegin + remapCount - 1;

        beginInsertRows(parentIndex, remapRowBegin, rowEnd);

        for (int i = remapRowBegin; i <= rowEnd; ++i)
        {
            std::string remapName = mapInterface_->getIndexedRemapName(mapName, i);
            QStringList expressionList;
            for (auto const& expression : mapInterface_->getMapExpressions(mapName, remapName))
            {
                expressionList.append(QString::fromStdString(expression));
            }
            increaseReferencesInPastedMap(expressionList, referenceCalculator);

            emit memoryRemapAdded(i, QString::fromStdString(mapName));
        }

        endInsertRows();
    }

    emit contentChanged();
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
// Function: MemoryMapsModel::isValidExpressionColumn()
//-----------------------------------------------------------------------------
bool MemoryMapsModel::isValidExpressionColumn(QModelIndex const& index) const
{
    return index.column() == MemoryMapsColumns::IS_PRESENT || index.column() == MemoryMapsColumns::AUB_COLUMN;
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsModel::expressionOrValueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::expressionOrValueForIndex(QModelIndex const& index) const
{
    if (isValidExpressionColumn(index))
    {
        return expressionForIndex(index);
    }
    else
    {
        return valueForIndex(index);
    }
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsModel::validateIndex()
//-----------------------------------------------------------------------------
bool MemoryMapsModel::validateIndex(QModelIndex const& index) const
{
    int columnIndex = index.column();

    QPair<QString, QString> mapRemapNames = getIndexedMapRemapNames(index);
    std::string mapName = mapRemapNames.first.toStdString();
    std::string remapName = mapRemapNames.second.toStdString();

    if (index.parent().isValid())
    {
        if (columnIndex == MemoryMapsColumns::NAME_COLUMN)
        {
            return mapInterface_->remapHasValidName(mapName, remapName);
        }
        else if (columnIndex == MemoryMapsColumns::REMAPSTATE_COLUMN)
        {
            if (docRevision_ == Document::Revision::Std14)
            {
                return mapInterface_->remapHasValidRemapState(mapName, remapName);
            }
        }
        else if (columnIndex == MemoryMapsColumns::IS_PRESENT)
        {
            return mapInterface_->itemHasValidIsPresent(mapName, remapName);
        }
    }

    else if (!index.parent().isValid())
    {
        if (columnIndex == MemoryMapsColumns::NAME_COLUMN)           
        {
            return mapInterface_->itemHasValidName(mapName);
        }
        else if (columnIndex == MemoryMapsColumns::AUB_COLUMN)
        {
            return mapInterface_->memoryMapHasValidAddressUnitBits(mapName);
        }
        else if (columnIndex == MemoryMapsColumns::IS_PRESENT)
        {
            return mapInterface_->itemHasValidIsPresent(mapName);
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::formattedExpressionForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::formattedExpressionForIndex(QModelIndex const& index) const
{
    QPair<QString, QString> mapRemapNames = getIndexedMapRemapNames(index);
    std::string mapName = mapRemapNames.first.toStdString();
    std::string remapName = mapRemapNames.second.toStdString();

    if (index.column() == MemoryMapsColumns::IS_PRESENT)
    {
        return QString::fromStdString(mapInterface_->getIsPresentFormattedExpression(mapName, remapName));
    }
    else if (index.column() == MemoryMapsColumns::AUB_COLUMN)
    {
        return QString::fromStdString(mapInterface_->getAddressUnitBitsFormattedExpression(mapName));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: memorymapsmodel::expressionForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::expressionForIndex(QModelIndex const& index) const
{
    QPair<QString, QString> mapRemapNames = getIndexedMapRemapNames(index);
    std::string mapName = mapRemapNames.first.toStdString();
    std::string remapName = mapRemapNames.second.toStdString();

    if (index.column() == MemoryMapsColumns::IS_PRESENT)
    {
        return QString::fromStdString(mapInterface_->getIsPresentExpression(mapName, remapName));
    }
    else if (index.column() == MemoryMapsColumns::AUB_COLUMN)
    {
        return QString::fromStdString(mapInterface_->getAddressUnitBitsExpression(mapName));
    }

    return valueForIndex(index);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapsModel::valueForIndex()
//-----------------------------------------------------------------------------
QVariant MemoryMapsModel::valueForIndex(QModelIndex const& index) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    bool itemIsMemoryMap = !index.parent().isValid();
    QPair<QString, QString> mapRemapNames = getIndexedMapRemapNames(index);
    std::string mapName = mapRemapNames.first.toStdString();
    std::string remapName = mapRemapNames.second.toStdString();

    if (index.column() == MemoryMapsColumns::NAME_COLUMN)
    {
        std::string itemName = mapName;
        if (!itemIsMemoryMap)
        {
            itemName = remapName;
        }
        return QString::fromStdString(itemName);
    }
    else if (index.column() == MemoryMapsColumns::AUB_COLUMN)
    {
        return QString::fromStdString(mapInterface_->getAddressUnitBitsValue(mapName));
    }
    else if (index.column() == MemoryMapsColumns::INTERFACE_COLUMN)
    {
        return QString::fromStdString(mapInterface_->getInterfaceBinding(mapName));
    }
    else if (index.column() == MemoryMapsColumns::REMAPSTATE_COLUMN)
    {
        if (docRevision_ == Document::Revision::Std22)
        {
            return QString::fromStdString(mapInterface_->getRemapModeReferenceString(mapName, remapName));
        }

        return QString::fromStdString(mapInterface_->getRemapState(mapName, remapName));
    }
    else if (index.column() == MemoryMapsColumns::IS_PRESENT)
    {
        return QString::fromStdString(mapInterface_->getIsPresentValue(mapName, remapName));
    }
    else if (index.column() == MemoryMapsColumns::DESCRIPTION_COLUMN)
    {
        if (itemIsMemoryMap)
        {
            return QString::fromStdString(mapInterface_->getDescription(mapName));
        }
        else
        {
            return QString::fromStdString(mapInterface_->getRemapDescription(mapName, remapName));
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

//-----------------------------------------------------------------------------
// Function: MemoryMapsModel::onRemoveAllChildItemsFrom()
//-----------------------------------------------------------------------------
void MemoryMapsModel::onRemoveAllChildItemsFrom(QModelIndex const& itemIndex)
{
    if (isIndexValid(itemIndex) && !itemIndex.parent().isValid())
    {
        std::string mapName = mapInterface_->getIndexedItemName(itemIndex.row());

        int remapCount = mapInterface_->remapCount(mapName);
        if (remapCount > 0)
        {
            int lastRemapIndex = remapCount - 1;
            beginRemoveRows(itemIndex, 0, lastRemapIndex);

            for (int i = 0; i < remapCount; ++i)
            {
                std::string remapName = mapInterface_->getIndexedRemapName(mapName, 0);
                decreaseReferencesWithRemovedMemoryMap(QString::fromStdString(mapName),
                    QString::fromStdString(remapName));

                emit memoryRemapRemoved(0, QString::fromStdString(mapName));

                mapInterface_->removeRemap(mapName, remapName);
            }

            endRemoveRows();

            emit contentChanged();
        }
    }
}
